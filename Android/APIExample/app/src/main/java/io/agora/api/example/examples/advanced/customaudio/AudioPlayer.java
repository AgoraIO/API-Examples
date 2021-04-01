package io.agora.api.example.examples.advanced.customaudio;

import android.media.AudioFormat;
import android.media.AudioTrack;
import android.util.Log;

public class AudioPlayer {

    private static final int DEFAULT_PLAY_MODE = AudioTrack.MODE_STREAM;
    private static final String  TAG = "AudioPlayer";

    private AudioTrack mAudioTrack;
    private AudioStatus mAudioStatus = AudioStatus.STOPPED ;

    public  AudioPlayer(int streamType, int sampleRateInHz, int channelConfig, int audioFormat){
        if(mAudioStatus == AudioStatus.STOPPED) {
            int Val = 0;
            if(1 == channelConfig)
                Val = AudioFormat.CHANNEL_OUT_MONO;
            else if(2 == channelConfig)
                Val = AudioFormat.CHANNEL_OUT_STEREO;
            else
                Log.e(TAG,  "channelConfig is wrong !");

            int mMinBufferSize = AudioTrack.getMinBufferSize(sampleRateInHz, Val, audioFormat);
            Log.i(TAG, " sampleRateInHz :" + sampleRateInHz + " channelConfig :" + channelConfig + " audioFormat: " + audioFormat + " mMinBufferSize: " + mMinBufferSize);
            if (mMinBufferSize == AudioTrack.ERROR_BAD_VALUE) {
                Log.e(TAG,"AudioTrack.ERROR_BAD_VALUE : " + AudioTrack.ERROR_BAD_VALUE) ;
            }

            mAudioTrack = new AudioTrack(streamType, sampleRateInHz, Val, audioFormat, mMinBufferSize, DEFAULT_PLAY_MODE);
            if (mAudioTrack.getState() == AudioTrack.STATE_UNINITIALIZED) {
                throw new RuntimeException("Error on AudioTrack created");
            }
            mAudioStatus = AudioStatus.INITIALISING;
        }
        Log.e(TAG, "mAudioStatus: " + mAudioStatus);
    }

    public boolean startPlayer() {
        if(mAudioStatus == AudioStatus.INITIALISING) {
            mAudioTrack.play();
            mAudioStatus = AudioStatus.RUNNING;
        }
        Log.e("AudioPlayer", "mAudioStatus: " + mAudioStatus);
        return true;
    }

    public void stopPlayer() {
        if(null != mAudioTrack){
            mAudioStatus = AudioStatus.STOPPED;
            mAudioTrack.stop();
            mAudioTrack.release();
            mAudioTrack = null;
        }
        Log.e(TAG, "mAudioStatus: " + mAudioStatus);
    }

    public boolean play(byte[] audioData, int offsetInBytes, int sizeInBytes) {
        if(mAudioStatus == AudioStatus.RUNNING) {
            mAudioTrack.write(audioData, offsetInBytes, sizeInBytes);
        }else{
            Log.e(TAG, "=== No data to AudioTrack !! mAudioStatus: " + mAudioStatus);
        }
        return true;
    }
}