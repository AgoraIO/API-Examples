package io.agora.api.example.compose.utils;

import android.media.AudioFormat;
import android.media.AudioTrack;
import android.util.Log;

/**
 * The type Audio player.
 */
public class AudioPlayer {

    private static final int DEFAULT_PLAY_MODE = AudioTrack.MODE_STREAM;
    private static final String TAG = "AudioPlayer";

    private AudioTrack mAudioTrack;
    private AudioStatus mAudioStatus = AudioStatus.STOPPED;

    private final int streamType;
    private final int format;
    private final int sampleRateInHz;
    private final int audioFormat;
    private final int mMinBufferSize;

    /**
     * Instantiates a new Audio player.
     *
     * @param streamType     the stream type
     * @param sampleRateInHz the sample rate in hz
     * @param channelConfig  the channel config
     * @param audioFormat    the audio format
     */
    public AudioPlayer(int streamType, int sampleRateInHz, int channelConfig, int audioFormat) {
        this.streamType = streamType;
        this.sampleRateInHz = sampleRateInHz;
        this.audioFormat = audioFormat;

        if (1 == channelConfig) {
            format = AudioFormat.CHANNEL_OUT_MONO;
        } else if (2 == channelConfig) {
            format = AudioFormat.CHANNEL_OUT_STEREO;
        } else {
            format = AudioFormat.CHANNEL_OUT_MONO;
        }

        mMinBufferSize = AudioTrack.getMinBufferSize(sampleRateInHz, format, audioFormat);
        Log.e(TAG, " sampleRateInHz :" + sampleRateInHz + " channelConfig :" + channelConfig + " audioFormat: " + audioFormat + " mMinBufferSize: " + mMinBufferSize);
        if (mMinBufferSize == AudioTrack.ERROR_BAD_VALUE) {
            Log.e(TAG, "AudioTrack.ERROR_BAD_VALUE : " + AudioTrack.ERROR_BAD_VALUE);
        }
    }

    /**
     * Start player boolean.
     *
     * @return the boolean
     */
    public boolean startPlayer() {
        if (mAudioStatus == AudioStatus.STOPPED) {

            mAudioTrack = new AudioTrack(streamType, sampleRateInHz, format, audioFormat, mMinBufferSize, DEFAULT_PLAY_MODE);
            if (mAudioTrack.getState() == AudioTrack.STATE_UNINITIALIZED) {
                throw new RuntimeException("Error on AudioTrack created");
            }

            mAudioTrack.play();
            mAudioStatus = AudioStatus.RUNNING;
        }
        Log.e("AudioPlayer", "mAudioStatus: " + mAudioStatus);
        return true;
    }

    /**
     * Stop player.
     */
    public void stopPlayer() {
        if (null != mAudioTrack) {
            mAudioStatus = AudioStatus.STOPPED;
            mAudioTrack.stop();
            mAudioTrack.release();
            mAudioTrack = null;
        }
        Log.e(TAG, "mAudioStatus: " + mAudioStatus);
    }

    /**
     * Play boolean.
     *
     * @param audioData     the audio data
     * @param offsetInBytes the offset in bytes
     * @param sizeInBytes   the size in bytes
     * @return the boolean
     */
    public boolean play(byte[] audioData, int offsetInBytes, int sizeInBytes) {
        if (mAudioStatus == AudioStatus.RUNNING) {
            mAudioTrack.write(audioData, offsetInBytes, sizeInBytes);
        } else {
            Log.e(TAG, "=== No data to AudioTrack !! mAudioStatus: " + mAudioStatus);
        }
        return true;
    }

    /**
     * The enum Audio status.
     */
    public enum AudioStatus {
        /**
         * Running audio status.
         */
        RUNNING,
        /**
         * Stopped audio status.
         */
        STOPPED
    }
}