package io.agora.rtc.screencapture;

import android.annotation.TargetApi;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import io.agora.rtc.screencapture.aidl.INotification;
import io.agora.rtc.screencapture.aidl.IScreenSharing;
import io.agora.rtc.screencapture.impl.ScreenSharingService;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class ScreenShareClient {
    private static final String TAG = ScreenShareClient.class.getSimpleName();
    private static IScreenSharing mScreenShareSvc;
    private IStateListener mStateListener;
    private static volatile ScreenShareClient mInstance;

//    private ScreenSharingClient() {
//    }

    public static ScreenShareClient getInstance() {
        if (mInstance == null) {
            synchronized (ScreenShareClient.class) {
                if (mInstance == null) {
                    mInstance = new ScreenShareClient();
                }
            }
        }

        return mInstance;
    }

    private final ServiceConnection mScreenShareConn = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            mScreenShareSvc = IScreenSharing.Stub.asInterface(service);

            try {
                mScreenShareSvc.registerCallback(mNotification);
                mScreenShareSvc.startShare();
            } catch (RemoteException e) {
                e.printStackTrace();
                Log.e(TAG, Log.getStackTraceString(e));
            }

        }

        public void onServiceDisconnected(ComponentName className) {
            mScreenShareSvc = null;
        }
    };

    private final INotification mNotification = new INotification.Stub() {
        /**
         * This is called by the remote service to tell us about error happened.
         * Note that IPC calls are dispatched through a thread
         * pool running in each process, so the code executing here will
         * NOT be running in our main thread like most other things -- so,
         * if to update the UI, we need to use a Handler to hop over there.
         */
        public void onError(int error) {
            Log.e(TAG, "screen sharing service error happened: " + error);
            mStateListener.onError(error);
        }

        public void onTokenWillExpire() {
            Log.d(TAG, "access token for screen sharing service will expire soon");
            mStateListener.onTokenWillExpire();
        }
    };

    @TargetApi(21)
    public void start(Context context, String appId, String token, String channelName, int uid, VideoEncoderConfiguration vec) {
        if (mScreenShareSvc == null) {
            Intent intent = new Intent(context, ScreenSharingService.class);
            intent.putExtra(io.agora.rtc.screencapture.Constant.APP_ID, appId);
            intent.putExtra(io.agora.rtc.screencapture.Constant.ACCESS_TOKEN, token);
            intent.putExtra(io.agora.rtc.screencapture.Constant.CHANNEL_NAME, channelName);
            intent.putExtra(io.agora.rtc.screencapture.Constant.UID, uid);
            intent.putExtra(io.agora.rtc.screencapture.Constant.WIDTH, vec.dimensions.width);
            intent.putExtra(io.agora.rtc.screencapture.Constant.HEIGHT, vec.dimensions.height);
            intent.putExtra(io.agora.rtc.screencapture.Constant.FRAME_RATE, vec.frameRate);
            intent.putExtra(io.agora.rtc.screencapture.Constant.BITRATE, vec.bitrate);
            intent.putExtra(Constant.ORIENTATION_MODE, vec.orientationMode.getValue());
            context.bindService(intent, mScreenShareConn, Context.BIND_AUTO_CREATE);
        } else {
            try {
                mScreenShareSvc.startShare();
            } catch (RemoteException e) {
                e.printStackTrace();
                Log.e(TAG, Log.getStackTraceString(e));
            }
        }

    }

    @TargetApi(21)
    public void stop(Context context) {
        if (mScreenShareSvc != null) {
            try {
                mScreenShareSvc.stopShare();
                mScreenShareSvc.unregisterCallback(mNotification);
            } catch (RemoteException e) {
                e.printStackTrace();
                Log.e(TAG, Log.getStackTraceString(e));
            } finally {
                mScreenShareSvc = null;
            }
        }
        context.unbindService(mScreenShareConn);
    }

    @TargetApi(21)
    public void renewToken(String token) {
        if (mScreenShareSvc != null) {
            try {
                mScreenShareSvc.renewToken(token);
            } catch (RemoteException e) {
                e.printStackTrace();
                Log.e(TAG, Log.getStackTraceString(e));
            }
        } else {
            Log.e(TAG, "screen sharing service not exist");
        }
    }

    @TargetApi(21)
    public void setListener(IStateListener listener) {
        mStateListener = listener;
    }

    public interface IStateListener {
        void onError(int error);

        void onTokenWillExpire();
    }
}
