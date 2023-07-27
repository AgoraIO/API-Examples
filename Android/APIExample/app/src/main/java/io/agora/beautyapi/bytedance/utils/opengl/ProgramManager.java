/*
 * MIT License
 *
 * Copyright (c) 2023 Agora Community
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package io.agora.beautyapi.bytedance.utils.opengl;


import com.bytedance.labcv.effectsdk.BytedEffectConstants;

public class ProgramManager {


    public ProgramManager() {
    }

    private ProgramTexture2d mProgramTexture2D;
    private ProgramTextureOES mProgramTextureOES;

    public  Program getProgram(BytedEffectConstants.TextureFormat srcTetxureFormat){
        switch (srcTetxureFormat){
            case Texure2D:
                if (null == mProgramTexture2D){
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

    public void release(){
        if (null != mProgramTexture2D){
            mProgramTexture2D.release();
            mProgramTexture2D = null;

        }
        if (null != mProgramTextureOES){
            mProgramTextureOES.release();
            mProgramTextureOES = null;

        }
    }
}
