package com.sensetime.effects.view;

import android.graphics.Bitmap;

/**
 * Created by cuishiwang on 17/08/2017.
 */

public class StickerOptionsItem {
    public String name;
    public Bitmap unselectedtIcon;
    public Bitmap selectedtIcon;

    public StickerOptionsItem(String name, Bitmap unselectedtIcon, Bitmap selectedtIcon) {
        this.name = name;
        this.unselectedtIcon = unselectedtIcon;
        this.selectedtIcon = selectedtIcon;
    }
}
