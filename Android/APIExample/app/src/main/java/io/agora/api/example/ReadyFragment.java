package io.agora.api.example;


import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.AppCompatTextView;
import androidx.navigation.NavController;
import androidx.navigation.NavDestination;
import androidx.navigation.Navigation;

import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.Constant;
import io.agora.api.example.common.model.ExampleBean;
import io.agora.api.example.utils.PermissonUtils;

/**
 * @author cjw
 */
public class ReadyFragment extends BaseFragment {
    private static final String TAG = ReadyFragment.class.getSimpleName();

    private AppCompatTextView tips;
    private ExampleBean exampleBean;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
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
        ActionBar actionBar = ((AppCompatActivity) getActivity()).getSupportActionBar();
        if (actionBar != null) {
            actionBar.setTitle(exampleBean.getName());
            actionBar.setHomeButtonEnabled(true);
            actionBar.setDisplayHomeAsUpEnabled(true);
        }

        tips = view.findViewById(R.id.tips);
        tips.setText(getString(exampleBean.getTipsId()));
        view.findViewById(R.id.next).setOnClickListener(v -> {
            checkOrRequestPermisson(new PermissonUtils.PermissionResultCallback() {
                @Override
                public void onPermissionsResult(boolean allPermissionsGranted, String[] permissions, int[] grantResults) {
                    if (allPermissionsGranted) {
                        navigationDest();
                    } else {
                        showLongToast(getString(R.string.permission));
                    }
                }
            });
        });
    }

    private void navigationDest() {
        NavController navController = Navigation.findNavController(requireView());
        navController.navigate(exampleBean.getActionId());
        navController.addOnDestinationChangedListener(new NavController.OnDestinationChangedListener() {
            @Override
            public void onDestinationChanged(@NonNull NavController controller,
                                             @NonNull NavDestination destination,
                                             @Nullable Bundle arguments) {
                if (destination.getId() == R.id.Ready) {
                    controller.navigateUp();
                    controller.removeOnDestinationChangedListener(this);
                }
            }
        });
    }


}
