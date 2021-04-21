#include <jni.h>
#include <android/log.h>
#include <cstring>
#include "include/IAgoraMediaEngine.h"

#include "include/IAgoraRtcEngine.h"
#include <string.h>
#include "io_agora_advancedvideo_rawdata_MediaPreProcessing.h"
#include "include/VMUtil.h"

#include <map>

using namespace std;

jobject gCallBack = nullptr;
jclass gCallbackClass = nullptr;
jmethodID recordAudioMethodId = nullptr;
jmethodID playbackAudioMethodId = nullptr;
jmethodID playBeforeMixAudioMethodId = nullptr;
jmethodID mixAudioMethodId = nullptr;
jmethodID captureVideoMethodId = nullptr;
jmethodID preEncodeVideoMethodId = nullptr;
jmethodID renderVideoMethodId = nullptr;
void *_javaDirectPlayBufferCapture = nullptr;
void *_javaDirectPlayBufferRecordAudio = nullptr;
void *_javaDirectPlayBufferPlayAudio = nullptr;
void *_javaDirectPlayBufferBeforeMixAudio = nullptr;
void *_javaDirectPlayBufferMixAudio = nullptr;
map<int, void *> decodeBufferMap;
volatile bool mAvailable = false;

static JavaVM *gJVM = nullptr;

/**Listener to get video frame*/
class AgoraVideoFrameObserver : public agora::media::IVideoFrameObserver
{

public:
    AgoraVideoFrameObserver()
    {

    }

    ~AgoraVideoFrameObserver()
    {

    }

    void
    getVideoFrame(VideoFrame &videoFrame, _jmethodID *jmethodID, void *_byteBufferObject,
                  unsigned int uid)
    {
        if (_byteBufferObject == nullptr)
        {
            return;
        }

        int width = videoFrame.width;
        int height = videoFrame.height;
        size_t widthAndHeight = (size_t) videoFrame.yStride * height;
        size_t length = widthAndHeight * 3 / 2;

        AttachThreadScoped ats(gJVM);
        JNIEnv *env = ats.env();

        memcpy(_byteBufferObject, videoFrame.yBuffer, widthAndHeight);
        memcpy((uint8_t *) _byteBufferObject + widthAndHeight, videoFrame.uBuffer,
               widthAndHeight / 4);
        memcpy((uint8_t *) _byteBufferObject + widthAndHeight * 5 / 4, videoFrame.vBuffer,
               widthAndHeight / 4);


        if (!mAvailable)
        {
            // check gCallBack is available.
            return;
        }

        if (uid == 0)
        {
            env->CallVoidMethod(gCallBack, jmethodID, videoFrame.type, width, height, length,
                                videoFrame.yStride, videoFrame.uStride,
                                videoFrame.vStride, videoFrame.rotation,
                                videoFrame.renderTimeMs);
        } else
        {
            env->CallVoidMethod(gCallBack, jmethodID, uid, videoFrame.type, width, height,
                                length,
                                videoFrame.yStride, videoFrame.uStride,
                                videoFrame.vStride, videoFrame.rotation,
                                videoFrame.renderTimeMs);
        }
    }

    void writebackVideoFrame(VideoFrame &videoFrame, void *byteBuffer)
    {
        if (byteBuffer == nullptr)
        {
            return;
        }

        int width = videoFrame.width;
        int height = videoFrame.height;
        size_t widthAndHeight = (size_t) videoFrame.yStride * height;

        memcpy(videoFrame.yBuffer, byteBuffer, widthAndHeight);
        memcpy(videoFrame.uBuffer, (uint8_t *) byteBuffer + widthAndHeight, widthAndHeight / 4);
        memcpy(videoFrame.vBuffer, (uint8_t *) byteBuffer + widthAndHeight * 5 / 4,
               widthAndHeight / 4);
    }

public:
    /**Occurs each time the SDK receives a video frame captured by the local camera.
     * After you successfully register the video frame observer, the SDK triggers this callback each
     *  time a video frame is received. In this callback, you can get the video data captured by the
     *  local camera. You can then pre-process the data according to your scenarios.
     * After pre-processing, you can send the processed video data back to the SDK by setting the
     *  videoFrame parameter in this callback.
     * @param videoFrame
     * @return
     *   Whether or not to ignore the current video frame if the pre-processing fails:
     *     true: Do not ignore.
     *     false: Ignore the current video frame, and do not send it back to the SDK.
     * PS:
     *   This callback does not support sending processed RGBA video data back to the SDK.*/
    virtual bool onCaptureVideoFrame(VideoFrame &videoFrame) override
    {
        getVideoFrame(videoFrame, captureVideoMethodId, _javaDirectPlayBufferCapture, 0);
        __android_log_print(ANDROID_LOG_DEBUG, "AgoraVideoFrameObserver", "onCaptureVideoFrame");
        writebackVideoFrame(videoFrame, _javaDirectPlayBufferCapture);
        return true;
    }

    /**Occurs each time the SDK receives a video frame sent by the remote user.
     * After you successfully register the video frame observer and isMultipleChannelFrameWanted
     *  return false, the SDK triggers this callback each time a video frame is received. In this
     *  callback, you can get the video data sent by the remote user. You can then post-process the
     *  data according to your scenarios.
     * After post-processing, you can send the processed data back to the SDK by setting the videoFrame
     *  parameter in this callback.
     * @param uid ID of the remote user who sends the current video frame.
     * @param videoFrame
     * @return
     *   Whether or not to ignore the current video frame if the post-processing fails:
     *    true: Do not ignore.
     *    false: Ignore the current video frame, and do not send it back to the SDK.
     * PS:
     *   This callback does not support sending processed RGBA video data back to the SDK.*/
    virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame &videoFrame) override
    {
        __android_log_print(ANDROID_LOG_DEBUG, "AgoraVideoFrameObserver", "onRenderVideoFrame");
        map<int, void *>::iterator it_find;
        it_find = decodeBufferMap.find(uid);

        if (it_find != decodeBufferMap.end())
        {
            if (it_find->second != nullptr)
            {
                getVideoFrame(videoFrame, renderVideoMethodId, it_find->second, uid);
                writebackVideoFrame(videoFrame, it_find->second);
            }
        }
        return true;
    }


    /**Occurs each time the SDK receives a video frame before encoding.
     * @param videoFrame
     * @return
     *   Whether or not to ignore the current video frame if the pre-processing fails:
     *     true: Do not ignore.
     *     false: Ignore the current video frame, and do not send it back to the SDK.
     * PS:
     *   This callback does not support sending processed RGBA video data back to the SDK.*/
    virtual bool onPreEncodeVideoFrame(VideoFrame& videoFrame) override {
        getVideoFrame(videoFrame, preEncodeVideoMethodId, _javaDirectPlayBufferCapture, 0);
        __android_log_print(ANDROID_LOG_DEBUG, "AgoraVideoFrameObserver", "onPreEncodeVideoFrame");
        writebackVideoFrame(videoFrame, _javaDirectPlayBufferCapture);
        return true;
    }

    virtual VIDEO_FRAME_TYPE getVideoFormatPreference() override {
        return FRAME_TYPE_YUV420; // Please don't modify videoFormatPreference in this raw data processing plugin, otherwise it won't work.
    }
};

/**Listener to get audio frame*/
class AgoraAudioFrameObserver : public agora::media::IAudioFrameObserver
{

public:
    AgoraAudioFrameObserver()
    {
        gCallBack = nullptr;
    }

    ~AgoraAudioFrameObserver()
    {
    }

    void getAudioFrame(AudioFrame &audioFrame, _jmethodID *jmethodID, void *_byteBufferObject,
                       unsigned int uid)
    {
        if (_byteBufferObject == nullptr)
        {
            return;
        }

        AttachThreadScoped ats(gJVM);
        JNIEnv *env = ats.env();
        if (env == nullptr)
        {
            return;
        }
        int len = audioFrame.samples * audioFrame.bytesPerSample;
        memcpy(_byteBufferObject, audioFrame.buffer, (size_t) len); // * sizeof(int16_t)

        if (!mAvailable)
        {
            // check gCallBack is available.
            return;
        }

        if (uid == 0)
        {
            env->CallVoidMethod(gCallBack, jmethodID, audioFrame.type, audioFrame.samples,
                                audioFrame.bytesPerSample,
                                audioFrame.channels, audioFrame.samplesPerSec,
                                audioFrame.renderTimeMs, len);
        } else
        {
            env->CallVoidMethod(gCallBack, jmethodID, uid, audioFrame.type, audioFrame.samples,
                                audioFrame.bytesPerSample,
                                audioFrame.channels, audioFrame.samplesPerSec,
                                audioFrame.renderTimeMs, len);
        }
    }

    void writebackAudioFrame(AudioFrame &audioFrame, void *byteBuffer)
    {
        if (byteBuffer == nullptr)
        {
            return;
        }

        int len = audioFrame.samples * audioFrame.bytesPerSample;
        memcpy(audioFrame.buffer, byteBuffer, (size_t) len);
    }

public:
    /**Retrieves the recorded audio frame.
     * @param audioFrame
     * @return
     *   true: Valid buffer in AudioFrame, and the recorded audio frame is sent out.
     *   false: Invalid buffer in AudioFrame, and the recorded audio frame is discarded.*/
    virtual bool onRecordAudioFrame(AudioFrame &audioFrame) override
    {
        getAudioFrame(audioFrame, recordAudioMethodId, _javaDirectPlayBufferRecordAudio, 0);
        writebackAudioFrame(audioFrame, _javaDirectPlayBufferRecordAudio);
        return true;
    }

    /**Retrieves the audio playback frame for getting the audio.
     * @param audioFrame
     * @return
     *   true: Valid buffer in AudioFrame, and the audio playback frame is sent out.
         false: Invalid buffer in AudioFrame, and the audio playback frame is discarded.*/
    virtual bool onPlaybackAudioFrame(AudioFrame &audioFrame) override
    {
        getAudioFrame(audioFrame, playbackAudioMethodId, _javaDirectPlayBufferPlayAudio, 0);
        writebackAudioFrame(audioFrame, _javaDirectPlayBufferPlayAudio);
        return true;
    }

    /**Retrieves the audio frame of a specified user before mixing.
     * The SDK triggers this callback if isMultipleChannelFrameWanted returns false.
     * @param uid The user ID
     * @param audioFrame
     * @return
     *   true: Valid buffer in AudioFrame, and the mixed recorded and playback audio frame is sent out.
     *   false: Invalid buffer in AudioFrame, and the mixed recorded and playback audio frame is discarded.*/
    virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame &audioFrame) override
    {
        getAudioFrame(audioFrame, playBeforeMixAudioMethodId, _javaDirectPlayBufferBeforeMixAudio,
                      uid);
        writebackAudioFrame(audioFrame, _javaDirectPlayBufferBeforeMixAudio);
        return true;
    }

    /**Retrieves the mixed recorded and playback audio frame.
     * @param audioFrame
     * @return
     *   true: Valid buffer in AudioFrame and the mixed recorded and playback audio frame is sent out.
     *   false: Invalid buffer in AudioFrame and the mixed recorded and playback audio frame is discarded.*/
    virtual bool onMixedAudioFrame(AudioFrame &audioFrame) override
    {
        getAudioFrame(audioFrame, mixAudioMethodId, _javaDirectPlayBufferMixAudio, 0);
        writebackAudioFrame(audioFrame, _javaDirectPlayBufferMixAudio);
        return true;
    }


};


static AgoraAudioFrameObserver s_audioFrameObserver;

static AgoraVideoFrameObserver s_videoFrameObserver;
static agora::rtc::IRtcEngine *rtcEngine = nullptr;

#ifdef __cplusplus
extern "C" {
#endif

int __attribute__((visibility("default")))
loadAgoraRtcEnginePlugin(agora::rtc::IRtcEngine *engine)
{
    __android_log_print(ANDROID_LOG_DEBUG, "agora-raw-data-plugin", "loadAgoraRtcEnginePlugin");
    rtcEngine = engine;
    return 0;
}

void __attribute__((visibility("default")))
unloadAgoraRtcEnginePlugin(agora::rtc::IRtcEngine *engine)
{
    __android_log_print(ANDROID_LOG_DEBUG, "agora-raw-data-plugin", "unloadAgoraRtcEnginePlugin");

    rtcEngine = nullptr;
}

JNIEXPORT void JNICALL Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setCallback
        (JNIEnv *env, jclass, jobject callback)
{
    if (!rtcEngine) return;

    env->GetJavaVM(&gJVM);

    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtcEngine, agora::INTERFACE_ID_TYPE::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine)
    {
        /**Registers a video frame observer object.
         * You need to implement the IVideoFrameObserver class in this method, and register callbacks
         *  according to your scenarios.
         * After you successfully register the video frame observer, the SDK triggers the registered
         *  callbacks each time a video frame is received.
         * @param observer Video frame observer object instance. If NULL is passed in, the registration is canceled.
         * @return
         *   0: Success.
         *   < 0: Failure.
         * PS:
         *   When handling the video data returned in the callbacks, pay attention to the changes in
         *    the width and height parameters, which may be adapted under the following circumstances:
         *   When the network condition deteriorates, the video resolution decreases incrementally.
         *   If the user adjusts the video profile, the resolution of the video returned in the callbacks also changes.*/
        int code = mediaEngine->registerVideoFrameObserver(&s_videoFrameObserver);
        /**Registers an audio frame observer object.
         * This method is used to register an audio frame observer object (register a callback).
         * This method is required to register callbacks when the engine is required to provide an
         * onRecordAudioFrame or onPlaybackAudioFrame callback.
         * @param observer Audio frame observer object instance. If NULL is passed in, the registration is canceled.
         * @return
         *   0: Success.
         *   < 0: Failure.*/
        int ret = mediaEngine->registerAudioFrameObserver(&s_audioFrameObserver);
    }

    if (gCallBack == nullptr)
    {
        gCallBack = env->NewGlobalRef(callback);
        gCallbackClass = env->GetObjectClass(gCallBack);

        /**Get the MethodId of each callback function through the callback object.
         * Pass the data back to the java layer through these methods*/
        recordAudioMethodId = env->GetMethodID(gCallbackClass, "onRecordAudioFrame", "(IIIIIJI)V");
        playbackAudioMethodId = env->GetMethodID(gCallbackClass, "onPlaybackAudioFrame",
                                                 "(IIIIIJI)V");
        playBeforeMixAudioMethodId = env->GetMethodID(gCallbackClass,
                                                      "onPlaybackAudioFrameBeforeMixing",
                                                      "(IIIIIIJI)V");
        mixAudioMethodId = env->GetMethodID(gCallbackClass, "onMixedAudioFrame", "(IIIIIJI)V");

        captureVideoMethodId = env->GetMethodID(gCallbackClass, "onCaptureVideoFrame",
                                                "(IIIIIIIIJ)V");
        preEncodeVideoMethodId = env->GetMethodID(gCallbackClass, "onPreEncodeVideoFrame",
                                                  "(IIIIIIIIJ)V");
        renderVideoMethodId = env->GetMethodID(gCallbackClass, "onRenderVideoFrame",
                                               "(IIIIIIIIIJ)V");

        __android_log_print(ANDROID_LOG_DEBUG, "setCallback", "setCallback done successfully");
    }

    mAvailable = true;
}

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setVideoCaptureByteBuffer
        (JNIEnv *env, jclass, jobject bytebuffer)
{
    _javaDirectPlayBufferCapture = env->GetDirectBufferAddress(bytebuffer);
}

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setAudioRecordByteBuffer
        (JNIEnv *env, jclass, jobject bytebuffer)
{
    _javaDirectPlayBufferRecordAudio = env->GetDirectBufferAddress(bytebuffer);
}

JNIEXPORT void JNICALL Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setAudioPlayByteBuffer
        (JNIEnv *env, jclass, jobject bytebuffer)
{
    _javaDirectPlayBufferPlayAudio = env->GetDirectBufferAddress(bytebuffer);
}

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setBeforeAudioMixByteBuffer
        (JNIEnv *env, jclass, jobject bytebuffer)
{
    _javaDirectPlayBufferBeforeMixAudio = env->GetDirectBufferAddress(bytebuffer);
}

JNIEXPORT void JNICALL Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setAudioMixByteBuffer
        (JNIEnv *env, jclass, jobject bytebuffer)
{
    _javaDirectPlayBufferMixAudio = env->GetDirectBufferAddress(bytebuffer);
}

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setVideoDecodeByteBuffer
        (JNIEnv *env, jclass, jint uid, jobject byteBuffer)
{
    if (byteBuffer == nullptr)
    {
        decodeBufferMap.erase(uid);
    } else
    {
        void *_javaDirectDecodeBuffer = env->GetDirectBufferAddress(byteBuffer);
        decodeBufferMap.insert(make_pair(uid, _javaDirectDecodeBuffer));
        __android_log_print(ANDROID_LOG_DEBUG, "agora-raw-data-plugin",
                            "setVideoDecodeByteBuffer uid: %u, _javaDirectDecodeBuffer: %p",
                            uid, _javaDirectDecodeBuffer);
    }
}

JNIEXPORT void JNICALL Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_releasePoint
        (JNIEnv *env, jclass)
{
    mAvailable = false;

    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtcEngine, agora::INTERFACE_ID_TYPE::AGORA_IID_MEDIA_ENGINE);

    if (mediaEngine)
    {
        /**Release video and audio observers*/
        mediaEngine->registerVideoFrameObserver(NULL);
        mediaEngine->registerAudioFrameObserver(NULL);
    }

    if (gCallBack != nullptr)
    {
        env->DeleteGlobalRef(gCallBack);
        gCallBack = nullptr;
    }
    gCallbackClass = nullptr;

    recordAudioMethodId = nullptr;
    playbackAudioMethodId = nullptr;
    playBeforeMixAudioMethodId = nullptr;
    mixAudioMethodId = nullptr;
    captureVideoMethodId = nullptr;
    preEncodeVideoMethodId = nullptr;
    renderVideoMethodId = nullptr;

    _javaDirectPlayBufferCapture = nullptr;
    _javaDirectPlayBufferRecordAudio = nullptr;
    _javaDirectPlayBufferPlayAudio = nullptr;
    _javaDirectPlayBufferBeforeMixAudio = nullptr;
    _javaDirectPlayBufferMixAudio = nullptr;

    decodeBufferMap.clear();
}

#ifdef __cplusplus
}
#endif
