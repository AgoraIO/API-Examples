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
import io.agora.rtc.live.LiveInjectStreamConfig;

@Example(
        group = "Live BROADCASTING",
        name = "RTMP Injection",
        actionId = R.id.action_mainFragment_to_RTMPInjection
)
public class RTMPInjection extends JoinChannel {
    private EditText et_url;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_rtmp_injection, container, false);
        initView(view);
        return view;
    }

    @Override
    protected void initView(View view) {
        super.initView(view);
        et_url = view.findViewById(R.id.et_url);
        et_url.setText("rtmp://58.200.131.2:1935/livetv/hunantv");
        view.findViewById(R.id.btn_inject).setOnClickListener(this);
    }

    private void inject() {
        LiveInjectStreamConfig config = new LiveInjectStreamConfig();
        engine.addInjectStreamUrl(et_url.getText().toString(), config);
    }

    @Override
    public void onClick(View v) {
        super.onClick(v);
        if (v.getId() == R.id.btn_inject) {
            inject();
        }
    }
}
