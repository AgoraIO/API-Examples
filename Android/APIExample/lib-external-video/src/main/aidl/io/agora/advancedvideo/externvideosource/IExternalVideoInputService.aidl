package io.agora.advancedvideo.externvideosource;

interface IExternalVideoInputService {
    // the type of external video input is one of
    // ExternalVideoInputManager TYPE_LOCAL_VIDEO,
    // TYPE_SCREEN_SHARE
    // Bundle contains any information that is
    // necessary for this external video input
    // returns true if the input has been set or
    // has replaced the current input, false otherwise.
    boolean setExternalVideoInput(int type, in Intent intent);
}
