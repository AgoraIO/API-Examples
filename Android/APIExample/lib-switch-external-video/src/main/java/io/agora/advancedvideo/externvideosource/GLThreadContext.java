package io.agora.advancedvideo.externvideosource;

import android.opengl.EGLContext;

import io.agora.api.component.gles.ProgramTextureOES;
import io.agora.api.component.gles.core.EglCore;


public class GLThreadContext {
    public EglCore eglCore;
    public EGLContext context;
    public ProgramTextureOES program;
}
