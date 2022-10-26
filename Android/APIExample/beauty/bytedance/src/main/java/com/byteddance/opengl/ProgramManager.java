package com.byteddance.opengl;

import com.byteddance.model.TextureFormat;

public class ProgramManager {
    private ProgramTexture2d mProgramTexture2D;
    private ProgramTextureOES mProgramTextureOES;

    public Program getProgram(TextureFormat format) {
        switch (format) {
            case Texture2D:
                if (null == mProgramTexture2D) {
                    mProgramTexture2D = new ProgramTexture2d();
                }
                return mProgramTexture2D;
            case Texture_Oes:
                if (null == mProgramTextureOES) {
                    mProgramTextureOES = new ProgramTextureOES();
                }
                return mProgramTextureOES;
        }

        return null;
    }

    public void release() {
        if (null != mProgramTexture2D) {
            mProgramTexture2D.release();
            mProgramTexture2D = null;
        }

        if (null != mProgramTextureOES) {
            mProgramTextureOES.release();
            mProgramTextureOES = null;
        }
    }
}
