/*
 * Copyright (C) 2016 Facishare Technology Co., Ltd. All Rights Reserved.
 */
package io.agora.api.example.common.floatwindow;

import android.content.Context;
import android.graphics.Point;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ViewConfiguration;
import android.view.WindowManager;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.animation.Interpolator;
import android.widget.FrameLayout;

/**
 * The type Av call float view.
 */
public class AVCallFloatView extends FrameLayout {
    private static final String TAG = "AVCallFloatView";

    /**
     * Record the horizontal coordinate value of the finger press on the small floating window view
     */
    private float xInView;

    /**
     * Record the vertical coordinate value of the finger press on the small floating window view
     */
    private float yInView;
    /**
     * Record the current finger position's horizontal coordinate value on the screen
     */
    private float xInScreen;

    /**
     * Record the current finger position's vertical coordinate value on the screen
     */
    private float yInScreen;

    /**
     * Record the horizontal coordinate value of the finger press on the screen
     */
    private float xDownInScreen;

    /**
     * Record the vertical coordinate value of the finger press on the screen
     */
    private float yDownInScreen;

    private boolean isAnchoring = false;
    private boolean isShowing = false;
    private WindowManager windowManager = null;
    private WindowManager.LayoutParams mParams = null;


    /**
     * Instantiates a new Av call float view.
     *
     * @param context the context
     */
    public AVCallFloatView(Context context) {
        super(context);
        initView();
    }

    private void initView() {
        windowManager = (WindowManager) getContext().getSystemService(Context.WINDOW_SERVICE);
    }

    /**
     * Sets params.
     *
     * @param params the params
     */
    public void setParams(WindowManager.LayoutParams params) {
        mParams = params;
    }

    /**
     * Sets is showing.
     *
     * @param isShowing the is showing
     */
    public void setIsShowing(boolean isShowing) {
        this.isShowing = isShowing;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (isAnchoring) {
            return true;
        }
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                xInView = event.getX();
                yInView = event.getY();
                xDownInScreen = event.getRawX();
                yDownInScreen = event.getRawY();
                xInScreen = event.getRawX();
                yInScreen = event.getRawY();
                break;
            case MotionEvent.ACTION_MOVE:
                xInScreen = event.getRawX();
                yInScreen = event.getRawY();
                // Update the position of the small floating window when the finger moves
                updateViewPosition();
                break;
            case MotionEvent.ACTION_UP:
                if (Math.abs(xDownInScreen - xInScreen) <= ViewConfiguration.get(getContext()).getScaledTouchSlop()
                        && Math.abs(yDownInScreen - yInScreen) <= ViewConfiguration.get(getContext()).getScaledTouchSlop()) {
                    // Click effect
                    Log.d(TAG, "this float window is clicked");
                } else {
                    // Adsorption effect
                    anchorToSide();
                }
                break;
            default:
                break;
        }
        return true;
    }

    private void anchorToSide() {
        isAnchoring = true;
        Point size = new Point();
        windowManager.getDefaultDisplay().getSize(size);
        int screenWidth = size.x;
        int screenHeight = size.y;
        int middleX = mParams.x + getWidth() / 2;


        int animTime = 0;
        int xDistance = 0;
        int yDistance = 0;

        int dp_25 = dp2px(15);


        if (middleX <= dp_25 + getWidth() / 2) { //1
            xDistance = dp_25 - mParams.x;
        } else if (middleX <= screenWidth / 2) { //2
            xDistance = dp_25 - mParams.x;
        } else if (middleX >= screenWidth - getWidth() / 2 - dp_25) { //3
            xDistance = screenWidth - mParams.x - getWidth() - dp_25;
        } else { //4
            xDistance = screenWidth - mParams.x - getWidth() - dp_25;
        }


        if (mParams.y < dp_25) { //1
            yDistance = dp_25 - mParams.y;
        } else if (mParams.y + getHeight() + dp_25 >= screenHeight) { //2
            yDistance = screenHeight - dp_25 - mParams.y - getHeight();
        }
        Log.e(TAG, "xDistance  " + xDistance + "   yDistance" + yDistance);

        final float animFactorWidth = 600f;
        final float animFactorHeight = 900f;
        animTime = Math.abs(xDistance) > Math.abs(yDistance) ? (int) (((float) xDistance / (float) screenWidth) * animFactorWidth)
                : (int) (((float) yDistance / (float) screenHeight) * animFactorHeight);
        this.post(new AnchorAnimRunnable(Math.abs(animTime), xDistance, yDistance, System.currentTimeMillis()));
    }

    /**
     * Dp 2 px int.
     *
     * @param dp the dp
     * @return the int
     */
    public int dp2px(float dp) {
        final float scale = getContext().getResources().getDisplayMetrics().density;
        return (int) (dp * scale + 0.5f);
    }

    private final class AnchorAnimRunnable implements Runnable {

        private int animTime;
        private long currentStartTime;
        private Interpolator interpolator;
        private int xDistance;
        private int yDistance;
        private int startX;
        private int startY;

        private AnchorAnimRunnable(int animTime, int xDistance, int yDistance, long currentStartTime) {
            this.animTime = animTime;
            this.currentStartTime = currentStartTime;
            interpolator = new AccelerateDecelerateInterpolator();
            this.xDistance = xDistance;
            this.yDistance = yDistance;
            startX = mParams.x;
            startY = mParams.y;
        }

        @Override
        public void run() {
            if (System.currentTimeMillis() >= currentStartTime + animTime) {
                if (mParams.x != (startX + xDistance) || mParams.y != (startY + yDistance)) {
                    mParams.x = startX + xDistance;
                    mParams.y = startY + yDistance;
                    windowManager.updateViewLayout(AVCallFloatView.this, mParams);
                }
                isAnchoring = false;
                return;
            }
            float delta = interpolator.getInterpolation((System.currentTimeMillis() - currentStartTime) / (float) animTime);
            int xMoveDistance = (int) (xDistance * delta);
            int yMoveDistance = (int) (yDistance * delta);
            Log.e(TAG, "delta:  " + delta + "  xMoveDistance  " + xMoveDistance + "   yMoveDistance  " + yMoveDistance);
            mParams.x = startX + xMoveDistance;
            mParams.y = startY + yMoveDistance;
            if (!isShowing) {
                return;
            }
            windowManager.updateViewLayout(AVCallFloatView.this, mParams);
            AVCallFloatView.this.postDelayed(this, 16);
        }
    }

    private void updateViewPosition() {
        //Add movement error
        mParams.x = (int) (xInScreen - xInView);
        mParams.y = (int) (yInScreen - yInView);
        Log.e(TAG, "x  " + mParams.x + "   y  " + mParams.y);
        windowManager.updateViewLayout(this, mParams);
    }
}
