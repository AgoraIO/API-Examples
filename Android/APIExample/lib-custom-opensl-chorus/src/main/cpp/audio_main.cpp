/*
 * Copyright 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "jni_interface.h"
#include "audio_recorder.h"
#include "audio_player.h"
#include "audio_effect.h"
#include "audio_common.h"
#include "include/IAgoraMediaEngine.h"
#include <jni.h>
#include <SLES/OpenSLES_Android.h>
#include <sys/types.h>
#include <cassert>
#include <cstring>

#define PCM_PATH "/sdcard/test.pcm"

FILE * fpcm;

void mixWithLocalPcm(agora::media::IAudioFrameObserverBase::AudioFrame &audioFrame) {
//  readLocalPcm();
}

/*class AgoraAudioFrameObserver : public agora::media::IAudioFrameObserver {
public:

  bool onRecordAudioFrame(AudioFrame &audioFrame) override {
    LOGE("tshi ============= onRecordAudioFrame %d", audioFrame.samplesPerChannel);
    mixWithLocalPcm(audioFrame);
    return true;
  }

  bool onPlaybackAudioFrame(AudioFrame &audioFrame) override {
    LOGE("tshi ============= onPlaybackAudioFrame %d", audioFrame.samplesPerChannel);
    return true;
  }

  bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame &audioFrame) override {
    return true;
  }

  bool onMixedAudioFrame(AudioFrame &audioFrame) override {
    return true;
  }
};

static AgoraAudioFrameObserver audioframe_Observer_;*/

struct EchoAudioEngine {
  SLmilliHertz fastPathSampleRate_;
  uint32_t fastPathFramesPerBuf_;
  uint16_t sampleChannels_;
  uint16_t bitsPerSample_;

  SLObjectItf slEngineObj_;
  SLEngineItf slEngineItf_;

  AudioRecorder *recorder_;
  AudioPlayer *player_;
  AudioQueue *freeRecBufQueue_;  // Owner of the queue
  AudioQueue *recBufQueue_;   // Owner of the queue

  AudioQueue *freePlayBufQueue_;  // Owner of the queue
  AudioQueue *playBufQueue_;   // Owner of the queue

  sample_buf *bufs_;
  sample_buf *playBufs_;
  uint32_t bufCount_;
  uint32_t frameCount_;
  int64_t echoDelay_;
  float echoDecay_;
  AudioDelay *delayEffect_;

  agora::rtc::IRtcEngine* agoraEngine_;
  agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine_;
};
static EchoAudioEngine engine;

void* data_;

bool EngineService(void *ctx, uint32_t msg, void *data);

void initAgoraEngine(long agoraEngine) {
  memset(&engine, 0, sizeof(engine));
  LOGE("tshi initAgoraEngine");
  if (agoraEngine != -1) {
    engine.agoraEngine_ = reinterpret_cast<agora::rtc::IRtcEngine*>(agoraEngine);
    auto ret = engine.agoraEngine_->setClientRole(agora::rtc::CLIENT_ROLE_BROADCASTER);
    ret = engine.agoraEngine_->setExternalAudioSink(48000, 2);
    ret = engine.agoraEngine_->enableEchoCancellationExternal(true, 0);
    ret = engine.agoraEngine_->setRecordingAudioFrameParameters(
        48000, 2, agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, 48000 / 100 * 2);
    engine.mediaEngine_.queryInterface(engine.agoraEngine_, agora::rtc::INTERFACE_ID_TYPE::AGORA_IID_MEDIA_ENGINE);
  }
  if (engine.mediaEngine_) {
//    engine.mediaEngine_->registerAudioFrameObserver(&audioframe_Observer_);
  }
}

void createSLEngine(int sampleRate, int framesPerBuf, long delayInMs, float decay) {
  SLresult result;
  engine.fastPathSampleRate_ = static_cast<SLmilliHertz>(sampleRate) * 1000;
  engine.fastPathFramesPerBuf_ = static_cast<uint32_t>(framesPerBuf);
  engine.sampleChannels_ = AUDIO_SAMPLE_CHANNELS;
  engine.bitsPerSample_ = SL_PCMSAMPLEFORMAT_FIXED_16;
  LOGE("engine.fastPathSampleRate_: %d", sampleRate);

  result = slCreateEngine(&engine.slEngineObj_, 0, NULL, 0, NULL, NULL);
  SLASSERT(result);

  result =
      (*engine.slEngineObj_)->Realize(engine.slEngineObj_, SL_BOOLEAN_FALSE);
  SLASSERT(result);

  result = (*engine.slEngineObj_)
               ->GetInterface(engine.slEngineObj_, SL_IID_ENGINE,
                              &engine.slEngineItf_);
  SLASSERT(result);

  // compute the RECOMMENDED fast audio buffer size:
  //   the lower latency required
  //     *) the smaller the buffer should be (adjust it here) AND
  //     *) the less buffering should be before starting player AFTER
  //        receiving the recorder buffer
  //   Adjust the bufSize here to fit your bill [before it busts]
  uint32_t bufSize = engine.fastPathFramesPerBuf_ * engine.sampleChannels_ *
                     engine.bitsPerSample_;
  bufSize = (bufSize + 7) >> 3;  // bits --> byte
  engine.bufCount_ = BUF_COUNT;
  engine.bufs_ = allocateSampleBufs(engine.bufCount_, bufSize);
  engine.playBufs_ = allocateSampleBufs(engine.bufCount_, bufSize);
  assert(engine.bufs_);

  engine.freeRecBufQueue_ = new AudioQueue(engine.bufCount_);
  engine.recBufQueue_ = new AudioQueue(engine.bufCount_);
  assert(engine.freeRecBufQueue_ && engine.recBufQueue_);
  for (uint32_t i = 0; i < engine.bufCount_; i++) {
    engine.freeRecBufQueue_->push(&engine.bufs_[i]);
  }

  engine.freePlayBufQueue_ = new AudioQueue(engine.bufCount_);
  engine.playBufQueue_ = new AudioQueue(engine.bufCount_);
  assert(engine.freePlayBufQueue_ && engine.playBufQueue_);
  for (uint32_t i = 0; i < engine.bufCount_; i++) {
    engine.freePlayBufQueue_->push(&engine.playBufs_[i]);
  }
//  memset(data_, 0, sizeof(sample_buf));

  uint32_t allocSize = (bufSize + 3) & ~3;  // padding to 4 bytes aligned
  uint32_t i;
  data_ = new uint16_t[allocSize / 2];

  engine.echoDelay_ = delayInMs;
  engine.echoDecay_ = decay;
  engine.delayEffect_ = new AudioDelay(
      engine.fastPathSampleRate_, engine.sampleChannels_, engine.bitsPerSample_,
      engine.echoDelay_, engine.echoDecay_);
  assert(engine.delayEffect_);
}

bool configureEcho(int delayInMs, float decay) {
  engine.echoDelay_ = delayInMs;
  engine.echoDecay_ = decay;

  engine.delayEffect_->setDelayTime(delayInMs);
  engine.delayEffect_->setDecayWeight(decay);
  return false;
}

bool createSLBufferQueueAudioPlayer() {
  SampleFormat sampleFormat;
  memset(&sampleFormat, 0, sizeof(sampleFormat));
  sampleFormat.pcmFormat_ = static_cast<uint16_t>(engine.bitsPerSample_);
  sampleFormat.framesPerBuf_ = engine.fastPathFramesPerBuf_;

  sampleFormat.channels_ = static_cast<uint16_t>(engine.sampleChannels_);
  sampleFormat.sampleRate_ = engine.fastPathSampleRate_;

  engine.player_ = new AudioPlayer(&sampleFormat, engine.slEngineItf_);
  assert(engine.player_);
  if (engine.player_ == nullptr) return false;

  engine.player_->SetBufQueue(engine.playBufQueue_, engine.freePlayBufQueue_);
  engine.player_->RegisterCallback(EngineService, (void *)&engine);

  return true;
}

void deleteSLBufferQueueAudioPlayer() {
  if (engine.player_) {
    delete engine.player_;
    engine.player_ = nullptr;
  }
}

bool createAudioRecorder() {
  SampleFormat sampleFormat;
  memset(&sampleFormat, 0, sizeof(sampleFormat));
  sampleFormat.pcmFormat_ = static_cast<uint16_t>(engine.bitsPerSample_);
  sampleFormat.channels_ = static_cast<uint16_t>(engine.sampleChannels_);
  sampleFormat.sampleRate_ = engine.fastPathSampleRate_;
  sampleFormat.framesPerBuf_ = engine.fastPathFramesPerBuf_;
  engine.recorder_ = new AudioRecorder(&sampleFormat, engine.slEngineItf_);
  if (!engine.recorder_) {
    return false;
  }
  engine.recorder_->SetBufQueues(engine.freeRecBufQueue_, engine.recBufQueue_);
  engine.recorder_->RegisterCallback(EngineService, (void *)&engine);
  return true;
}

bool createEchoPipeLine() {
  SampleFormat sampleFormat;
  memset(&sampleFormat, 0, sizeof(sampleFormat));
  sampleFormat.pcmFormat_ = (uint16_t)engine.bitsPerSample_;
  sampleFormat.framesPerBuf_ = engine.fastPathFramesPerBuf_;

  sampleFormat.channels_ = (uint16_t)engine.sampleChannels_;
  sampleFormat.sampleRate_ = engine.fastPathSampleRate_;

  engine.player_ = new AudioPlayer(&sampleFormat, engine.slEngineItf_);
  assert(engine.player_);
  if (engine.player_ == nullptr) return false;

  engine.player_->SetBufQueue(engine.recBufQueue_, engine.freeRecBufQueue_);
  engine.player_->RegisterCallback(EngineService, (void *)&engine);

  engine.recorder_ = new AudioRecorder(&sampleFormat, engine.slEngineItf_);
  if (!engine.recorder_) {
    return false;
  }
  engine.recorder_->SetBufQueues(engine.freeRecBufQueue_, engine.recBufQueue_);
  engine.recorder_->RegisterCallback(EngineService, (void *)&engine);
  return true;
}

void deleteAudioRecorder() {
  if (engine.recorder_) delete engine.recorder_;

  engine.recorder_ = nullptr;
}

void startPlay() {
  engine.frameCount_ = 0;
  /*
   * start player: make it into waitForData state
   */
  engine.recorder_->Start();
  if (SL_BOOLEAN_FALSE == engine.player_->Start()) {
    return;
  }
}

void stopPlay() {
  engine.recorder_->Stop();
  engine.player_->Stop();

  delete engine.recorder_;
  delete engine.player_;
  engine.recorder_ = NULL;
  engine.player_ = NULL;
}

void deleteSLEngine(JNIEnv *env, jclass type) {
  delete engine.recBufQueue_;
  delete engine.freeRecBufQueue_;
  delete engine.playBufQueue_;
  delete engine.freePlayBufQueue_;
  releaseSampleBufs(engine.bufs_, engine.bufCount_);
  releaseSampleBufs(engine.playBufs_, engine.bufCount_);
  if (engine.slEngineObj_ != NULL) {
    (*engine.slEngineObj_)->Destroy(engine.slEngineObj_);
    engine.slEngineObj_ = NULL;
    engine.slEngineItf_ = NULL;
  }

  if (engine.delayEffect_) {
    delete engine.delayEffect_;
    engine.delayEffect_ = nullptr;
  }
}

uint32_t dbgEngineGetBufCount(void) {
  uint32_t count = engine.player_->dbgGetDevBufCount();
  count += engine.recorder_->dbgGetDevBufCount();
  count += engine.freeRecBufQueue_->size();
  count += engine.recBufQueue_->size();

  LOGE(
      "Buf Disrtibutions: PlayerDev=%d, RecDev=%d, FreeQ=%d, "
      "RecQ=%d",
      engine.player_->dbgGetDevBufCount(),
      engine.recorder_->dbgGetDevBufCount(), engine.freeRecBufQueue_->size(),
      engine.recBufQueue_->size());
  if (count != engine.bufCount_) {
    LOGE("====Lost Bufs among the queue(supposed = %d, found = %d)", BUF_COUNT,
         count);
  }
  return count;
}

int pushed = 0;
int skipped = 0;

void DumpAudioFrame(void *data) {
  // TODO
}

std::mutex interchangeMutex_;
void SetRecordedFrame(void *data) {
  std::lock_guard<std::mutex> lock(interchangeMutex_);
  sample_buf *buf = static_cast<sample_buf *>(data);

  agora::media::IAudioFrameObserver::AudioFrame audio_frame;
  audio_frame.type = agora::media::IAudioFrameObserver::FRAME_TYPE_PCM16;
  audio_frame.samplesPerChannel = 480;
  audio_frame.bytesPerSample = agora::rtc::TWO_BYTES_PER_SAMPLE;
  audio_frame.channels = 2;
  audio_frame.samplesPerSec = 48000;
  audio_frame.renderTimeMs = 0;
  audio_frame.avsync_type = 0;
  audio_frame.buffer = buf->buf_;

  if (!engine.mediaEngine_) {
    memcpy(data_, buf->buf_, 1920);
    DumpAudioFrame(data_);
    return;
  }
  engine.mediaEngine_->pushCaptureAudioFrame(&audio_frame);
}

void GetAndProcessFrame(void *data) {
  std::lock_guard<std::mutex> lock(interchangeMutex_);
  sample_buf *buf = static_cast<sample_buf *>(data);
  agora::media::IAudioFrameObserver::AudioFrame audio_frame;
  audio_frame.type = agora::media::IAudioFrameObserver::FRAME_TYPE_PCM16;
  audio_frame.samplesPerChannel = 480;
  audio_frame.bytesPerSample = agora::rtc::TWO_BYTES_PER_SAMPLE;
  audio_frame.channels = 2;
  audio_frame.samplesPerSec = 48000;
  audio_frame.renderTimeMs = 0;
  audio_frame.avsync_type = 0;
  audio_frame.buffer = buf->buf_;
  LOGE("tshi audio_frame.buffer： %d", static_cast<char*>(audio_frame.buffer)[0]);

  if (!engine.mediaEngine_) {
    memcpy(buf->buf_, data_, 1920);
    DumpAudioFrame(data_);
    return;
  }
  engine.mediaEngine_->pullAudioFrame(&audio_frame);
  engine.mediaEngine_->pushReverseAudioFrame(&audio_frame);
}
/*
 * simple message passing for player/recorder to communicate with engine
 */
bool EngineService(void *ctx, uint32_t msg, void *data) {
  assert(ctx == &engine);
  switch (msg) {
    case ENGINE_SERVICE_MSG_RETRIEVE_DUMP_BUFS: {
      *(static_cast<uint32_t *>(data)) = dbgEngineGetBufCount();
      break;
    }
    case ENGINE_SERVICE_MSG_RECORDED_AUDIO_AVAILABLE: {
      // adding audio delay effect
      SetRecordedFrame(data);
      break;
    }
    case ENGINE_SERVICE_MSG_PLAYOUT_AUDIO_NEEDED: {
      GetAndProcessFrame(data);
      break;
    }
    default:
      assert(false);
  }

  return true;
}

long handle;

extern "C"
JNIEXPORT void JNICALL
Java_io_agora_rte_custom_1opensl_1chorus_OpenSlChorusPlayer_initOpenslDemo(JNIEnv *env, jobject thiz, jlong handle) {
  initAgoraEngine(handle);

//  fpcm = fopen(PCM_PATH, );
}

extern "C"
JNIEXPORT void JNICALL
Java_io_agora_rte_custom_1opensl_1chorus_OpenSlChorusPlayer_startOpenslDemo(JNIEnv *env, jobject thiz) {

  createSLEngine(
      48000,
      480,
      0,
      0);
  if(!createSLBufferQueueAudioPlayer()) {
    return;
  }
  if(!createAudioRecorder()) {
    deleteSLBufferQueueAudioPlayer();
    return;
  }
  startPlay();   // startPlay() triggers startRecording()
}

extern "C"
JNIEXPORT void JNICALL
Java_io_agora_rte_custom_1opensl_1chorus_OpenSlChorusPlayer_stopOpenslDemo(JNIEnv *env, jobject thiz) {
  stopPlay();  // stopPlay() triggers stopRecording()
  deleteAudioRecorder();
  deleteSLBufferQueueAudioPlayer();
}

extern "C"
JNIEXPORT void JNICALL Java_io_agora_rte_custom_1opensl_1chorus_OpenSlChorusPlayer_startEchoTest(JNIEnv *env, jobject thiz) {
  handle = -1;
  createSLEngine(
      48000,
      480,
      0,
      0);
  if(!createEchoPipeLine()) {
    return;
  }
  startPlay();   // startPlay() triggers startRecording()
}

extern "C"
JNIEXPORT void JNICALL Java_io_agora_rte_custom_1opensl_1chorus_OpenSlChorusPlayer_stopEchoTest(JNIEnv *env, jobject thiz) {
  stopPlay();  // stopPlay() triggers stopRecording()
  deleteAudioRecorder();
  deleteSLBufferQueueAudioPlayer();
}
