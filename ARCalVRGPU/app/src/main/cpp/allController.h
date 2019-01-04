#ifndef ARCALVR_ALL_CONTROLLER_H
#define ARCALVR_ALL_CONTROLLER_H

#include <string>
#include <vector>
#include <android/asset_manager.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/stitching.hpp>
//#include <cvrKernel/CalVR.h>
//#include <cvrKernel/InteractionEvent.h>

#include "jni_interface.h"
#include "perfMonitor.h"

//#include <cvrKernel/InteractionEvent.h>
//#include <jni.h>
//#include <cvrUtil/AndroidHelper.h>
//class JNICallBackCallback:public osg::NodeCallback{
//private:
//    typedef struct funcParam{
//        jmethodID id;
//        int funMode;
//    }mFun;
//    JNIEnv * _env;
//    jclass _helper_class;
//    jobject _obj;
//    std::unordered_map<std::string, mFun > _map;
//public:
//    std::vector<float> sizeArr;
//    JNICallBackCallback(JNIEnv* env, jclass hclass, jobject obj):
//    _env(env), _helper_class(hclass), _obj(obj){}
//
//    void registerCallBackFunction(std::string funcName, const char* signature, int funMode = 0);
//
////    virtual void operator()(osg::Node*node, osg::NodeVisitor * nv);
//};

class allController {
protected:
//    cvr::CalVR *_CalVR;
    AAssetManager * _asset_manager;
    perfMonitor * _fpsMonitor;
//    JNICallBackCallback* JNIcallback;

    // AUX OSG Node & drawable
//    osg::ref_ptr<osg::Group>  _root, _sceneGroup;
//    bgDrawable *_bgDrawable;
    int _touchX, _touchY;
    bool _detectStart = false;

//    osg::ref_ptr<osg::Geode> createDebugOSGSphere(osg::Vec3f pos);

    cv::Mat _panoImg;
//        cv::Ptr<cv::Stitcher> _stitcher;
    cv::Stitcher _stitcher;
    std::vector<cv::Mat> _stichImages;
public:
    allController(AAssetManager *assetManager);

    ~allController();

    void onCreate(const char * calvr_path);

    void onViewChanged(int rot, int width, int height);

    void onPause();

    void onResume();

    void onDrawFrame();

    void processFrameImage(cv::Mat image);

//    void onSingleTouchDown(cvr::TouchType type, float x, float y);
//
//    void onSingleTouchUp(cvr::TouchType type, float x, float y);
//
//    void onDoubleTouch(cvr::TouchType type, float x, float y);
//
//    void onTouchMove(cvr::TouchType type, float x, float y);

    float getFPS(){return _fpsMonitor->Update();}
};

#endif
