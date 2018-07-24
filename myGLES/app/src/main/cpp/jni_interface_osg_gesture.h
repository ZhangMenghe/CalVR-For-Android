//
// Created by menghe on 7/22/2018.
//

#ifndef MYGLES_JNI_INTERFACE_OSG_GESTURE_H
#define MYGLES_JNI_INTERFACE_OSG_GESTURE_H


#include <jni.h>

#define JNI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_com_example_menghe_mygles_JniInterfaceGesture_##funcName

//define helper functions to provide access to Java from C via JNI
extern "C" {
JNI_METHOD(void, JNIDrawSphere)(JNIEnv *env, jclass,jlong osgAppAddr);
JNI_METHOD(void, JNIonTouched)(JNIEnv *, jclass,jboolean down,jfloat x, jfloat y);
JNI_METHOD(void, JNIonMove)(JNIEnv *, jclass, jfloat x, jfloat y);
}

#endif //MYGLES_JNI_INTERFACE_OSG_GESTURE_H
