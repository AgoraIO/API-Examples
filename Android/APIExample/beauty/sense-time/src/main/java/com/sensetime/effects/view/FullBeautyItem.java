package com.sensetime.effects.view;

import android.graphics.Bitmap;

import java.util.ArrayList;

public class FullBeautyItem {

    private Bitmap unselectedtIcon;
    private Bitmap selectedIcon;
    private String text;
    private int makeupAllProgress = 100;
    private int filterProgress = 100;

    //美颜参数
    private float[] beautifyParams = {0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f};

    //美妆
    private ArrayList<MakeupInfo> makeupList;

    //滤镜参数
    private FilterInfo filter;

    public FullBeautyItem(String text, Bitmap unselectedIcon, Bitmap selectedIcon, float[] beautifyParams, ArrayList makeupList,
                          FilterInfo filter){
        this.text = text;
        this.unselectedtIcon = unselectedIcon;
        this.selectedIcon = selectedIcon;
        this.beautifyParams = beautifyParams;
        this.makeupList = makeupList;
        this.filter = filter;
    }

    public Bitmap getUnselectedIcon() {
        return unselectedtIcon;
    }

    public void setUnselectedIcon(Bitmap unselectedIcon) {
        this.unselectedtIcon = unselectedtIcon;
    }

    public Bitmap getSelectedIcon() {
        return selectedIcon;
    }

    public void setSelectedIcon(Bitmap selectedIcon) {
        this.selectedIcon = selectedIcon;
    }

    public String getText() {
        return text;
    }

    public void setText(String text) {
        this.text = text;
    }

    public float[] getBeautifyParams(){
        return beautifyParams;
    }

    public ArrayList<MakeupInfo> getMakeupList(){
        return makeupList;
    }

    public FilterInfo getFilter(){
        return filter;
    }

    public int getMakeupAllProgress() {
        return makeupAllProgress;
    }

    public void setMakeupAllProgress(int makeupAllProgress) {
        this.makeupAllProgress = makeupAllProgress;
    }

    public int getFilterProgress() {
        return filterProgress;
    }

    public void setFilterProgress(int filterProgress) {
        this.filterProgress = filterProgress;
    }
}
