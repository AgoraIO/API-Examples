package io.agora.api.example;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;

import io.agora.api.component.Constant;
import io.agora.api.example.annotation.Example;
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
    }

    @Override
    public boolean onSupportNavigateUp() {
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment);
        return NavigationUI.navigateUp(navController, appBarConfiguration)
                || super.onSupportNavigateUp();
    }

    @Override
    public void onListFragmentInteraction(Example item) {
        ExampleBean exampleBean = new ExampleBean(item.index(), item.group(), item.name(), item.actionId(), item.tipsId());
        Bundle bundle = new Bundle();
        bundle.putParcelable(Constant.DATA, exampleBean);
        try {
            Navigation.findNavController(this, R.id.nav_host_fragment)
                    .navigate(R.id.action_mainFragment_to_Ready, bundle);
        } catch (Exception e) {
            Log.e(this.getClass().getSimpleName(), "", e);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main_activity, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        if (item.getItemId() == R.id.setting) {
            startActivity(new Intent(this, SettingActivity.class));
        }
        return false;
    }
}
