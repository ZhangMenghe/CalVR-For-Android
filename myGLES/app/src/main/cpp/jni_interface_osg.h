//
// Created by menghe on 7/12/2018.
//

#ifndef MYGLES_JNI_INTERFACE_OSG_H
#define MYGLES_JNI_INTERFACE_OSG_H

#include <jni.h>

//#define JNI_METHOD(returnType, funcName)\
//    JNIEXPORT returnType JNICALL        \
//        Java_com_example_arcalvr_JniInterfaceOSG_##funcName

#define JNI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_com_example_menghe_mygles_JniInterfaceOSG_##funcName

//define helper functions to provide access to Java from C via JNI
extern "C" {
JNI_METHOD(jlong, JNIcreateController)(JNIEnv *env, jclass, jobject asset_manager);

JNI_METHOD(void, JNIdrawFrame)(JNIEnv *, jclass, jboolean btn_status_normal);
JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv *env, jclass, jstring res_path);
JNI_METHOD(void, JNIonViewChanged)(JNIEnv *, jclass, jint rotation, jint width, jint height);

JNI_METHOD(void, JNIonTouched)(JNIEnv *, jclass, jfloat x, jfloat y);
JNI_METHOD(jboolean, JNIhasDetectedPlane)(JNIEnv *, jclass);

JNI_METHOD(void, JNIonResume)(JNIEnv *, jclass, jobject contex, jobject activitiy);
JNI_METHOD(void, JNIonPause)(JNIEnv *, jclass);
JNI_METHOD(void, JNIonDestroy)(JNIEnv *, jclass, long controller_addr);
JNI_METHOD(void, JNIDebugScene)(JNIEnv *, jclass, jstring filename);
JNI_METHOD(jfloat, JNIgetFPS)(JNIEnv *, jclass);
JNIEnv * GetJniEnv();
}
#endif