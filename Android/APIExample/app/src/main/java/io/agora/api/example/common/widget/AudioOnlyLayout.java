package io.agora.api.example.common.widget;

import android.content.Context;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.Map;

import io.agora.api.example.R;

/**
 * The type Audio only layout.
 */
public class AudioOnlyLayout extends FrameLayout {

    private TextView tvUserType, tvUserId;
    private TableLayout tlState;

    /**
     * Instantiates a new Audio only layout.
     *
     * @param context the context
     */
    public AudioOnlyLayout(@NonNull Context context) {
        this(context, null);
    }

    /**
     * Instantiates a new Audio only layout.
     *
     * @param context the context
     * @param attrs   the attrs
     */
    public AudioOnlyLayout(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Instantiates a new Audio only layout.
     *
     * @param context      the context
     * @param attrs        the attrs
     * @param defStyleAttr the def style attr
     */
    public AudioOnlyLayout(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initView();
    }


    private void initView() {
        View rootView = View.inflate(getContext(), R.layout.widget_audio_only_layout, this);
        tvUserType = rootView.findViewById(R.id.tv_user_type);
        tvUserId = rootView.findViewById(R.id.tv_user_id);
        tlState = rootView.findViewById(R.id.table_layout_state);
    }

    /**
     * Update user info.
     *
     * @param uid     the uid
     * @param isLocal the is local
     */
    public void updateUserInfo(String uid, boolean isLocal) {
        tvUserId.setText(uid + "");
        tvUserType.setText(isLocal ? "Local" : "Remote");
    }

    /**
     * Update stats.
     *
     * @param states the states
     */
    public void updateStats(Map<String, String> states) {
        tlState.removeAllViews();
        if (states == null || states.size() <= 0) {
            return;
        }

        for (Map.Entry<String, String> entry : states.entrySet()) {
            TableRow row = new TableRow(getContext());
            TextView keyTv = new TextView(getContext());
            keyTv.setTextSize(10);
            keyTv.setTextColor(Color.BLACK);
            keyTv.setText(entry.getKey());
            row.addView(keyTv);
            TextView valueTv = new TextView(getContext());
            valueTv.setTextSize(10);
            valueTv.setTextColor(Color.BLACK);
            valueTv.setText(entry.getValue());
            row.addView(valueTv);
            tlState.addView(row);
        }
    }
}
