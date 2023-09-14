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


import java.nio.FloatBuffer;

/**
 * Base class for stuff we like to draw.
 */
public class Drawable2d {
    private static final int SIZEOF_FLOAT = 4;
    public static final int COORDS_PER_VERTEX = 2;

    public static final int TEXTURE_COORD_STRIDE = COORDS_PER_VERTEX * SIZEOF_FLOAT;
    public static final int VERTEXTURE_STRIDE = COORDS_PER_VERTEX * SIZEOF_FLOAT;


    /**
     * Simple equilateral triangle (1.0 per side).  Centered on (0,0).
     */
    private static final float TRIANGLE_COORDS[] = {
         0.0f,  0.577350269f,   // 0 top
        -0.5f, -0.288675135f,   // 1 bottom left
         0.5f, -0.288675135f    // 2 bottom right
    };
    private static final float TRIANGLE_TEX_COORDS[] = {
        0.5f, 0.0f,     // 0 top center
        0.0f, 1.0f,     // 1 bottom left
        1.0f, 1.0f,     // 2 bottom right
    };
    private static final FloatBuffer TRIANGLE_BUF =
            GlUtil.createFloatBuffer(TRIANGLE_COORDS);
    private static final FloatBuffer TRIANGLE_TEX_BUF =
            GlUtil.createFloatBuffer(TRIANGLE_TEX_COORDS);

    /**
     * Simple square, specified as a triangle strip.  The square is centered on (0,0) and has
     * a size of 1x1.
     * <p>
     * Triangles are 0-1-2 and 2-1-3 (counter-clockwise winding).
     */
    private static final float RECTANGLE_COORDS[] = {
        -0.5f, -0.5f,   // 0 bottom left
         0.5f, -0.5f,   // 1 bottom right
        -0.5f,  0.5f,   // 2 top left
         0.5f,  0.5f,   // 3 top right
    };
    /** {zh} 
     * FrameBuffer 与屏幕的坐标系是垂直镜像的，所以在将纹理绘制到一个 FrameBuffer 或屏幕上
     * 的时候，他们用的纹理顶点坐标是不同的，需要注意。
     */
    /** {en} 
     * The coordinate system of the FrameBuffer and the screen is mirrored vertically, so when drawing the texture to a FrameBuffer or screen
     * , the vertex coordinates of the texture they use are different, which needs attention.
     */

    private static final float RECTANGLE_TEX_COORDS[] = {
            0.0f, 1.0f,     // 0 bottom left
            1.0f, 1.0f,     // 1 bottom right
            0.0f, 0.0f,     // 2 top left
            1.0f, 0.0f      // 3 top right
    };
    private static final float RECTANGLE_TEX_COORDS1[] = {
            0.0f, 0.0f,     // 0 bottom left
            1.0f, 0.0f,     // 1 bottom right
            0.0f, 1.0f,     // 2 top left
            1.0f, 1.0f      // 3 top right
    };
    private static final FloatBuffer RECTANGLE_BUF =
            GlUtil.createFloatBuffer(RECTANGLE_COORDS);
    private static final FloatBuffer RECTANGLE_TEX_BUF =
            GlUtil.createFloatBuffer(RECTANGLE_TEX_COORDS);
    private static final FloatBuffer RECTANGLE_TEX_BUF1 =
            GlUtil.createFloatBuffer(RECTANGLE_TEX_COORDS1);

    /**
     * A "full" square, extending from -1 to +1 in both dimensions.  When the model/view/projection
     * matrix is identity, this will exactly cover the viewport.
     * <p>
     * The texture coordinates are Y-inverted relative to RECTANGLE.  (This seems to work out
     * right with external textures from SurfaceTexture.)
     */
    private static final float FULL_RECTANGLE_COORDS[] = {
        -1.0f, -1.0f,   // 0 bottom left
         1.0f, -1.0f,   // 1 bottom right
        -1.0f,  1.0f,   // 2 top left
         1.0f,  1.0f,   // 3 top right
    };
    /** {zh} 
     * FrameBuffer 与屏幕的坐标系是垂直镜像的，所以在将纹理绘制到一个 FrameBuffer 或屏幕上
     * 的时候，他们用的纹理顶点坐标是不同的，需要注意。
     */
    /** {en} 
     * The coordinate system of the FrameBuffer and the screen is mirrored vertically, so when drawing the texture to a FrameBuffer or screen
     * , the vertex coordinates of the texture they use are different, which needs attention.
     */

    private static final float FULL_RECTANGLE_TEX_COORDS[] = {
        0.0f, 1.0f,     // 0 bottom left
        1.0f, 1.0f,     // 1 bottom right
        0.0f, 0.0f,     // 2 top left
        1.0f, 0.0f      // 3 top right
    };

    private static final float FULL_RECTANGLE_TEX_COORDS1[] = {
        0.0f, 0.0f,     // 0 bottom left
        1.0f, 0.0f,     // 1 bottom right
        0.0f, 1.0f,     // 2 top left
        1.0f, 1.0f      // 3 top right
    };
    private static final FloatBuffer FULL_RECTANGLE_BUF =
            GlUtil.createFloatBuffer(FULL_RECTANGLE_COORDS);
    private static final FloatBuffer FULL_RECTANGLE_TEX_BUF =
            GlUtil.createFloatBuffer(FULL_RECTANGLE_TEX_COORDS);
    private static final FloatBuffer FULL_RECTANGLE_TEX_BUF1 =
            GlUtil.createFloatBuffer(FULL_RECTANGLE_TEX_COORDS1);


    private FloatBuffer mVertexArray;
    private FloatBuffer mTexCoordArray;
    private FloatBuffer mTexCoordArrayFB;
    private int mVertexCount;
    private int mCoordsPerVertex;
    private int mVertexStride;
    private int mTexCoordStride;
    private Prefab mPrefab;

    /**
     * Enum values for constructor.
     */
    public enum Prefab {
        TRIANGLE, RECTANGLE, FULL_RECTANGLE
    }

    /**
     * Prepares a drawable from a "pre-fabricated" shape definition.
     * <p>
     * Does no EGL/GL operations, so this can be done at any time.
     */
    public Drawable2d(Prefab shape) {
        switch (shape) {
            case TRIANGLE:
                mVertexArray = TRIANGLE_BUF;
                mTexCoordArray = TRIANGLE_TEX_BUF;
                mTexCoordArrayFB = TRIANGLE_TEX_BUF;
                mCoordsPerVertex = 2;
                mVertexStride = mCoordsPerVertex * SIZEOF_FLOAT;
                mVertexCount = TRIANGLE_COORDS.length / mCoordsPerVertex;
                break;
            case RECTANGLE:
                mVertexArray = RECTANGLE_BUF;
                mTexCoordArray = RECTANGLE_TEX_BUF;
                mTexCoordArrayFB = RECTANGLE_TEX_BUF1;
                mCoordsPerVertex = 2;
                mVertexStride = mCoordsPerVertex * SIZEOF_FLOAT;
                mVertexCount = RECTANGLE_COORDS.length / mCoordsPerVertex;
                break;
            case FULL_RECTANGLE:
                mVertexArray = FULL_RECTANGLE_BUF;
                mTexCoordArray = FULL_RECTANGLE_TEX_BUF;
                mTexCoordArrayFB = FULL_RECTANGLE_TEX_BUF1;
                mCoordsPerVertex = 2;
                mVertexStride = mCoordsPerVertex * SIZEOF_FLOAT;
                mVertexCount = FULL_RECTANGLE_COORDS.length / mCoordsPerVertex;
                break;
            default:
                throw new RuntimeException("Unknown shape " + shape);
        }
        mTexCoordStride = 2 * SIZEOF_FLOAT;
        mPrefab = shape;
    }

    /**
     * Returns the array of vertices.
     * <p>
     * To avoid allocations, this returns internal state.  The caller must not modify it.
     */
    public FloatBuffer getVertexArray() {
        return mVertexArray;
    }

    /**
     * Returns the array of texture coordinates.
     * <p>
     * To avoid allocations, this returns internal state.  The caller must not modify it.
     */
    public FloatBuffer getTexCoordArray() {
        return mTexCoordArray;
    }

    /** {zh} 
     * @brief 返回 frameBuffer 绘制用 texture coordinates
     */
    /** {en} 
     * @brief Returns texture coordinates for drawing frameBuffer
     */

    public FloatBuffer getTexCoorArrayFB() {
        return mTexCoordArrayFB;
    }

    /**
     * Returns the number of vertices stored in the vertex array.
     */
    public int getVertexCount() {
        return mVertexCount;
    }

    /**
     * Returns the width, in bytes, of the data for each vertex.
     */
    public int getVertexStride() {
        return mVertexStride;
    }

    /**
     * Returns the width, in bytes, of the data for each texture coordinate.
     */
    public int getTexCoordStride() {
        return mTexCoordStride;
    }

    /**
     * Returns the number of position coordinates per vertex.  This will be 2 or 3.
     */
    public int getCoordsPerVertex() {
        return mCoordsPerVertex;
    }

    public void updateVertexArray(float[] FULL_RECTANGLE_COORDS) {
        mVertexArray = GlUtil.createFloatBuffer(FULL_RECTANGLE_COORDS);
        mVertexCount = FULL_RECTANGLE_COORDS.length / COORDS_PER_VERTEX;
    }

    public void updateTexCoordArray(float[] FULL_RECTANGLE_TEX_COORDS) {
        mTexCoordArray = GlUtil.createFloatBuffer(FULL_RECTANGLE_TEX_COORDS);
    }

    public void updateTexCoordArrayFB(float[] coords) {
        mTexCoordArrayFB = GlUtil.createFloatBuffer(coords);
    }

    @Override
    public String toString() {
        if (mPrefab != null) {
            return "[Drawable2d: " + mPrefab + "]";
        } else {
            return "[Drawable2d: ...]";
        }
    }
}
