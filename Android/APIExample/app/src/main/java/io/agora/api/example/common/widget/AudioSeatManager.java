package io.agora.api.example.common.widget;

import android.view.View;

import java.util.ArrayList;

/**
 * The type Audio seat manager.
 */
public class AudioSeatManager {

    private final AudioOnlyLayout[] audioOnlyLayouts;

    /**
     * Instantiates a new Audio seat manager.
     *
     * @param seats the seats
     */
    public AudioSeatManager(AudioOnlyLayout... seats) {
        audioOnlyLayouts = new AudioOnlyLayout[seats.length];
        for (int i = 0; i < audioOnlyLayouts.length; i++) {
            audioOnlyLayouts[i] = seats[i];
        }
    }

    /**
     * Up local seat.
     *
     * @param uid the uid
     */
    public void upLocalSeat(int uid) {
        AudioOnlyLayout localSeat = audioOnlyLayouts[0];
        localSeat.setTag(uid);
        localSeat.setVisibility(View.VISIBLE);
        localSeat.updateUserInfo(uid + "", true);
    }

    /**
     * Up remote seat.
     *
     * @param uid the uid
     */
    public void upRemoteSeat(int uid) {
        AudioOnlyLayout idleSeat = null;
        for (AudioOnlyLayout audioOnlyLayout : audioOnlyLayouts) {
            if (audioOnlyLayout.getTag() == null) {
                idleSeat = audioOnlyLayout;
                break;
            }
        }
        if (idleSeat != null) {
            idleSeat.setTag(uid);
            idleSeat.setVisibility(View.VISIBLE);
            idleSeat.updateUserInfo(uid + "", false);
        }
    }

    /**
     * Get seat remote uid list array list.
     *
     * @return the array list
     */
    public ArrayList<Integer> getSeatRemoteUidList() {
        ArrayList<Integer> uidList = new ArrayList<>();
        for (int i = 1; i < audioOnlyLayouts.length; i++) {
            AudioOnlyLayout audioOnlyLayout = audioOnlyLayouts[i];
            Object tag = audioOnlyLayout.getTag();
            if (tag instanceof Integer) {
                uidList.add((Integer) tag);
            }
        }
        return uidList;
    }

    /**
     * Down seat.
     *
     * @param uid the uid
     */
    public void downSeat(int uid) {
        AudioOnlyLayout seat = null;
        for (AudioOnlyLayout audioOnlyLayout : audioOnlyLayouts) {
            Object tag = audioOnlyLayout.getTag();
            if (tag instanceof Integer && (Integer) tag == uid) {
                seat = audioOnlyLayout;
                break;
            }
        }
        if (seat != null) {
            seat.setTag(null);
            seat.setVisibility(View.INVISIBLE);
        }
    }

    /**
     * Get local seat audio only layout.
     *
     * @return the audio only layout
     */
    public AudioOnlyLayout getLocalSeat() {
        return audioOnlyLayouts[0];
    }

    /**
     * Get remote seat audio only layout.
     *
     * @param uid the uid
     * @return the audio only layout
     */
    public AudioOnlyLayout getRemoteSeat(int uid) {
        AudioOnlyLayout seat = null;
        for (AudioOnlyLayout audioOnlyLayout : audioOnlyLayouts) {
            Object tag = audioOnlyLayout.getTag();
            if (tag instanceof Integer && (Integer) tag == uid) {
                seat = audioOnlyLayout;
                break;
            }
        }
        return seat;
    }

    /**
     * Down all seats.
     */
    public void downAllSeats() {
        for (AudioOnlyLayout audioOnlyLayout : audioOnlyLayouts) {
            audioOnlyLayout.setTag(null);
            audioOnlyLayout.setVisibility(View.INVISIBLE);
        }
    }
}
