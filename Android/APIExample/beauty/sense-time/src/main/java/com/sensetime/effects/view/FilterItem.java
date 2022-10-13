package com.sensetime.effects.view;

import android.graphics.Bitmap;

public class FilterItem {
    public String name;
    public Bitmap icon;
    public String model;

    public FilterItem(String name, Bitmap icon, String modelName) {
        this.name = name;
        this.icon = icon;
        this.model = modelName;
    }
}
