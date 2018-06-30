#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jni_interface.h"
#include "drawPrimitive.h"

//maintain a reference to VM
static JavaVM *g_vm = nullptr;

//JNI Library function: call when native lib is load(System.loadLibrary)
jint JNI_OnLoad(JavaVM *vm, void *) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}


JNI_METHOD(void, JNIsetupGrphicDraw)
(JNIEnv * env, jclass, jint width, jint height, jobject asset_manager) {
    AAssetManager * cpp_asset_manager = AAssetManager_fromJava(env, asset_manager);
    initilization(width, height, cpp_asset_manager);
}
JNI_METHOD(void, JNIdrawFrame)
(JNIEnv *, jclass) {
    draw();
}


JNIEnv *GetJniEnv() {
    JNIEnv *env;
    jint result = g_vm->AttachCurrentThread(&env, nullptr);
    return result == JNI_OK ? env : nullptr;
}