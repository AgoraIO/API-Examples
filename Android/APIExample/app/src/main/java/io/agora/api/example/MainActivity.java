package io.agora.api.example;

import android.content.Intent;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;

import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.Constant;
import io.agora.api.example.common.model.ExampleBean;

/**
 * @author cjw
 */
public class MainActivity extends AppCompatActivity implements MainFragment.OnListFragmentInteractionListener {
    private AppBarConfiguration appBarConfiguration;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment);
        appBarConfiguration = new AppBarConfiguration.Builder(navController.getGraph()).build();
        NavigationUI.setupActionBarWithNavController(this, navController, appBarConfiguration);

        handleIntent(getIntent());
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        handleIntent(intent);
    }

    private void handleIntent(Intent intent) {
        if (intent != null) {
            // 处理从通知栏启动的逻辑
            // 可以添加特定的标志来识别是否是从通知栏启动
        }
    }

    @Override
    public boolean onSupportNavigateUp() {
        getOnBackPressedDispatcher().onBackPressed();
        return true;
    }

    @Override
    public void onListFragmentInteraction(Example item) {
        ExampleBean exampleBean = new ExampleBean(item.index(), item.group(), item.name(), item.actionId(), item.tipsId());
        Bundle bundle = new Bundle();
        bundle.putParcelable(Constant.DATA, exampleBean);
        Navigation.findNavController(this, R.id.nav_host_fragment)
                .navigate(R.id.action_mainFragment_to_Ready, bundle);
    }

}
