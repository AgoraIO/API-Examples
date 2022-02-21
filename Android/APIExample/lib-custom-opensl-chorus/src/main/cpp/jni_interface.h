/*
 * Copyright 2018 The Android Open Source Project
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
#ifndef JNI_INTERFACE_H
#define JNI_INTERFACE_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_io_agora_rte_custom_1opensl_1chorus_OpenSlChorusPlayer_initOpenslDemo(
    JNIEnv *env, jobject thiz, jlong handle);
JNIEXPORT void JNICALL Java_io_agora_rte_custom_1opensl_1chorus_OpenSlChorusPlayer_startOpenslDemo(
    JNIEnv *env, jobject thiz);
JNIEXPORT void JNICALL Java_io_agora_rte_custom_1opensl_1chorus_OpenSlChorusPlayer_stopOpenslDemo(
    JNIEnv *env, jobject thiz);
JNIEXPORT void JNICALL Java_io_agora_rte_custom_1opensl_1chorus_OpenSlChorusPlayer_startEchoTest(
    JNIEnv *env, jobject thiz);
JNIEXPORT void JNICALL Java_io_agora_rte_custom_1opensl_1chorus_OpenSlChorusPlayer_stopEchoTest(
    JNIEnv *env, jobject thiz);
#ifdef __cplusplus
}
#endif
#endif