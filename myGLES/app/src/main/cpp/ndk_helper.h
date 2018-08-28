#ifndef NDK_HELPER_H
#define NDK_HELPER_H
#include <jni.h>
namespace ndk_helper{
    //maintain a reference to VM
    static JavaVM *g_vm = nullptr;

//JNI Library function: call when native lib is load(System.loadLibrary)
    jint JNI_OnLoad(JavaVM *vm, void *) {
        g_vm = vm;
        return JNI_VERSION_1_6;
    }

//access JNI env on the current thread
//TODO: detach the thread when thread exists
    JNIEnv * GetJniEnv(){
        JNIEnv *env;
        jint result = g_vm->AttachCurrentThread(&env, nullptr);
        return result == JNI_OK ? env : nullptr;
    }
}

#endif


