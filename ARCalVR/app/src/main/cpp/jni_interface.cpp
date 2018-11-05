#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jni_interface.h"
#include "allController.h"

namespace {
    //maintain a reference to VM
    static JavaVM *g_vm = nullptr;
    //global environment
    jlong nativeAppAddr = 0;

    inline jlong controllerPtr(allController * native_controller){
        return reinterpret_cast<intptr_t>(native_controller);
    }
    inline allController * controllerNative(jlong ptr){
        return reinterpret_cast<allController *>(ptr);
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
    nativeAppAddr =  controllerPtr(new allController(cpp_asset_manager));
    return nativeAppAddr;
}

JNI_METHOD(void, JNIdrawFrame)
(JNIEnv *, jclass) {
    controllerNative(nativeAppAddr)->onDrawFrame();
}

JNI_METHOD(void, JNIonGlSurfaceCreated)
(JNIEnv * env, jclass, jstring calvr_path){
    const char* cpath = env->GetStringUTFChars(calvr_path, JNI_FALSE);
    controllerNative(nativeAppAddr)->onCreate(cpath);
    env->ReleaseStringUTFChars(calvr_path, cpath);
}

JNI_METHOD(void, JNIonViewChanged)
(JNIEnv*, jclass, jint rotation, jint width, jint height){
    controllerNative(nativeAppAddr)->onViewChanged(rotation, width, height);
}

JNI_METHOD(void, JNIonSingleTouchUp)(JNIEnv*, jclass, jint type, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onSingleTouchUp(static_cast<cvr::TouchType>(type), x, y);
}

JNI_METHOD(void, JNIonSingleTouchDown)(JNIEnv*, jclass, jint type, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onSingleTouchDown(static_cast<cvr::TouchType>(type), x, y);
}

JNI_METHOD(void, JNIonDoubleTouch)(JNIEnv*, jclass, jint type, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onDoubleTouch(static_cast<cvr::TouchType>(type), x, y);
}

JNI_METHOD(void, JNIonTouchMove)(JNIEnv*, jclass, jint type, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onTouchMove(static_cast<cvr::TouchType>(type), x, y);
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
JNI_METHOD(jfloat, JNIgetFPS)(JNIEnv *, jclass){
    return controllerNative(nativeAppAddr)->getFPS();
}

JNIEnv *GetJniEnv() {
    JNIEnv *env;
    jint result = g_vm->AttachCurrentThread(&env, nullptr);
    return result == JNI_OK ? env : nullptr;
}