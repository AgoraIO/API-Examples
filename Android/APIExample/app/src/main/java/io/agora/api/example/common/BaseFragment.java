package io.agora.api.example.common;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;
import android.widget.Toast;

import androidx.activity.OnBackPressedCallback;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;
import androidx.navigation.Navigation;

public class BaseFragment extends Fragment {
    protected Handler handler;
    private AlertDialog mAlertDialog;
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
        requireActivity().getOnBackPressedDispatcher().addCallback(onBackPressedCallback);
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

    protected void showAlert(String message) {

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
            mAlertDialog.setMessage(message);
            mAlertDialog.show();
        });
    }

    protected final void showLongToast(final String msg) {
        runOnUIThread(() -> {
            Context context = getContext();
            if (context == null) {
                return;
            }
            Toast.makeText(context, msg, Toast.LENGTH_LONG).show();
        });
    }

    protected final void showShortToast(final String msg) {
        runOnUIThread(() -> {
            Context context = getContext();
            if (context == null) {
                return;
            }
            Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
        });
    }

    protected final void runOnUIThread(Runnable runnable) {
        this.runOnUIThread(runnable, 0);
    }

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

    protected void onBackPressed() {
        View view = getView();
        if (view != null) {
            Navigation.findNavController(view).navigateUp();
        }
    }
}
