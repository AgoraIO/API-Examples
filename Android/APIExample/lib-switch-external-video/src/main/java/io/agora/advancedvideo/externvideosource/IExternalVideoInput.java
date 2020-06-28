package io.agora.advancedvideo.externvideosource;

import android.util.Size;
import android.view.Surface;

public interface IExternalVideoInput {
    /**
     * Called when the external video manager is
     * initializing this video input
     * @param target The drawing target of the video input
     */
    void onVideoInitialized(Surface target);

    /**
     * Called when the external video manager wants
     * to stop this video input
     * @param context The context of the GL thread
     */
    void onVideoStopped(GLThreadContext context);


    boolean isRunning();
    /**
     * Called when a complete video frame data is prepared to be
     * processed. This is usually used to draw local preview,
     * as well as other frame processing procedure before
     * being transmitted to remote users.
     * @param context The context of the GL thread
     * @param textureId texture id
     * @param transform the transformation matrix of the texture
     */
    void onFrameAvailable(GLThreadContext context,  int textureId, float[] transform);

    /**
     * @return the size of the frames
     */
    Size onGetFrameSize();

    /**
     * Determines the time to wait for the next possible frame due
     * to the presentation time of frames of different video types.
     * @return time to wait
     */
    int timeToWait();
}
