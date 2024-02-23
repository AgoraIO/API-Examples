package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.mediaplayer.Constants.MediaPlayerState.PLAYER_STATE_OPEN_COMPLETED;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.google.android.material.bottomsheet.BottomSheetDialog;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.Constant;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.TokenUtils;
import io.agora.mediaplayer.Constants;
import io.agora.mediaplayer.IMediaPlayer;
import io.agora.mediaplayer.IMediaPlayerObserver;
import io.agora.mediaplayer.data.CacheStatistics;
import io.agora.mediaplayer.data.PlayerPlaybackStats;
import io.agora.mediaplayer.data.PlayerUpdatedInfo;
import io.agora.mediaplayer.data.SrcInfo;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.RtcEngineEx;
import io.agora.rtc2.SpatialAudioParams;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.spatialaudio.ILocalSpatialAudioEngine;
import io.agora.spatialaudio.LocalSpatialAudioConfig;
import io.agora.spatialaudio.RemoteVoicePositionInfo;
import io.agora.spatialaudio.SpatialAudioZone;

/**
 * The type Spatial sound.
 */
@Example(
        index = 22,
        group = ADVANCED,
        name = R.string.item_spatial_sound,
        actionId = R.id.action_mainFragment_to_spatial_sound,
        tipsId = R.string.spatial_sound
)
public class SpatialSound extends BaseFragment {
    private static final String TAG = SpatialSound.class.getSimpleName();

    private static final int AXIS_MAX_DISTANCE = 10;

    private View rootView;
    private ImageView localIv, mediaPlayerLeftIv, mediaPlayerRightIv;
    private TextView tipTv, remoteLeftTv, remoteRightTv, zoneTv;
    private Button joinBtn;
    private EditText channelIdEt;
    private Switch switchMic, switchZone;


    private RtcEngineEx engine;
    private IMediaPlayer mediaPlayerLeft, mediaPlayerRight;
    private volatile boolean isJoined;
    private ILocalSpatialAudioEngine localSpatial;
    private final InnerRtcEngineEventHandler iRtcEngineEventHandler = new InnerRtcEngineEventHandler();
    private final Map<String, BottomSheetDialog> cacheDialogs = new HashMap<>();

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_spatial_sound, container, false);
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }
        try {
            /*Creates an RtcEngine instance.
             * @param context The context of Android Activity
             * @param appId The App ID issued to you by Agora. See <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id">
             *              How to get the App ID</a>
             * @param handler IRtcEngineEventHandler is an abstract class providing default implementation.
             *                The SDK uses this class to report to the app on SDK runtime events.*/
            String appId = getString(R.string.agora_app_id);
            RtcEngineConfig config = new RtcEngineConfig();
            config.mContext = getContext().getApplicationContext();
            config.mAppId = appId;
            config.mEventHandler = iRtcEngineEventHandler;
            config.mAreaCode = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getAreaCode();
            engine = (RtcEngineEx) RtcEngine.create(config);
            /*
             * This parameter is for reporting the usages of APIExample to agora background.
             * Generally, it is not necessary for you to set this parameter.
             */
            engine.setParameters("{"
                    + "\"rtc.report_app_scenario\":"
                    + "{"
                    + "\"appScenario\":" + 100 + ","
                    + "\"serviceType\":" + 11 + ","
                    + "\"appVersion\":\"" + RtcEngine.getSdkVersion() + "\""
                    + "}"
                    + "}");
            /* setting the local access point if the private cloud ip was set, otherwise the config will be invalid.*/
            LocalAccessPointConfiguration localAccessPointConfiguration = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getPrivateCloudConfig();
            if (localAccessPointConfiguration != null) {
                // This api can only be used in the private media server scenario, otherwise some problems may occur.
                engine.setLocalAccessPoint(localAccessPointConfiguration);
            }
            engine.enableAudio();

            localSpatial = ILocalSpatialAudioEngine.create();
            LocalSpatialAudioConfig localSpatialAudioConfig = new LocalSpatialAudioConfig();
            localSpatialAudioConfig.mRtcEngine = engine;
            localSpatial.initialize(localSpatialAudioConfig);

            localSpatial.setMaxAudioRecvCount(2);
            localSpatial.setAudioRecvRange(AXIS_MAX_DISTANCE);
            localSpatial.setDistanceUnit(1);

            engine.setChannelProfile(io.agora.rtc2.Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);

        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        rootView = view.findViewById(R.id.root_view);
        channelIdEt = view.findViewById(R.id.et_channel);
        joinBtn = view.findViewById(R.id.btn_join);
        tipTv = view.findViewById(R.id.tv_tip);
        mediaPlayerLeftIv = view.findViewById(R.id.iv_mediaplayer_left);
        mediaPlayerRightIv = view.findViewById(R.id.iv_mediaplayer_right);
        localIv = view.findViewById(R.id.iv_local);
        remoteLeftTv = view.findViewById(R.id.iv_remote_left);
        remoteRightTv = view.findViewById(R.id.iv_remote_right);
        tipTv.setText(R.string.spatial_sound_tip);
        switchMic = view.findViewById(R.id.switch_microphone);
        switchZone = view.findViewById(R.id.switch_zone);
        zoneTv = view.findViewById(R.id.tv_zone);
        zoneTv.setVisibility(View.INVISIBLE);

        joinBtn.setOnClickListener(v -> {
            CommonUtil.hideInputBoard(requireActivity(), channelIdEt);
            if (!isJoined) {
                joinChannel();
            } else {
                leftChannel();
            }
        });
        localIv.setOnTouchListener(new ListenerOnTouchListener() {
            @Override
            protected void onPositionChanged() {
                float[] pos = getVoicePosition(localIv);
                float[] forward = new float[]{1.0F, 0.0F, 0.0F};
                float[] right = new float[]{0.0F, 1.0F, 0.0F};
                float[] up = new float[]{0.0F, 0.0F, 1.0F};
                Log.d(TAG, "updateSelfPosition >> pos=" + Arrays.toString(pos));
                localSpatial.updateSelfPosition(pos, forward, right, up);
            }
        });
        switchMic.setOnCheckedChangeListener((buttonView, isChecked) ->
                localSpatial.muteLocalAudioStream(!isChecked));
        switchZone.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                zoneTv.setVisibility(View.VISIBLE);

                // create room
                SpatialAudioZone mediaPlayerLeftZone = new SpatialAudioZone();
                mediaPlayerLeftZone.zoneSetId = 1;
                mediaPlayerLeftZone.audioAttenuation = 1f;
                float[] voicePosition = getVoicePosition(zoneTv);
                float[] viewRelativeSizeInAxis = getViewRelativeSizeInAxis(zoneTv);
                mediaPlayerLeftZone.position = new float[]{voicePosition[0], voicePosition[1], 0};
                mediaPlayerLeftZone.forward = new float[]{1.f, 0, 0};
                mediaPlayerLeftZone.right = new float[]{0, 1.f, 0};
                mediaPlayerLeftZone.up = new float[]{0, 0, 1.f};
                mediaPlayerLeftZone.forwardLength = viewRelativeSizeInAxis[1];
                mediaPlayerLeftZone.rightLength = viewRelativeSizeInAxis[0];
                mediaPlayerLeftZone.upLength = AXIS_MAX_DISTANCE;
                localSpatial.setZones(new SpatialAudioZone[]{mediaPlayerLeftZone});
            } else {
                zoneTv.setVisibility(View.INVISIBLE);
                localSpatial.setZones(null);
            }
        });
    }

    private void joinChannel() {
        String channelId = channelIdEt.getText().toString();

        engine.setDefaultAudioRoutetoSpeakerphone(true);

        engine.setClientRole(io.agora.rtc2.Constants.CLIENT_ROLE_BROADCASTER);

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, ret -> {

            ChannelMediaOptions option = new ChannelMediaOptions();
            option.autoSubscribeAudio = true;

            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/
            int res = engine.joinChannel(ret, channelId, 0, option);
            if (res != 0) {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
                Log.e(TAG, RtcEngine.getErrorDescription(Math.abs(res)));
                return;
            }
            // Prevent repeated entry
            joinBtn.setEnabled(false);
        });

        localSpatial.muteLocalAudioStream(!switchMic.isChecked());

        float[] pos = getVoicePosition(localIv);
        float[] forward = new float[]{1.0F, 0.0F, 0.0F};
        float[] right = new float[]{0.0F, 1.0F, 0.0F};
        float[] up = new float[]{0.0F, 0.0F, 1.0F};
        localSpatial.updateSelfPosition(pos, forward, right, up);
    }

    private void leftChannel() {
        isJoined = false;

        engine.leaveChannel();
        localSpatial.clearRemotePositions();

        isJoined = false;
        joinBtn.setText(R.string.join);

        mediaPlayerLeftIv.setVisibility(View.GONE);
        mediaPlayerRightIv.setVisibility(View.GONE);
        localIv.setTranslationX(0);
        localIv.setTranslationY(0);
        localIv.setVisibility(View.GONE);
        remoteLeftTv.setTag(null);
        remoteLeftTv.setVisibility(View.GONE);
        remoteRightTv.setTag(null);
        remoteRightTv.setVisibility(View.GONE);
        tipTv.setVisibility(View.GONE);
        zoneTv.setVisibility(View.GONE);
        switchZone.setVisibility(View.GONE);
        switchZone.setChecked(false);

        cacheDialogs.clear();

        unInitMediaPlayers();
    }


    private void initMediaPlayers() {
        mediaPlayerLeft = createLoopMediaPlayer();
        mediaPlayerLeft.open(Constant.URL_PLAY_AUDIO_FILES, 0);
        localSpatial.updatePlayerPositionInfo(mediaPlayerLeft.getMediaPlayerId(), getVoicePositionInfo(mediaPlayerLeftIv));

        mediaPlayerRight = createLoopMediaPlayer();
        mediaPlayerRight.open(Constant.URL_DOWNBEAT, 0);
        localSpatial.updatePlayerPositionInfo(mediaPlayerRight.getMediaPlayerId(), getVoicePositionInfo(mediaPlayerRightIv));

        mediaPlayerLeftIv.setOnClickListener(v -> showMediaPlayerSettingDialog(mediaPlayerLeft));
        mediaPlayerRightIv.setOnClickListener(v -> showMediaPlayerSettingDialog(mediaPlayerRight));
    }

    private void showMediaPlayerSettingDialog(IMediaPlayer mediaPlayer) {
        String key = "MediaPlayer_" + mediaPlayer.getMediaPlayerId();
        BottomSheetDialog dialog = cacheDialogs.get(key);
        if (dialog != null) {
            dialog.show();
            return;
        }
        boolean isPlaying = mediaPlayer.getState() == Constants.MediaPlayerState.PLAYER_STATE_PAUSED;
        SpatialAudioParams spatialAudioParams = new SpatialAudioParams();
        dialog = showCommonSettingDialog(
                isPlaying,
                spatialAudioParams,
                (buttonView, isChecked) -> {
                    if (isChecked) {
                        mediaPlayer.pause();
                    } else {
                        mediaPlayer.resume();
                    }
                },
                (buttonView, isChecked) -> {
                    spatialAudioParams.enable_blur = isChecked;
                    mediaPlayer.setSpatialAudioParams(spatialAudioParams);
                },
                (buttonView, isChecked) -> {
                    spatialAudioParams.enable_air_absorb = isChecked;
                    mediaPlayer.setSpatialAudioParams(spatialAudioParams);
                },
                new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                        localSpatial.setPlayerAttenuation(mediaPlayer.getMediaPlayerId(), (double) (progress * 1.0f / seekBar.getMax()), false);
                    }

                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {

                    }

                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {

                    }
                }
        );
        cacheDialogs.put(key, dialog);
    }

    private void showRemoteUserSettingDialog(int uid) {
        String key = "RemoteUser_" + uid;
        BottomSheetDialog dialog = cacheDialogs.get(key);
        if (dialog != null) {
            dialog.show();
            return;
        }
        SpatialAudioParams spatialAudioParams = new SpatialAudioParams();
        dialog = showCommonSettingDialog(
                false,
                spatialAudioParams,
                (buttonView, isChecked) -> {
                    localSpatial.muteRemoteAudioStream(uid, isChecked);
                },
                (buttonView, isChecked) -> {
                    spatialAudioParams.enable_blur = isChecked;
                    engine.setRemoteUserSpatialAudioParams(uid, spatialAudioParams);
                },
                (buttonView, isChecked) -> {
                    spatialAudioParams.enable_air_absorb = isChecked;
                    engine.setRemoteUserSpatialAudioParams(uid, spatialAudioParams);
                },
                new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                        localSpatial.setRemoteAudioAttenuation(uid, (double) (progress * 1.0f / seekBar.getMax()), false);
                    }

                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {

                    }

                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {

                    }
                }
        );
        cacheDialogs.put(key, dialog);
    }


    private void unInitMediaPlayers() {
        mediaPlayerLeft.destroy();
        mediaPlayerLeft = null;

        mediaPlayerRight.destroy();
        mediaPlayerRight = null;
    }

    private IMediaPlayer createLoopMediaPlayer() {
        IMediaPlayer mediaPlayer = engine.createMediaPlayer();
        mediaPlayer.registerPlayerObserver(new IMediaPlayerObserver() {
            @Override
            public void onPlayerStateChanged(Constants.MediaPlayerState state, Constants.MediaPlayerReason reason) {
                if (state.equals(PLAYER_STATE_OPEN_COMPLETED)) {
                    mediaPlayer.setLoopCount(-1);
                    mediaPlayer.play();
                }
            }

            @Override
            public void onPositionChanged(long positionMs, long timestampMs) {

            }

            @Override
            public void onPlayerEvent(Constants.MediaPlayerEvent eventCode, long elapsedTime, String message) {

            }

            @Override
            public void onMetaData(Constants.MediaPlayerMetadataType type, byte[] data) {

            }

            @Override
            public void onPlayBufferUpdated(long playCachedBuffer) {

            }

            @Override
            public void onPreloadEvent(String src, Constants.MediaPlayerPreloadEvent event) {

            }

            @Override
            public void onAgoraCDNTokenWillExpire() {

            }

            @Override
            public void onPlayerSrcInfoChanged(SrcInfo from, SrcInfo to) {

            }

            @Override
            public void onPlayerInfoUpdated(PlayerUpdatedInfo info) {

            }

            @Override
            public void onPlayerCacheStats(CacheStatistics stats) {

            }

            @Override
            public void onPlayerPlaybackStats(PlayerPlaybackStats stats) {

            }

            @Override
            public void onAudioVolumeIndication(int volume) {

            }
        });
        return mediaPlayer;
    }

    private RemoteVoicePositionInfo getVoicePositionInfo(View view) {
        RemoteVoicePositionInfo positionInfo = new RemoteVoicePositionInfo();
        positionInfo.forward = new float[]{1.0F, 0.0F, 0.0F};
        positionInfo.position = getVoicePosition(view);
        return positionInfo;
    }

    private float[] getVoicePosition(View view) {
        float transX = view.getTranslationX();
        float transY = view.getTranslationY();
        double posForward = -1 * AXIS_MAX_DISTANCE * transY / ((rootView.getHeight()) / 2.0f);
        double posRight = AXIS_MAX_DISTANCE * transX / ((rootView.getWidth()) / 2.0f);
        //Log.d(TAG, "VoicePosition posForward=" + posForward + ", posRight=" + posRight);
        return new float[]{(float) posForward, (float) posRight, 0.0F};
    }

    private float[] getViewRelativeSizeInAxis(View view) {
        return new float[]{
                AXIS_MAX_DISTANCE * view.getWidth() * 1.0f / (rootView.getWidth() / 2.0f),
                AXIS_MAX_DISTANCE * view.getHeight() * 1.0f / (rootView.getHeight() / 2.0f),
        };
    }

    private BottomSheetDialog showCommonSettingDialog(boolean isMute, SpatialAudioParams params,
                                                      CompoundButton.OnCheckedChangeListener muteCheckListener,
                                                      CompoundButton.OnCheckedChangeListener blurCheckListener,
                                                      CompoundButton.OnCheckedChangeListener airborneCheckListener,
                                                      SeekBar.OnSeekBarChangeListener attenuationSeekChangeListener
    ) {
        BottomSheetDialog dialog = new BottomSheetDialog(requireContext());
        View dialogView = LayoutInflater.from(requireContext()).inflate(R.layout.dialog_spatial_sound, null);
        Switch muteSwitch = dialogView.findViewById(R.id.switch_mute);
        muteSwitch.setChecked(isMute);
        muteSwitch.setOnCheckedChangeListener(muteCheckListener);
        Switch blurSwitch = dialogView.findViewById(R.id.switch_blur);
        blurSwitch.setChecked(params.enable_blur != null && params.enable_blur);
        blurSwitch.setOnCheckedChangeListener(blurCheckListener);
        Switch airborneSwitch = dialogView.findViewById(R.id.switch_airborne);
        airborneSwitch.setChecked(params.enable_air_absorb != null && params.enable_air_absorb);
        airborneSwitch.setOnCheckedChangeListener(airborneCheckListener);
        TextView attenuationTv = dialogView.findViewById(R.id.tv_attenuation);
        SeekBar attenuationSb = dialogView.findViewById(R.id.sb_attenuation);
        attenuationTv.setText(String.valueOf(params.speaker_attenuation == null ? 0.5 : params.speaker_attenuation));
        attenuationSb.setProgress((int) ((params.speaker_attenuation == null ? 0.5 : params.speaker_attenuation) * attenuationSb.getMax()));
        attenuationSb.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float value = progress * 1.0f / seekBar.getMax();
                attenuationTv.setText(String.valueOf(value));
                if (attenuationSeekChangeListener != null) {
                    attenuationSeekChangeListener.onProgressChanged(seekBar, progress, fromUser);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        dialog.setContentView(dialogView);
        dialog.setCanceledOnTouchOutside(true);
        dialog.show();
        return dialog;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
        handler.post(RtcEngine::destroy);
        engine = null;
    }

    private abstract static class ListenerOnTouchListener implements View.OnTouchListener {
        private float startX, startY, tranX, tranY, curX, curY, maxX, maxY, minX, minY;

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    startX = event.getRawX();
                    startY = event.getRawY();
                    tranX = v.getTranslationX();
                    tranY = v.getTranslationY();
                    if (v.getParent() instanceof ViewGroup) {
                        maxX = (((ViewGroup) v.getParent()).getWidth() - v.getWidth() + 1) / 2;
                        maxY = (((ViewGroup) v.getParent()).getHeight() - v.getHeight() + 1) / 2;
                        minX = -maxX;
                        minY = -maxY;
                    }
                    break;
                case MotionEvent.ACTION_MOVE:
                    curX = event.getRawX();
                    curY = event.getRawY();
                    float newTranX = tranX + curX - startX;
                    if (minX != 0 && newTranX < minX) {
                        newTranX = minX;
                    }
                    if (maxX != 0 && newTranX > maxX) {
                        newTranX = maxX;
                    }
                    v.setTranslationX(newTranX);
                    float newTranY = tranY + curY - startY;
                    if (minY != 0 && newTranY < minY) {
                        newTranY = minY;
                    }
                    if (maxY != 0 && newTranY > maxY) {
                        newTranY = maxY;
                    }
                    v.setTranslationY(newTranY);
                    onPositionChanged();
                    break;
                default:
                    break;
            }
            return true;
        }

        /**
         * On position changed.
         */
        protected abstract void onPositionChanged();
    }

    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private final class InnerRtcEngineEventHandler extends IRtcEngineEventHandler {
        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            super.onJoinChannelSuccess(channel, uid, elapsed);

            isJoined = true;

            runOnUIThread(() -> {
                joinBtn.setEnabled(true);
                joinBtn.setText(R.string.leave);

                mediaPlayerLeftIv.setVisibility(View.VISIBLE);
                mediaPlayerRightIv.setVisibility(View.VISIBLE);
                localIv.setVisibility(View.VISIBLE);
                tipTv.setVisibility(View.VISIBLE);
                switchZone.setVisibility(View.VISIBLE);

                initMediaPlayers();
            });
        }

        /**
         * Error code description can be found at:
         * en: https://api-ref.agora.io/en/video-sdk/android/4.x/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         * cn: https://docs.agora.io/cn/video-call-4.x/API%20Reference/java_ng/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         */
        @Override
        public void onError(int err) {
            Log.e(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
            showAlert(String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
        }

        /**
         * Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
         *
         * @param uid     ID of the user whose audio state changes.
         * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
         *                until this callback is triggered.
         */
        @Override
        public void onUserJoined(int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);
            Log.i(TAG, "onUserJoined->" + uid);
            showLongToast(String.format("user %d joined!", uid));
            runOnUIThread(() -> {
                if (remoteLeftTv.getTag() == null) {
                    remoteLeftTv.setTag(uid);
                    remoteLeftTv.setVisibility(View.VISIBLE);
                    remoteLeftTv.setText(uid + "");
                    RemoteVoicePositionInfo info = getVoicePositionInfo(remoteLeftTv);
                    Log.d(TAG, "left remote user >> pos=" + Arrays.toString(info.position));
                    localSpatial.updateRemotePosition(uid, info);

                    remoteLeftTv.setOnClickListener(v -> showRemoteUserSettingDialog(uid));
                } else if (remoteRightTv.getTag() == null) {
                    remoteRightTv.setTag(uid);
                    remoteRightTv.setVisibility(View.VISIBLE);
                    remoteRightTv.setText(uid + "");
                    localSpatial.updateRemotePosition(uid, getVoicePositionInfo(remoteRightTv));

                    remoteRightTv.setOnClickListener(v -> showRemoteUserSettingDialog(uid));
                }
            });
        }

        /**
         * Occurs when a remote user (Communication)/host (Live Broadcast) leaves the channel.
         *
         * @param uid    ID of the user whose audio state changes.
         * @param reason Reason why the user goes offline:
         *               USER_OFFLINE_QUIT(0): The user left the current channel.
         *               USER_OFFLINE_DROPPED(1): The SDK timed out and the user dropped offline because no data
         *               packet was received within a certain period of time. If a user quits the
         *               call and the message is not passed to the SDK (due to an unreliable channel),
         *               the SDK assumes the user dropped offline.
         *               USER_OFFLINE_BECOME_AUDIENCE(2): (Live broadcast only.) The client role switched from
         *               the host to the audience.
         */
        @Override
        public void onUserOffline(int uid, int reason) {
            Log.i(TAG, String.format("user %d offline! reason:%d", uid, reason));
            showLongToast(String.format("user %d offline! reason:%d", uid, reason));
            runOnUIThread(() -> {
                if (remoteLeftTv.getTag() instanceof Integer && (int) remoteLeftTv.getTag() == uid) {
                    remoteLeftTv.setTag(null);
                    remoteLeftTv.setVisibility(View.GONE);
                    localSpatial.removeRemotePosition(uid);
                } else if (remoteRightTv.getTag() instanceof Integer && (int) remoteRightTv.getTag() == uid) {
                    remoteRightTv.setTag(null);
                    remoteRightTv.setVisibility(View.GONE);
                    localSpatial.removeRemotePosition(uid);
                }
            });
        }

    }
}
