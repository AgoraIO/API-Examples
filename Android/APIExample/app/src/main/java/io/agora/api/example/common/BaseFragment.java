package io.agora.api.example.common;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.view.View;
import android.widget.Toast;

import androidx.activity.OnBackPressedCallback;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;
import androidx.navigation.Navigation;

/**
 * The type Base fragment.
 */
public class BaseFragment extends Fragment {
    /**
     * The Handler.
     */
    protected Handler handler;
    private AlertDialog mAlertDialog;
    private String mAlertMessage;
    private final OnBackPressedCallback onBackPressedCallback = new OnBackPressedCallback(false) {
        @Override
        public void handleOnBackPressed() {
            onBackPressed();
        }
    };

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        handler = new Handler(Looper.getMainLooper());
        requireActivity().getOnBackPressedDispatcher().addCallback(this, onBackPressedCallback);
    }

    @Override
    public void onAttach(@NonNull Context context) {
        super.onAttach(context);
        onBackPressedCallback.setEnabled(true);
    }

    @Override
    public void onDetach() {
        super.onDetach();
        onBackPressedCallback.setEnabled(false);
    }

    /**
     * Show alert.
     *
     * @param message the message
     */
    protected void showAlert(String message) {
        this.showAlert(message, true);
    }

    /**
     * Show alert.
     *
     * @param message       the message
     * @param showRepeatMsg the show repeat msg
     */
    protected void showAlert(String message, boolean showRepeatMsg) {
        runOnUIThread(() -> {
            Context context = getContext();
            if (context == null) {
                return;
            }
            if (mAlertDialog == null) {
                mAlertDialog = new AlertDialog.Builder(context).setTitle("Tips")
                        .setPositiveButton("OK", (dialog, which) -> dialog.dismiss())
                        .create();
            }
            if (!showRepeatMsg && !TextUtils.isEmpty(mAlertMessage) && mAlertMessage.equals(message)) {
                return;
            }
            mAlertMessage = message;
            mAlertDialog.setMessage(message);
            mAlertDialog.show();
        });
    }

    /**
     * Reset alert.
     */
    protected void resetAlert() {
        runOnUIThread(() -> mAlertMessage = "");
    }

    /**
     * Show long toast.
     *
     * @param msg the msg
     */
    protected final void showLongToast(final String msg) {
        runOnUIThread(() -> {
            Context context = getContext();
            if (context == null) {
                return;
            }
            Toast.makeText(context, msg, Toast.LENGTH_LONG).show();
        });
    }

    /**
     * Show short toast.
     *
     * @param msg the msg
     */
    protected final void showShortToast(final String msg) {
        runOnUIThread(() -> {
            Context context = getContext();
            if (context == null) {
                return;
            }
            Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
        });
    }

    /**
     * Run on ui thread.
     *
     * @param runnable the runnable
     */
    protected final void runOnUIThread(Runnable runnable) {
        this.runOnUIThread(runnable, 0);
    }

    /**
     * Run on ui thread.
     *
     * @param runnable the runnable
     * @param delay    the delay
     */
    protected final void runOnUIThread(Runnable runnable, long delay) {
        if (handler != null && runnable != null && getContext() != null) {
            if (delay <= 0 && handler.getLooper().getThread() == Thread.currentThread()) {
                runnable.run();
            } else {
                handler.postDelayed(() -> {
                    if (getContext() != null) {
                        runnable.run();
                    }
                }, delay);
            }
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
        if (mAlertDialog != null) {
            mAlertDialog.dismiss();
            mAlertDialog = null;
        }
    }

    /**
     * On back pressed.
     */
    protected void onBackPressed() {
        View view = getView();
        if (view != null) {
            Navigation.findNavController(view).navigateUp();
        }
    }
}
