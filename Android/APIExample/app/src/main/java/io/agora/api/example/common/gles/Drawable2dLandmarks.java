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

package io.agora.api.example.common.gles;


import io.agora.api.example.common.gles.core.Drawable2d;

/**
 * Base class for stuff we like to draw.
 */
public class Drawable2dLandmarks extends Drawable2d {


    private float pointsCoords[] = new float[150];

    public Drawable2dLandmarks() {
        updateVertexArray(pointsCoords);
    }
}
