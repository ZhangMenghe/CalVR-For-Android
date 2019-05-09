#ifndef TESTAPP_JNI_INTERFACE_H
#define TESTAPP_JNI_INTERFACE_H

#include <jni.h>

#define JNI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_lapras_testapp_JNIInterface_##funcName

//define helper functions to provide access to Java from C via JNI
extern "C"{
JNI_METHOD(jlong, JNIonCreate)(JNIEnv* env, jclass , jobject asset_manager);

JNI_METHOD(void, JNIsendDCMImgs)(JNIEnv*, jobject,  jobjectArray, jint);

JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jobject);

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv * env, jclass);

JNI_METHOD(void, JNIonSurfaceChanged)(JNIEnv * env, jclass, jint, jint);

JNI_METHOD(void, JNIonSingleTouchDown)(JNIEnv * env, jclass, jfloat, jfloat);

JNI_METHOD(void, JNIonTouchMove)(JNIEnv * env, jclass, jfloat, jfloat);

JNI_METHOD(void, JNIonDoubleTouch)(JNIEnv * env, jclass, jint, jfloat, jfloat);

JNI_METHOD(jboolean, JNIchangeRender)(JNIEnv*, jobject);

JNI_METHOD(void, JNIsetSwitches)(JNIEnv * env, jclass, jint , jboolean);

JNI_METHOD(void, JNIsetParam)(JNIEnv * env, jclass, jint, jfloat);

JNI_METHOD(jboolean , JNIgetOriginalChecked)(JNIEnv*, jobject, jint);

JNI_METHOD(jfloat , JNIgetOriginalValue)(JNIEnv*, jobject, jint);

JNI_METHOD(jfloat , JNIgetFPS)(JNIEnv*, jobject);
}


#endif
