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
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.Constant;
import io.agora.mediaplayer.Constants;
import io.agora.mediaplayer.IMediaPlayer;
import io.agora.mediaplayer.IMediaPlayerObserver;
import io.agora.mediaplayer.data.PlayerUpdatedInfo;
import io.agora.mediaplayer.data.SrcInfo;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.spatialaudio.ILocalSpatialAudioEngine;
import io.agora.spatialaudio.LocalSpatialAudioConfig;
import io.agora.spatialaudio.RemoteVoicePositionInfo;

@Example(
        index = 22,
        group = ADVANCED,
        name = R.string.item_spatial_sound,
        actionId = R.id.action_mainFragment_to_spatial_sound,
        tipsId = R.string.spatial_sound
)
public class SpatialSound extends BaseFragment {
    private static final String TAG = SpatialSound.class.getSimpleName();

    private ImageView listenerIv;
    private ImageView speakerIv;
    private TextView startTv;
    private TextView tipTv;
    private View rootView;

    private RtcEngine engine;
    private IMediaPlayer mediaPlayer;
    private ILocalSpatialAudioEngine localSpatial;

    private final ListenerOnTouchListener listenerOnTouchListener = new ListenerOnTouchListener();
    private final InnerRtcEngineEventHandler iRtcEngineEventHandler = new InnerRtcEngineEventHandler();

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_spatial_sound, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        rootView = view.findViewById(R.id.root_view);
        listenerIv = view.findViewById(R.id.iv_listener);
        speakerIv = view.findViewById(R.id.iv_speaker);
        startTv = view.findViewById(R.id.tv_start);
        tipTv = view.findViewById(R.id.tv_tip);
        speakerIv.setOnTouchListener(listenerOnTouchListener);
        startTv.setOnClickListener(v -> startRecord());

        tipTv.setText(R.string.spatial_sound_tip);
    }

    private void startRecord() {
        startTv.setVisibility(View.GONE);

        engine.setDefaultAudioRoutetoSpeakerphone(true);
        mediaPlayer.open(Constant.URL_PLAY_AUDIO_FILES, 0);

        LocalSpatialAudioConfig localSpatialAudioConfig = new LocalSpatialAudioConfig();
        localSpatialAudioConfig.mRtcEngine = engine;
        localSpatial = ILocalSpatialAudioEngine.create();
        localSpatial.initialize(localSpatialAudioConfig);
        localSpatial.muteLocalAudioStream(true);
        localSpatial.muteAllRemoteAudioStreams(true);
        localSpatial.setAudioRecvRange(50);
        localSpatial.setDistanceUnit(1);
        float[] pos = new float[]{0.0F, 0.0F, 0.0F};
        float[] forward = new float[]{1.0F, 0.0F, 0.0F};
        float[] right = new float[]{0.0F, 1.0F, 0.0F};
        float[] up = new float[]{0.0F, 0.0F, 1.0F};
        localSpatial.updateSelfPosition(pos, forward, right, up);

        startPlayWithSpatialSound();
    }

    private void startPlayWithSpatialSound() {
        resetSpeaker();
        listenerIv.setVisibility(View.VISIBLE);
        speakerIv.setVisibility(View.VISIBLE);

        updateSpatialSoundParam();
    }

    private void resetSpeaker(){
        speakerIv.setTranslationY(-150);
        speakerIv.setTranslationX(0);
    }

    private void updateSpatialSoundParam() {
        float transX = speakerIv.getTranslationX();
        float transY = speakerIv.getTranslationY();
        double viewDistance = Math.sqrt(Math.pow(transX, 2) + Math.pow(transY, 2));
        double viewMaxDistance = Math.sqrt(Math.pow((rootView.getWidth() - speakerIv.getWidth()) / 2.0f, 2) + Math.pow((rootView.getHeight() - speakerIv.getHeight()) / 2.0f, 2));
        double spkMaxDistance = 3;
        double spkMinDistance = 1;

        double spkDistance = spkMaxDistance * (viewDistance / viewMaxDistance);
        if (spkDistance < spkMinDistance) {
            spkDistance = spkMinDistance;
        }
        if (spkDistance > spkMaxDistance) {
            spkDistance = spkMaxDistance;
        }
        double degree = getDegree((int) transX, (int) transY);
        if (transX > 0) {
            degree = 360 - degree;
        }

        double posForward = spkDistance * Math.cos(degree);
        double posRight = spkDistance * Math.sin(degree);

        RemoteVoicePositionInfo positionInfo = new RemoteVoicePositionInfo();
        positionInfo.forward = new float[]{1.0F, 0.0F, 0.0F};
        positionInfo.position = new float[]{(float) posForward, (float) posRight, 0.0F};
        localSpatial.updatePlayerPositionInfo(mediaPlayer.getMediaPlayerId(), positionInfo);
    }

    private int getDegree(int point1X, int point1Y) {
        int vertexPointX = 0, vertexPointY = 0, point0X = 0;
        int point0Y = -10;
        int vector = (point0X - vertexPointX) * (point1X - vertexPointX) + (point0Y - vertexPointY) * (point1Y - vertexPointY);
        double sqrt = Math.sqrt(
                (Math.abs((point0X - vertexPointX) * (point0X - vertexPointX)) + Math.abs((point0Y - vertexPointY) * (point0Y - vertexPointY)))
                        * (Math.abs((point1X - vertexPointX) * (point1X - vertexPointX)) + Math.abs((point1Y - vertexPointY) * (point1Y - vertexPointY)))
        );
        double radian = Math.acos(vector / sqrt);
        return (int) (180 * radian / Math.PI);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mediaPlayer.stop();
        handler.removeCallbacksAndMessages(null);
        handler.post(RtcEngine::destroy);
        engine = null;
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
            /**Creates an RtcEngine instance.
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
            config.mAreaCode = ((MainApplication)getActivity().getApplication()).getGlobalSettings().getAreaCode();
            engine = RtcEngine.create(config);
            /**
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
            mediaPlayer = engine.createMediaPlayer();
            mediaPlayer.registerPlayerObserver(iMediaPlayerObserver);
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    private IMediaPlayerObserver iMediaPlayerObserver = new IMediaPlayerObserver() {
        @Override
        public void onPlayerStateChanged(io.agora.mediaplayer.Constants.MediaPlayerState mediaPlayerState, io.agora.mediaplayer.Constants.MediaPlayerError mediaPlayerError) {
            Log.e(TAG, "onPlayerStateChanged mediaPlayerState " + mediaPlayerState);
            if (mediaPlayerState.equals(PLAYER_STATE_OPEN_COMPLETED)) {
                mediaPlayer.setLoopCount(-1);
                mediaPlayer.play();
            }
        }

        @Override
        public void onPositionChanged(long position) {

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
        public void onCompleted() {

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
        public void onAudioVolumeIndication(int volume) {

        }
    };


    private class ListenerOnTouchListener implements View.OnTouchListener {
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
                    updateSpatialSoundParam();
                    break;
                case MotionEvent.ACTION_UP:
                    break;
            }
            return true;
        }
    }

    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private class InnerRtcEngineEventHandler extends IRtcEngineEventHandler {
        /**
         * Reports a warning during SDK runtime.
         * Warning code: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_warn_code.html
         */
        @Override
        public void onWarning(int warn) {
            Log.w(TAG, String.format("onWarning code %d message %s", warn, RtcEngine.getErrorDescription(warn)));
        }

        /**
         * Reports an error during SDK runtime.
         * Error code: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
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
        }

    }
}
