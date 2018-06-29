//
// Created by menghe on 6/29/2018.
//

#include "jni_interface.h"
#include "drawPrimitive.h"

    //maintain a reference to VM
    static JavaVM *g_vm = nullptr;

    //JNI Library function: call when native lib is load(System.loadLibrary)
    jint JNI_OnLoad(JavaVM *vm, void *) {
        g_vm = vm;
        return JNI_VERSION_1_6;
    }
    JNI_METHOD(void, setupGrphicDraw)
    (JNIEnv *, jclass, jint width, jint height) {
        initilization(width, height);
    }
    JNI_METHOD(void, drawFrame)
    (JNIEnv *, jclass) {
        draw();
    }
    JNIEnv *GetJniEnv() {
        JNIEnv *env;
        jint result = g_vm->AttachCurrentThread(&env, nullptr);
        return result == JNI_OK ? env : nullptr;
    }