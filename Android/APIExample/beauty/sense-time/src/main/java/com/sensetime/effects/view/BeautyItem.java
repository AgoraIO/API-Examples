package com.sensetime.effects.view;

import android.graphics.Bitmap;

public class BeautyItem {

    private int progress;
    private Bitmap unselectedtIcon;
    private Bitmap selectedIcon;
    private String text;

    public BeautyItem(String text, Bitmap unselectedtIcon, Bitmap selectedtIcon){
        this.text = text;
        this.unselectedtIcon = unselectedtIcon;
        this.selectedIcon = selectedtIcon;
    }

    public int getProgress() {
        return progress;
    }

    public void setProgress(int progress) {
        this.progress = progress;
    }

    public Bitmap getUnselectedtIcon() {
        return unselectedtIcon;
    }

    public void setUnselectedtIcon(Bitmap unselectedtIcon) {
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
}
