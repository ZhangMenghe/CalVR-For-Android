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

calvrController::calvrController(AAssetManager *assetManager)
:_asset_manager(assetManager){
//    _viewer = new osgViewer::Viewer();
    _viewer = new cvr::CVRViewer();
    _viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
    _root = new Group;
    _menu =  cvr::MenuManager::instance();
    _scene = cvr::SceneManager::instance();
    _config = cvr::ConfigManager::instance();
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
    setenv("CALVR_RESOURCE_DIR", homeDir+"icons/");
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
    if(!_scene->init())
        LOGE("==========SCENE INITIALIZATION FAIL=========");

    _root->addChild(_scene->getSceneRoot());

    if(!_config->init())
        LOGE("==========CONFIG INITIALIZATION FAIL========");

    if(!_menu->init())
        LOGE("==========MENU INITIALIZATION FAIL=========");

    _viewer->setSceneData(_root.get());
}

void calvrController::onDrawFrame(){
//    calvr->frame();
    _menu->update();
    _viewer->frame();
}

void calvrController::onViewChanged(int rot, int width, int height){
    _viewer->setUpViewerAsEmbeddedInWindow(0,0,width,height);
//    calvr->onViewChanged(width, height);
}

void calvrController::onResourceLoaded(const char *path) {
    FILE* fp = fopen(path, "r");
    if(nullptr == fp){
        LOGE("FAILED TO LOAD RESOURCE FILE");
        return;
    }
}