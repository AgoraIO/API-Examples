package io.agora.api.example;

import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.navigation.ActionOnlyNavDirections;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;

import io.agora.api.example.annotation.Example;

public class MainActivity extends AppCompatActivity implements MainFragment.OnListFragmentInteractionListener
{
    private AppBarConfiguration appBarConfiguration;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment);
        appBarConfiguration = new AppBarConfiguration.Builder(navController.getGraph()).build();
        NavigationUI.setupActionBarWithNavController(this, navController, appBarConfiguration);
    }

    @Override
    public boolean onSupportNavigateUp()
    {
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment);
        return NavigationUI.navigateUp(navController, appBarConfiguration)
                || super.onSupportNavigateUp();
    }

    @Override
    public void onListFragmentInteraction(Example item)
    {
        Navigation.findNavController(this, R.id.nav_host_fragment).navigate(new ActionOnlyNavDirections(item.actionId()));
    }
}
