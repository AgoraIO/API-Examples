package io.agora.api.example;

import android.Manifest;
import android.annotation.SuppressLint;
import android.os.Build;
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

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import java.util.ArrayList;
import java.util.List;

import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.Constant;
import io.agora.api.example.common.model.ExampleBean;

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
            runOnPermissionGranted(new Runnable() {
                @Override
                public void run() {
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
            });
        });
    }

    @SuppressLint("WrongConstant")
    private void runOnPermissionGranted(@NonNull Runnable runnable) {
        List<String> permissionList = new ArrayList<>();
        permissionList.add(Permission.READ_EXTERNAL_STORAGE);
        permissionList.add(Permission.WRITE_EXTERNAL_STORAGE);
        permissionList.add(Permission.RECORD_AUDIO);
        permissionList.add(Permission.CAMERA);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            permissionList.add(Permission.READ_PHONE_STATE);
            permissionList.add(Manifest.permission.BLUETOOTH_CONNECT);
        }

        String[] permissionArray = new String[permissionList.size()];
        permissionList.toArray(permissionArray);

        if (AndPermission.hasPermissions(this, permissionArray)) {
            runnable.run();
            return;
        }
        // Request permission
        AndPermission.with(this).runtime().permission(
                permissionArray
        ).onGranted(permissions -> {
            // Permissions Granted
            runnable.run();
        }).start();
    }
}
