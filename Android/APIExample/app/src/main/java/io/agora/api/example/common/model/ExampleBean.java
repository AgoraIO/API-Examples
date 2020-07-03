package io.agora.api.example.common.model;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * @author cjw
 */
public class ExampleBean implements Parcelable {
    private String group;
    private String name;
    private int actionId;
    private int tipsId;

    public ExampleBean(String group, String name, int actionId, int tipsId) {
        this.group = group;
        this.name = name;
        this.actionId = actionId;
        this.tipsId = tipsId;
    }

    public String getGroup() {
        return group;
    }

    public void setGroup(String group) {
        this.group = group;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getActionId() {
        return actionId;
    }

    public void setActionId(int actionId) {
        this.actionId = actionId;
    }

    public int getTipsId() {
        return tipsId;
    }

    public void setTipsId(int tipsId) {
        this.tipsId = tipsId;
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(this.group);
        dest.writeString(this.name);
        dest.writeInt(this.actionId);
        dest.writeInt(this.tipsId);
    }

    public ExampleBean() {
    }

    protected ExampleBean(Parcel in) {
        this.group = in.readString();
        this.name = in.readString();
        this.actionId = in.readInt();
        this.tipsId = in.readInt();
    }

    public static final Creator<ExampleBean> CREATOR = new Creator<ExampleBean>() {
        @Override
        public ExampleBean createFromParcel(Parcel source) {
            return new ExampleBean(source);
        }

        @Override
        public ExampleBean[] newArray(int size) {
            return new ExampleBean[size];
        }
    };
}
