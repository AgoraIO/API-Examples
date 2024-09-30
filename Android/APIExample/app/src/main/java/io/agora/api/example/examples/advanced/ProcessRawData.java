package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.video.VideoCanvas.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.File;
import java.io.OutputStream;
import java.nio.ByteBuffer;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.api.example.utils.YUVUtils;
import io.agora.base.NV21Buffer;
import io.agora.base.VideoFrame;
import io.agora.base.internal.video.YuvHelper;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.IVideoFrameObserver;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

/**
 * The type Process raw data.
 */
@Example(
        index = 11,
        group = ADVANCED,
        name = R.string.item_processraw,
        actionId = R.id.action_mainFragment_to_ProcessRawData,
        tipsId = R.string.processrawdata
)
public class ProcessRawData extends BaseFragment implements View.OnClickListener {
    private static final String TAG = ProcessRawData.class.getSimpleName();

    private FrameLayout fl_local, fl_remote;
    private Button join, snapshotBtn;
    private EditText et_channel;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false, isSnapshot = false;
    private ByteBuffer videoNV21Buffer;
    private byte[] videoNV21;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }
        try {
            RtcEngineConfig config = new RtcEngineConfig();
            /*
             * The context of Android Activity
             */
            config.mContext = context.getApplicationContext();
            /*
             * The App ID issued to you by Agora. See <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id"> How to get the App ID</a>
             */
            config.mAppId = getString(R.string.agora_app_id);
            /* Sets the channel profile of the Agora RtcEngine.
             CHANNEL_PROFILE_COMMUNICATION(0): (Default) The Communication profile.
             Use this profile in one-on-one calls or group calls, where all users can talk freely.
             CHANNEL_PROFILE_LIVE_BROADCASTING(1): The Live-Broadcast profile. Users in a live-broadcast
             channel have a role as either broadcaster or audience. A broadcaster can both send and receive streams;
             an audience can only receive streams.*/
            config.mChannelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
            /*
             * IRtcEngineEventHandler is an abstract class providing default implementation.
             * The SDK uses this class to report to the app on SDK runtime events.
             */
            config.mEventHandler = iRtcEngineEventHandler;
            config.mAudioScenario = Constants.AudioScenario.getValue(Constants.AudioScenario.DEFAULT);
            config.mAreaCode = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getAreaCode();
            engine = RtcEngine.create(config);
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
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_process_rawdata, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        snapshotBtn = view.findViewById(R.id.btn_snapshot);
        et_channel = view.findViewById(R.id.et_channel);
        join.setOnClickListener(this);
        snapshotBtn.setOnClickListener(this);
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
    }

    @Override
    public void onDestroy() {
        /*leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.registerVideoFrameObserver(null);
            engine.leaveChannel();
            engine.stopPreview();
        }
        engine = null;
        super.onDestroy();
        handler.post(RtcEngine::destroy);
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
                // Check permission
                checkOrRequestPermisson(new PermissonUtils.PermissionResultCallback() {
                    @Override
                    public void onPermissionsResult(boolean allPermissionsGranted, String[] permissions, int[] grantResults) {
                        // Permissions Granted
                        if (allPermissionsGranted) {
                            joinChannel(channelId);
                        }
                    }
                });
            } else {
                joined = false;
                engine.registerVideoFrameObserver(null);
                /*After joining a channel, the user must call the leaveChannel method to end the
                 * call before joining another channel. This method returns 0 if the user leaves the
                 * channel and releases all resources related to the call. This method call is
                 * asynchronous, and the user has not exited the channel when the method call returns.
                 * Once the user leaves the channel, the SDK triggers the onLeaveChannel callback.
                 * A successful leaveChannel method call triggers the following callbacks:
                 *      1:The local client: onLeaveChannel.
                 *      2:The remote client: onUserOffline, if the user leaving the channel is in the
                 *          Communication channel, or is a BROADCASTER in the Live Broadcast profile.
                 * @returns 0: Success.
                 *          < 0: Failure.
                 * PS:
                 *      1:If you call the destroy method immediately after calling the leaveChannel
                 *          method, the leaveChannel process interrupts, and the SDK does not trigger
                 *          the onLeaveChannel callback.
                 *      2:If you call the leaveChannel method during CDN live streaming, the SDK
                 *          triggers the removeInjectStreamUrl method.*/
                engine.leaveChannel();
                engine.stopPreview();
                join.setText(getString(R.string.join));
            }
        } else if (v.getId() == R.id.btn_snapshot) {
            isSnapshot = true;
        }
    }

    private void joinChannel(String channelId) {
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }

        // Create render view by RtcEngine
        SurfaceView surfaceView = new SurfaceView(context);
        // Add to the local container
        fl_local.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        engine.setupLocalVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0));

        /*In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                ((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
        ));
        /*Set up to play remote sound with receiver*/
        engine.setDefaultAudioRoutetoSpeakerphone(true);

        engine.registerVideoFrameObserver(iVideoFrameObserver);
        engine.enableVideo();

        engine.startPreview();

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, token -> {
            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/

            ChannelMediaOptions option = new ChannelMediaOptions();
            option.autoSubscribeAudio = true;
            option.autoSubscribeVideo = true;
            option.publishCameraTrack = true;
            int res = engine.joinChannel(token, channelId, 0, option);
            if (res != 0) {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
                return;
            }
            // Prevent repeated entry
            join.setEnabled(false);
        });
    }

    private final IVideoFrameObserver iVideoFrameObserver = new IVideoFrameObserver() {
        @Override
        public boolean onCaptureVideoFrame(int sourceType, VideoFrame videoFrame) {
            Log.i(TAG, "OnEncodedVideoImageReceived" + Thread.currentThread().getName());

            long startTime = System.currentTimeMillis();
            VideoFrame.Buffer buffer = videoFrame.getBuffer();

            // Obtain texture id from buffer.
            // if(buffer instanceof VideoFrame.TextureBuffer){
            //     int textureId = ((VideoFrame.TextureBuffer) buffer).getTextureId();
            // }

            VideoFrame.I420Buffer i420Buffer = buffer.toI420();
            int width = i420Buffer.getWidth();
            int height = i420Buffer.getHeight();

            // Test Result
            // device: HUAWEI DUB-AL00
            // consume time: 46ms, 54ms, 43ms, 47ms, 57ms, 42ms
            // byte[] i420 = YUVUtils.toWrappedI420(i420Buffer.getDataY(), i420Buffer.getDataU(), i420Buffer.getDataV(), width, height);
            // byte[] nv21 = YUVUtils.I420ToNV21(i420, width, height);

            // *Recommend method*.
            // Test Result
            // device: HUAWEI DUB-AL00
            // consume time: 11ms, 8ms, 10ms, 10ms, 9ms, 10ms
            int nv21MinSize = (int) ((width * height * 3 + 1) / 2.0f);
            if (videoNV21Buffer == null || videoNV21Buffer.capacity() < nv21MinSize) {
                videoNV21Buffer = ByteBuffer.allocateDirect(nv21MinSize);
                videoNV21 = new byte[nv21MinSize];
            }
            YuvHelper.I420ToNV12(i420Buffer.getDataY(), i420Buffer.getStrideY(),
                    i420Buffer.getDataV(), i420Buffer.getStrideV(),
                    i420Buffer.getDataU(), i420Buffer.getStrideU(),
                    videoNV21Buffer, width, height);
            videoNV21Buffer.position(0);
            videoNV21Buffer.get(videoNV21);
            byte[] nv21 = videoNV21;

            Log.d(TAG, "VideoFrame to nv21 --- consume time: " + (System.currentTimeMillis() - startTime) + "ms");

            // Release the buffer!
            i420Buffer.release();

            if (isSnapshot) {
                isSnapshot = false;

                Bitmap bitmap = YUVUtils.nv21ToBitmap(getContext(),
                        nv21,
                        width,
                        height);
                Matrix matrix = new Matrix();
                matrix.setRotate(videoFrame.getRotation());
                // 围绕原地进行旋转
                Bitmap newBitmap = Bitmap.createBitmap(bitmap, 0, 0, width, height, matrix, false);
                // save to file
                saveBitmap2Gallery(newBitmap);

                bitmap.recycle();
            }

            videoFrame.replaceBuffer(new NV21Buffer(nv21, width, height, null), videoFrame.getRotation(), videoFrame.getTimestampNs());
            return true;
        }

        @Override
        public boolean onPreEncodeVideoFrame(int sourceType, VideoFrame videoFrame) {
            return false;
        }

        @Override
        public boolean onMediaPlayerVideoFrame(VideoFrame videoFrame, int i) {
            return false;
        }

        @Override
        public boolean onRenderVideoFrame(String s, int i, VideoFrame videoFrame) {
            return false;
        }

        @Override
        public int getVideoFrameProcessMode() {
            return IVideoFrameObserver.PROCESS_MODE_READ_WRITE;
        }

        @Override
        public int getVideoFormatPreference() {
            return IVideoFrameObserver.VIDEO_PIXEL_DEFAULT;
        }

        @Override
        public boolean getRotationApplied() {
            return false;
        }

        @Override
        public boolean getMirrorApplied() {
            return false;
        }

        @Override
        public int getObservedFramePosition() {
            return IVideoFrameObserver.POSITION_POST_CAPTURER;
        }
    };

    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {
        /**
         * Error code description can be found at:
         * en: https://api-ref.agora.io/en/video-sdk/android/4.x/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         * cn: https://docs.agora.io/cn/video-call-4.x/API%20Reference/java_ng/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         */
        @Override
        public void onError(int err) {
            Log.w(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
        }


        /**Occurs when a user leaves the channel.
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.*/
        @Override
        public void onLeaveChannel(RtcStats stats) {
            super.onLeaveChannel(stats);
            Log.i(TAG, String.format("local user %d leaveChannel!", myUid));
            showLongToast(String.format("local user %d leaveChannel!", myUid));
        }

        /**Occurs when the local user joins a specified channel.
         * The channel name assignment is based on channelName specified in the joinChannel method.
         * If the uid is not specified when joinChannel is called, the server automatically assigns a uid.
         * @param channel Channel name
         * @param uid User ID
         * @param elapsed Time elapsed (ms) from the user calling joinChannel until this callback is triggered*/
        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
            joined = true;
            handler.post(new Runnable() {
                @Override
                public void run() {
                    join.setEnabled(true);
                    join.setText(getString(R.string.leave));
                }
            });
        }

        /**Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
         * @param uid ID of the user whose audio state changes.
         * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
         *                until this callback is triggered.*/
        @Override
        public void onUserJoined(int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);
            Log.i(TAG, "onUserJoined->" + uid);
            showLongToast(String.format("user %d joined!", uid));
            /*Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            }
            handler.post(() -> {
                /*Display remote video stream*/
                // Create render view by RtcEngine
                SurfaceView surfaceView = new SurfaceView(context);
                surfaceView.setZOrderMediaOverlay(true);
                if (fl_remote.getChildCount() > 0) {
                    fl_remote.removeAllViews();
                }
                // Add to the remote container
                fl_remote.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.MATCH_PARENT));
                // Setup remote video to render
                engine.setupRemoteVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, uid));
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
         *               the host to the audience.*/
        @Override
        public void onUserOffline(int uid, int reason) {
            Log.i(TAG, String.format("user %d offline! reason:%d", uid, reason));
            showLongToast(String.format("user %d offline! reason:%d", uid, reason));
            handler.post(new Runnable() {
                @Override
                public void run() {
                    /*Clear render view
                     Note: The video will stay at its last frame, to completely remove it you will need to
                     remove the SurfaceView from its parent*/
                    engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                }
            });
        }
    };

    /**
     * Save bitmap 2 gallery.
     *
     * @param bm the bm
     */
    public void saveBitmap2Gallery(Bitmap bm) {
        long currentTime = System.currentTimeMillis();

        // name the file
        String imageFileName = "IMG_AGORA_" + currentTime + ".jpg";
        String imageFilePath;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            imageFilePath = Environment.DIRECTORY_PICTURES + File.separator + "Agora" + File.separator;
        } else {
            imageFilePath = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).getAbsolutePath()
                    + File.separator + "Agora" + File.separator;
        }

        // write to file

        OutputStream outputStream;
        ContentResolver resolver = requireContext().getContentResolver();
        ContentValues newScreenshot = new ContentValues();
        Uri insert;
        newScreenshot.put(MediaStore.Images.ImageColumns.DATE_ADDED, currentTime);
        newScreenshot.put(MediaStore.Images.ImageColumns.DISPLAY_NAME, imageFileName);
        newScreenshot.put(MediaStore.Images.ImageColumns.MIME_TYPE, "image/jpg");
        newScreenshot.put(MediaStore.Images.ImageColumns.WIDTH, bm.getWidth());
        newScreenshot.put(MediaStore.Images.ImageColumns.HEIGHT, bm.getHeight());
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                newScreenshot.put(MediaStore.Images.ImageColumns.RELATIVE_PATH, imageFilePath);
            } else {
                // make sure the path is existed
                File imageFileDir = new File(imageFilePath);
                if (!imageFileDir.exists()) {
                    boolean mkdir = imageFileDir.mkdirs();
                    if (!mkdir) {
                        showLongToast("save failed, error: cannot create folder. Make sure app has the permission.");
                        return;
                    }
                }
                newScreenshot.put(MediaStore.Images.ImageColumns.DATA, imageFilePath + imageFileName);
                newScreenshot.put(MediaStore.Images.ImageColumns.TITLE, imageFileName);
            }

            // insert a new image
            insert = resolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, newScreenshot);
            // write data
            outputStream = resolver.openOutputStream(insert);

            bm.compress(Bitmap.CompressFormat.PNG, 80, outputStream);
            outputStream.flush();
            outputStream.close();

            newScreenshot.clear();
            newScreenshot.put(MediaStore.Images.ImageColumns.SIZE, new File(imageFilePath).length());
            resolver.update(insert, newScreenshot, null, null);

            showLongToast("save success, you can view it in gallery");
        } catch (Exception e) {
            showLongToast("save failed, error: " + e.getMessage());
            e.printStackTrace();
        }

    }
}
