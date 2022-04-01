package io.agora.api.example;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import io.agora.api.example.common.Constant;
import io.agora.api.example.common.model.ExampleBean;
import io.agora.api.example.examples.advanced.AdjustVolume;
import io.agora.api.example.examples.advanced.CDNStreaming.EntryFragment;
import io.agora.api.example.examples.advanced.ChannelEncryption;
import io.agora.api.example.examples.advanced.CustomRemoteVideoRender;
import io.agora.api.example.examples.advanced.HostAcrossChannel;
import io.agora.api.example.examples.advanced.InCallReport;
import io.agora.api.example.examples.advanced.JoinMultipleChannel;
import io.agora.api.example.examples.advanced.LiveStreaming;
import io.agora.api.example.examples.advanced.MediaPlayer;
import io.agora.api.example.examples.advanced.PlayAudioFiles;
import io.agora.api.example.examples.advanced.PreCallTest;
import io.agora.api.example.examples.advanced.ProcessAudioRawData;
import io.agora.api.example.examples.advanced.ProcessRawData;
import io.agora.api.example.examples.advanced.PushExternalVideo;
import io.agora.api.example.examples.advanced.RhythmPlayer;
import io.agora.api.example.examples.advanced.SendDataStream;
import io.agora.api.example.examples.advanced.SetAudioProfile;
import io.agora.api.example.examples.advanced.SimpleExtension;
import io.agora.api.example.examples.advanced.VideoProcessExtension;
import io.agora.api.example.examples.advanced.SpatialSound;
import io.agora.api.example.examples.advanced.VideoQuickSwitch;
import io.agora.api.example.examples.advanced.RTMPStreaming;
import io.agora.api.example.examples.advanced.SwitchCameraScreenShare;
import io.agora.api.example.examples.advanced.VideoMetadata;
import io.agora.api.example.examples.advanced.VoiceEffects;
import io.agora.api.example.examples.advanced.customaudio.CustomAudioSource;
import io.agora.api.example.examples.basic.JoinChannelAudio;
import io.agora.api.example.examples.basic.JoinChannelVideo;
import io.agora.rtc2.RtcEngine;

/**
 * @author cjw
 */
public class ExampleActivity extends AppCompatActivity {
    private static final String TAG = "ExampleActivity";

    private ExampleBean exampleBean;

    private TextView version;

    public static void instance(Activity activity, ExampleBean exampleBean) {
        Intent intent = new Intent(activity, ExampleActivity.class);
        intent.putExtra(Constant.DATA, exampleBean);
        activity.startActivity(intent);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_example_layout);
        exampleBean = getIntent().getParcelableExtra(Constant.DATA);
        version = findViewById(R.id.versionNumber);
        version.setText(String.format(getString(R.string.sdkversion1), RtcEngine.getSdkVersion()));
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setTitle(exampleBean.getName());
            actionBar.setHomeButtonEnabled(true);
            actionBar.setDisplayHomeAsUpEnabled(true);
        }

        Fragment fragment;
        switch (exampleBean.getActionId()) {
            case R.id.action_mainFragment_to_joinChannelAudio:
                fragment = new JoinChannelAudio();
                break;
            case R.id.action_mainFragment_to_joinChannelVideo:
                fragment = new JoinChannelVideo();
                break;
            case R.id.action_mainFragment_to_CustomAudioSource:
                fragment = new CustomAudioSource();
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
                fragment = new VideoQuickSwitch();
                break;
            case R.id.action_mainFragment_to_MultiChannel:
                fragment = new JoinMultipleChannel();
                break;
            case R.id.action_mainFragment_to_SetAudioProfile:
                fragment = new SetAudioProfile();
                break;
            case R.id.action_mainFragment_to_PlayAudioFiles:
                fragment = new PlayAudioFiles();
                break;
            case R.id.action_mainFragment_to_VoiceEffects:
                fragment = new VoiceEffects();
                break;
            case R.id.action_mainFragment_to_MediaPlayer:
                fragment = new MediaPlayer();
                break;
            case R.id.action_mainFragment_to_RTMPStreaming:
                fragment = new EntryFragment();
                break;
            case R.id.action_mainFragment_to_RTCToRTMP:
                fragment = new RTMPStreaming();
                break;
            case R.id.action_mainFragment_to_SwitchCameraScreenShare:
                fragment = new SwitchCameraScreenShare();
                break;
            case R.id.action_mainFragment_to_VideoMetadata:
                fragment = new VideoMetadata();
                break;
            case R.id.action_mainFragment_to_InCallReport:
                fragment = new InCallReport();
                break;
            case R.id.action_mainFragment_to_AdjustVolume:
                fragment = new AdjustVolume();
                break;
            case R.id.action_mainFragment_to_PreCallTest:
                fragment = new PreCallTest();
                break;
            case R.id.action_mainFragment_to_hostacrosschannel:
                fragment = new HostAcrossChannel();
                break;
            case R.id.action_mainFragment_to_channel_encryption:
                fragment = new ChannelEncryption();
                break;
            case R.id.action_mainFragment_to_live_streaming:
                fragment = new LiveStreaming();
                break;
            case R.id.action_mainFragment_senddatastream:
                fragment = new SendDataStream();
                break;
            case R.id.action_mainFragment_raw_audio:
                fragment = new ProcessAudioRawData();
                break;
            case R.id.action_mainFragment_extension:
                fragment = new SimpleExtension();
                break;
            case R.id.action_mainFragment_video_enhancement:
                fragment = new VideoProcessExtension();
                break;
            case R.id.action_mainFragment_rhythm_player:
                fragment = new RhythmPlayer();
                break;
            case R.id.action_mainFragment_to_spatial_sound:
                fragment = new SpatialSound();
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
