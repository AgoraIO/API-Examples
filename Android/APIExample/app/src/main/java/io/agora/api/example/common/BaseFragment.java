package io.agora.api.example.common;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;

public class BaseFragment extends Fragment
{
    protected Handler handler;
    private AlertDialog mAlertDialog;
    private String mAlertDialogMsg;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        handler = new Handler(Looper.getMainLooper());
    }

    protected void showAlert(String message) {

        runOnUIThread(() -> {
            Context context = getContext();
            if(context == null){
                return;
            }
            if (mAlertDialog == null) {
                mAlertDialog = new AlertDialog.Builder(context).setTitle("Tips")
                        .setPositiveButton("OK", (dialog, which) -> dialog.dismiss())
                        .create();
            }
            if (!message.equals(mAlertDialogMsg)) {
                mAlertDialogMsg = message;
                mAlertDialog.setMessage(mAlertDialogMsg);
                mAlertDialog.show();
            }
        });
    }

    protected final void showLongToast(final String msg)
    {
        runOnUIThread(() -> {
            Context context = getContext();
            if(context == null){
                return;
            }
            Toast.makeText(context, msg, Toast.LENGTH_LONG).show();
        });
    }

    protected final void runOnUIThread(Runnable runnable){
        this.runOnUIThread(runnable, 0);
    }

    protected final void runOnUIThread(Runnable runnable, long delay){
        if(handler != null && runnable != null && getContext() != null){
            if (delay <= 0 && handler.getLooper().getThread() == Thread.currentThread()) {
                runnable.run();
            }else{
                handler.postDelayed(() -> {
                    if(getContext() != null){
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
        if(mAlertDialog != null){
            mAlertDialog.dismiss();
            mAlertDialog = null;
        }
    }
}
