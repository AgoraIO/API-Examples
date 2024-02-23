/*
 * Copyright 2014 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package io.agora.api.example.common.gles.core;

import java.nio.FloatBuffer;

/**
 * Base class for stuff we like to draw.
 */
public class Drawable2d {

    /**
     * The constant SIZEOF_FLOAT.
     */
    public static final int SIZEOF_FLOAT = 4;
    /**
     * The constant COORDS_PER_VERTEX.
     */
    public static final int COORDS_PER_VERTEX = 2;
    /**
     * The constant TEXTURE_COORD_STRIDE.
     */
    public static final int TEXTURE_COORD_STRIDE = 2 * SIZEOF_FLOAT;
    /**
     * The constant VERTEXTURE_STRIDE.
     */
    public static final int VERTEXTURE_STRIDE = COORDS_PER_VERTEX * SIZEOF_FLOAT;

    private FloatBuffer mTexCoordArray;
    private FloatBuffer mVertexArray;
    private int mVertexCount;

    /**
     * Instantiates a new Drawable 2 d.
     */
    public Drawable2d() {
    }

    /**
     * Prepares a drawable from a "pre-fabricated" shape definition.
     * <p>
     * Does no EGL/GL operations, so this can be done at any time.
     *
     * @param fullRectangleCoords     the full rectangle coords
     * @param fullRectangleTexCoords the full rectangle tex coords
     */
    public Drawable2d(float[] fullRectangleCoords, float[] fullRectangleTexCoords) {
        updateVertexArray(fullRectangleCoords);
        updateTexCoordArray(fullRectangleTexCoords);
    }

    /**
     * Update vertex array.
     *
     * @param fullRectangleCoords the full rectangle coords
     */
    public void updateVertexArray(float[] fullRectangleCoords) {
        mVertexArray = GlUtil.createFloatBuffer(fullRectangleCoords);
        mVertexCount = fullRectangleCoords.length / COORDS_PER_VERTEX;
    }

    /**
     * Update tex coord array.
     *
     * @param fullRectangleTexCoords the full rectangle tex coords
     */
    public void updateTexCoordArray(float[] fullRectangleTexCoords) {
        mTexCoordArray = GlUtil.createFloatBuffer(fullRectangleTexCoords);
    }

    /**
     * Returns the array of vertices.
     * <p>
     * To avoid allocations, this returns internal state.  The caller must not modify it.
     *
     * @return the float buffer
     */
    public FloatBuffer vertexArray() {
        return mVertexArray;
    }

    /**
     * Returns the array of texture coordinates.
     * <p>
     * To avoid allocations, this returns internal state.  The caller must not modify it.
     *
     * @return the float buffer
     */
    public FloatBuffer texCoordArray() {
        return mTexCoordArray;
    }

    /**
     * Returns the number of vertices stored in the vertex array.
     *
     * @return the int
     */
    public int vertexCount() {
        return mVertexCount;
    }

}
