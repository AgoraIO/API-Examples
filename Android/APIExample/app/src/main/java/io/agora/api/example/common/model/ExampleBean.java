package io.agora.api.example.common.model;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * The type Example bean.
 *
 * @author cjw
 */
public class ExampleBean implements Parcelable {
    private int index;
    private String group;
    private int name;
    private int actionId;
    private int tipsId;

    /**
     * Instantiates a new Example bean.
     *
     * @param index    the index
     * @param group    the group
     * @param name     the name
     * @param actionId the action id
     * @param tipsId   the tips id
     */
    public ExampleBean(int index, String group, int name, int actionId, int tipsId) {
        this.index = index;
        this.group = group;
        this.name = name;
        this.actionId = actionId;
        this.tipsId = tipsId;
    }

    /**
     * Gets index.
     *
     * @return the index
     */
    public int getIndex() {
        return index;
    }

    /**
     * Sets index.
     *
     * @param index the index
     */
    public void setIndex(int index) {
        this.index = index;
    }

    /**
     * Gets group.
     *
     * @return the group
     */
    public String getGroup() {
        return group;
    }

    /**
     * Sets group.
     *
     * @param group the group
     */
    public void setGroup(String group) {
        this.group = group;
    }

    /**
     * Gets name.
     *
     * @return the name
     */
    public int getName() {
        return name;
    }

    /**
     * Sets name.
     *
     * @param name the name
     */
    public void setName(int name) {
        this.name = name;
    }

    /**
     * Gets action id.
     *
     * @return the action id
     */
    public int getActionId() {
        return actionId;
    }

    /**
     * Sets action id.
     *
     * @param actionId the action id
     */
    public void setActionId(int actionId) {
        this.actionId = actionId;
    }

    /**
     * Gets tips id.
     *
     * @return the tips id
     */
    public int getTipsId() {
        return tipsId;
    }

    /**
     * Sets tips id.
     *
     * @param tipsId the tips id
     */
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
        dest.writeInt(this.name);
        dest.writeInt(this.actionId);
        dest.writeInt(this.tipsId);
    }

    /**
     * Instantiates a new Example bean.
     */
    public ExampleBean() {
    }

    /**
     * Instantiates a new Example bean.
     *
     * @param in the in
     */
    protected ExampleBean(Parcel in) {
        this.group = in.readString();
        this.name = in.readInt();
        this.actionId = in.readInt();
        this.tipsId = in.readInt();
    }

    /**
     * The constant CREATOR.
     */
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
