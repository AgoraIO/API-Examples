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

import android.util.Log
import java.io.File
import java.io.FileOutputStream
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.concurrent.Executors

object LogUtils {
    private const val beautyType = "FaceUnity"
    private val timeFormat = SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS", Locale.ROOT)
    private val logFileName = "agora_beautyapi_${beautyType.toLowerCase(Locale.US)}_android.log"
    private val workerThread = Executors.newSingleThreadExecutor()
    private var logOutputStream: FileOutputStream? = null

    @JvmStatic
    fun setLogFilePath(path: String){
        if(path.isEmpty()){
            e("LogUtils", "setLogFilePath >> path is empty!")
            return
        }
        val direction = File(path)
        if(!direction.exists()){
            direction.mkdirs()
        }
        val file = File(direction, logFileName)
        if(!file.exists()){
            file.createNewFile()
        }
        val append = file.length() < 2 * 1024 * 1024
        logOutputStream = FileOutputStream(file, append)
    }


    @JvmStatic
    fun i(tag: String, content: String, vararg args: Any) {
        val consoleMessage = "[BeautyAPI][$beautyType] : ${String.format(content, args)}"
        val fileMessage = "${timeFormat.format(Date())} : [BeautyAPI][$beautyType][$tag][INFO] : ${String.format(content, args)}"
        Log.v(tag, consoleMessage)
        saveToFile(fileMessage)
    }

    @JvmStatic
    fun d(tag: String, content: String, vararg args: Any) {
        val consoleMessage = "[BeautyAPI][$beautyType] : ${String.format(content, args)}"
        val fileMessage = "${timeFormat.format(Date())} : [BeautyAPI][$beautyType][$tag][DEBUG] : ${String.format(content, args)}"
        Log.d(tag, consoleMessage)
        saveToFile(fileMessage)
    }

    @JvmStatic
    fun w(tag: String, content: String, vararg args: Any){
        val consoleMessage = "[BeautyAPI][$beautyType] : ${String.format(content, args)}"
        val fileMessage = "${timeFormat.format(Date())} : [BeautyAPI][$beautyType][$tag][WARN] : ${String.format(content, args)}"
        Log.w(tag, consoleMessage)
        saveToFile(fileMessage)
    }

    @JvmStatic
    fun e(tag: String, content: String, vararg args: Any){
        val consoleMessage = "[BeautyAPI][$beautyType] : ${String.format(content, args)}"
        val fileMessage = "${timeFormat.format(Date())} : [BeautyAPI][$beautyType][$tag][ERROR] : ${String.format(content, args)}"
        Log.e(tag, consoleMessage)
        saveToFile(fileMessage)
    }


    private fun saveToFile(message: String){
        val outputStream = logOutputStream ?: return
        workerThread.execute {
            outputStream.write(message.toByteArray())
            if(!message.endsWith("\n")){
                outputStream.write("\n".toByteArray())
            }
        }
    }
}