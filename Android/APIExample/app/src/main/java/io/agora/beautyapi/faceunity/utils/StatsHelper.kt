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

package io.agora.beautyapi.faceunity.utils

import android.os.Handler
import android.os.Looper
import io.agora.beautyapi.faceunity.BeautyStats
import kotlin.math.max
import kotlin.math.min

class StatsHelper(
    private val statsDuration: Long,
    private val onStatsChanged: (BeautyStats) -> Unit
) {

    private val mMainHandler = Handler(Looper.getMainLooper())
    private var mStartTime = 0L
    private var mCostList = mutableListOf<Long>()
    private var mCostMax = 0L
    private var mCostMin = Long.MAX_VALUE

    fun once(cost: Long) {
        val curr = System.currentTimeMillis()
        if (mStartTime == 0L) {
            mStartTime = curr
        } else if (curr - mStartTime >= statsDuration) {
            mStartTime = curr
            var total = 0L
            mCostList.forEach {
                total += it
            }
            val average = total / mCostList.size
            val costMin = mCostMin
            val costMax = mCostMax
            mMainHandler.post {
                onStatsChanged.invoke(BeautyStats(costMin, costMax, average))
            }

            mCostList.clear()
            mCostMax = 0L
            mCostMin = Long.MAX_VALUE
        }

        mCostList.add(cost)
        mCostMax = max(mCostMax, cost)
        mCostMin = min(mCostMin, cost)
    }

    fun reset() {
        mMainHandler.removeCallbacksAndMessages(null)
        mStartTime = 0
        mCostList.clear()
        mCostMax = 0L
        mCostMin = Long.MAX_VALUE
    }


}