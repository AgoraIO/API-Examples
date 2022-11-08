package io.agora.beauty.bytedance;

import static io.agora.beauty.bytedance.ItemGetContract.*;

import android.content.Context;
import android.opengl.GLES11Ext;
import android.text.TextUtils;

import com.byteddance.effect.EffectHelper;
import com.byteddance.effect.ResourceHelper;
import com.byteddance.model.ComposerNode;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import io.agora.beauty.base.IBeautyByteDance;

public class BeautyByteDanceImpl implements IBeautyByteDance {

    private final EffectHelper mEffectHelper;
    private final Context mContext;
    private volatile boolean isReleased = false;
    private final ExternParam externParam = new ExternParam();

    public BeautyByteDanceImpl(Context context){
        mContext = context;
        mEffectHelper = new EffectHelper(context);
        mEffectHelper.initEffectSDK();

        initFilters();
    }

    private void  initFilters(){
        mEffectHelper.setFilter(getDefaultFilterPath());
    }

    private String getDefaultFilterPath() {
        File[] filters = ResourceHelper.getFilterResources(mContext);
        if (filters != null && filters.length > 0) {
            File def = filters[0];
            return def.getAbsolutePath();
        } else {
            return "";
        }
    }

    @Override
    public int process(int oesTexId, int width, int height, int rotation) {
        if(isReleased){
            return -1;
        }
        return mEffectHelper.processTexture(
                oesTexId,
                GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                width,
                height,
                rotation,
                System.currentTimeMillis()
        );
    }

    @Override
    public void release() {
        isReleased = true;
        mEffectHelper.destroyEffectSDK();
    }

    @Override
    public void setFaceBeautifyEnable(boolean enable) {
        if(isReleased){
            return;
        }
        List<ComposerNode> list = new ArrayList<>();
        if(!enable){
            if(externParam.getNodes()!=null)
                list.addAll(Arrays.asList(externParam.getNodes()));
            list.add(new ComposerNode(TYPE_BEAUTY_FACE_SMOOTH, NODE_BEAUTY_LIVE, "smooth", 0));
            list.add(new ComposerNode(TYPE_BEAUTY_FACE_WHITEN, NODE_BEAUTY_LIVE, "whiten", 0));
            list.add(new ComposerNode(TYPE_BEAUTY_FACE_SHARPEN, NODE_BEAUTY_LIVE, "sharp", 0));
            externParam.setFilter(null);
        }
        else{
            if(externParam.getNodes()!=null)
                list.addAll(Arrays.asList(externParam.getNodes()));
            list.add(new ComposerNode(TYPE_BEAUTY_FACE_SMOOTH, NODE_BEAUTY_LIVE, "smooth", 1));
            list.add(new ComposerNode(TYPE_BEAUTY_FACE_WHITEN, NODE_BEAUTY_LIVE, "whiten", 1));
            list.add(new ComposerNode(TYPE_BEAUTY_FACE_SHARPEN, NODE_BEAUTY_LIVE, "sharp", 1));
            ExternParam.FilterItem filterItem = new ExternParam.FilterItem();
            filterItem.setKey("Filter_01_38");
            filterItem.setValue(0.6f);
            externParam.setFilter(filterItem);
        }
        externParam.setNodes(list.toArray(new ComposerNode[list.size()]));
        updateEffectsByParam();
    }

    @Override
    public void setMakeUpEnable(boolean enable) {
        if(isReleased){
            return;
        }
        List<ComposerNode> list = new ArrayList<>();
        if(!enable){
            if(externParam.getNodes()!=null)
                list.addAll(Arrays.asList(externParam.getNodes()));
            list.add(new ComposerNode(TYPE_MAKEUP_BLUSHER, "blush/richang", "Internal_Makeup_Blusher", 0));
            list.add(new ComposerNode(TYPE_MAKEUP_LIP, "lip/sironghong", "Internal_Makeup_Lips", 0));
            list.add(new ComposerNode(TYPE_MAKEUP_PUPIL, "pupil/hunxuezong", "Internal_Makeup_Pupil", 0));
            list.add(new ComposerNode(TYPE_MAKEUP_EYESHADOW, "eyeshadow/dadizong", "Internal_Makeup_Eye", 0));
            list.add(new ComposerNode(TYPE_MAKEUP_EYEBROW, "eyebrow/BK01", "Internal_Makeup_Brow", 0));
        }
        else{
            if(externParam.getNodes()!=null)
                list.addAll(Arrays.asList(externParam.getNodes()));
            list.add(new ComposerNode(TYPE_MAKEUP_BLUSHER, "blush/richang", "Internal_Makeup_Blusher", 1));
            list.add(new ComposerNode(TYPE_MAKEUP_LIP, "lip/sironghong", "Internal_Makeup_Lips", 1));
            list.add(new ComposerNode(TYPE_MAKEUP_PUPIL, "pupil/hunxuezong", "Internal_Makeup_Pupil", 1));
            list.add(new ComposerNode(TYPE_MAKEUP_EYESHADOW, "eyeshadow/dadizong", "Internal_Makeup_Eye", 1));
            list.add(new ComposerNode(TYPE_MAKEUP_EYEBROW, "eyebrow/BK01", "Internal_Makeup_Brow", 1));
        }
        externParam.setNodes(list.toArray(new ComposerNode[list.size()]));
        updateEffectsByParam();
    }

    @Override
    public void setStickerEnable(boolean enable) {
        if(isReleased){
            return;
        }
        if(!enable){
            externParam.setSticker(null);
        }
        else {
            externParam.setSticker(ResourceHelper.getStickerPath(mContext, "matting_bg"));
        }
        updateEffectsByParam();
    }

    @Override
    public void setBodyBeautifyEnable(boolean enable) {
        List<ComposerNode> list = new ArrayList<>();
        if(!enable){
            if(externParam.getNodes()!=null)
                list.addAll(Arrays.asList(externParam.getNodes()));
            list.add(new ComposerNode(TYPE_BEAUTY_BODY_THIN, NODE_ALL_SLIM, "BEF_BEAUTY_BODY_THIN", 0));
            list.add(new ComposerNode(TYPE_BEAUTY_BODY_LONG_LEG, NODE_ALL_SLIM, "BEF_BEAUTY_BODY_LONG_LEG", 0));
            list.add(new ComposerNode(TYPE_BEAUTY_BODY_SHRINK_HEAD, NODE_ALL_SLIM, "BEF_BEAUTY_BODY_SHRINK_HEAD", 0));
        }
        else{
            if(externParam.getNodes()!=null)
                list.addAll(Arrays.asList(externParam.getNodes()));
            list.add(new ComposerNode(TYPE_BEAUTY_BODY_THIN, NODE_ALL_SLIM, "BEF_BEAUTY_BODY_THIN", 1));
            list.add(new ComposerNode(TYPE_BEAUTY_BODY_LONG_LEG, NODE_ALL_SLIM, "BEF_BEAUTY_BODY_LONG_LEG", 1));
            list.add(new ComposerNode(TYPE_BEAUTY_BODY_SHRINK_HEAD, NODE_ALL_SLIM, "BEF_BEAUTY_BODY_SHRINK_HEAD", 1));
        }
        externParam.setNodes(list.toArray(new ComposerNode[list.size()]));
        updateEffectsByParam();
    }

    private void updateEffectsByParam() {
        if(externParam.getNodeArray()!=null && externParam.getNodeArray().length > 0){
            mEffectHelper.setComposeNodes(externParam.getNodeArray());
            for(ComposerNode node : externParam.getNodes()){
                mEffectHelper.updateComposeNode(node, true);
            }
        }
        mEffectHelper.setSticker(externParam.getSticker());
        if (null != externParam.getFilter() && !TextUtils.isEmpty(externParam.getFilter().getKey())) {
            mEffectHelper.setFilter(ResourceHelper.getFilterResourcePathByName(mContext, externParam.getFilter().getKey()));
            mEffectHelper.updateFilterIntensity(externParam.getFilter().getValue());
        }
    }
}
