//
// Created by menghe on 7/22/2018.
//

#include "jni_interface_osg_gesture.h"
//
// Created by menghe on 7/11/2018.
//

#include <jni.h>
#include <string>
#include <GLES2/gl2.h>
#include <android/asset_manager_jni.h>

#include "gesture_controller.h"
#include "osgController.h"
using namespace std;

namespace {
    //global environment
    jlong nativeAppAddr = 0;
    const float TOUCH_MOTION_FACTOR = 0.001;
    inline jlong controllerPtr(gesture_controller * native_controller){
        return reinterpret_cast<intptr_t>(native_controller);
    }
    inline gesture_controller * controllerNative(jlong ptr){
        return reinterpret_cast<gesture_controller *>(ptr);
    }

}

/*Native Application methods*/
JNI_METHOD(void, JNICreateNode)
(JNIEnv* env, jclass, jlong osgAppAddr, jobject asset_manager){
    nativeAppAddr =  controllerPtr(new gesture_controller());
    AAssetManager * cpp_asset_manager = AAssetManager_fromJava(env, asset_manager);
    controllerNative(nativeAppAddr)->createNode(reinterpret_cast<osg_controller::osgController *>(osgAppAddr),cpp_asset_manager);
}

JNI_METHOD(void, JNIonTouched)
(JNIEnv*, jclass,jboolean down, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onTouched(down==JNI_TRUE, x * TOUCH_MOTION_FACTOR, y*TOUCH_MOTION_FACTOR);
}
JNI_METHOD(void, JNIonMove)
(JNIEnv*, jclass, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onMove(x*TOUCH_MOTION_FACTOR, y*TOUCH_MOTION_FACTOR);
}