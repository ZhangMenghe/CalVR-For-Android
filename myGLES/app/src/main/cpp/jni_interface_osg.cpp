//
// Created by menghe on 7/11/2018.
//

#include <jni.h>
#include <string>
#include <GLES2/gl2.h>
#include <android/asset_manager_jni.h>

#include "osgController.h"
#include "jni_interface_osg.h"

using namespace std;
using namespace osg_controller;

namespace {
    //maintain a reference to VM
    static JavaVM *g_vm = nullptr;
    //global environment
    jlong nativeAppAddr = 0;

    inline jlong controllerPtr(osgController * native_controller){
        return reinterpret_cast<intptr_t>(native_controller);
    }
    inline osg_controller::osgController * controllerNative(jlong ptr){
        return reinterpret_cast<osgController *>(ptr);
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
    nativeAppAddr =  controllerPtr(new osgController(cpp_asset_manager));
    return nativeAppAddr;
}


JNI_METHOD(void, JNIdrawFrame)
        (JNIEnv *, jclass, jboolean btn_status_normal) {
    //draw();//drawPrimitive
    controllerNative(nativeAppAddr)->onDrawFrame(btn_status_normal);
}

JNI_METHOD(void, JNIonGlSurfaceCreated)
        (JNIEnv *env, jclass, jstring res_path){
    const char* cpath = env->GetStringUTFChars(res_path, JNI_FALSE);
    controllerNative(nativeAppAddr)->onCreate(cpath);
    env->ReleaseStringUTFChars(res_path, cpath);
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
(JNIEnv*, jclass){
return static_cast<jboolean>
(controllerNative(nativeAppAddr)->hasDetectedPlane()? JNI_TRUE:JNI_FALSE);
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
JNI_METHOD(void, JNIDebugScene)(JNIEnv *env, jclass, jstring filename){
    const char* cpath = env->GetStringUTFChars(filename, JNI_FALSE);
    controllerNative(nativeAppAddr)->debug_loadScene(cpath);
    env->ReleaseStringUTFChars(filename, cpath);
}
JNI_METHOD(jfloat, JNIgetFPS)(JNIEnv *, jclass){
    return controllerNative(nativeAppAddr)->getFPS();
}
JNIEnv *GetJniEnv() {
    JNIEnv *env;
    jint result = g_vm->AttachCurrentThread(&env, nullptr);
    return result == JNI_OK ? env : nullptr;
}
