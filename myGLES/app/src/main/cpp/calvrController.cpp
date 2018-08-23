#include <cvrUtil/AndroidGetenv.h>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include <cvrMenu/MenuManager.h>
#include <cvrKernel/SceneManager.h>
#include <cvrConfig/ConfigManager.h>

#include "calvrController.h"
#include "utils.h"
#include "osg_utils.h"
#include <cvrUtil/AndroidGetenv.h>
#include <cvrMenu/SubMenu.h>

using namespace controller;
using namespace osg;
using namespace cvr;
calvrController::calvrController(AAssetManager *assetManager)
:_asset_manager(assetManager){
//    _viewer = new osgViewer::Viewer();
    _viewer = new cvr::CVRViewer();
    _viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
    _root = new Group;
    _menu =  cvr::MenuManager::instance();
    _scene = cvr::SceneManager::instance();
    _config = cvr::ConfigManager::instance();
    _interactionManager = cvr::InteractionManager::instance();
//    _tracking = TrackingManager::instance();
//    _navigation = cvr::Navigation::instance();
    _spatialViz = new SpatialViz();
    _menuBasics = new MenuBasics();
    initialize_camera();
}

void calvrController::initialize_camera() {
    osg::ref_ptr<osg::Camera> mainCam = _viewer->getCamera();
    mainCam->setClearColor(osg::Vec4f(0.81, 0.77, 0.75,1.0));
    osg::Vec3d eye = osg::Vec3d(0,-10,0);
    osg::Vec3d center = osg::Vec3d(0,0,.0);
    osg::Vec3d up = osg::Vec3d(0,0,1);
    mainCam->setViewMatrixAsLookAt(eye,center,up); // usual up vector
    mainCam->setRenderOrder(osg::Camera::NESTED_RENDER);

    _viewer->setCameraManipulator(new osgGA::TrackballManipulator);
    _viewer->getCameraManipulator()->setHomePosition(eye,center,up,false);
}

void calvrController::createDebugOSGSphere(osg::Vec3 pos) {
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Sphere(pos, 0.2f));
    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
    osg::ref_ptr<osg::Geode> node = new osg::Geode;
    Program * program = osg_utils::createShaderProgram("shaders/lightingOSG.vert","shaders/lightingOSG.frag",_asset_manager);

    osg::StateSet * stateSet = shape->getOrCreateStateSet();
    stateSet->setAttributeAndModes(program);
    stateSet->addUniform( new osg::Uniform("lightDiffuse",
                                           osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("lightSpecular",
                                           osg::Vec4(1.0f, 1.0f, 0.4f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("shininess", 64.0f) );

    stateSet->addUniform( new osg::Uniform("lightPosition", osg::Vec3(0.2,0.8,1)));

     node->addDrawable(shape.get());
    _root->addChild(node);
}
void calvrController::setupDefaultEnvironment(const char *root_path) {
    std::string homeDir = std::string(root_path) + "/";
    setenv("CALVR_HOME", homeDir);
    setenv("CALVR_CONFIG_DIR", homeDir+"config/");
    setenv("CALVR_RESOURCE_DIR", homeDir+"resources/");
    setenv("CALVR_CONFIG_FILE", homeDir+"config/config.xml");
}

void calvrController::onCreate(const char * calvr_path){
//    calvr = new cvr::CalVR();
//    if(!calvr->init(calvr_path)){
//        delete calvr;
//        return;
//    }
    setupDefaultEnvironment(calvr_path);

    createDebugOSGSphere(osg::Vec3(.0f, 0.1f, .0f));

    std::string fontfile = getenv("CALVR_RESOURCE_DIR");

    fontfile = fontfile + "arial.ttf";
    ///utils::loadFontFile(_asset_manager, fontfile.c_str());

    if(!_scene->init())
        LOGE("==========SCENE INITIALIZATION FAIL=========");

    _root->addChild(_scene->getSceneRoot());

    if(!_config->init())
        LOGE("==========CONFIG INITIALIZATION FAIL========");

    if(!_menu->init())
        LOGE("==========MENU INITIALIZATION FAIL=========");

    if(!_interactionManager->init())
        LOGE("==========INTERACTION MANAGER FAIL=========");

//    if(!_tracking->init())
//        LOGE("==========TRACKING MANAGER FAIL=========");
//
//    if(!_navigation->init())
//        LOGE("=========NAVIGATION FAIL===========");
    if(!_menuBasics->init())
        LOGE("MENU BASICS");

    if(!_spatialViz->init())
        LOGE("SPATIALVIZ INITIALIZATION FAIL");

    _viewer->setSceneData(_root.get());
}

void calvrController::onDrawFrame(){
//    calvr->frame();
    _menu->update();
    _interactionManager->update();
//    _tracking->update();
//    _navigation->update();
    _viewer->frame();
}

void calvrController::onViewChanged(int rot, int width, int height){
    _viewer->setUpViewerAsEmbeddedInWindow(0,0,width,height);
    _screenWidth = width;
    _screenHeight = height;
    _screen_ratio = height/540;
//    calvr->onViewChanged(width, height);
}

void calvrController::onResourceLoaded(const char *path) {
    FILE* fp = fopen(path, "r");
    if(nullptr == fp){
        LOGE("FAILED TO LOAD RESOURCE FILE");
        return;
    }
}
osg::Vec3f calvrController::screenToWorld(float x, float y) {
    return osg::Vec3f((x-_screenWidth/2)/_screen_ratio, 0, (_screenHeight/2 - y)/_screen_ratio);
}
void calvrController::onTouch(float x, float y) {
    LOGE("=================TOUCHED HERE============%f=====%f======", x, y);
//    MouseInteractionEvent * mie = new MouseInteractionEvent();
//    mie->setInteraction(BUTTON_UP);
//    mie->setButton(0);
    PointerInteractionEvent * pie = new PointerInteractionEvent();
    pie->setPointerType(PointerInteractionEvent::TOUCH_POINTER);
    pie->setButton(1);//secondary button
    pie->setInteraction(Interaction::BUTTON_DOWN);
    _interactionManager->addEvent(pie);
}
void calvrController::onDoubleTouch(float x, float y){
    PointerInteractionEvent * pie = new PointerInteractionEvent();
    pie->setPointerType(PointerInteractionEvent::TOUCH_POINTER);
    pie->setButton(1);//secondary button
    osg::Matrix m;
    m.makeTranslate(screenToWorld(x,y));
    pie->setTransform(m);
    pie->setInteraction(Interaction::BUTTON_DOUBLE_CLICK);
    _interactionManager->addEvent(pie);
}

void calvrController::onTouchMove(float srcx, float srcy, float destx, float desty) {
    LOGE("=================TOUCHED MOVE HERE=======================");
}