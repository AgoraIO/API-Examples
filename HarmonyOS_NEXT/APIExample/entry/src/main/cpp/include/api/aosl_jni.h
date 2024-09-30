/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 21st, 2018
 * Module:	AOSL JNI relatives header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_JNI_H__
#define __AOSL_JNI_H__

#include <api/aosl_defs.h>

#ifdef __ANDROID__
#include <jni.h>


#ifdef __cplusplus
extern "C" {
#endif



extern __aosl_api__ JavaVM *aosl_java_vm ();
extern __aosl_api__ int aosl_set_java_vm (JavaVM *jvm);

extern __aosl_api__ JNIEnv *aosl_this_jni_env (int *attached_p);

/**
 * Set Java Application Context.
 * Parameter:
 *         ctxt: Main Activity Context, i.e. the main activity;
 * Return value:
 *           <0: indicates error, check aosl_errno for detail;
 *            0: successful;
 * Remarks:
 *     For Android, the ctxt MUST BE the Main Activity object,
 *     but for other Java environment, ctxt can be any object
 *     which is loaded by the main Java thread.
 **/
extern __aosl_api__ int aosl_java_set_context (jobject ctxt);

extern __aosl_api__ void aosl_thread_java_detach ();


#ifdef __cplusplus
}
#endif
#endif /* __ANDROID__ */


#endif /* __AOSL_JNI_H__ */
