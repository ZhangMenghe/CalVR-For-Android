#ifndef ARCALVR_JNI_INTERFACE_H
#define ARCALVR_JNI_INTERFACE_H

#include <jni.h>

#define JNI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_com_samsung_arcalvr_1gpu_JniInterface_##funcName

#define MAIN_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_com_samsung_arcalvr_MainActivity_##funcName

//define helper functions to provide access to Java from C via JNI
extern "C"{
JNI_METHOD(jlong, JNIcreateController)(JNIEnv* env, jclass , jobject asset_manager);

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv *, jclass, jstring calvr_path);

JNI_METHOD(void, JNIonViewChanged)(JNIEnv*, jclass, jint rotation, jint width, jint height);

JNI_METHOD(void, JNIdrawFrame)(JNIEnv *, jclass);

JNI_METHOD(void, JNIonResume)(JNIEnv * env, jclass);

JNI_METHOD(void, JNIonPause)(JNIEnv *, jclass);

JNI_METHOD(void, JNIonDestroy)(JNIEnv *, jclass, long controller_addr);

JNI_METHOD(jfloat, JNIgetFPS)(JNIEnv *, jclass);

JNI_METHOD(void, JNIonSingleTouchDown)(JNIEnv*, jclass, jint type, jfloat x, jfloat y);

JNI_METHOD(void, JNIonSingleTouchUp)(JNIEnv*, jclass, jint type, jfloat x, jfloat y);

JNI_METHOD(void, JNIonDoubleTouch)(JNIEnv*, jclass, jint type, jfloat x, jfloat y);

JNI_METHOD(void, JNIonTouchMove)(JNIEnv*, jclass, jint type, jfloat x, jfloat y);

MAIN_METHOD(void, JNIOnMainActivityCreated)(JNIEnv*, jobject);


JNIEnv * GetJniEnv();

jobject GetMainActivityObj();
}


#endif
