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

package io.agora.beautyapi.sensetime.utils

import io.agora.base.internal.Logging

object LogUtils {
    private const val beautyType = "SenseTime"

    @JvmStatic
    fun i(tag: String, content: String, vararg args: Any) {
        val consoleMessage = "[BeautyAPI][$beautyType] : ${String.format(content, args)}"
        Logging.log(Logging.Severity.LS_INFO, tag, consoleMessage)
    }

    @JvmStatic
    fun d(tag: String, content: String, vararg args: Any) {
        val consoleMessage = "[BeautyAPI][$beautyType] : ${String.format(content, args)}"
        Logging.d(tag, consoleMessage)
    }

    @JvmStatic
    fun w(tag: String, content: String, vararg args: Any){
        val consoleMessage = "[BeautyAPI][$beautyType] : ${String.format(content, args)}"
        Logging.w(tag, consoleMessage)
    }

    @JvmStatic
    fun e(tag: String, content: String, vararg args: Any){
        val consoleMessage = "[BeautyAPI][$beautyType] : ${String.format(content, args)}"
        Logging.e(tag, consoleMessage)
    }

}