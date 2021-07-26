//
// Created by 张涛 on 2020/2/27.
//

#if defined(__ANDROID__) || defined(TARGET_OS_ANDROID)

#include "JniHelper.h"

namespace agora {
    namespace extension {
        JniHelper *JniHelper::jniHelper = nullptr;

        JniHelper::JniHelper(JavaVM *jvm) : javaVm(jvm) {

        }

        JniHelper::~JniHelper() {
            AndroidContextHelper::releaseContext(getEnv());
            javaVm = nullptr;
        }

        JNIEnv *JniHelper::getEnv() {
            JNIEnv *env = nullptr;
            int status = javaVm->GetEnv((void **) &env, JNI_VERSION_1_6);
            return env;
        }

        void JniHelper::release() {
            if (jniHelper) {
                delete jniHelper;
                jniHelper = nullptr;
            }
        }

        JniHelper *JniHelper::createJniHelper(JavaVM *jvm) {
            if (!jniHelper) {
                jniHelper = new JniHelper(jvm);
            }
            return jniHelper;
        }

        JniHelper *JniHelper::getJniHelper() {
            return jniHelper;
        }

        jobject JniHelper::getGlobalContext(JNIEnv **env) {
            jclass activityThread = (*env)->FindClass("android/app/ActivityThread");
            jmethodID currentActivityThread = (*env)->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
            jobject at = (*env)->CallStaticObjectMethod(activityThread, currentActivityThread);

            jmethodID getApplication = (*env)->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");
            jobject context = (*env)->CallObjectMethod(at, getApplication);
            return context;
        }

        JNIEnv *JniHelper::attachCurrentThread() {
            JNIEnv *env = nullptr;
            int status = javaVm->GetEnv((void **) &env, JNI_VERSION_1_6);
            if (status == JNI_EDETACHED) {
                status = javaVm->AttachCurrentThread(&env, nullptr);
                if (status != 0) {
                    return nullptr;
                }
                isAttached = true;
                return env;
            }
            return env;
        }

        void JniHelper::detachCurrentThread() {
            if (isAttached) {
                javaVm->DetachCurrentThread();
            }
            isAttached = false;
        }

        void * AndroidContextHelper::_context = nullptr;
        void AndroidContextHelper::setContext(void *context) {
            _context = context;
        }
        void * AndroidContextHelper::getContext() {
            return _context;
        }
        void AndroidContextHelper::releaseContext(JNIEnv *env) {
            if(_context) {
                env->DeleteGlobalRef(static_cast<jobject>(_context));
                _context = nullptr;
            }
        }
    }
}

#endif //defined(__ANDROID__) || defined(TARGET_OS_ANDROID)
