#ifndef ARCALVR_ALL_CONTROLLER_H
#define ARCALVR_ALL_CONTROLLER_H

#include <string>
#include <android/asset_manager.h>

#include <cvrKernel/CalVR.h>
#include "bgDrawable.h"

class allController {
protected:
    cvr::CalVR *_CalVR;
    AAssetManager * _asset_manager;

    // AUX OSG Node & drawable
    osg::ref_ptr<osg::Group>  _root, _sceneGroup;

    std::stack<cvr::glState> _glStateStack;
    bgDrawable *_bgDrawable;

    osg::ref_ptr<osg::Geode> createDebugOSGSphere(osg::Vec3f pos);
public:
    allController(AAssetManager *assetManager);

    ~allController();

    void onCreate(const char * calvr_path);

    void onPause();

    void onResume(void * env, void* context, void* activity);

    void onDrawFrame();

    void onViewChanged(int rot, int width, int height);

    void onResourceLoaded(const char* path);

    void onSingleTouchDown(int pointer_num, float x, float y);

    void onSingleTouchUp(int pointer_num, float x, float y);

    void onDoubleTouch(int pointer_num, float x, float y);

    void onTouchMove(int pointer_num, float x, float y);

    float getFPS();
};



#endif
