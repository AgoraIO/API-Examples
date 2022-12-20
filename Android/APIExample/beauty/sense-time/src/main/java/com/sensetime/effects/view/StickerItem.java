package com.sensetime.effects.view;

import android.graphics.Bitmap;
import android.text.TextUtils;

public class StickerItem {
    public String name;
    public Bitmap icon;
    public String path;
    public StickerState state = StickerState.NORMAL_STATE;//0 未下载状态，也是默认状态，1，正在下载状态,2,下载完毕状态

    public StickerItem(String name, Bitmap icon, String path) {
        this.name = name;
        this.icon = icon;
        this.path = path;
        if (TextUtils.isEmpty(this.path)) {
            state = StickerState.NORMAL_STATE;
        } else {
            state = StickerState.DONE_STATE;
        }
    }

    public void recycle() {
        if (icon != null && !icon.isRecycled()) {
            icon.recycle();
            icon = null;
        }
    }
}
