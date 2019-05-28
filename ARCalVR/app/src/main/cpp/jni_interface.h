#ifndef ARCALVR_JNI_INTERFACE_H
#define ARCALVR_JNI_INTERFACE_H

#include <jni.h>

#define JNI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_com_samsung_arcalvr_JniInterface_##funcName

#define UI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_com_samsung_arcalvr_UIsController_##funcName


//define helper functions to provide access to Java from C via JNI
extern "C"{
JNI_METHOD(jlong, JNIcreateController)(JNIEnv* env, jclass , jobject asset_manager);

JNI_METHOD(void, JNIdrawFrame)(JNIEnv *, jclass);

JNI_METHOD(void, JNIonGlSurfaceCreated)(JNIEnv *, jclass, jstring calvr_path);

JNI_METHOD(void, JNIonViewChanged)(JNIEnv*, jclass, jint rotation, jint width, jint height);

JNI_METHOD(void, JNIonSingleTouchDown)(JNIEnv*, jclass, jint type, jfloat x, jfloat y);

JNI_METHOD(void, JNIonSingleTouchUp)(JNIEnv*, jclass, jint type, jfloat x, jfloat y);

JNI_METHOD(void, JNIonDoubleTouch)(JNIEnv*, jclass, jint type, jfloat x, jfloat y);

JNI_METHOD(void, JNIonTouchMove)(JNIEnv*, jclass, jint type, jfloat x, jfloat y);

JNI_METHOD(void, JNIonResume)(JNIEnv * env, jclass, jobject contex, jobject activitiy);

JNI_METHOD(void, JNIonPause)(JNIEnv *, jclass);

JNI_METHOD(void, JNIonDestroy)(JNIEnv *, jclass, long controller_addr);

JNI_METHOD(jfloat, JNIgetFPS)(JNIEnv *, jclass);

UI_METHOD(void, JUIonUICreated)(JNIEnv *, jobject);


JNIEnv * GetJniEnv();

jobject GetJUIObject();
}


#endif
