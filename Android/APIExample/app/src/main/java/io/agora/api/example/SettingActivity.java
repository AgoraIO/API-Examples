package io.agora.api.example;

import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;

import androidx.annotation.Nullable;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.AppCompatSpinner;
import androidx.appcompat.widget.AppCompatTextView;

import io.agora.api.example.common.model.GlobalSettings;
import io.agora.rtc.RtcEngine;

/**
 * @author cjw
 */
public class SettingActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener {
    private static final String TAG = SettingActivity.class.getSimpleName();

    private AppCompatTextView sdkVersion;
    private AppCompatSpinner orientationSpinner, fpsSpinner, dimensionSpinner;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting_layout);
        ActionBar actionBar = getSupportActionBar();
        if(actionBar != null)
        {
            actionBar.setHomeButtonEnabled(true);
            actionBar.setDisplayHomeAsUpEnabled(true);
        }
        sdkVersion = findViewById(R.id.sdkVersion);
        sdkVersion.setText(String.format(getString(R.string.sdkversion1), RtcEngine.getSdkVersion()));
        orientationSpinner = findViewById(R.id.orientation_spinner);
        fpsSpinner = findViewById(R.id.frame_rate_spinner);
        dimensionSpinner = findViewById(R.id.dimension_spinner);
        String[] mItems = getResources().getStringArray(R.array.orientations);
        String[] labels = new String[mItems.length];
        for(int i = 0;i<mItems.length;i++){
            int resId = getResources().getIdentifier( mItems[i], "string", getPackageName() );
            labels[i] = getString(resId);
        }
        ArrayAdapter<String> arrayAdapter =new ArrayAdapter<String>(this,android.R.layout.simple_spinner_dropdown_item, labels);
        orientationSpinner.setAdapter(arrayAdapter);
        orientationSpinner.setOnItemSelectedListener(this);
        fpsSpinner.setOnItemSelectedListener(this);
        dimensionSpinner.setOnItemSelectedListener(this);
        fetchGlobalSettings();
    }

    private void fetchGlobalSettings(){
        String[] mItems = getResources().getStringArray(R.array.orientations);
        String selectedItem = ((MainApplication) getApplication()).getGlobalSettings().getVideoEncodingOrientation();
        int i = 0;
        if(selectedItem!=null){
            for(String item : mItems){
                if(selectedItem.equals(item)){
                    break;
                }
                i++;
            }
        }
        orientationSpinner.setSelection(i);
        mItems = getResources().getStringArray(R.array.fps);
        selectedItem = ((MainApplication) getApplication()).getGlobalSettings().getVideoEncodingFrameRate();
        i = 0;
        if(selectedItem!=null){
            for(String item : mItems){
                if(selectedItem.equals(item)){
                    break;
                }
                i++;
            }
        }
        fpsSpinner.setSelection(i);
        mItems = getResources().getStringArray(R.array.dimensions);
        selectedItem = ((MainApplication) getApplication()).getGlobalSettings().getVideoEncodingDimension();
        i = 0;
        if(selectedItem!=null){
            for(String item : mItems){
                if(selectedItem.equals(item)){
                    break;
                }
                i++;
            }
        }
        dimensionSpinner.setSelection(i);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            this.finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
        if(adapterView.getId() == R.id.orientation_spinner){
            GlobalSettings globalSettings = ((MainApplication)getApplication()).getGlobalSettings();
            globalSettings.setVideoEncodingOrientation(getResources().getStringArray(R.array.orientations)[i]);
        }
        else if(adapterView.getId() == R.id.frame_rate_spinner){
            GlobalSettings globalSettings = ((MainApplication)getApplication()).getGlobalSettings();
            globalSettings.setVideoEncodingFrameRate(getResources().getStringArray(R.array.fps)[i]);
        }
        else if(adapterView.getId() == R.id.dimension_spinner){
            GlobalSettings globalSettings = ((MainApplication)getApplication()).getGlobalSettings();
            globalSettings.setVideoEncodingDimension(getResources().getStringArray(R.array.dimensions)[i]);
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> adapterView) {

    }
}
