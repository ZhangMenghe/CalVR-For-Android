#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jni_interface.h"
#include "drawPrimitive.h"
#include "nativeController.h"

namespace {
    //maintain a reference to VM
    static JavaVM *g_vm = nullptr;
    //global environment
    jlong nativeAppAddr = 0;

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
JNI_METHOD(jlong, JNIcreateController)
(JNIEnv* env, jclass, jobject asset_manager){
    AAssetManager * cpp_asset_manager = AAssetManager_fromJava(env, asset_manager);
    nativeAppAddr =  controllerPtr(new controller::nativeController(cpp_asset_manager));
    return nativeAppAddr;
}

/*Interface for GLES*/
JNI_METHOD(void, JNIsetupGrphicDraw)
(JNIEnv * env, jclass, jint width, jint height, jobject asset_manager) {
    AAssetManager * cpp_asset_manager = AAssetManager_fromJava(env, asset_manager);
    initilization(width, height, cpp_asset_manager);
}

JNI_METHOD(void, JNIdrawFrame)
(JNIEnv *, jclass) {
    //draw();//drawPrimitive
    controllerNative(nativeAppAddr)->onDrawFrame();
}

JNI_METHOD(void, JNIonGlSurfaceCreated)
(JNIEnv *, jclass){
    controllerNative(nativeAppAddr)->onCreate();
}

JNI_METHOD(void, JNIonViewChanged)
(JNIEnv*, jclass, jint rotation, jint width, jint height){
    controllerNative(nativeAppAddr)->onViewChanged(rotation, width, height);
}


JNI_METHOD(void, JNIonTouched)
(JNIEnv*, jclass, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onTouched(x, y);
}
JNI_METHOD(jboolean, JNIhasDetectedPlane)
(JNIEnv*, jclass , long controller_addr){
    return static_cast<jboolean>
        (controllerNative(controller_addr)->hasDetectedPlane()? JNI_TRUE:JNI_FALSE);
}

JNI_METHOD(void, JNIonResume)(JNIEnv * env, jclass, jobject contex, jobject activitiy){
    controllerNative(nativeAppAddr)->onResume(env, contex, activitiy);
}
JNI_METHOD(void, JNIonPause)(JNIEnv *, jclass){
    controllerNative(nativeAppAddr)->onPause();
}
JNI_METHOD(void, JNIonDestroy)(JNIEnv *, jclass, long controller_addr){
    delete controllerNative(controller_addr);
}

JNIEnv *GetJniEnv() {
    JNIEnv *env;
    jint result = g_vm->AttachCurrentThread(&env, nullptr);
    return result == JNI_OK ? env : nullptr;
}