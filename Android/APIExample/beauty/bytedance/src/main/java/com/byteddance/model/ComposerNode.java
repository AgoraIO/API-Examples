package com.byteddance.model;

/**
 * Created by QunZhang on 2019-07-26 10:53
 */
public class ComposerNode {
    private int id;
    private String node;
    private String key;
    private float value;

    public ComposerNode(int id) {
        this(id, null, null, 0F);
    }

    public ComposerNode(int id, String node) {
        this(id, node, null, 0F);
    }

    public ComposerNode(int id, String node, String key) {
        this(id, node, key, 0F);
    }

    public ComposerNode(int id, String node, String key, float value) {
        this.id = id;
        this.node = node;
        this.key = key;
        this.value = value;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getNode() {
        return node;
    }

    public void setNode(String node) {
        this.node = node;
    }

    public String getKey() {
        return key;
    }

    public void setKey(String key) {
        this.key = key;
    }

    public float getValue() {
        return value;
    }

    public void setValue(float value) {
        this.value = value;
    }

    @Override
    public boolean equals(Object obj) {
        return (obj instanceof ComposerNode) && ((ComposerNode) obj).id == id;
    }

    @Override
    public int hashCode() {
        return id;
    }
}
