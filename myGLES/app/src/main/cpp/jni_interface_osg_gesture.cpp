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

    inline jlong controllerPtr(gesture_controller * native_controller){
        return reinterpret_cast<intptr_t>(native_controller);
    }
    inline gesture_controller * controllerNative(jlong ptr){
        return reinterpret_cast<gesture_controller *>(ptr);
    }

}

/*Native Application methods*/
JNI_METHOD(void, JNIDrawSphere)
(JNIEnv* env, jclass, jlong osgAppAddr){
    nativeAppAddr =  controllerPtr(new gesture_controller());
    controllerNative(nativeAppAddr)->createSphere(reinterpret_cast<osg_controller::osgController *>(osgAppAddr));
}

JNI_METHOD(void, JNIonTouched)
(JNIEnv*, jclass, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onTouched(x, y);
}