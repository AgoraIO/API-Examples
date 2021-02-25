//
//  AgoraRtcChannelPublishHelper.m
//  player_demo_apple
//
//  Created by zhanxiaochao on 2020/1/13.
//  Copyright © 2020 agora. All rights reserved.
//

#import "AgoraRtcChannelPublishHelper.h"
#import <AgoraRtcKit/IAgoraRtcEngine.h>
#import <AgoraRtcKit/IAgoraMediaEngine.h>
#import "AudioCircularBuffer.h"
#import "scoped_ptr.h"
#import <mutex>
using namespace AgoraRTC;
static NSObject *threadLockPush = [[NSObject alloc] init];
static NSObject *threadLockPlay = [[NSObject alloc] init];

class AgoraAudioFrameObserver:public agora::media::IAudioFrameObserver
{
private:
    int16_t * record_buf_tmp_ = nullptr;
    char *    record_audio_mix_ = nullptr;
    int16_t * record_send_buf_ = nullptr;
    
    int16_t * play_buf_tmp_ = nullptr;
    char  *   play_audio_mix_ = nullptr;
    int16_t * play_send_buf_ = nullptr;
    scoped_ptr<AudioCircularBuffer<char>> record_audio_buf_;
    scoped_ptr<AudioCircularBuffer<char>> play_audio_buf_;
public:
    std::atomic<float>  publishSignalValue_{1.0f};
    std::atomic<float>  playOutSignalValue_{1.0f};
    std::atomic<bool>   isOnlyAudioPlay_{false};
    AgoraAudioFrameObserver(){
        record_audio_buf_.reset(new AudioCircularBuffer<char>(true,2048));
        play_audio_buf_.reset(new AudioCircularBuffer<char>(true,2048));
    }
    ~AgoraAudioFrameObserver()
    {
        if (record_buf_tmp_) {
            free(record_buf_tmp_);
        }
        if(record_audio_mix_){
            free(record_audio_mix_);
        }
        if(record_send_buf_){
            free(record_send_buf_);
        }
        
        if (play_buf_tmp_) {
            free(play_buf_tmp_);
        }
        if(play_audio_mix_){
            free(play_audio_mix_);
        }
        if (play_send_buf_) {
            free(play_send_buf_);
        }
    }
    void resetAudioBuffer(){
        
        record_audio_buf_.reset(new AudioCircularBuffer<char>(2048,true));
        play_audio_buf_.reset(new AudioCircularBuffer<char>(2048,true));
    }
    void setPublishSignalVolume(int volume){
        @synchronized (threadLockPush) {
            publishSignalValue_ = volume/100.0f;
        }
    }
    void enableOnlyAudioPlay(bool isEnable){
        isOnlyAudioPlay_ = isEnable;
    }
    void setPlayoutSignalVolume(int volume){
         @synchronized (threadLockPlay) {
             playOutSignalValue_ = volume/100.0f;
         }
     }
    void pushData(char *data,int length){
        {
            if (!isOnlyAudioPlay_) {
                record_audio_buf_->Push(data, length);
            }
        }
        {
            play_audio_buf_->Push(data, length);
        }

    }
    virtual bool onRecordAudioFrame(AudioFrame& audioFrame){
        @synchronized (threadLockPush) {
            int bytes = audioFrame.samples * audioFrame.channels * audioFrame.bytesPerSample;
            int ret = record_audio_buf_->mAvailSamples - bytes;
            if ( ret < 0) {
                return false;
            }
            //计算重采样钱的数据大小 重采样的采样率 * SDK回调时间 * 声道数 * 字节数
            if (!record_buf_tmp_) {
                record_buf_tmp_ = (int16_t *)malloc(bytes);
            }
            if(!record_audio_mix_){
                record_audio_mix_ = (char *)malloc(bytes);
            }
            if(!record_send_buf_){
                record_send_buf_ = (int16_t *)malloc(bytes);
            }
            record_audio_buf_->Pop(record_audio_mix_, bytes);
            int16_t* p16 = (int16_t*) record_audio_mix_;
            memcpy(record_buf_tmp_, audioFrame.buffer, bytes);
            for (int i = 0; i < bytes / 2; ++i) {
                record_buf_tmp_[i] += (p16[i] * publishSignalValue_);
                //audio overflow
                if (record_buf_tmp_[i] > 32767) {
                    record_send_buf_[i] = 32767;
                }
                else if (record_buf_tmp_[i] < -32768) {
                    record_send_buf_[i] = -32768;
                }
                else {
                    record_send_buf_[i] = record_buf_tmp_[i];
                }
            }
            memcpy(audioFrame.buffer, record_send_buf_,bytes);
        }
        return true;
    }
    /**
     * Occurs when the playback audio frame is received.
     * @param audioframe The reference to the audio frame: AudioFrame.
     * @return
     * - true: The playback audio frame is valid and is encoded and sent.
     * - false: The playback audio frame is invalid and is not encoded or sent.
     */
    virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame){
    @synchronized (threadLockPlay) {
        
        int bytes = audioFrame.samples * audioFrame.channels * audioFrame.bytesPerSample;
        int ret = play_audio_buf_->mAvailSamples - bytes;
        if (ret < 0) {
            return false;
        }
        //计算重采样钱的数据大小 重采样的采样率 * SDK回调时间 * 声道数 * 字节数
        if(!play_buf_tmp_){
            play_buf_tmp_ = (int16_t *)malloc(bytes);
        }
        if(!play_audio_mix_){
            play_audio_mix_ = (char *)malloc(bytes);
        }
        if(!play_send_buf_){
            play_send_buf_ = (int16_t *)malloc(bytes);
        }
        play_audio_buf_->Pop(play_audio_mix_, bytes);
        int16_t* p16 = (int16_t*) play_audio_mix_;
        memcpy(play_buf_tmp_, audioFrame.buffer, bytes);
        for (int i = 0; i < bytes / 2; ++i) {
            play_buf_tmp_[i] += (p16[i] * playOutSignalValue_);
            //audio overflow
            if (play_buf_tmp_[i] > 32767) {
                play_send_buf_[i] = 32767;
            }
            else if (play_buf_tmp_[i] < -32768) {
                play_send_buf_[i] = -32768;
            }
            else {
                play_send_buf_[i] = play_buf_tmp_[i];
            }
        }
        memcpy(audioFrame.buffer, play_buf_tmp_,bytes);
    }
        return true;
    }
    /**
     * Occurs when the mixed audio data is received.
     * @param audioframe The reference to the audio frame: AudioFrame.
     * @return
     * - true: The mixed audio data is valid and is encoded and sent.
     * - false: The mixed audio data is invalid and is not encoded or sent.
     */
    virtual bool onMixedAudioFrame(AudioFrame& audioFrame){
        return false;
    }
    /**
     * Occurs when the playback audio frame before mixing is received.
     * @param audioframe The reference to the audio frame: AudioFrame.
     * @return
     * - true: The playback audio frame before mixing is valid and is encoded and sent.
     * - false: The playback audio frame before mixing is invalid and is not encoded or sent.
     */
    virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame){
        return false;
    }
};
@interface AgoraRtcChannelPublishHelper()<AgoraMediaPlayerDelegate,AgoraVideoSourceProtocol>
{
    std::unique_ptr<AgoraAudioFrameObserver> audioFrameObserver;
    BOOL isPublishVideo;
}
@property (nonatomic, weak)AgoraMediaPlayer *playerKit;
@property (nonatomic, weak)AgoraRtcEngineKit *rtcEngineKit;
@property (nonatomic, weak)id<AgoraRtcChannelPublishHelperDelegate> delegate;
@property (nonatomic, assign)bool isDispatchMainQueue;

@end
@implementation AgoraRtcChannelPublishHelper

static AgoraRtcChannelPublishHelper *instance = NULL;
+ (instancetype)shareInstance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        if (instance == NULL) {
            instance = [[AgoraRtcChannelPublishHelper alloc] init];
        }
    });
    return instance;
}
// 连接 MediaPlayer 到主版本 RTC SDK
- (void)attachPlayerToRtc:(AgoraMediaPlayer *)playerKit RtcEngine:(AgoraRtcEngineKit *)rtcEngine enableVideoSource:(bool)enable{
    audioFrameObserver = std::make_unique<AgoraAudioFrameObserver>();
    isPublishVideo = false;
    audioFrameObserver->setPublishSignalVolume(0);
    self.isDispatchMainQueue = false;
    playerKit.delegate = self;
    if (enable) {
        [rtcEngine setVideoSource:self];
    }
    [rtcEngine setParameters:@"{\"che.audio.keep.audiosession\":true}"];
    [rtcEngine setAudioProfile:AgoraAudioProfileMusicStandardStereo scenario:AgoraAudioScenarioChatRoomEntertainment];
    [rtcEngine setRecordingAudioFrameParametersWithSampleRate:48000 channel:2 mode:AgoraAudioRawFrameOperationModeReadWrite samplesPerCall:960];
    [rtcEngine setPlaybackAudioFrameParametersWithSampleRate:48000 channel:2 mode:AgoraAudioRawFrameOperationModeReadWrite samplesPerCall:960];

    [self registerRtcEngine:rtcEngine];
    _playerKit = playerKit;
    _rtcEngineKit = rtcEngine;
    [self resetAudioBuf];
}
// 启动/停止推送音频流到频道
- (void)publishAudio{
    @synchronized (self) {
        audioFrameObserver->setPublishSignalVolume(100);
    }
}
- (void)unpublishAudio{
    @synchronized (self) {
        audioFrameObserver->setPublishSignalVolume(0);
        [self resetAudioBuf];
    }

}
- (void)enableOnlyLocalAudioPlay:(bool)isEnable

{
    @synchronized (self) {
        audioFrameObserver->enableOnlyAudioPlay(isEnable);
    }
}
// 启动/停止推送视频流到频道
- (void)publishVideo{
    @synchronized (self) {
        isPublishVideo = true;
    }
}
- (void)unpublishVideo{
    
    @synchronized (self) {
        isPublishVideo = false;
    }
}
// 调节推送到频道内音频流的音量
- (void)adjustPublishSignalVolume:(int)volume{
    
    @synchronized (self) {
        audioFrameObserver->setPublishSignalVolume(volume);
    }
}
-(void)adjustPlayoutSignalVolume:(int)volume
{   @synchronized (self) {
         audioFrameObserver->setPlayoutSignalVolume(volume);
     }
}
// 断开 MediaPlayer 和 RTC SDK 的关联
- (void)detachPlayerFromRtc{
    @synchronized (self) {
        isPublishVideo=false;
        audioFrameObserver->setPublishSignalVolume(0);
        [self unregisterRtcEngine:_rtcEngineKit];
        [_rtcEngineKit setVideoSource:NULL];
        _playerKit.delegate = NULL;
        
    }
}
- (void)resetAudioBuf{
    @synchronized (self) {
        audioFrameObserver->resetAudioBuffer();
    }
}
- (void)AgoraMediaPlayer:(AgoraMediaPlayer *_Nonnull)playerKit
    didReceiveVideoFrame:(CVPixelBufferRef)pixelBuffer{
    @synchronized (self) {
        if (!isPublishVideo) {
            return;
        }
        //pushExternalCVPixelBuffer
        [self.consumer consumePixelBuffer:pixelBuffer withTimestamp:CMTimeMake(CACurrentMediaTime()*1000, 1000) rotation:AgoraVideoRotationNone];

    }

}
- (void)registerRtcEngine:(AgoraRtcEngineKit *)rtcEngine
{
    agora::rtc::IRtcEngine* rtc_engine = (agora::rtc::IRtcEngine*)rtcEngine.getNativeHandle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtc_engine, agora::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine) {
        mediaEngine->registerAudioFrameObserver(audioFrameObserver.get());
    }
}
- (void)unregisterRtcEngine:(AgoraRtcEngineKit *)rtcEngine
{
    agora::rtc::IRtcEngine* rtc_engine = (agora::rtc::IRtcEngine*)rtcEngine.getNativeHandle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtc_engine, agora::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine) {
        mediaEngine->registerAudioFrameObserver(NULL);
    }
}

- (void)AgoraMediaPlayer:(AgoraMediaPlayer *_Nonnull)playerKit
    didReceiveAudioFrame:(CMSampleBufferRef)audioFrame{
    //pushExternalAudioBuffer
    CMBlockBufferRef audioBuffer  = CMSampleBufferGetDataBuffer(audioFrame);
    OSStatus err;
    size_t lengthAtOffSet;
    size_t totalBytes;
    char *samples;
    err = CMBlockBufferGetDataPointer(audioBuffer, 0, &lengthAtOffSet, &totalBytes, &samples);
    if (totalBytes == 0) {
        return;
    }
    audioFrameObserver->pushData(samples, (int)totalBytes);

}
@synthesize consumer;

- (AgoraVideoBufferType)bufferType {
    return AgoraVideoBufferTypePixelBuffer;
}

- (void)shouldDispose {
    
}

- (BOOL)shouldInitialize {
    return true;
}

- (void)shouldStart {
    
}

- (void)shouldStop {
    
}

- (AgoraVideoCaptureType)captureType {
    return AgoraVideoCaptureTypeUnknown;
}


- (AgoraVideoContentHint)contentHint {
    return AgoraVideoContentHintNone;
}


/// Description of state of Mediaplayer's state
/// @param playerKit AgoraMediaPlayer
/// @param state AgoraMediaPlayerState
/// @param reason AgoraMediaPlayerStateReason
/// @param error AgoraMediaPlayerError
- (void)AgoraMediaPlayer:(AgoraMediaPlayer *_Nonnull)playerKit
       didChangedToState:(AgoraMediaPlayerState)state
                   error:(AgoraMediaPlayerError)error
{
    
    if (self.delegate && [self.delegate respondsToSelector:@selector(AgoraRtcChannelPublishHelperDelegate:didChangedToState:error:)]) {
        __weak typeof(self) weakSelf = self;
        [self executeBlock:^{
            if (state == AgoraMediaPlayerStateOpenCompleted) {
                [weakSelf.playerKit mute:true];
                [weakSelf resetAudioBuf];
            }
            [self.delegate AgoraRtcChannelPublishHelperDelegate:weakSelf.playerKit didChangedToState:state error:error];
        }];
    }
    
}

/// callback of position
/// @param playerKit AgoraMediaPlayer
/// @param position position
- (void)AgoraMediaPlayer:(AgoraMediaPlayer *_Nonnull)playerKit
    didChangedToPosition:(NSInteger)position
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(AgoraRtcChannelPublishHelperDelegate:didChangedToPosition:)]) {
        __weak typeof(self) weakSelf = self;
        [self executeBlock:^{
            [self.delegate AgoraRtcChannelPublishHelperDelegate:weakSelf.playerKit didChangedToPosition:position];
        }];
    }
}

/// callback of seek state
/// @param playerkit AgoraMediaPlayer
/// @param state Description of seek state
- (void)AgoraMediaPlayer:(AgoraMediaPlayer *)playerKit didOccurEvent:(AgoraMediaPlayerEvent)event
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(AgoraRtcChannelPublishHelperDelegate:didOccureEvent:)]) {
        __weak typeof(self) weakSelf = self;
        [self executeBlock:^{
            if (event == AgoraMediaPlayerEventSeekComplete) {
                [weakSelf resetAudioBuf];
            }
            [self.delegate AgoraRtcChannelPublishHelperDelegate:weakSelf.playerKit didOccureEvent:event];
        }];
    }
    
}

/// callback of SEI
/// @param playerkit AgoraMediaPlayer
/// @param data SEI's data
- (void)AgoraMediaPlayer:(AgoraMediaPlayer *)playerKit metaDataType:(AgoraMediaPlayerMetaDataType)type didReceiveData:(NSString *)data length:(NSInteger)length{
    if (self.delegate && [self.delegate respondsToSelector:@selector(AgoraRtcChannelPublishHelperDelegate:didReceiveData:length:)]) {
        __weak typeof(self) weakSelf = self;
        [self executeBlock:^{
            [self.delegate AgoraRtcChannelPublishHelperDelegate:weakSelf.playerKit didReceiveData:data length:length];
        }];
    }
    
}
- (void)registerRtcChannelPublishHelperDelegate:(id<AgoraRtcChannelPublishHelperDelegate>)delegate{
    @synchronized (self) {
        self.delegate = delegate;
    }
}
- (void)executeBlock:(void (^)())block {
  if (self.isDispatchMainQueue) {
    dispatch_async(dispatch_get_main_queue(), ^{
      block();
    });
  } else {
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
      block();
    });
  }
}

@end


