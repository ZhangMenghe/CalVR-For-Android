//
// Created by menghe on 8/3/2018.
//
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jni_interface_calvr.h"
#include "calvrController.h"

namespace {
    //global environment
    jlong nativeAppAddr = 0;

    inline jlong controllerPtr(controller::calvrController * native_controller){
        return reinterpret_cast<intptr_t>(native_controller);
    }
    inline controller::calvrController * controllerNative(jlong ptr){
        return reinterpret_cast<controller::calvrController *>(ptr);
    }
}

/*Native Application methods*/
JNI_METHOD(jlong, JNIcreateController)
(JNIEnv* env, jclass, jobject asset_manager){
    AAssetManager * cpp_asset_manager = AAssetManager_fromJava(env, asset_manager);
    nativeAppAddr =  controllerPtr(new controller::calvrController(cpp_asset_manager));
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

JNI_METHOD(void, JNIonResourceLoaded)
(JNIEnv* env, jclass, jstring path){
    const char* cpath = env->GetStringUTFChars(path, JNI_FALSE);
    controllerNative(nativeAppAddr)->onResourceLoaded(cpath);
    env->ReleaseStringUTFChars(path, cpath);
}

JNI_METHOD(void, JNIonViewChanged)
(JNIEnv*, jclass, jint rotation, jint width, jint height){
    controllerNative(nativeAppAddr)->onViewChanged(rotation, width, height);
}

JNI_METHOD(void, JNIonSingleTouchUp)(JNIEnv*, jclass, jint pointer_num, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onSingleTouchUp(pointer_num, x, y);
}

JNI_METHOD(void, JNIonSingleTouchDown)(JNIEnv*, jclass, jint pointer_num, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onSingleTouchDown(pointer_num, x, y);
}

JNI_METHOD(void, JNIonDoubleTouch)(JNIEnv*, jclass, jint pointer_num, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onDoubleTouch(pointer_num, x, y);
}

JNI_METHOD(void, JNIonTouchMove)(JNIEnv*, jclass, jint pointer_num, jfloat destx, jfloat desty){
    controllerNative(nativeAppAddr)->onTouchMove(pointer_num, destx, desty);
}