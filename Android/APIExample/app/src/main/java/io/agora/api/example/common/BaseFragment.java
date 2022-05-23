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

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        handler = new Handler(Looper.getMainLooper());
    }

    protected void showAlert(String message)
    {
        handler.post(()->{
            Context context = getContext();
            if (context == null) {
                return;
            }

            new AlertDialog.Builder(context).setTitle("Tips").setMessage(message)
                    .setPositiveButton("OK", (dialog, which) -> dialog.dismiss())
                    .show();
        });
    }

    protected final void showLongToast(final String msg)
    {
        handler.post(new Runnable()
        {
            @Override
            public void run()
            {
                if (BaseFragment.this == null || getContext() == null)
                {return;}
                Toast.makeText(getContext().getApplicationContext(), msg, Toast.LENGTH_LONG).show();
            }
        });
    }
}
