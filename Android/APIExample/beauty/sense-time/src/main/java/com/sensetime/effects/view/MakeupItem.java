package com.sensetime.effects.view;

import android.graphics.Bitmap;

public class MakeupItem {
    public String name;
    public Bitmap icon;
    public String path;

    public MakeupItem(String name, Bitmap icon, String path) {
        this.name = name;
        this.icon = icon;
        this.path = path;
    }
}
