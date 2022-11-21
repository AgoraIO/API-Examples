package com.bytedance.labcv.common.model;

import java.io.Serializable;

/**
 * Created on 5/11/21 8:51 PM
 */
public class ButtonItem implements Serializable {
    private int title;
    private int icon;
    private int desc;

    public ButtonItem() {
    }
    public ButtonItem(int icon, int desc) {
        this.icon = icon;
        this.desc = desc;
    }

    public ButtonItem(int title, int icon, int desc) {
        this.title = title;
        this.icon = icon;
        this.desc = desc;
    }

    public int getTitle() {
        return title;
    }

    public ButtonItem setTitle(int title) {
        this.title = title;
        return this;
    }

    public int getIcon() {
        return icon;
    }

    public ButtonItem setIcon(int icon) {
        this.icon = icon;
        return this;
    }

    public int getDesc() {
        return desc;
    }

    public ButtonItem setDesc(int desc) {
        this.desc = desc;
        return this;
    }
}
