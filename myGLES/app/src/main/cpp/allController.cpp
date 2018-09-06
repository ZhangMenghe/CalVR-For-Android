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

    _ar_controller = new arcoreController;
    _bgDrawable = new bgDrawable();
    _sceneGroup = new Group;

    ////////////////////////////
//    stroke = new Stroke;
    _strokeDrawable = new strokeDrawable;
    _cameraPoseDrawable = new pointDrawable;

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
ref_ptr<osg::Geode> allController::createPointingStick(osg::Vec3f pos){
    osg::ref_ptr<osg::Geode> node = new osg::Geode;
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Box(pos, 0.01f, 0.5f, 0.01f));
    shape->setColor(osg::Vec4f(1.0f, .05f, .0f, 1.0f));
    Program * program = osg_utils::createShaderProgram("shaders/osgFollowCam.vert", "shaders/osgFollowCam.frag", _asset_manager);
    StateSet * stateSet = shape->getOrCreateStateSet();
    stateSet->setAttributeAndModes(program);
    _uniform_mvp = new Uniform(osg::Uniform::FLOAT_MAT4, "uMVP");
    stateSet->addUniform(_uniform_mvp);

    stateSet->addUniform( new osg::Uniform("lightDiffuse",
                                           osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("lightSpecular",
                                           osg::Vec4(1.0f, 1.0f, 0.4f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("shininess", 64.0f) );

    stateSet->addUniform( new osg::Uniform("lightPosition", osg::Vec3(0,0,1)));

    node->addDrawable(shape);
    return node;
}
ref_ptr<osg::Geode> allController::createDebugOSGSphere(osg::Vec3 pos) {
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Sphere(pos, 0.05f));
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

    stateSet->addUniform( new osg::Uniform("lightPosition", osg::Vec3(0,0,1)));

    node->addDrawable(shape.get());

    return node;
}

void allController::setupDefaultEnvironment(const char *root_path) {
    std::string homeDir = std::string(root_path) + "/";
    setenv("CALVR_HOST_NAME", "calvrHost");
    setenv("CALVR_HOME", homeDir);
    setenv("CALVR_ICON_DIR", homeDir+"icons/");
    setenv("CALVR_CONFIG_DIR", homeDir+"config/");
    setenv("CALVR_RESOURCE_DIR", homeDir+"resources/");
    setenv("CALVR_CONFIG_FILE", homeDir+"config/config.xml");
}

void allController::onCreate(const char * calvr_path){
//    calvr = new cvr::CalVR();
//    if(!calvr->init(calvr_path)){
//        delete calvr;
//        return;
//    }
    setupDefaultEnvironment(calvr_path);
//    _sceneGroup->addChild(createDebugOSGSphere(osg::Vec3(.0f,0.5f,.0f)));


    _sceneGroup->addChild( _strokeDrawable->createDrawableNode(_asset_manager,&glStateStack));
//    _strokeDrawable->getGLNode()->setNodeMask(0);
//    _sceneGroup->addChild(_cameraPoseDrawable->createDrawableNode(_asset_manager,&glStateStack));
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

//    _bgDrawable->getGLNode()->addChild(createPointingStick(osg::Vec3(.0f,.0f,.0f)));

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

    osg::Matrixd* mat = new osg::Matrixd(glm::value_ptr(_ar_controller->view_mat));
    Vec3d eye, center, up;
    mat->getLookAt(eye, center, up);
    _viewer->getCamera()->setViewMatrixAsLookAt(Vec3d(eye.x(), -eye.z(), eye.y()),
                                                Vec3d(center.x(), -center.z(), center.y()),
                                                Vec3d(up.x(), -up.z(), up.y()));
    _distance = 1000;
    debug_flag = true;

    if(!moveCam && debug_flag){

        _distance = 0;
        debug_flag = false;
    }
    float * transUV = _ar_controller->updateBackgroundRender();
    if(nullptr != transUV)
        _bgDrawable->updateOnFrame(transUV);

//    _ar_controller->renderStroke(_strokeDrawable);

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
//    if(moveCam)
        DrawRay(osg::Vec3f(0,0,0));
}

void allController::onViewChanged(int rot, int width, int height){
    _viewer->setUpViewerAsEmbeddedInWindow(0,0,width,height);
    _ar_controller->onViewChanged(rot, width, height);

//    double fovy, ratio, znear, zfar;
//    _viewer->getCamera()->getProjectionMatrixAsPerspective(fovy,ratio,znear,zfar);
//    _viewer->getCamera()->setProjectionMatrixAsPerspective(fovy, (float)width/height, znear, zfar);
//    stroke->onViewChange(_viewer->getCamera()->getProjectionMatrix());

    _screenWidth = width;
    _screenHeight = height;
    _screen_ratio = height/540;
//    calvr->onViewChanged(width, height);
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
//    return osg::Vec3f((x-_screenWidth/2)/_screen_ratio, 0, (_screenHeight/2 - y)/_screen_ratio);
    return osg::Vec3f(0,0,0);
}

void allController::commonMouseEvent(cvr::MouseInteractionEvent * mie,
                                       int pointer_num){
    mie->setButton(pointer_num - 1);
    mie->setHand(0);

    float * camera_pos = _ar_controller->getCameraPose();
    osg::Quat camRot = osg::Quat(camera_pos[0],-camera_pos[2],camera_pos[1],camera_pos[3]);
    Vec3f hand_pose = Vec3f(camera_pos[4], -camera_pos[6], camera_pos[5]-0.025f) + camRot * Vec3f(.0f, 0.1f,-80.0f);

    LOGE("===FROM: %f, %f, %f", hand_pose.x(), hand_pose.y(), hand_pose.z());

    osg::Matrix m, n;
    m.makeRotate(camRot);
    n.makeTranslate(hand_pose);
    mie->setTransform(m * n);
    _tracking->setTouchEventMatrix(m*n);
    _interactionManager->addEvent(mie);
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
    _touchX = x; _touchY = y;
    MouseInteractionEvent * mie = new MouseInteractionEvent();
    mie->setInteraction(BUTTON_DOWN);
    commonMouseEvent(mie, pointer_num);
//    if(pointer_num!=1)
//        commonMouseEvent(mie, pointer_num, x, y);
//    else{
//        mie->setButton(0);
//        mie->setHand(0);
//
//        float * camera_pos = _ar_controller->getCameraPose();
//        osg::Quat camRot = osg::Quat(camera_pos[0],-camera_pos[2],camera_pos[1],camera_pos[3]);
//        Vec3f hand_pose = Vec3f(camera_pos[4], -camera_pos[6], camera_pos[5]-0.025f) + camRot * Vec3f(.0f, 0.1f,-0.1f);
//
//        LOGE("===FROM: %f, %f, %f", hand_pose.x(), hand_pose.y(), hand_pose.z());
//
//        osg::Matrix m, n;
//        m.makeRotate(camRot);
//        n.makeTranslate(hand_pose);
//        mie->setTransform(m * n);
//        _tracking->setTouchEventMatrix(m*n);
//        _interactionManager->addEvent(mie);
//    }

//    if(pointer_num != 1)
//        return;
//    _strokeDrawable->getGLNode()->setNodeMask(~0);
//    _pointerBntDown = true;
}

void allController::onSingleTouchUp(int pointer_num, float x, float y){
    MouseInteractionEvent * mie = new MouseInteractionEvent();
    mie->setInteraction(BUTTON_UP);
    commonMouseEvent(mie, pointer_num);

//    if(pointer_num != 1)
//        return;
//    _strokeDrawable->getGLNode()->setNodeMask(0);
//    _pointerBntDown = true;
}

void allController::onDoubleTouch(int pointer_num, float x, float y){
    MouseInteractionEvent * mie = new MouseInteractionEvent();
    mie->setInteraction(BUTTON_DOUBLE_CLICK);
    commonMouseEvent(mie, pointer_num, x, y);
}

void allController::onTouchMove(int pointer_num, float destx, float desty) {
//    if(pointer_num != 1)
//        return;
//
//    _touchX = destx;  _touchY = desty;
//    MouseInteractionEvent * mie = new MouseInteractionEvent();
//    mie->setInteraction(BUTTON_DRAG);
//    commonMouseEvent(mie, pointer_num, destx, desty);
}

void allController::DrawRay(osg::Vec3f pos, bool pointReal){
    float offset[2];
    offset[0] = (_touchX-_screenWidth/2) / _screenWidth;
    offset[1] = (_screenHeight/2 - _touchY) / _screenHeight;

    _ar_controller->renderStroke(_strokeDrawable, offset, pointReal);

    //shoot the ray to check the interaction with menu
    MouseInteractionEvent * mie = new MouseInteractionEvent();
    mie->setInteraction(BUTTON_DRAG);
    commonMouseEvent(mie, 1);
}