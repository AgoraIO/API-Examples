package com.sensetime.effects.view;

public class FilterInfo {
    private String type;
    private String filterName;
    private int strength;

    public FilterInfo(String type, String filterName, int strength){
        this.type = type;
        this.filterName = filterName;
        this.strength = strength;
    }

    public String getFilterName() {
        return filterName;
    }

    public int getStrength() {
        return strength;
    }

    public String getType() {
        return type;
    }
}
