package io.agora.api.example;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import io.agora.api.example.common.model.GlobalSettings;
import io.agora.api.example.databinding.ActivitySettingLayoutBinding;
import io.agora.rtc2.RtcEngine;

/**
 * @author cjw
 */
public class SettingActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener {
    private static final String TAG = SettingActivity.class.getSimpleName();

    private ActivitySettingLayoutBinding mBinding;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mBinding = ActivitySettingLayoutBinding.inflate(LayoutInflater.from(this));
        setContentView(mBinding.getRoot());
        mBinding.sdkVersion.setText(String.format(getString(R.string.sdkversion1), RtcEngine.getSdkVersion()));
        String[] mItems = getResources().getStringArray(R.array.orientations);
        String[] labels = new String[mItems.length];
        for(int i = 0;i<mItems.length;i++){
            int resId = getResources().getIdentifier( mItems[i], "string", getPackageName() );
            labels[i] = getString(resId);
        }
        ArrayAdapter<String> arrayAdapter =new ArrayAdapter<String>(this,android.R.layout.simple_spinner_dropdown_item, labels);
        mBinding.orientationSpinner.setAdapter(arrayAdapter);
        mBinding.orientationSpinner.setOnItemSelectedListener(this);
        mBinding.frameRateSpinner.setOnItemSelectedListener(this);
        mBinding.dimensionSpinner.setOnItemSelectedListener(this);
        mBinding.areaSpinner.setOnItemSelectedListener(this);
        fetchGlobalSettings();
    }

    private void fetchGlobalSettings(){
        GlobalSettings globalSettings = ((MainApplication)getApplication()).getGlobalSettings();

        String[] mItems = getResources().getStringArray(R.array.orientations);
        String selectedItem = globalSettings.getVideoEncodingOrientation();
        int i = 0;
        if(selectedItem!=null){
            for(String item : mItems){
                if(selectedItem.equals(item)){
                    break;
                }
                i++;
            }
        }
        mBinding.orientationSpinner.setSelection(i);
        mItems = getResources().getStringArray(R.array.fps);
        selectedItem = globalSettings.getVideoEncodingFrameRate();
        i = 0;
        if(selectedItem!=null){
            for(String item : mItems){
                if(selectedItem.equals(item)){
                    break;
                }
                i++;
            }
        }
        mBinding.frameRateSpinner.setSelection(i);
        mItems = getResources().getStringArray(R.array.dimensions);
        selectedItem = globalSettings.getVideoEncodingDimension();
        i = 0;
        if(selectedItem!=null){
            for(String item : mItems){
                if(selectedItem.equals(item)){
                    break;
                }
                i++;
            }
        }
        mBinding.dimensionSpinner.setSelection(i);
        mItems = getResources().getStringArray(R.array.areaCode);
        selectedItem = globalSettings.getAreaCodeStr();
        i = 0;
        if(selectedItem!=null){
            for(String item : mItems){
                if(selectedItem.equals(item)){
                    break;
                }
                i++;
            }
        }
        mBinding.areaSpinner.setSelection(i);


        mBinding.privateCloudLayout.etIpAddress.setText(globalSettings.privateCloudIp);
        mBinding.privateCloudLayout.swLogReport.setChecked(globalSettings.privateCloudLogReportEnable);
        mBinding.privateCloudLayout.etLogServerDomain.setText(globalSettings.privateCloudLogServerDomain);
        mBinding.privateCloudLayout.etLogServerPort.setText(globalSettings.privateCloudLogServerPort + "");
        mBinding.privateCloudLayout.etLogServerPath.setText(globalSettings.privateCloudLogServerPath);
        mBinding.privateCloudLayout.swUseHttps.setChecked(globalSettings.privateCloudUseHttps);
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();

        GlobalSettings globalSettings = ((MainApplication)getApplication()).getGlobalSettings();
        globalSettings.privateCloudIp = mBinding.privateCloudLayout.etIpAddress.getText().toString();
        globalSettings.privateCloudLogReportEnable = mBinding.privateCloudLayout.swLogReport.isChecked();
        globalSettings.privateCloudLogServerDomain = mBinding.privateCloudLayout.etLogServerDomain.getText().toString();
        globalSettings.privateCloudLogServerPort = Integer.parseInt(mBinding.privateCloudLayout.etLogServerPort.getText().toString());
        globalSettings.privateCloudLogServerPath = mBinding.privateCloudLayout.etLogServerPath.getText().toString();
        globalSettings.privateCloudUseHttps = mBinding.privateCloudLayout.swUseHttps.isChecked();
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
        else if(adapterView.getId() == R.id.area_spinner){
            GlobalSettings globalSettings = ((MainApplication)getApplication()).getGlobalSettings();
            globalSettings.setAreaCodeStr(getResources().getStringArray(R.array.areaCode)[i]);
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> adapterView) {

    }
}
