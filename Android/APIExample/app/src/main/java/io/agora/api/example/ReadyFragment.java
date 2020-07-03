package io.agora.api.example;

import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.widget.AppCompatTextView;

import io.agora.api.component.Constant;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.model.ExampleBean;

public class ReadyFragment extends BaseFragment {
    private static final String TAG = ReadyFragment.class.getSimpleName();

    private AppCompatTextView tips;
    private ExampleBean exampleBean;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        exampleBean = getArguments().getParcelable(Constant.DATA);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_ready_layout, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        tips = view.findViewById(R.id.tips);
        tips.setText(getString(exampleBean.getTipsId()));
        view.findViewById(R.id.next).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        getActivity().onBackPressed();
                    }
                }, 300);
                ExampleActivity.instance(getActivity(), exampleBean);
            }
        });
    }
}
