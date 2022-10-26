package io.agora.beauty.bytedance;

import com.byteddance.model.ComposerNode;

public class ExternParam {
    private ComposerNode[] nodes;
    private FilterItem filter;
    private String sticker;
    private String media;
    private int imgDur;


    public String[] getNodeArray(){
        if (null == nodes || nodes.length == 0){
            return null;
        }
        int length = nodes.length;
        String[] nodeArray = new String[length];
        for (int i  = 0; i < length;i++){
            nodeArray[i] = nodes[i].getNode();
        }
        return nodeArray;
    }

    public ComposerNode[] getNodes() {
        return nodes;
    }

    public void setNodes(ComposerNode[] nodes) {
        this.nodes = nodes;
    }

    public FilterItem getFilter() {
        return filter;
    }

    public void setFilter(FilterItem filter) {
        this.filter = filter;
    }

    public String getSticker() {
        return sticker;
    }

    public void setSticker(String sticker) {
        this.sticker = sticker;
    }

    public String getMedia() {
        return media;
    }

    public void setMedia(String media) {
        this.media = media;
    }

    public int getImgDur() {
        return imgDur;
    }

    public void setImgDur(int imgDur) {
        this.imgDur = imgDur;
    }


    public static class FilterItem {
        private String key;
        private float value;

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
    }
}
