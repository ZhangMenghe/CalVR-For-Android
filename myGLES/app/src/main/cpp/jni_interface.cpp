#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jni_interface.h"
#include "drawPrimitive.h"
#include "nativeController.h"

namespace {
    //maintain a reference to VM
    static JavaVM *g_vm = nullptr;
    inline jlong controllerPtr(controller::nativeController * native_controller){
        return reinterpret_cast<intptr_t>(native_controller);
    }
    inline controller::nativeController * controllerNative(jlong ptr){
        return reinterpret_cast<controller::nativeController *>(ptr);
    }
}


//JNI Library function: call when native lib is load(System.loadLibrary)
jint JNI_OnLoad(JavaVM *vm, void *) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}

/*Native Application methods*/


/*Interface for GLES*/
JNI_METHOD(void, JNIsetupGrphicDraw)
(JNIEnv * env, jclass, jint width, jint height, jobject asset_manager) {
    AAssetManager * cpp_asset_manager = AAssetManager_fromJava(env, asset_manager);
    initilization(width, height, cpp_asset_manager);
}
JNI_METHOD(void, JNIdrawFrame)
(JNIEnv *, jclass) {
    draw();
}
JNI_METHOD(jlong, JNIcreateController)
(JNIEnv* env, jclass, jobject asset_manager){
    AAssetManager * cpp_asset_manager = AAssetManager_fromJava(env, asset_manager);
    return controllerPtr(new controller::nativeController(cpp_asset_manager));
}
JNI_METHOD(void, JNIonTouched)
(JNIEnv*, jclass, jlong controller_addr, jfloat x, jfloat y){
    controllerNative(controller_addr)->onTouched(x, y);
}

//JNI_METHOD(void, JNIOnResume)
//(JNIEnv *, jclass){
//    ArInstallStatus
//}
JNIEnv *GetJniEnv() {
    JNIEnv *env;
    jint result = g_vm->AttachCurrentThread(&env, nullptr);
    return result == JNI_OK ? env : nullptr;
}