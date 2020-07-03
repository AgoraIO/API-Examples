package io.agora.api.example;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;

import androidx.annotation.Nullable;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import io.agora.api.component.Constant;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.model.ExampleBean;
import io.agora.api.example.examples.advanced.CustomRemoteVideoRender;
import io.agora.api.example.examples.advanced.ProcessRawData;
import io.agora.api.example.examples.advanced.PushExternalVideo;
import io.agora.api.example.examples.advanced.QuickSwitchChannel;
import io.agora.api.example.examples.advanced.RTMPInjection;
import io.agora.api.example.examples.advanced.RTMPStreaming;
import io.agora.api.example.examples.advanced.StreamEncrypt;
import io.agora.api.example.examples.advanced.SwitchExternalVideo;
import io.agora.api.example.examples.advanced.VideoMetadata;
import io.agora.api.example.examples.advanced.customaudio.CustomAudioRecord;
import io.agora.api.example.examples.basic.JoinChannelAudio;
import io.agora.api.example.examples.basic.JoinChannelVideo;

/**
 * @author cjw
 */
public class ExampleActivity extends AppCompatActivity {
    private static final String TAG = "ExampleActivity";

    private ExampleBean exampleBean;

    public static void instance(Activity activity, ExampleBean exampleBean)
    {
        Intent intent = new Intent(activity, ExampleActivity.class);
        intent.putExtra(Constant.DATA, exampleBean);
        activity.startActivity(intent);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_example_layout);
        ActionBar actionBar = getSupportActionBar();
        if(actionBar != null){
            actionBar.setHomeButtonEnabled(true);
            actionBar.setDisplayHomeAsUpEnabled(true);
        }
        exampleBean = getIntent().getParcelableExtra(Constant.DATA);
        Fragment fragment;
        switch (exampleBean.getActionId())
        {
            case R.id.action_mainFragment_to_joinChannelAudio:
                 fragment = new JoinChannelAudio();
                break;
            case R.id.action_mainFragment_to_joinChannelVideo:
                fragment = new JoinChannelVideo();
                break;
            case R.id.action_mainFragment_to_CustomAudioRecord:
                fragment = new CustomAudioRecord();
                break;
            case R.id.action_mainFragment_to_CustomRemoteRender:
                fragment = new CustomRemoteVideoRender();
                break;
            case R.id.action_mainFragment_to_ProcessRawData:
                fragment = new ProcessRawData();
                break;
            case R.id.action_mainFragment_to_PushExternalVideo:
                fragment = new PushExternalVideo();
                break;
            case R.id.action_mainFragment_to_QuickSwitch:
                fragment = new QuickSwitchChannel();
                break;
            case R.id.action_mainFragment_to_RTMPInjection:
                fragment = new RTMPInjection();
                break;
            case R.id.action_mainFragment_to_RTMPStreaming:
                fragment = new RTMPStreaming();
                break;
            case R.id.action_mainFragment_to_StreamEncrypt:
                fragment = new StreamEncrypt();
                break;
            case R.id.action_mainFragment_to_SwitchExternalVideo:
                fragment = new SwitchExternalVideo();
                break;
            case R.id.action_mainFragment_to_VideoMetadata:
                fragment = new VideoMetadata();
                break;
            default:
                fragment = new JoinChannelAudio();
                break;
        }
        getSupportFragmentManager().beginTransaction()
                .replace(R.id.fragment_Layout, fragment)
                .commit();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            this.finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
