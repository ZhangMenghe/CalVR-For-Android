//
// Created by menghe on 6/29/2018.
//

#ifndef MYGLES_JNI_INTERFACE_H
#define MYGLES_JNI_INTERFACE_H

#include <jni.h>
#define JNI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_com_example_menghe_mygles_JniInterface_##funcName

//define helper functions to provide access to Java from C via JNI
extern "C"{
    JNI_METHOD(void, JNIsetupGrphicDraw)(JNIEnv *, jclass, jint width, jint height, jobject asset_manager);
    JNI_METHOD(void, JNIdrawFrame)(JNIEnv *, jclass);
    JNI_METHOD(jlong, JNIcreateController)(JNIEnv* env, jclass, jobject asset_manager);
    JNI_METHOD(void, JNIonTouched)(JNIEnv *, jclass, jlong controller_addr, jfloat x, jfloat y);
    //access JNI env on the current thread
    //TODO: detach the thread when thread exists
    JNIEnv * GetJniEnv();
}

#endif //MYGLES_JNI_INTERFACE_H
