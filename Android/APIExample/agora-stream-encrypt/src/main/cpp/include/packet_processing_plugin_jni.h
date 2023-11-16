#ifndef ADVANCED_VIDEO_ANDROID_PACKET_PROCESSING_PLUGIN_JNI_H
#define ADVANCED_VIDEO_ANDROID_PACKET_PROCESSING_PLUGIN_JNI_H

#include <jni.h>

extern "C" JNIEXPORT void JNICALL
Java_io_agora_api_streamencrypt_PacketProcessor_doRegisterProcessing(JNIEnv *env, jclass clazz, jlong rtcEngineHandler);

extern "C" JNIEXPORT void JNICALL
Java_io_agora_api_streamencrypt_PacketProcessor_doUnregisterProcessing(JNIEnv *env, jclass clazz, jlong rtcEngineHandler);

#endif //ADVANCED_VIDEO_ANDROID_PACKET_PROCESSING_PLUGIN_JNI_H