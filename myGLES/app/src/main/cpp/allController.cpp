#include <cvrUtil/AndroidGetenv.h>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include <cvrMenu/MenuManager.h>
#include <cvrKernel/SceneManager.h>
#include <cvrConfig/ConfigManager.h>

#include "allController.h"
#include "utils.h"
#include "osg_utils.h"
#include <cvrUtil/AndroidGetenv.h>
#include <cvrMenu/SubMenu.h>
#include "Stroke.h"
using namespace controller;
using namespace osg;
using namespace cvr;
allController::allController(AAssetManager *assetManager)
:_asset_manager(assetManager){
//    _viewer = new osgViewer::Viewer();
    _viewer = new cvr::CVRViewer();
    _viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
    _root = new Group;
    _menu =  cvr::MenuManager::instance();
    _scene = cvr::SceneManager::instance();
    _config = cvr::ConfigManager::instance();
    _interactionManager = cvr::InteractionManager::instance();
    _tracking = TrackingManager::instance();
    _navigation = cvr::Navigation::instance();
    _communication = cvr::ComController::instance();
    _spatialViz = new SpatialViz();
//    _menuBasics = new MenuBasics();

    _ar_controller = new arcoreController();
    _bgDrawable = new bgDrawable();
    _sceneGroup = new Group;
    _rayNodeTrans = new MatrixTransform;

    ////////////////////////////
//    singleWindowMultipleCameras();
    initialize_camera();
}

allController::~allController(){
    if(_spatialViz)
    {
        delete _spatialViz;
    }
    // should be friend class to call deconstructor
//    if(_file)
//    {
//        delete _file;
//    }
    /*if(_menu)
    {
        delete _menu;
    }
//    if(_threadedLoader)
//    {
//        delete _threadedLoader;
//    }
    if(_communication)
    {
        delete _communication;
    }
    if(_scene)
    {
        delete _scene;
    }
//    if(_screens)
//    {
//        delete _screens;
//    }
    if(_viewer)
    {
        delete _viewer;
    }
    if(_navigation)
    {
        delete _navigation;
    }
    if(_interactionManager)
    {
        delete _interactionManager;
    }
    if(_tracking)
    {
        delete _tracking;
    }
    if(_communication)
    {
        delete _communication;
    }
    if(_config)
    {
        delete _config;
    }*/
}

void allController::singleWindowMultipleCameras() {
    ref_ptr<osg::Camera> staticaCamera = new Camera;
    staticaCamera->setRenderOrder(osg::Camera::POST_RENDER);
    osg::Vec3d eye = osg::Vec3d(0,-10,0);
    osg::Vec3d center = osg::Vec3d(0,0,.0);
    osg::Vec3d up = osg::Vec3d(0,0,1);
    staticaCamera->setViewMatrixAsLookAt(eye,center,up); // usual up vector
    staticaCamera->setRenderOrder(osg::Camera::NESTED_RENDER);
    staticaCamera->setCullingMode( osg::CullSettings::NO_CULLING );

//    staticaCamera->addChild(createDebugOSGSphere(osg::Vec3(.0f,0.5f,.0f)));
//    _sceneGroup->addChild(staticaCamera);
    _sceneGroup->addChild(createDebugOSGSphere(osg::Vec3(.0f,0.5f,.0f)));

    createPointRay();
}

void allController::initialize_camera() {
    osg::ref_ptr<osg::Camera> mainCam = _viewer->getCamera();
    mainCam->setClearColor(osg::Vec4f(0.81, 0.77, 0.75,1.0));
    osg::Vec3d eye = osg::Vec3d(0,-10,0);
    osg::Vec3d center = osg::Vec3d(0,0,.0);
    osg::Vec3d up = osg::Vec3d(0,0,1);
    mainCam->setViewMatrixAsLookAt(eye,center,up); // usual up vector
    mainCam->setRenderOrder(osg::Camera::NESTED_RENDER);
    mainCam->setCullingMode( osg::CullSettings::NO_CULLING );
}

ref_ptr<osg::Geode> allController::createDebugOSGSphere(osg::Vec3 pos) {
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Sphere(pos, 0.8f));
    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
    osg::ref_ptr<osg::Geode> node = new osg::Geode;
    Program * program = osg_utils::createShaderProgram("shaders/lightingOSG.vert","shaders/lightingOSG.frag",_asset_manager);

    osg::StateSet * stateSet = shape->getOrCreateStateSet();
    stateSet->setAttributeAndModes(program);

    _projMat = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uProjMat");
    stateSet->addUniform(_projMat);

//    stateSet->addUniform( new osg::Uniform("uProjMat",
//                                           *(new osg::RefMatrixf(_viewer->getCamera()->getProjectionMatrix()))));

    stateSet->addUniform( new osg::Uniform("uViewMat",
                                           *(new osg::RefMatrixf(_viewer->getCamera()->getViewMatrix()))));

    stateSet->addUniform( new osg::Uniform("lightDiffuse",
                                           osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("lightSpecular",
                                           osg::Vec4(1.0f, 1.0f, 0.4f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("shininess", 64.0f) );

    stateSet->addUniform( new osg::Uniform("lightPosition", osg::Vec3(0,0,1)));

    node->addDrawable(shape.get());

    return node;
}

void allController::setupDefaultEnvironment(const char *root_path) {
    std::string homeDir = std::string(root_path) + "/";
    setenv("CALVR_HOST_NAME", "calvrHost");
    setenv("CALVR_HOME", homeDir);
    setenv("CALVR_ICON_DIR", homeDir + "icons/");
    setenv("CALVR_CONFIG_DIR", homeDir+"config/");
    setenv("CALVR_RESOURCE_DIR", homeDir+"resources/");
    setenv("CALVR_CONFIG_FILE", homeDir+"config/config.xml");
}

void allController::createPointRay() {
    Geode * rayNode = new Geode;
    Vec3f start_pos = osg::Vec3f(0,0,0);
    Vec3f end_pos = osg::Vec3f(.0f, 1000.0f, 50.0f);
    stroke = new Stroke(start_pos, end_pos);
    rayNode->addDrawable(stroke);

    _rayNodeTrans->addChild(rayNode);
    _sceneGroup->addChild(_rayNodeTrans);
    _rayNodeTrans->setNodeMask(0);
}

void allController::onCreate(const char * calvr_path){
//    calvr = new cvr::CalVR();
//    if(!calvr->init(calvr_path)){
//        delete calvr;
//        return;
//    }
    setupDefaultEnvironment(calvr_path);
    _sceneGroup->addChild(createDebugOSGSphere(osg::Vec3(.0f,0.5f,.0f)));


    //Initialization should follow a specific order

    if(!_config->init())
        LOGE("==========CONFIG INITIALIZATION FAIL========");
    if(!_communication->init())
        LOGE("==========INTERACTION MANAGER FAIL=========");
    if(!_tracking->init())
        LOGE("==========TRACKING MANAGER FAIL=========");
    if(!_interactionManager->init())
        LOGE("==========INTERACTION MANAGER FAIL=========");
    if(!_navigation->init())
        LOGE("=========NAVIGATION FAIL===========");
    if(!_scene->init())
        LOGE("==========SCENE INITIALIZATION FAIL=========");


    if(!_menu->init())
        LOGE("==========MENU INITIALIZATION FAIL=========");

//    if(!_menuBasics->init())
//        LOGE("MENU BASICS");

    if(!_spatialViz->init())
        LOGE("SPATIALVIZ INITIALIZATION FAIL");



    _root->addChild(_bgDrawable->createDrawableNode(_asset_manager, &glStateStack));
    //This will make sure camera always in the background
    _sceneGroup->getOrCreateStateSet()->setRenderBinDetails(2,"RenderBin");
    _sceneGroup->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
    _root->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    _sceneGroup->addChild(_scene->getSceneRoot());
    _root->addChild(_sceneGroup);
    _viewer->setSceneData(_root.get());
}

void allController::onDrawFrame(bool moveCam){
    _ar_controller->onDrawFrame(_bgDrawable->GetTextureId());


    if(moveCam){
        osg::Matrixd* mat = new osg::Matrixd(glm::value_ptr(_ar_controller->view_mat));
        Vec3d eye, center, up;
        mat->getLookAt(eye, center, up);
        _viewer->getCamera()->setViewMatrixAsLookAt(Vec3d(eye.x(), -eye.z(), eye.y()),
                                                    Vec3d(center.x(), -center.z(), center.y()),
                                                    Vec3d(up.x(), -up.z(), up.y()));
//        LOGE("==EYE==%f===%f===%f", eye.x(), -eye.z(), eye.y());
//        osg::Matrixd proj = _viewer->getCamera()->getProjectionMatrix();
//
//        osg::Matrix MVPW = _viewer->getCamera()->getViewMatrix() * proj;
//        osg::Vec3f point = Vec3f(eye.x(), -eye.z(), eye.y()) * MVPW;
//        LOGE("==Proj==%f===%f===%f", point.x(), point.y(), point.z());
//        LOGE("==Proj2==%f===%f===%f", point.x()/point.z(), point.y())/point.z();
        _distance = 1000;
        debug_flag = true;
    }

    if(!moveCam && debug_flag){

        _distance = 0;
        debug_flag = false;
    }
    float * transUV = _ar_controller->updateBackgroundRender();
    if(nullptr != transUV)
        _bgDrawable->updateOnFrame(transUV);


    _viewer->frameStart();
    _viewer->advance(USE_REFERENCE_TIME);
//    _viewer->eventTraversal();

    _tracking->update();
    _scene->update();
    _menu->update();
    _interactionManager->update();

    _navigation->update();
    _scene->postEventUpdate();
//    _viewer->frame();
    _viewer->updateTraversal();
    _viewer->renderingTraversals();
    if(_communication->getIsSyncError())
        LOGE("Sync error");
    if(_pointerBntDown)
        DrawRay(osg::Vec3f(0,0,0));
}

void allController::onViewChanged(int rot, int width, int height){
    _viewer->setUpViewerAsEmbeddedInWindow(0,0,width,height);

    double fovy, ratio, znear, zfar;
    _viewer->getCamera()->getProjectionMatrixAsPerspective(fovy,ratio,znear,zfar);
    _viewer->getCamera()->setProjectionMatrixAsPerspective(fovy, (float)width/height, znear, zfar);
    _projMat->set(*(new osg::RefMatrixf(_viewer->getCamera()->getProjectionMatrix())));

    _screenWidth = width;
    _screenHeight = height;
    _screen_ratio = height/540;
//    calvr->onViewChanged(width, height);
    _ar_controller->onViewChanged(rot, width, height);
}
void allController::onPause() {
    _ar_controller->onPause();
}

void allController::onResume(void *env, void *context, void *activity) {
    _ar_controller->onResume(env, context, activity);
}
void allController::onResourceLoaded(const char *path) {
    FILE* fp = fopen(path, "r");
    if(nullptr == fp){
        LOGE("FAILED TO LOAD RESOURCE FILE");
        return;
    }
}
osg::Vec3f allController::screenToWorld(float x, float y) {
//    osg::Matrixd proj = _viewer->getCamera()->getProjectionMatrix();
//
//    osg::Viewport* viewport = _viewer->getCamera()->getViewport();
//    double mx = viewport->x() + (int)((double )viewport->width()*(x*0.5+0.5));
//    double my = viewport->y() + (int)((double )viewport->height()*(y*0.5+0.5));
//
//    osg::Matrix MVPW = _viewer->getCamera()->getViewMatrix() * proj;
////    MVPW.postMult( _viewer->getCamera()->getViewport()->computeWindowMatrix() );
//
//
//    osg::Vec3 nearPoint = osg::Vec3((x-_screenWidth/2)/_screen_ratio,
//                                    0.f,
//                                    (_screenHeight/2 - y)/_screen_ratio) * MVPW;
//    LOGE("debug========%f=======%f=========%f", nearPoint[0], nearPoint[1], nearPoint[2]);
//    LOGE("debug2========%f=======%f======%d======%d", x,y,_screenWidth,_screenHeight);
//    return osg::Vec3f((x-_screenWidth/2)/_screen_ratio, 0, (_screenHeight/2 - y)/_screen_ratio);
    return osg::Vec3f(0,0,0);
}

void allController::commonMouseEvent(cvr::MouseInteractionEvent * mie,
                                       int pointer_num, float x, float y){
    mie->setButton(pointer_num - 1);
    mie->setX(x);
    mie->setY(y);
    mie->setHand(0);
    osg::Matrix m;
    m.makeTranslate(screenToWorld(x,y));
    mie->setTransform(m);
    _tracking->setTouchEventMatrix(m);
    _interactionManager->addEvent(mie);
}

void allController::onSingleTouchDown(int pointer_num, float x, float y) {
    MouseInteractionEvent * mie = new MouseInteractionEvent();
    mie->setInteraction(BUTTON_DOWN);
    commonMouseEvent(mie, pointer_num, x, y);
    /////
    _rayNodeTrans->setNodeMask(~0);
    _pointerBntDown = true;
//    DrawRay(screenToWorld(x,y));
}

void allController::onSingleTouchUp(int pointer_num, float x, float y){
    MouseInteractionEvent * mie = new MouseInteractionEvent();
    mie->setInteraction(BUTTON_UP);
    commonMouseEvent(mie, pointer_num, x, y);
    //hide ray
    _rayNodeTrans->setNodeMask(0);
    _pointerBntDown = false;
}

void allController::onDoubleTouch(int pointer_num, float x, float y){
    MouseInteractionEvent * mie = new MouseInteractionEvent();
    mie->setInteraction(BUTTON_DOUBLE_CLICK);
    commonMouseEvent(mie, pointer_num, x, y);
}

void allController::onTouchMove(int pointer_num, float destx, float desty) {
    MouseInteractionEvent * mie = new MouseInteractionEvent();
    mie->setInteraction(BUTTON_DRAG);
    commonMouseEvent(mie, pointer_num, destx, desty);
//    DrawRay(osg::Vec3f(.0,.0,.0));
}

void allController::DrawRay(osg::Vec3f pos){
//    float * camera_pose  = _ar_controller->getCurrentCameraPose();
////    LOGE("===============%f================%f=================%f==============", camera_pose[4], camera_pose[5], camera_pose[6]);
//    _rayNodeTrans->setMatrix(osg::Matrixd::translate(osg::Vec3f(camera_pose[4], -camera_pose[6], camera_pose[5])));
//    stroke->setBegin(camera_pose[4], camera_pose[5]);
//    myPat->setPosition(osg::Vec3(camera_pose[4], 0, 0));
}