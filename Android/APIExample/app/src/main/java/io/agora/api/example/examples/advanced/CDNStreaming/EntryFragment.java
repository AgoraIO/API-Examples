package io.agora.api.example.examples.advanced.CDNStreaming;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.Spinner;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;

import static io.agora.api.example.common.model.Examples.ADVANCED;

@Example(
        index = 4,
        group = ADVANCED,
        name = R.string.item_rtmpstreaming,
        actionId = R.id.action_mainFragment_to_RTMPStreaming,
        tipsId = R.string.rtmpstreaming
)
public class EntryFragment extends BaseFragment implements View.OnClickListener
{
    private static final String TAG = EntryFragment.class.getSimpleName();
    private Spinner streamMode;
    private EditText et_channel;

    private boolean isAgoraChannel(){
        return "AGORA_CHANNEL".equals(streamMode.getSelectedItem().toString());
    }

    private String getChannelName(){
        return et_channel.getText().toString();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState)
    {
        View view = inflater.inflate(R.layout.fragment_cdn_entry, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {
        super.onViewCreated(view, savedInstanceState);
        view.findViewById(R.id.btn_host_join).setOnClickListener(this);
        view.findViewById(R.id.btn_audience_join).setOnClickListener(this);
        et_channel = view.findViewById(R.id.et_channel);
        streamMode = view.findViewById(R.id.streamModeSpinner);
        streamMode.setOnItemSelectedListener(new StreamModeOnItemSelectedListener());
    }

    private class  StreamModeOnItemSelectedListener implements AdapterView.OnItemSelectedListener {
        @Override
        public void onItemSelected(AdapterView<?> adapter, View view, int position, long id) {
            et_channel.setHint(position == 0 ? R.string.agora_channel_hint : R.string.cdn_url_hint);
        }

        @Override
        public void onNothingSelected(AdapterView<?> arg0) {
        }
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
    }

    @Override
    public void onClick(View v)
    {
        // Check permission
        if (AndPermission.hasPermissions(this, Permission.Group.STORAGE, Permission.Group.MICROPHONE, Permission.Group.CAMERA))
        {
            join(v);
            return;
        }
        // Request permission
        AndPermission.with(this).runtime().permission(
                Permission.Group.STORAGE,
                Permission.Group.MICROPHONE,
                Permission.Group.CAMERA
        ).onGranted(permissions ->
        {
            // Permissions Granted
            join(v);
        }).start();
    }

    private void join(View v) {
        if (v.getId() == R.id.btn_host_join){
            HostFragment fragment = new HostFragment();
            Bundle bundle = new Bundle();
            bundle.putString(getString(R.string.key_channel_name), getChannelName());
            bundle.putBoolean(getString(R.string.key_is_agora_channel), isAgoraChannel());
            fragment.setArguments(bundle);
            getActivity().getSupportFragmentManager().beginTransaction()
                    .replace(R.id.fragment_Layout, fragment)
                    .commit();
        }
        else if (v.getId() == R.id.btn_audience_join){
            AudienceFragment fragment = new AudienceFragment();
            Bundle bundle = new Bundle();
            bundle.putString(getString(R.string.key_channel_name), getChannelName());
            bundle.putBoolean(getString(R.string.key_is_agora_channel), isAgoraChannel());
            fragment.setArguments(bundle);
            getActivity().getSupportFragmentManager().beginTransaction()
                    .replace(R.id.fragment_Layout, fragment)
                    .commit();
        }
    }
}
