#include <cvrUtil/AndroidStdio.h>
#include "allController.h"
#include <cvrUtil/AndroidHelper.h>
#include <osg/ShapeDrawable>

using namespace cvr;
using namespace osg;

allController::allController(AAssetManager *assetManager)
        :_asset_manager(assetManager){
    _CalVR = new CalVR();
    _root = nullptr;
    _sceneGroup = nullptr;
    _bgDrawable = new bgDrawable(&_glStateStack);
}

allController::~allController(){
    delete _CalVR;
}

void allController::onCreate(const char * calvr_path){
    if(!_CalVR->init(calvr_path, _asset_manager))
        LOGE("Failed to init calvr kernel");

    _root = new Group;
    _sceneGroup = new Group;


    _root->addChild(_bgDrawable->createDrawableNode());
    ARCoreManager::instance()->setCameraTextureTarget(_bgDrawable->GetTextureId());
//    _root->addChild(_sceneGroup);
//    _sceneGroup->addChild(_CalVR->getSceneRoot());
//    _root->addChild(createDebugOSGSphere(Vec3f(.0f, 0.5f, .0f)));
//    _sceneGroup->getOrCreateStateSet()->setRenderBinDetails(2,"RenderBin");
//    _sceneGroup->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
//    _root->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    _CalVR->setSceneData(_root.get());
}

void allController::onPause(){
    _CalVR->onPause();
}

void allController::onResume(void * env, void* context, void* activity){
    _CalVR->onResume(env, context, activity);
}

void allController::onDrawFrame(){
    _CalVR->frame();
    _bgDrawable->updateOnFrame(ARCoreManager::instance()->getTransformedUVs());
}

void allController::onViewChanged(int rot, int width, int height){
    _CalVR->onViewChanged(rot, width, height);
}

void allController::onResourceLoaded(const char* path){}

void allController::onSingleTouchDown(int pointer_num, float x, float y){}

void allController::onSingleTouchUp(int pointer_num, float x, float y){}

void allController::onDoubleTouch(int pointer_num, float x, float y){}

void allController::onTouchMove(int pointer_num, float x, float y){}

float allController::getFPS(){return 60.0f;}

ref_ptr<osg::Geode> allController::createDebugOSGSphere(osg::Vec3 pos) {
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Sphere(pos, 0.05f));
    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
    osg::ref_ptr<osg::Geode> node = new osg::Geode;
    Program * program = assetLoader::instance()->createShaderProgramFromFile("shaders/lighting.vert","shaders/lighting.frag");

    osg::StateSet * stateSet = shape->getOrCreateStateSet();
    stateSet->setAttributeAndModes(program);

    stateSet->addUniform( new osg::Uniform("lightDiffuse",
                                           osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("lightSpecular",
                                           osg::Vec4(1.0f, 1.0f, 0.4f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("shininess", 64.0f) );

    stateSet->addUniform( new osg::Uniform("lightPosition", osg::Vec3(0,0,1)));

    node->addDrawable(shape.get());

    return node;
}