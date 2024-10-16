package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseVbFragment;
import io.agora.api.example.databinding.FragmentUrlLiveStreamBinding;
import io.agora.api.example.utils.CommonUtil;
import io.agora.rte.Canvas;
import io.agora.rte.CanvasConfig;
import io.agora.rte.CanvasInitialConfig;
import io.agora.rte.Config;
import io.agora.rte.Constants;
import io.agora.rte.Error;
import io.agora.rte.Player;
import io.agora.rte.PlayerInfo;
import io.agora.rte.PlayerObserver;
import io.agora.rte.Rte;
import io.agora.rte.ViewConfig;
import io.agora.rte.exception.RteException;

/**
 * This demo demonstrates how to make a live stream with url
 */
@Example(
        index = 26,
        group = ADVANCED,
        name = R.string.ultra_live_streaming_with_url,
        actionId = R.id.action_mainFragment_to_url_live_stream,
        tipsId = R.string.tip_ultra_live_streaming_with_url
)
public class UrlLiveStream extends BaseVbFragment<FragmentUrlLiveStreamBinding> implements View.OnClickListener {
    private static final String TAG = UrlLiveStream.class.getSimpleName();
    private Handler mHandler = new Handler();
    private Rte mRte;
    private Player mPlayer;
    private Canvas mCanvas;
    private PlayerObserver mPlayerObserver;
    //player init flag
    private boolean isInitSuccess;
    //prepare resource flag
    private boolean isPrepareSuccess;
    private String playerStateDesc = "";
    private String initPlayerStateDesc = "";

    @Override
    protected FragmentUrlLiveStreamBinding getViewBinding(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return FragmentUrlLiveStreamBinding.inflate(inflater, container, false);
    }

    @Override
    protected void initView() {
        binding.btnStart.setOnClickListener(this);
        binding.btnStop.setOnClickListener(this);
        binding.btnPause.setOnClickListener(this);
        binding.btnPlay.setOnClickListener(this);
    }

    @Override
    protected void initData() {
        binding.etRteUrl.setText("rte://" + getString(R.string.agora_app_id));
        try {
            mRte = new Rte(null);
            Config config = new Config();
            config.setAppId(getContext().getString(R.string.agora_app_id));
            mRte.setConfigs(config);
            mRte.initMediaEngine((Error error) -> {
                io.agora.rte.Constants.ErrorCode errCode = error.code();
                if (io.agora.rte.Constants.ErrorCode.OK != errCode) {
                    Log.e(TAG, "initData: initMediaEngine errCode=" + errCode);
                } else {
                    mHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                mPlayer = new Player(mRte, null);
                                mPlayerObserver = new RtePlayerObserver();
                                mPlayer.registerObserver(mPlayerObserver);
                                CanvasConfig canvasConfig = new CanvasConfig();
                                canvasConfig.setVideoRenderMode(io.agora.rte.Constants.VideoRenderMode.FIT);
                                CanvasInitialConfig canvasInitialConfig = new CanvasInitialConfig();
                                mCanvas = new Canvas(mRte, canvasInitialConfig);
                                mCanvas.setConfigs(canvasConfig);
                                binding.surfaceView.setZOrderMediaOverlay(true);
                                // Add to the local container
                                mCanvas.addView(binding.surfaceView, new ViewConfig());
                                mPlayer.setCanvas(mCanvas);
                                isInitSuccess = true;
                                initPlayerStateDesc = "init player success";
                                Log.d(TAG, initPlayerStateDesc);
                                updatePlayButtonState();
                                //init success
                            } catch (RteException e) {
                                initPlayerStateDesc = "init player error = " + e.getMessage();
                                Log.e(TAG, initPlayerStateDesc);
                                isInitSuccess = false;
                                updatePlayButtonState();
                            }
                        }
                    });
                }

            });
        } catch (Exception e) {
            initPlayerStateDesc = "init rte error = " + e.getMessage();
            Log.e(TAG, initPlayerStateDesc);
            isInitSuccess = false;
            updatePlayButtonState();
        }
    }

    private void startWithUrl(String url) {
        if (isInitSuccess) {
            mPlayer.openWithUrl(url, 0, (Error error) -> {
                Constants.ErrorCode code = error.code();
                if (code == Constants.ErrorCode.OK) {
                    // open resource success
                    initPlayerStateDesc = "open resurce success";
                    Log.d(TAG, initPlayerStateDesc);
                    isPrepareSuccess = true;
                    updatePlayButtonState();
                } else {
                    initPlayerStateDesc = "open resurce failed error = " + code.name();
                    Log.e(TAG, initPlayerStateDesc);
                    isPrepareSuccess = false;
                    updatePlayButtonState();
                    stopPlay();
                }
            });
        }
    }

    public int stopPlay() {
        try {
            if (mPlayer != null) {
                mPlayer.stop();
            }
        } catch (RteException e) {
            Log.e(TAG, "stopPlay: fail =" + e.getMessage());
        }
        return 0;
    }

    public void play() {
        try {
            if (mPlayer != null && !isPlaying()) {
                mPlayer.play();
            }
        } catch (RteException e) {
            Log.e(TAG, "play fail =" + e.getMessage());
        }
    }

    public void pause() {
        try {
            if (mPlayer != null && isPlaying()) {
                mPlayer.pause();
            }
        } catch (RteException e) {
            Log.e(TAG, "pause fail =" + e.getMessage());
        }
    }

    public boolean isPlaying() {
        try {
            PlayerInfo playerInfo = new PlayerInfo();
            mPlayer.getInfo(playerInfo);
            return Constants.PlayerState.PLAYING == Constants.PlayerState.fromInt(playerInfo.state());
        } catch (RteException e) {
            Log.e(TAG, "isPlaying fail =" + e.getMessage());
        }
        return false;
    }

    public void release() {
        isInitSuccess = false;
        isPrepareSuccess = false;
        try {
            if (mPlayer != null) {
                mPlayer.unregisterObserver(mPlayerObserver);
            }
        } catch (RteException exception) {
            Log.e(TAG, "release: faile = " + exception.getMessage());
        }
        mPlayer = null;
        mPlayerObserver = null;
        mCanvas = null;
        mRte = null;
    }

    private void updatePlayButtonState() {
        runOnUIThread(new Runnable() {
            @Override
            public void run() {
                binding.tvInitPlayerState.setText("init state = " + initPlayerStateDesc);
                binding.tvPlayerState.setText("player state = " + playerStateDesc);
                if (isInitSuccess) {
                    if (isPrepareSuccess) {
                        binding.btnStart.setVisibility(View.GONE);
                        binding.llContainerPlayPause.setVisibility(View.VISIBLE);
                        binding.btnStop.setVisibility(View.VISIBLE);
                    } else {
                        binding.btnStart.setVisibility(View.VISIBLE);
                        binding.llContainerPlayPause.setVisibility(View.GONE);
                        binding.btnStop.setVisibility(View.GONE);
                    }
                } else {
                    binding.llContainerPlayPause.setVisibility(View.GONE);
                    binding.btnStop.setVisibility(View.GONE);
                    binding.btnStart.setVisibility(View.VISIBLE);
                }
            }
        });
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        release();
    }

    @SuppressLint("WrongConstant")
    @Override
    public void onClick(View v) {
        if (v == binding.btnStart) {
            String url = binding.etRteUrl.getText().toString();
            startWithUrl(url);
            CommonUtil.hideInputBoard(getActivity(), binding.etRteUrl);
        } else if (v == binding.btnStop) {
            stopPlay();
            CommonUtil.hideInputBoard(getActivity(), binding.etRteUrl);

        } else if (v == binding.btnPlay) {
            play();
        } else if (v == binding.btnPause) {
            pause();
        }
    }

    private class RtePlayerObserver extends PlayerObserver {
        @Override
        public void onStateChanged(int old_state, int new_state, Error error) {
            Constants.PlayerState playerState = Constants.PlayerState.fromInt(new_state);
            playerStateDesc = playerState.name();
            switch (playerState) {
                case IDLE:
                    Log.d(TAG, "onStateChanged: IDLE");
                    break;
                case OPENING:
                    Log.d(TAG, "onStateChanged: OPENING");
                    break;
                case OPEN_COMPLETED:
                    Log.d(TAG, "onStateChanged: OPEN_COMPLETED");
                    break;
                case PLAYING:
                    Log.d(TAG, "onStateChanged: PLAYING");
                    break;
                case PAUSED:
                    Log.d(TAG, "onStateChanged: PAUSED");
                    break;
                case PLAYBACK_COMPLETED:
                    Log.d(TAG, "onStateChanged: PLAYBACK_COMPLETED");
                    break;
                case STOPPED:
                    Log.d(TAG, "onStateChanged: STOPPED");
                    isPrepareSuccess = false;
                    break;
                case FAILED:
                    Constants.ErrorCode code = error.code();
                    playerStateDesc = playerStateDesc + " error code = " + code.name();
                    Log.d(TAG, "onStateChanged: FAILED error code = " + Constants.ErrorCode.getValue(code));
                    isPrepareSuccess = false;
                    stopPlay();
                    break;
            }
            updatePlayButtonState();
        }

        @Override
        public void onEvent(int event) {
            Constants.PlayerEvent currentEvent = Constants.PlayerEvent.fromInt(event);
            Log.d(TAG, "onEvent: currentEvent = " + currentEvent);
        }
    }
}
