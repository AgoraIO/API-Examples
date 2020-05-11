package io.agora.api.example.common;

import android.content.Context;

import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;

public class BaseFragment extends Fragment {
    protected void showAlert(String message) {
        Context context = getContext();
        if (context == null) return;

        new AlertDialog.Builder(context).setTitle("Tips").setMessage(message)
                .setPositiveButton("OK", (dialog, which) -> dialog.dismiss())
                .show();
    }
}
