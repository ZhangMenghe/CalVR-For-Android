//
// Created by menghe on 7/11/2018.
//

#include <jni.h>
#include <string>
#include <GLES2/gl2.h>

#include "osg_application.h"

using namespace std;

osgApplication app;

extern "C"{
JNIEXPORT void JNICALL Java_com_example_menghe_mygles_simpleOsgActivity_step(JNIEnv*env, jobject obj);
JNIEXPORT void JNICALL Java_com_example_menghe_mygles_simpleOsgActivity_init(JNIEnv*env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_example_menghe_mygles_simpleOsgActivity_loadModel(JNIEnv*env, jobject obj, jstring filename);
};

JNIEXPORT void JNICALL Java_com_example_menghe_mygles_simpleOsgActivity_step(JNIEnv*env, jobject obj){
    app.Draw();
}
JNIEXPORT void JNICALL Java_com_example_menghe_mygles_simpleOsgActivity_init(JNIEnv*env, jobject obj, jint width, jint height){
    app.setupWindow(width, height);
}
JNIEXPORT void JNICALL Java_com_example_menghe_mygles_simpleOsgActivity_loadModel(JNIEnv*env, jobject obj, jstring filename){
    const char *cpath = env->GetStringUTFChars(filename, JNI_FALSE);
    app.loadScene(cpath);
    env->ReleaseStringUTFChars(filename, cpath);
}
