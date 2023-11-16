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

package io.agora.api.example.utils

import android.content.Context
import android.util.Log
import java.io.BufferedInputStream
import java.io.BufferedOutputStream
import java.io.BufferedReader
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStreamReader

/**
 * File kt utils
 *
 * @constructor Create empty File kt utils
 */
object FileKtUtils {
    private const val TAG = "FileUtils"

    /**
     * Get assets string
     *
     * @param context
     * @param path
     * @return
     */
    fun getAssetsString(context: Context, path: String): String {
        val sb = StringBuilder()
        var isr: InputStreamReader? = null
        var br: BufferedReader? = null
        // 读取license文件内容
        try {
            isr = InputStreamReader(context.resources.assets.open(path))
            br = BufferedReader(isr)
            var line: String? = null
            while (br.readLine().also { line = it } != null) {
                sb.append(line).append("\n")
            }
        } catch (e: IOException) {
            Log.e(TAG, "getAssetsString error: $e")
        } finally {
            if (isr != null) {
                try {
                    isr.close()
                } catch (e: IOException) {
                    Log.e(TAG, "", e)
                }
            }
            if (br != null) {
                try {
                    br.close()
                } catch (e: IOException) {
                    Log.e(TAG, "", e)
                }
            }
        }
        return sb.toString()
    }

    /**
     * Copy assets
     *
     * @param context
     * @param assetsPath
     * @param targetPath
     */
    fun copyAssets(context: Context, assetsPath: String, targetPath: String) {
        // 获取assets目录assetDir下一级所有文件以及文件夹
        val fileNames = context.resources.assets.list(assetsPath)
        // 如果是文件夹(目录),则继续递归遍历
        if (fileNames?.isNotEmpty() == true) {
            val targetFile = File(targetPath)
            if (!targetFile.exists() && !targetFile.mkdirs()) {
                return
            }
            for (fileName in fileNames) {
                copyAssets(
                    context,
                    "$assetsPath/$fileName",
                    "$targetPath/$fileName"
                )
            }
        } else {
            copyAssetsFile(context, assetsPath, targetPath)
        }
    }

    private fun copyAssetsFile(context: Context, assetsFile: String, targetPath: String) {
        val dest = File(targetPath)
        dest.parentFile?.mkdirs()
        val input = BufferedInputStream(context.assets.open(assetsFile))
        val output = BufferedOutputStream(FileOutputStream(dest))
        val buffer = ByteArray(1024)
        var length = 0
        while (input.read(buffer).also { length = it } != -1) {
            output.write(buffer, 0, length)
        }
        output.close()
        input.close()
    }
}