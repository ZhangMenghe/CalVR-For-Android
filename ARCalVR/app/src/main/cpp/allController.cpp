#include <cvrUtil/AndroidStdio.h>
#include "allController.h"
using namespace cvr;
using namespace osg;

allController::allController(AAssetManager *assetManager)
        :_asset_manager(assetManager){
    _CalVR = new CalVR();
    _root = nullptr;
    _sceneGroup = nullptr;
}

allController::~allController(){
    delete _CalVR;
}

void allController::onCreate(const char * calvr_path){
    if(!_CalVR->init(calvr_path, _asset_manager))
        LOGE("Failed to init calvr kernel");
    _root = new Group;
    _sceneGroup = new Group;
    _root->addChild(_sceneGroup);
//    _root->addChild(_CalVR->getBackground());
    _sceneGroup->addChild(_CalVR->getSceneRoot());

    _sceneGroup->getOrCreateStateSet()->setRenderBinDetails(2,"RenderBin");
    _sceneGroup->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
    _root->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    _CalVR->setSceneData(_root.get());
}

void allController::onPause(){}

void allController::onResume(void * env, void* context, void* activity){}

void allController::onDrawFrame(){
    _CalVR->frame();
}

void allController::onViewChanged(int rot, int width, int height){
    _CalVR->onViewChanged(width, height);
}

void allController::onResourceLoaded(const char* path){}

void allController::onSingleTouchDown(int pointer_num, float x, float y){}

void allController::onSingleTouchUp(int pointer_num, float x, float y){}

void allController::onDoubleTouch(int pointer_num, float x, float y){}

void allController::onTouchMove(int pointer_num, float x, float y){}

float allController::getFPS(){return 60.0f;}