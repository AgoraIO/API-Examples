package io.agora.api.example.examples.live_broadcasting;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.examples.basic_video_audio.JoinChannel;
import io.agora.rtc.live.LiveTranscoding;

@Example(
        group = "Live BROADCASTING",
        name = "RTMP Streaming",
        actionId = R.id.action_mainFragment_to_RTMPPublishing
)
public class RTMPPublishing extends JoinChannel {
    private EditText et_url;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_rtmp_publishing, container, false);
        initView(view);
        return view;
    }

    @Override
    protected void initView(View view) {
        super.initView(view);
        et_url = view.findViewById(R.id.et_url);
        view.findViewById(R.id.btn_publish).setOnClickListener(this);
    }

    private void publish() {
        LiveTranscoding transcoding = new LiveTranscoding();
        LiveTranscoding.TranscodingUser transcodingUser = new LiveTranscoding.TranscodingUser();
        transcodingUser.width = transcoding.width;
        transcodingUser.height = transcoding.height;
        transcodingUser.uid = myUid;
        transcoding.addUser(transcodingUser);
        engine.setLiveTranscoding(transcoding);
        engine.addPublishStreamUrl(et_url.getText().toString(), true);
    }

    @Override
    public void onClick(View v) {
        super.onClick(v);
        if (v.getId() == R.id.btn_publish) {
            publish();
        }
    }
}
