#ifndef ARCALVR_ALL_CONTROLLER_H
#define ARCALVR_ALL_CONTROLLER_H

#include <string>
#include <android/asset_manager.h>

#include <cvrKernel/CalVR.h>
#include <cvrKernel/InteractionEvent.h>
#include "bgDrawable.h"
#include "jni_interface.h"

#include <cvrKernel/InteractionEvent.h>
#include <jni.h>
#include <cvrUtil/AndroidHelper.h>
class JNICallBackCallback:public osg::NodeCallback{
private:
    JNIEnv * _env;
    jclass _helper_class;
    jobject _obj;
    std::unordered_map<std::string, jmethodID > _map;
public:
    JNICallBackCallback(JNIEnv* env, jclass hclass, jobject obj):
    _env(env), _helper_class(hclass), _obj(obj){}

    void registerCallBackFunction(std::string funcName, const char* signature);

    virtual void operator()(osg::Node*node, osg::NodeVisitor * nv);
};

class allController {
protected:
    cvr::CalVR *_CalVR;
    AAssetManager * _asset_manager;

    // AUX OSG Node & drawable
    osg::ref_ptr<osg::Group>  _root, _sceneGroup;
    bgDrawable *_bgDrawable;
    int _touchX, _touchY;
    bool _detectStart = false;

    osg::ref_ptr<osg::Geode> createDebugOSGSphere(osg::Vec3f pos);

public:
    allController(AAssetManager *assetManager);

    ~allController();

    void onCreate(const char * calvr_path);

    void onViewChanged(int rot, int width, int height);

    void onPause();

    void onResume(void * env, void* context, void* activity);

    void onDrawFrame();

    void onSingleTouchDown(cvr::TouchType type, float x, float y);

    void onSingleTouchUp(cvr::TouchType type, float x, float y);

    void onDoubleTouch(cvr::TouchType type, float x, float y);

    void onTouchMove(cvr::TouchType type, float x, float y);

    float getFPS();

    void callJavaTest(const char* funcName);
};

#endif
