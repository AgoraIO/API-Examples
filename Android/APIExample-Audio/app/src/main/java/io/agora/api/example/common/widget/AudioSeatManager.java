package io.agora.api.example.common.widget;

import android.view.View;

public class AudioSeatManager {

    private final AudioOnlyLayout[] audioOnlyLayouts;

    public AudioSeatManager(AudioOnlyLayout... seats){
        audioOnlyLayouts = new AudioOnlyLayout[seats.length];
        for (int i = 0; i < audioOnlyLayouts.length; i++) {
            audioOnlyLayouts[i] = seats[i];
        }
    }

    public void upLocalSeat(int uid) {
        AudioOnlyLayout localSeat = audioOnlyLayouts[0];
        localSeat.setTag(uid);
        localSeat.setVisibility(View.VISIBLE);
        localSeat.updateUserInfo(uid + "", true);
    }

    public void upRemoteSeat(int uid){
        AudioOnlyLayout idleSeat = null;
        for (AudioOnlyLayout audioOnlyLayout : audioOnlyLayouts) {
            if (audioOnlyLayout.getTag() == null) {
                idleSeat = audioOnlyLayout;
                break;
            }
        }
        if(idleSeat != null){
            idleSeat.setTag(uid);
            idleSeat.setVisibility(View.VISIBLE);
            idleSeat.updateUserInfo(uid + "", false);
        }
    }

    public void downSeat(int uid){
        AudioOnlyLayout seat = null;
        for (AudioOnlyLayout audioOnlyLayout : audioOnlyLayouts) {
            Object tag = audioOnlyLayout.getTag();
            if (tag instanceof Integer && (Integer)tag == uid) {
                seat = audioOnlyLayout;
                break;
            }
        }
        if(seat != null){
            seat.setTag(null);
            seat.setVisibility(View.INVISIBLE);
        }
    }

    public AudioOnlyLayout getLocalSeat(){
        return audioOnlyLayouts[0];
    }

    public AudioOnlyLayout getRemoteSeat(int uid){
        AudioOnlyLayout seat = null;
        for (AudioOnlyLayout audioOnlyLayout : audioOnlyLayouts) {
            Object tag = audioOnlyLayout.getTag();
            if (tag instanceof Integer && (Integer)tag == uid) {
                seat = audioOnlyLayout;
                break;
            }
        }
        return seat;
    }

    public void downAllSeats(){
        for (AudioOnlyLayout audioOnlyLayout : audioOnlyLayouts) {
            audioOnlyLayout.setTag(null);
            audioOnlyLayout.setVisibility(View.INVISIBLE);
        }
    }
}
