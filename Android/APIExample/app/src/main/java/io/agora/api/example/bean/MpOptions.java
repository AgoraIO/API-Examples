package io.agora.api.example.bean;

import android.util.Log;

import org.json.JSONObject;

import java.lang.reflect.Field;

public class MpOptions {
    public static final String TAG = "MpOptions";

    /**
     * key:
     * makeup_options
     * <p>
     * value:
     * bool enable_mu;          (true: 美妆开; false: 美妆关)
     * int browStyle;           (眉毛类型 0 - 2: 关闭、类型1、类型2)
     * int browColor;           (眉毛颜色 0 - 2: 无、黑色、棕色)
     * float browStrength;      (眉毛强度 0.0 - 1.0)
     * int lashStyle;           (眼睫毛类型 0 - 2: 关闭、类型1、类型2)
     * int lashColor;           (眼睫毛颜色 0 - 2: 无、黑色、棕色)
     * float lashStrength;      (眼睫毛强度 0.0 - 1.0)
     * int shadowStyle;         (眼影类型 0 - 2: 关闭、类型1、类型2)
     * int shadowColor;         暂无
     * float shadowStrength;    (眼影强度 0.0 - 1.0)
     * int pupilStyle;          (瞳孔类型 0 - 2: 关闭、类型1、类型2)
     * int pupilColor;          暂无
     * float pupilStrength;     (瞳孔强度 0.0 - 1.0)
     * int blushStyle;          (腮红类型 0 - 2: 关闭、类型1、类型2)
     * int blushColor;          (腮红颜色 0 - 5: 无、1号色、2号色、3号色、4号色、5号色)
     * float blushStrength;     (腮红强度 0.0 - 1.0)
     * int lipStyle;            (唇彩类型 0 - 2: 关闭、类型1、类型2)
     * int lipColor;            (唇彩颜色 0 - 5: 无、1号色、2号色、3号色、4号色、5号色)
     * float lipStrength;       (唇彩强度 0.0 - 1.0)
     **/

    public boolean enable_mu;

    public int browStyle;
    public int browColor;
    public float browStrength;

    public int lashStyle;
    public int lashColor;
    public float lashStrength;

    public int shadowStyle;
    public float shadowStrength;

    public int pupilStyle;
    public float pupilStrength;

    public int blushStyle;
    public int blushColor;
    public float blushStrength;

    public int lipStyle;
    public int lipColor;
    public float lipStrength;

    public MpOptions() {
        this.enable_mu = false;
        this.browStyle = 0;
        this.browColor = 0;
        this.browStrength = 0.5f;
        this.lashStyle = 0;
        this.lashColor = 0;
        this.lashStrength = 0.5f;
        this.shadowStyle = 0;
        this.shadowStrength = 0.5f;
        this.pupilStyle = 0;
        this.pupilStrength = 0.5f;
        this.blushStyle = 0;
        this.blushColor = 0;
        this.blushStrength = 0.5f;
        this.lipStyle = 0;
        this.lipColor = 0;
        this.lipStrength = 0.5f;
    }

    public String toJson() {
        String json = "{}";
        JSONObject jsonObject = new JSONObject();

        try {
            Field[] fields = MpOptions.class.getDeclaredFields();
            for (Field field : fields) {
                field.setAccessible(true);
                String name = field.getName();
                Object value = field.get(this);
                jsonObject.put(name, value);
            }
        } catch (Exception e) {
            Log.e(TAG, "toJson: error:" + e.getMessage());
        }
        json = jsonObject.toString();
        Log.d(TAG, "toJson: " + json);
        return json;
    }
}
