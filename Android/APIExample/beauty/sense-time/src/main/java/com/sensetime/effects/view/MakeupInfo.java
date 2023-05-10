package com.sensetime.effects.view;

public class MakeupInfo {
    private String type;
    private String makeupName;
    private int strength;

    public MakeupInfo(String type, String makeupItem, int strength){
        this.type = type;
        this.makeupName = makeupItem;
        this.strength = strength;
    }

    public String getMakeupName() {
        return makeupName;
    }

    public int getStrength() {
        return strength;
    }

    public String getType() {
        return type;
    }
}
