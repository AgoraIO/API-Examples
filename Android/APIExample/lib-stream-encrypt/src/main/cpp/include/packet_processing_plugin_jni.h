#ifndef ADVANCED_VIDEO_ANDROID_PACKET_PROCESSING_PLUGIN_JNI_H
#define ADVANCED_VIDEO_ANDROID_PACKET_PROCESSING_PLUGIN_JNI_H

#include <jni.h>

extern "C" JNIEXPORT void JNICALL
Java_io_agora_api_streamencrypt_PacketProcessor_doRegisterProcessing(JNIEnv *env, jobject thiz);

extern "C" JNIEXPORT void JNICALL
Java_io_agora_api_streamencrypt_PacketProcessor_doUnregisterProcessing(JNIEnv *env, jobject thiz);

#endif //ADVANCED_VIDEO_ANDROID_PACKET_PROCESSING_PLUGIN_JNI_H
