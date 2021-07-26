//
// Created by 张涛 on 2020/2/27.
//

#ifndef AGORA_VIDEO_WITH_FACEUNITY_ANDROID_JNIHELPER_H
#define AGORA_VIDEO_WITH_FACEUNITY_ANDROID_JNIHELPER_H

#if defined(__ANDROID__) || defined(TARGET_OS_ANDROID)

#include <jni.h>

namespace agora {
    namespace extension {
        class JniHelper {
        private:
            JniHelper(JavaVM *jvm);

            JavaVM *javaVm;
            static JniHelper *jniHelper;

        public:
            bool isAttached = false;
            jclass agoraByteDanceNativeClz;

            ~JniHelper();

            static JniHelper *createJniHelper(JavaVM *jvm);

            static JniHelper *getJniHelper();

            static void release();

            jobject getGlobalContext(JNIEnv **env);

            JNIEnv *getEnv();

            JNIEnv *attachCurrentThread();

            void detachCurrentThread();


        };

        class AndroidContextHelper {
        public:
            AndroidContextHelper() = delete;
            static void setContext(void * context);
            static void * getContext();
            static void releaseContext(JNIEnv* env);
        private:
            static void * _context;
        };
    }
}

#endif //defined(__ANDROID__) || defined(TARGET_OS_ANDROID)

#endif //AGORA_VIDEO_WITH_FACEUNITY_ANDROID_JNIHELPER_H
