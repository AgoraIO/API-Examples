package io.agora.api.example.examples.audio;

import static io.agora.api.example.common.model.Examples.ADVANCED;

import android.os.Bundle;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.navigation.Navigation;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.databinding.FragmentAudiorouterPlayerBinding;

/**
 * The type Audio router player.
 */
@Example(
        index = 17,
        group = ADVANCED,
        name = R.string.item_audiorouter_player,
        actionId = R.id.action_mainFragment_to_AudioRouterPlayer,
        tipsId = R.string.audiorouter_palyer
)
public class AudioRouterPlayer extends BaseFragment {
    private static final String TAG = "AudioRouterPlayer";
    private FragmentAudiorouterPlayerBinding mBinding;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = FragmentAudiorouterPlayerBinding.inflate(inflater);
        return mBinding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        mBinding.btnJoin.setOnClickListener(v -> {
            String channelId = mBinding.etChannel.getText().toString();
            if (TextUtils.isEmpty(channelId)) {
                showAlert("Please enter channel id firstly!");
                return;
            }
            String player = (String) mBinding.spPlayer.getSelectedItem();
            Bundle args = new Bundle();
            args.putString("channelId", channelId);
            if (player.equals(getString(R.string.third_party_player_ijk))) {
                Navigation.findNavController(requireView()).navigate(R.id.action_AudioRouterPlayer_to_AudioRouterPlayerIjk, args);
            } else if (player.equals(getString(R.string.third_party_player_exo))) {
                Navigation.findNavController(requireView()).navigate(R.id.action_AudioRouterPlayer_to_AudioRouterPlayerExo, args);
            } else {
                Navigation.findNavController(requireView()).navigate(R.id.action_AudioRouterPlayer_to_AudioRouterPlayerNative, args);
            }
        });
    }

}
