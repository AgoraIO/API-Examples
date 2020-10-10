package io.agora.advancedvideo.rawdata;

import java.nio.ByteBuffer;

/**This class is mainly used to interact with the underlying C++*/
public class MediaPreProcessing {
    static {
        System.loadLibrary("apm-plugin-raw-data");
    }

    /**This callback interface is responsible for calling back the raw data from the underlying C++
     * to the data manager (ie MediaDataObserverPlugin)*/
    public interface ProgressCallback {
        /**Occurs each time the SDK receives a video frame captured by the local camera.
         * @param videoFrameType include FRAME_TYPE_YUV420、FRAME_TYPE_YUV422、FRAME_TYPE_RGBA
         * @param rotation the rotation of this frame before rendering the video. Supports 0, 90,
         *                 180, 270 degrees clockwise.
         * @param renderTimeMs The timestamp of the external audio frame. It is mandatory. You can
         *                     use this parameter for the following purposes:*/
        void onCaptureVideoFrame(int videoFrameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs);

        /**
         * Occurs each time the SDK receives a video frame before encoding.
         * @param videoFrameType include FRAME_TYPE_YUV420、FRAME_TYPE_YUV422、FRAME_TYPE_RGBA
         * @param rotation the rotation of this frame before rendering the video. Supports 0, 90,
         *                 180, 270 degrees clockwise.
         * @param renderTimeMs The timestamp of the external audio frame. It is mandatory. You can
         *                     use this parameter for the following purposes:*/
        void onPreEncodeVideoFrame(int videoFrameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs);

        /**Occurs each time the SDK receives a video frame captured by the local camera.
         * @param uid ID of the remote user who sends the current video frame.*/
        void onRenderVideoFrame(int uid, int videoFrameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs);

        /**Retrieves the recorded audio frame.
         * @param audioFrameType only support FRAME_TYPE_PCM16
         * @param samples The number of samples per channel in the audio frame.
         * @param bytesPerSample The number of bytes per audio sample, which is usually 16-bit (2-byte).
         * @param channels The number of audio channels.
         *                      1: Mono
         *                      2: Stereo (the data is interleaved)
         * @param samplesPerSec The sample rate.
         * @param renderTimeMs The timestamp of the external audio frame.
         * @param bufferLength audio frame size*/
        void onRecordAudioFrame(int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);

        /**Retrieves the audio playback frame for getting the audio.*/
        void onPlaybackAudioFrame(int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);

        /**Retrieves the audio frame of a specified user before mixing.*/
        void onPlaybackAudioFrameBeforeMixing(int uid, int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);

        /**Retrieves the mixed recorded and playback audio frame.*/
        void onMixedAudioFrame(int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);
    }

    public static native void setCallback(ProgressCallback callback);

    public static native void setVideoCaptureByteBuffer(ByteBuffer byteBuffer);

    public static native void setAudioRecordByteBuffer(ByteBuffer byteBuffer);

    public static native void setAudioPlayByteBuffer(ByteBuffer byteBuffer);

    public static native void setBeforeAudioMixByteBuffer(ByteBuffer byteBuffer);

    public static native void setAudioMixByteBuffer(ByteBuffer byteBuffer);

    public static native void setVideoDecodeByteBuffer(int uid, ByteBuffer byteBuffer);

    public static native void releasePoint();

}
