package io.agora.api.example.examples.advanced.beauty;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import io.agora.api.example.R;
import io.agora.api.example.common.BaseFragment;
import io.agora.beauty.base.IBeautySenseTime;

public class SceneTimeBeauty extends BaseFragment {

    private IBeautySenseTime iBeautySenseTime;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_scenetime_beauty, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        iBeautySenseTime = IBeautySenseTime.create(getContext());
        if(iBeautySenseTime == null){
            // TODO show the readme of compiling scene-time library.
            initReadMeView();
            return;
        }

        initVideoView();
        initRtcEngine();
    }

    private void initReadMeView() {

    }

    private void initVideoView() {

    }

    private void initRtcEngine() {

    }
}
