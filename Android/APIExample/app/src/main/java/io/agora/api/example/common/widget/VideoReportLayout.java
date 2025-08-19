package io.agora.api.example.common.widget;

import android.content.Context;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import io.agora.api.example.common.model.StatisticsInfo;
import io.agora.rtc2.IRtcEngineEventHandler;

/**
 * The type Video report layout.
 */
public class VideoReportLayout extends FrameLayout {

    private final StatisticsInfo statisticsInfo = new StatisticsInfo();
    private TextView reportTextView;
    private int reportUid = -1;

    /**
     * Instantiates a new Video report layout.
     *
     * @param context the context
     */
    public VideoReportLayout(@NonNull Context context) {
        super(context);
    }

    /**
     * Instantiates a new Video report layout.
     *
     * @param context the context
     * @param attrs   the attrs
     */
    public VideoReportLayout(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
    }

    /**
     * Instantiates a new Video report layout.
     *
     * @param context      the context
     * @param attrs        the attrs
     * @param defStyleAttr the def style attr
     */
    public VideoReportLayout(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    public void addView(View child, int index, ViewGroup.LayoutParams params) {
        super.addView(child, index, params);

        if (child instanceof SurfaceView || child instanceof TextureView) {
            reportTextView = new TextView(getContext());
            reportTextView.addOnAttachStateChangeListener(new OnAttachStateChangeListener() {
                @Override
                public void onViewAttachedToWindow(View v) {

                }

                @Override
                public void onViewDetachedFromWindow(View v) {
                    if (reportTextView != null) {
                        reportTextView.removeOnAttachStateChangeListener(this);
                        reportTextView = null;
                    }
                }
            });
            reportTextView.setTextColor(Color.parseColor("#eeeeee"));
            LayoutParams reportParams = new LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
            reportParams.topMargin = 16;
            reportParams.leftMargin = 16;
            addView(reportTextView, reportParams);
        }
    }

    /**
     * Sets report uid.
     *
     * @param uid the uid
     */
    public void setReportUid(int uid) {
        this.reportUid = uid;
    }

    /**
     * Gets report uid.
     *
     * @return the report uid
     */
    public int getReportUid() {
        return reportUid;
    }

    /**
     * Set local audio stats.
     *
     * @param stats the stats
     */
    public void setLocalAudioStats(IRtcEngineEventHandler.LocalAudioStats stats) {
        statisticsInfo.setLocalAudioStats(stats);
        setReportText(statisticsInfo.getLocalVideoStats());
    }

    /**
     * Set local audio stats.
     *
     * @param stats the stats
     */
    public void setLocalAudioStats(IRtcEngineEventHandler.LocalAudioStats stats, String... args) {
        statisticsInfo.setLocalAudioStats(stats);
        String reportText = statisticsInfo.getLocalVideoStats();
        StringBuilder builder = new StringBuilder(reportText);
        for (String arg : args) {
            builder.append(",")
                    .append("\n")
                    .append(arg);
        }
        setReportText(builder.toString());
    }


    /**
     * Set local video stats.
     *
     * @param stats the stats
     */
    public void setLocalVideoStats(IRtcEngineEventHandler.LocalVideoStats stats) {
        if (stats.uid != reportUid) {
            return;
        }
        statisticsInfo.setLocalVideoStats(stats);
        setReportText(statisticsInfo.getLocalVideoStats());
    }

    /**
     * Set local video stats.
     *
     * @param stats the stats
     */
    public void setLocalVideoStats(IRtcEngineEventHandler.LocalVideoStats stats, String... args) {
        if (stats.uid != reportUid) {
            return;
        }
        statisticsInfo.setLocalVideoStats(stats);
        String reportText = statisticsInfo.getLocalVideoStats();
        StringBuilder builder = new StringBuilder(reportText);
        for (String arg : args) {
            builder.append(",")
                    .append("\n")
                    .append(arg);
        }
        setReportText(builder.toString());
    }

    /**
     * Set remote audio stats.
     *
     * @param stats the stats
     */
    public void setRemoteAudioStats(IRtcEngineEventHandler.RemoteAudioStats stats) {
        if (stats.uid != reportUid) {
            return;
        }
        statisticsInfo.setRemoteAudioStats(stats);
        setReportText(statisticsInfo.getRemoteVideoStats());
    }

    /**
     * Set remote audio stats.
     *
     * @param stats the stats
     */
    public void setRemoteAudioStats(IRtcEngineEventHandler.RemoteAudioStats stats, String... args) {
        if (stats.uid != reportUid) {
            return;
        }
        statisticsInfo.setRemoteAudioStats(stats);
        String reportText = statisticsInfo.getRemoteVideoStats();
        StringBuilder builder = new StringBuilder(reportText);
        for (String arg : args) {
            builder.append(",")
                    .append("\n")
                    .append(arg);
        }
        setReportText(builder.toString());
    }


    /**
     * Set remote video stats.
     *
     * @param stats the stats
     */
    public void setRemoteVideoStats(IRtcEngineEventHandler.RemoteVideoStats stats) {
        if (stats.uid != reportUid) {
            return;
        }
        statisticsInfo.setRemoteVideoStats(stats);
        setReportText(statisticsInfo.getRemoteVideoStats());
    }

    /**
     * Set remote video stats.
     *
     * @param stats the stats
     */
    public void setRemoteVideoStats(IRtcEngineEventHandler.RemoteVideoStats stats, String... args) {
        if (stats.uid != reportUid) {
            return;
        }
        statisticsInfo.setRemoteVideoStats(stats);
        String reportText = statisticsInfo.getRemoteVideoStats();
        StringBuilder builder = new StringBuilder(reportText);
        for (String arg : args) {
            builder.append(",")
                    .append("\n")
                    .append(arg);
        }
        setReportText(builder.toString());
    }

    private void setReportText(String reportText) {
        if (reportTextView != null) {
            reportTextView.post(() -> {
                if (reportTextView != null) {
                    reportTextView.setText(reportText);
                }
            });
        }
    }

}
