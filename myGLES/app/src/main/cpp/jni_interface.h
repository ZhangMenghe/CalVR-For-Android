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
    JNI_METHOD(void, setupGrphicDraw)(JNIEnv *, jclass, jint width, jint height);
    JNI_METHOD(void, drawFrame)(JNIEnv *, jclass);

    //access JNI env on the current thread
    //TODO: detach the thread when thread exists
    JNIEnv * GetJniEnv();
}

#endif //MYGLES_JNI_INTERFACE_H
