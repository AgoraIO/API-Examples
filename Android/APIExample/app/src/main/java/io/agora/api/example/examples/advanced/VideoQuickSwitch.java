package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc.Constants.REMOTE_VIDEO_STATE_DECODING;
import static io.agora.rtc.video.VideoCanvas.RENDER_MODE_HIDDEN;
import static io.agora.rtc.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.content.Context;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.util.SparseArray;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.viewpager.widget.PagerAdapter;
import androidx.viewpager.widget.ViewPager;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import java.util.ArrayList;
import java.util.List;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.rtc.Constants;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.models.ChannelMediaOptions;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

/**---------------------------------------Important!!!----------------------------------------------
 * This example demonstrates how audience can quickly switch channels. The following points need to be noted:
 1: You can only access the channel as an audience{@link VideoQuickSwitch#joinChannel(String)}.
 2: If you want to see a normal remote screen, you need to set up several live rooms in advance and
 push the stream as a live one (the name of the live room is in the channels instance{"channel0", "channel1", "channel2"};
 at the same time, the appid you used to set up the live room should be consistent with this example program).
 * @author cjw*/
@Example(
        index = 2,
        group = ADVANCED,
        name = R.string.item_quickswitck,
        actionId = R.id.action_mainFragment_to_QuickSwitch,
        tipsId = R.string.quickswitchchannel
)
public class VideoQuickSwitch extends BaseFragment
{
    private static final String TAG = VideoQuickSwitch.class.getSimpleName();
    private ViewPager viewPager;
    private RtcEngine engine;
    private int myUid;
    private final String[] channels = new String[]{"channel0", "channel1", "channel2"};
    private List<String> channelList = new ArrayList<>();
    private ViewPagerAdapter viewPagerAdapter;
    private int currentIndex = 0;
    private int lastIndex = -1;
    private boolean noBroadcaster = true;
    private Runnable runnable = new Runnable()
    {
        @Override
        public void run()
        {
            if(noBroadcaster)
            {
                /**There is no broadcaster in the current channel*/
                viewPagerAdapter.notifyBroadcaster(currentIndex, !noBroadcaster);
            }
        }
    };

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        // Check if the context is valid
        Context context = getContext();
        if (context == null)
        {
            return;
        }
        try
        {
            /**Creates an RtcEngine instance.
             * @param context The context of Android Activity
             * @param appId The App ID issued to you by Agora. See <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id">
             *              How to get the App ID</a>
             * @param handler IRtcEngineEventHandler is an abstract class providing default implementation.
             *                The SDK uses this class to report to the app on SDK runtime events.*/
            engine = RtcEngine.create(context.getApplicationContext(), getString(R.string.agora_app_id), iRtcEngineEventHandler);
        }
        catch (Exception e)
        {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState)
    {
        View view = inflater.inflate(R.layout.fragment_quick_switch_channel, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {
        super.onViewCreated(view, savedInstanceState);
        viewPager = view.findViewById(R.id.viewPager);
        /**Prepare data*/
        for (String channel : channels)
        {
            channelList.add(channel);
        }
        viewPagerAdapter = new ViewPagerAdapter(getContext(), channelList);
        viewPager.setAdapter(viewPagerAdapter);
        viewPager.addOnPageChangeListener(new ViewPager.OnPageChangeListener()
        {
            @Override
            public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels)
            {
                if (positionOffset == 0f && position != currentIndex)
                {
                    viewPager.post(new Runnable()
                    {
                        @Override
                        public void run()
                        {
                            Log.i(TAG, "Will switch channel to " + channelList.get(position));

                            currentIndex = position;
                            if (lastIndex >= 0)
                            {
                                viewPagerAdapter.removeSurfaceViewByIndex(lastIndex);
                                viewPagerAdapter.notifyBroadcaster(lastIndex, true);
                            }

                            /**Since v2.9.0.
                             * Switches to a different channel.
                             * This method allows the audience of a Live-broadcast channel to switch to a different channel.
                             * After the user successfully switches to another channel, the onLeaveChannel
                             * and onJoinChannelSuccess callbacks are triggered to indicate that the
                             * user has left the original channel and joined a new one.
                             * @param token The token for authentication:
                             *                  In situations not requiring high security: You can use
                             *                      the temporary token generated at Console. For details,
                             *                      see Get a temporary token.
                             *                  In situations requiring high security: Set it as the token
                             *                      generated at your server. For details, see Get a token.
                             * @param channelName Unique channel name for the AgoraRTC session in the
                             *                      string format. The string length must be less than
                             *                      64 bytes. Supported character scopes are:
                             *                    All lowercase English letters: a to z.
                             *                    All uppercase English letters: A to Z.
                             *                    All numeric characters: 0 to 9.
                             *                    The space character.
                             *                    Punctuation characters and other symbols, including:
                             *                      "!", "#", "$", "%", "&", "(", ")", "+", "-", ":",
                             *                      ";", "<", "=", ".", ">", "?", "@", "[", "]", "^",
                             *                      "_", " {", "}", "|", "~", ",".
                             * @return
                             *   0: Success.
                             *   < 0: Failure.
                             * PS：
                             *   Important!!!This method applies to the audience role in a
                             *   Live-broadcast channel only.*/

                            ChannelMediaOptions option = new ChannelMediaOptions();
                            option.autoSubscribeAudio = true;
                            option.autoSubscribeVideo = true;
                            int code = engine.switchChannel(null, channelList.get(position), option);

                            lastIndex = currentIndex;
                        }
                    });
                }
            }
            @Override
            public void onPageSelected(int position)
            {}
            @Override
            public void onPageScrollStateChanged(int state)
            {}
        });
        /**Swipe left and right to switch channel tips*/
        showAlert(getString(R.string.swiptips));
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
        // Check permission
        if (AndPermission.hasPermissions(this, Permission.Group.STORAGE, Permission.Group.MICROPHONE, Permission.Group.CAMERA))
        {
            joinChannel(channelList.get(0));
            return;
        }
        // Request permission
        AndPermission.with(this).runtime().permission(
                Permission.READ_EXTERNAL_STORAGE,
                Permission.WRITE_EXTERNAL_STORAGE
        ).onGranted(permissions ->
        {
            // Permissions Granted
            joinChannel(channelList.get(0));
        }).start();
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        /**leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null)
        {
            engine.leaveChannel();
        }
        handler.post(RtcEngine::destroy);
        engine = null;
    }

    private final void joinChannel(String channelId)
    {
        // Check if the context is valid
        Context context = getContext();
        if (context == null)
        {
            return;
        }

        // Set audio route to speaker
        engine.setDefaultAudioRoutetoSpeakerphone(true);
        /** Sets the channel profile of the Agora RtcEngine.
         CHANNEL_PROFILE_COMMUNICATION(0): (Default) The Communication profile.
         Use this profile in one-on-one calls or group calls, where all users can talk freely.
         CHANNEL_PROFILE_LIVE_BROADCASTING(1): The Live-Broadcast profile. Users in a live-broadcast
         channel have a role as either broadcaster or audience. A broadcaster can both send and receive streams;
         an audience can only receive streams.*/
        engine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        /**In the demo, the default is to enter as the broadcaster.*/
        engine.setClientRole(IRtcEngineEventHandler.ClientRole.CLIENT_ROLE_AUDIENCE);
        // Enable video module
        engine.enableVideo();
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                ((MainApplication)getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication)getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication)getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
        ));
        /**Set up to play remote sound with receiver*/
        engine.setDefaultAudioRoutetoSpeakerphone(true);
        engine.setEnableSpeakerphone(false);

        /**Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        String accessToken = getString(R.string.agora_access_token);
        if (TextUtils.equals(accessToken, "") || TextUtils.equals(accessToken, "<#YOUR ACCESS TOKEN#>"))
        {
            accessToken = null;
        }

        /**Allows a user to join a channel.
         * if you do not specify the uid, we will generate the uid for you.
         * If your account has enabled token mechanism through the console, you must fill in the
         * corresponding token here. In general, it is not recommended to open the token mechanism in the test phase.*/

        ChannelMediaOptions option = new ChannelMediaOptions();
        option.autoSubscribeAudio = true;
        option.autoSubscribeVideo = true;
        int res = engine.joinChannel(accessToken, channelId, "Extra Optional Data", 0, option);
        if (res != 0)
        {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
            showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
            return;
        }
    }

    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler()
    {
        /**Reports a warning during SDK runtime.
         * Warning code: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_warn_code.html*/
        @Override
        public void onWarning(int warn)
        {
            Log.w(TAG, String.format("onWarning code %d message %s", warn, RtcEngine.getErrorDescription(warn)));
        }

        /**Reports an error during SDK runtime.
         * Error code: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html*/
        @Override
        public void onError(int err)
        {
            Log.e(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
            showAlert(String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
        }

        /**Occurs when a user leaves the channel.
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.
         * PS:
         *   Important! Because the channel is entered by the role of an audience, this callback will
         *   only be received when the broadcaster exits the channel.*/
        @Override
        public void onLeaveChannel(RtcStats stats)
        {
            super.onLeaveChannel(stats);
            Log.i(TAG, String.format("local user %d leaveChannel!", myUid));
            showLongToast(String.format("local user %d leaveChannel!", myUid));
            handler.removeCallbacks(runnable);
        }

        /**Occurs when the local user joins a specified channel.
         * The channel name assignment is based on channelName specified in the joinChannel method.
         * If the uid is not specified when joinChannel is called, the server automatically assigns a uid.
         * @param channel Channel name
         * @param uid User ID
         * @param elapsed Time elapsed (ms) from the user calling joinChannel until this callback is triggered*/
        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed)
        {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
            /**Determine if there is a host in the channel*/
            noBroadcaster = true;
            handler.post(runnable);
        }

        /**Since v2.9.0.
         * This callback indicates the state change of the remote audio stream.
         * PS: This callback does not work properly when the number of users (in the Communication profile) or
         *     broadcasters (in the Live-broadcast profile) in the channel exceeds 17.
         * @param uid ID of the user whose audio state changes.
         * @param state State of the remote audio
         *   REMOTE_AUDIO_STATE_STOPPED(0): The remote audio is in the default state, probably due
         *              to REMOTE_AUDIO_REASON_LOCAL_MUTED(3), REMOTE_AUDIO_REASON_REMOTE_MUTED(5),
         *              or REMOTE_AUDIO_REASON_REMOTE_OFFLINE(7).
         *   REMOTE_AUDIO_STATE_STARTING(1): The first remote audio packet is received.
         *   REMOTE_AUDIO_STATE_DECODING(2): The remote audio stream is decoded and plays normally,
         *              probably due to REMOTE_AUDIO_REASON_NETWORK_RECOVERY(2),
         *              REMOTE_AUDIO_REASON_LOCAL_UNMUTED(4) or REMOTE_AUDIO_REASON_REMOTE_UNMUTED(6).
         *   REMOTE_AUDIO_STATE_FROZEN(3): The remote audio is frozen, probably due to
         *              REMOTE_AUDIO_REASON_NETWORK_CONGESTION(1).
         *   REMOTE_AUDIO_STATE_FAILED(4): The remote audio fails to start, probably due to
         *              REMOTE_AUDIO_REASON_INTERNAL(0).
         * @param reason The reason of the remote audio state change.
         *   REMOTE_AUDIO_REASON_INTERNAL(0): Internal reasons.
         *   REMOTE_AUDIO_REASON_NETWORK_CONGESTION(1): Network congestion.
         *   REMOTE_AUDIO_REASON_NETWORK_RECOVERY(2): Network recovery.
         *   REMOTE_AUDIO_REASON_LOCAL_MUTED(3): The local user stops receiving the remote audio
         *               stream or disables the audio module.
         *   REMOTE_AUDIO_REASON_LOCAL_UNMUTED(4): The local user resumes receiving the remote audio
         *              stream or enables the audio module.
         *   REMOTE_AUDIO_REASON_REMOTE_MUTED(5): The remote user stops sending the audio stream or
         *               disables the audio module.
         *   REMOTE_AUDIO_REASON_REMOTE_UNMUTED(6): The remote user resumes sending the audio stream
         *              or enables the audio module.
         *   REMOTE_AUDIO_REASON_REMOTE_OFFLINE(7): The remote user leaves the channel.
         * @param elapsed Time elapsed (ms) from the local user calling the joinChannel method
         *                  until the SDK triggers this callback.*/
        @Override
        public void onRemoteAudioStateChanged(int uid, int state, int reason, int elapsed)
        {
            super.onRemoteAudioStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteAudioStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
        }

        /**Since v2.9.0.
         * Occurs when the remote video state changes.
         * PS: This callback does not work properly when the number of users (in the Communication
         *     profile) or broadcasters (in the Live-broadcast profile) in the channel exceeds 17.
         * @param uid ID of the remote user whose video state changes.
         * @param state State of the remote video:
         *   REMOTE_VIDEO_STATE_STOPPED(0): The remote video is in the default state, probably due
         *              to REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED(3), REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED(5),
         *              or REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE(7).
         *   REMOTE_VIDEO_STATE_STARTING(1): The first remote video packet is received.
         *   REMOTE_VIDEO_STATE_DECODING(2): The remote video stream is decoded and plays normally,
         *              probably due to REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY (2),
         *              REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED(4), REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED(6),
         *              or REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY(9).
         *   REMOTE_VIDEO_STATE_FROZEN(3): The remote video is frozen, probably due to
         *              REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION(1) or REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK(8).
         *   REMOTE_VIDEO_STATE_FAILED(4): The remote video fails to start, probably due to
         *              REMOTE_VIDEO_STATE_REASON_INTERNAL(0).
         * @param reason The reason of the remote video state change:
         *   REMOTE_VIDEO_STATE_REASON_INTERNAL(0): Internal reasons.
         *   REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION(1): Network congestion.
         *   REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY(2): Network recovery.
         *   REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED(3): The local user stops receiving the remote
         *               video stream or disables the video module.
         *   REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED(4): The local user resumes receiving the remote
         *               video stream or enables the video module.
         *   REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED(5): The remote user stops sending the video
         *               stream or disables the video module.
         *   REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED(6): The remote user resumes sending the video
         *               stream or enables the video module.
         *   REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE(7): The remote user leaves the channel.
         *   REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK(8): The remote media stream falls back to the
         *               audio-only stream due to poor network conditions.
         *   REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY(9): The remote media stream switches
         *               back to the video stream after the network conditions improve.
         * @param elapsed Time elapsed (ms) from the local user calling the joinChannel method until
         *               the SDK triggers this callback.*/
        @Override
        public void onRemoteVideoStateChanged(int uid, int state, int reason, int elapsed)
        {
            super.onRemoteVideoStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteVideoStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
            if(state == REMOTE_VIDEO_STATE_DECODING)
            {
                /**REMOTE_VIDEO_STATE_DECODING as the basis for judging whether there is a broadcaster
                 *  in the channel.
                 * But you should judge according to your own business logic, here is just for example,
                 *  not for reference.*/
                noBroadcaster = false;
            }
        }

        /**Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
         * @param uid ID of the user whose audio state changes.
         * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
         *                until this callback is triggered.
         * PS:
         *   Important! Because the channel is entered by the role of an audience, this callback will
         *   only be received when the broadcaster exits the channel.*/
        @Override
        public void onUserJoined(int uid, int elapsed)
        {
            super.onUserJoined(uid, elapsed);
            Log.i(TAG, "onUserJoined->" + uid);
            showLongToast(String.format("user %d joined!", uid));
            /**Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            }
            handler.post(() ->
            {
                if(uid != myUid)
                {
                    SurfaceView surfaceV = RtcEngine.CreateRendererView(getContext().getApplicationContext());
                    surfaceV.setZOrderMediaOverlay(true);
                    engine.setupRemoteVideo(new VideoCanvas(surfaceV, VideoCanvas.RENDER_MODE_HIDDEN, uid));
                    viewPagerAdapter.setSurfaceView(currentIndex, uid, surfaceV);
                }
            });
        }

        /**Occurs when a remote user (Communication)/host (Live Broadcast) leaves the channel.
         * @param uid ID of the user whose audio state changes.
         * @param reason Reason why the user goes offline:
         *   USER_OFFLINE_QUIT(0): The user left the current channel.
         *   USER_OFFLINE_DROPPED(1): The SDK timed out and the user dropped offline because no data
         *              packet was received within a certain period of time. If a user quits the
         *               call and the message is not passed to the SDK (due to an unreliable channel),
         *               the SDK assumes the user dropped offline.
         *   USER_OFFLINE_BECOME_AUDIENCE(2): (Live broadcast only.) The client role switched from
         *               the host to the audience.
         * PS:
         *   Important! Because the channel is entered by the role of an audience, this callback will
         *   only be received when the broadcaster exits the channel.*/
        @Override
        public void onUserOffline(int uid, int reason)
        {
            Log.i(TAG, String.format("user %d offline! reason:%d", uid, reason));
            showLongToast(String.format("user %d offline! reason:%d", uid, reason));
            handler.post(new Runnable()
            {
                @Override
                public void run()
                {
                    /**Clear render view
                     Note: The video will stay at its last frame, to completely remove it you will need to
                     remove the SurfaceView from its parent*/
                    engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                    viewPagerAdapter.removeSurfaceView(uid);
                }
            });
        }
    };

    public class ViewPagerAdapter extends PagerAdapter
    {
        private SparseArray<ViewGroup> viewList = new SparseArray<>();
        private Context context;
        private List<String> roomNameList = new ArrayList<>();

        public ViewPagerAdapter(Context context, List<String> roomNameList)
        {
            this.context = context;
            this.roomNameList = roomNameList;
        }

        @Override
        public Object instantiateItem(ViewGroup collection, int position)
        {
            ViewGroup layout = viewList.get(position);
            if (layout == null)
            {
                LayoutInflater inflater = LayoutInflater.from(context);
                layout = (ViewGroup) inflater.inflate(R.layout.view_item_quickswitch, collection, false);
                viewList.put(position, layout);

                TextView channel = layout.findViewById(R.id.channelName);
                channel.setText(String.format(getString(R.string.channelstr), roomNameList.get(position)));
            }

            collection.addView(layout);

            return layout;
        }

        @Override
        public void destroyItem(ViewGroup collection, int position, Object view)
        {
            collection.removeView((View) view);
        }

        @Override
        public int getCount()
        {
            return roomNameList.size();
        }

        private void setSurfaceView(int position, final int uid, final SurfaceView view)
        {
            final ViewGroup viewGroup = viewList.get(position);
            if (viewGroup != null)
            {
                ViewGroup surfaceContainer = viewGroup.findViewById(R.id.fl_remote);
                surfaceContainer.removeAllViews();
                view.setZOrderMediaOverlay(true);
                surfaceContainer.addView(view, new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.MATCH_PARENT));

                TextView uidTextView = viewGroup.findViewById(R.id.uid);
                uidTextView.setText(String.format("Uid: %d", uid));

                viewGroup.setTag(uid);
            }
        }

        private void removeSurfaceView(int uid)
        {
            for (int i = 0; i < viewList.size(); i++)
            {
                ViewGroup viewGroup = viewList.get(i);

                if (viewGroup.getTag() != null && ((Integer) viewGroup.getTag()) == uid)
                {
                    removeSurfaceView(viewGroup);
                }
            }
        }

        private void removeSurfaceViewByIndex(int index)
        {
            ViewGroup viewGroup = viewList.get(index);
            if (viewGroup != null)
            {
                removeSurfaceView(viewGroup);
            }
        }

        private void removeSurfaceView(ViewGroup viewGroup)
        {
            ViewGroup surfaceContainer = viewGroup.findViewById(R.id.fl_remote);
            surfaceContainer.removeAllViews();

            TextView uidTextView = viewGroup.findViewById(R.id.uid);
            uidTextView.setText("");
        }

        public void notifyBroadcaster(int index, boolean exists)
        {
            ViewGroup viewGroup = viewList.get(index);
            if (viewGroup != null)
            {
                TextView textView = viewGroup.findViewById(R.id.noBroadcaster);
                textView.setVisibility(exists ? View.GONE : View.VISIBLE);
            }
        }

        @Override
        public boolean isViewFromObject(@NonNull View view, @NonNull Object object)
        {
            return view == object;
        }

        @Override
        public CharSequence getPageTitle(int position)
        {
            return "";
        }
    }
}
