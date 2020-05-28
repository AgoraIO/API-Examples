package io.agora.api.example.examples.basic_video_audio;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.rtc.Constants;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;

@Example(
        group = "BASIC VIDEO/AUDIO",
        name = "AudioOnly",
        actionId = R.id.action_mainFragment_to_audioOnly
)
public class AudioOnly extends JoinChannel implements View.OnClickListener
{
    private Button mute, speaker;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState)
    {
        View view = inflater.inflate(R.layout.fragment_audio_only, container, false);
        initView(view);
        return view;
    }

    @Override
    protected void initView(View view)
    {
        super.initView(view);
        mute = view.findViewById(R.id.btn_mute);
        mute.setOnClickListener(this);
        speaker = view.findViewById(R.id.btn_speaker);
        speaker.setOnClickListener(this);
    }

    @Override
    public void onClick(View v)
    {
        super.onClick(v);
        if(v.getId() == R.id.btn_mute)
        {
            mute.setActivated(!mute.isActivated());
            mute.setText(getString(mute.isActivated() ? R.string.openmicrophone : R.string.closemicrophone));
            // Turn off / on the microphone, stop / start local audio collection and push streaming.
            engine.muteLocalAudioStream(mute.isActivated());
        }
        else if(v.getId() == R.id.btn_speaker)
        {
            speaker.setActivated(!speaker.isActivated());
            speaker.setText(getString(speaker.isActivated() ? R.string.earpiece : R.string.speaker));
            // Turn off / on the speaker and change the audio playback route.
            engine.setEnableSpeakerphone(speaker.isActivated());
        }
    }
}
