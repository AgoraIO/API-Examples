package com.sensetime.effects.view;

import android.graphics.Bitmap;

public class MediaItem {

    public String name;
    public Bitmap icon;
    public String model;

    public MediaItem(String name, Bitmap icon, String modelName) {
        this.name = name;
        this.icon = icon;
        this.model = modelName;
    }

    public MediaItem() {
    }

    public void setThumb(Bitmap image){
        this.icon = image;
    }

    public Bitmap getThumb(){
        return icon;
    }

    public void setVideoPath(String path){
        this.name = path;
    }

    public String getVideoPath(){
        return name;
    }

}
