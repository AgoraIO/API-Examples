package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;

import android.os.Bundle;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.navigation.Navigation;

import java.util.ArrayList;
import java.util.List;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.examples.advanced.beauty.ByteDanceBeautySDK;
import io.agora.api.example.examples.advanced.beauty.FaceUnityBeautySDK;
import io.agora.api.example.examples.advanced.beauty.SenseTimeBeautySDK;

/**
 * The type Third party beauty.
 */
@Example(
        index = 24,
        group = ADVANCED,
        name = R.string.item_third_party_beauty,
        actionId = R.id.action_mainFragment_to_third_party_beauty,
        tipsId = R.string.third_party_beauty
)
public class ThirdPartyBeauty extends BaseFragment {

    private EditText etChannel;
    private Spinner snBeautyType;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_third_party_beauty, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        List<Integer> beautyActionIds = new ArrayList<>();
        List<String> beautyLabels = new ArrayList<>();

        SenseTimeBeautySDK.INSTANCE.initBeautySDK(requireContext());
        FaceUnityBeautySDK.INSTANCE.initBeauty(requireContext());
        ByteDanceBeautySDK.INSTANCE.initBeautySDK(requireContext());

        // SceneTime Beauty
        beautyActionIds.add(R.id.action_third_party_beauty_to_scene_time);
        beautyLabels.add(getString(R.string.scenetime_beauty));

        // FaceUnity Beauty
        beautyActionIds.add(R.id.action_third_party_beauty_to_faceunity);
        beautyLabels.add(getString(R.string.faceunity_beauty));

        // ByteDance Beauty
        beautyActionIds.add(R.id.action_third_party_beauty_to_bytedance);
        beautyLabels.add(getString(R.string.bytedance_beauty));

        etChannel = view.findViewById(R.id.et_channel);
        snBeautyType = view.findViewById(R.id.sn_beauty_type);

        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(requireContext(),
                android.R.layout.simple_spinner_dropdown_item,
                beautyLabels);
        snBeautyType.setAdapter(arrayAdapter);

        view.findViewById(R.id.btn_join).setOnClickListener(v -> {
            int index = snBeautyType.getSelectedItemPosition();
            if (index >= beautyActionIds.size()) {
                return;
            }
            String channelName = etChannel.getText().toString();
            if (TextUtils.isEmpty(channelName)) {
                Toast.makeText(getContext(), "The channel name is empty!", Toast.LENGTH_SHORT).show();
                return;
            }
            Bundle args = new Bundle();
            args.putString(getString(R.string.key_channel_name), channelName);
            Navigation.findNavController(view)
                    .navigate(beautyActionIds.get(index), args);
        });
    }


    @Override
    public void onDestroy() {
        super.onDestroy();
        SenseTimeBeautySDK.INSTANCE.unInitBeautySDK();
        FaceUnityBeautySDK.INSTANCE.unInitBeauty();
    }
}
