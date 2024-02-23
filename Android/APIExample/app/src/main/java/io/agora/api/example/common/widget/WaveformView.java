package io.agora.api.example.common.widget;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Paint;
import android.graphics.Shader;
import android.util.AttributeSet;
import android.view.View;

import androidx.annotation.Nullable;

import java.util.ArrayList;

import io.agora.api.example.R;

/**
 * The type Waveform view.
 */
public class WaveformView extends View {
    private ArrayList<Short> datas = new ArrayList<>();
    private short max = 100;
    private float mWidth;
    private float mHeight;
    private float space = 1f;
    private Paint mWavePaint;
    private Paint baseLinePaint;
    private int mWaveColor = Color.WHITE;
    private int mBaseLineColor = Color.WHITE;
    private float waveStrokeWidth = 4f;
    private int invalidateTime = 1000 / 100;
    private long drawTime;
    private boolean isMaxConstant = false;

    /**
     * Instantiates a new Waveform view.
     *
     * @param context the context
     */
    public WaveformView(Context context) {
        this(context, null);
    }

    /**
     * Instantiates a new Waveform view.
     *
     * @param context the context
     * @param attrs   the attrs
     */
    public WaveformView(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Instantiates a new Waveform view.
     *
     * @param context      the context
     * @param attrs        the attrs
     * @param defStyleAttr the def style attr
     */
    public WaveformView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(attrs, defStyleAttr);
    }

    private void init(AttributeSet attrs, int defStyle) {
        final TypedArray a = getContext().obtainStyledAttributes(
                attrs, R.styleable.WaveView, defStyle, 0);
        mWaveColor = a.getColor(
                R.styleable.WaveView_waveColor,
                mWaveColor);
        mBaseLineColor = a.getColor(
                R.styleable.WaveView_baselineColor,
                mBaseLineColor);

        waveStrokeWidth = a.getDimension(
                R.styleable.WaveView_waveStokeWidth,
                waveStrokeWidth);

        max = (short) a.getInt(R.styleable.WaveView_maxValue, max);
        invalidateTime = a.getInt(R.styleable.WaveView_invalidateTime, invalidateTime);

        space = a.getDimension(R.styleable.WaveView_space, space);
        a.recycle();
        initPainters();

    }

    private void initPainters() {
        mWavePaint = new Paint();
        mWavePaint.setColor(mWaveColor); // 画笔为color
        mWavePaint.setStrokeWidth(waveStrokeWidth); // 设置画笔粗细
        mWavePaint.setAntiAlias(true);
        mWavePaint.setFilterBitmap(true);
        mWavePaint.setStrokeCap(Paint.Cap.ROUND);
        mWavePaint.setStyle(Paint.Style.FILL);
        Shader shader = new LinearGradient(0, 0, 1000, 0, 0xffffffff, 0xFFe850ee, Shader.TileMode.CLAMP);
        mWavePaint.setShader(shader);
        baseLinePaint = new Paint();
        baseLinePaint.setColor(mBaseLineColor); // 画笔为color
        baseLinePaint.setStrokeWidth(1f); // 设置画笔粗细
        baseLinePaint.setAntiAlias(true);
        baseLinePaint.setFilterBitmap(true);
        baseLinePaint.setStyle(Paint.Style.FILL);
    }

    /**
     * Gets max.
     *
     * @return the max
     */
    public short getMax() {
        return max;
    }

    /**
     * Sets max.
     *
     * @param max the max
     */
    public void setMax(short max) {
        this.max = max;
    }

    /**
     * Gets space.
     *
     * @return the space
     */
    public float getSpace() {
        return space;
    }

    /**
     * Sets space.
     *
     * @param space the space
     */
    public void setSpace(float space) {
        this.space = space;
    }

    /**
     * Gets wave color.
     *
     * @return the wave color
     */
    public int getmWaveColor() {
        return mWaveColor;
    }

    /**
     * Sets wave color.
     *
     * @param mWaveColor the m wave color
     */
    public void setmWaveColor(int mWaveColor) {
        this.mWaveColor = mWaveColor;
        invalidateNow();
    }

    /**
     * Gets base line color.
     *
     * @return the base line color
     */
    public int getmBaseLineColor() {
        return mBaseLineColor;
    }

    /**
     * Sets base line color.
     *
     * @param mBaseLineColor the m base line color
     */
    public void setmBaseLineColor(int mBaseLineColor) {
        this.mBaseLineColor = mBaseLineColor;
        invalidateNow();
    }

    /**
     * Gets wave stroke width.
     *
     * @return the wave stroke width
     */
    public float getWaveStrokeWidth() {
        return waveStrokeWidth;
    }

    /**
     * Sets wave stroke width.
     *
     * @param waveStrokeWidth the wave stroke width
     */
    public void setWaveStrokeWidth(float waveStrokeWidth) {
        this.waveStrokeWidth = waveStrokeWidth;
        invalidateNow();
    }

    /**
     * Gets invalidate time.
     *
     * @return the invalidate time
     */
    public int getInvalidateTime() {
        return invalidateTime;
    }

    /**
     * Sets invalidate time.
     *
     * @param invalidateTime the invalidate time
     */
    public void setInvalidateTime(int invalidateTime) {
        this.invalidateTime = invalidateTime;
    }

    /**
     * Is max constant boolean.
     *
     * @return the boolean
     */
    public boolean isMaxConstant() {
        return isMaxConstant;
    }

    /**
     * Sets max constant.
     *
     * @param maxConstant the max constant
     */
    public void setMaxConstant(boolean maxConstant) {
        isMaxConstant = maxConstant;
    }

    /**
     * 如果改变相应配置  需要刷新相应的paint设置
     */
    public void invalidateNow() {
        initPainters();
        invalidate();
    }

    /**
     * Add data.
     *
     * @param data the data
     */
    public void addData(short data) {

        if (data < 0) {
            data = (short) -data;
        }
        if (data > max && !isMaxConstant) {
            max = data;
        }
        if (datas.size() > mWidth / space) {
            synchronized (this) {
                datas.remove(0);
                datas.add(data);
            }
        } else {
            datas.add(data);
        }
        if (System.currentTimeMillis() - drawTime > invalidateTime) {
            invalidate();
            drawTime = System.currentTimeMillis();
        }

    }

    /**
     * Clear.
     */
    public void clear() {
        datas.clear();
        invalidateNow();
    }


    @Override
    protected void onDraw(Canvas canvas) {
        canvas.translate(0, mHeight / 2);
        drawBaseLine(canvas);
        drawWave(canvas);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        mWidth = w;
        mHeight = h;
    }

    private void drawWave(Canvas mCanvas) {
        for (int i = 0; i < datas.size(); i++) {
            float x = i * space;
            float y = (float) datas.get(i) / max * mHeight / 2;
            mCanvas.drawLine(x, -y, x, y, mWavePaint);
        }

    }

    private void drawBaseLine(Canvas mCanvas) {
        mCanvas.drawLine(0, 0, mWidth, 0, baseLinePaint);
    }
}