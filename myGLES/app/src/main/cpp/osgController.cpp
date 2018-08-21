//
// Created by menghe on 7/12/2018.
//

#include "osgController.h"
#include "utils.h"
#include "arcore_utils.h"
#include "osg_utils.h"
#include "arcoreController.h"
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>
#include <array>
#include <osg/Camera>

using namespace osg_controller;
using namespace osg;
using namespace std;
using namespace glm;
class LightPosCallback : public osg::Uniform::Callback
{
public:
    virtual void operator()( osg::Uniform* uniform,
                             osg::NodeVisitor* nv )
    {
        const osg::FrameStamp* fs = nv->getFrameStamp();
        if ( !fs ) return;
        float angle = osg::inDegrees( (float)fs->getFrameNumber() );
        uniform->set( osg::Vec3(20.0f * cosf(angle), 20.0f *
                                                     sinf(angle), 1.0f) );
    }
};

//Deprecated:
//    _camera_renderer = new osg_cameraRenderer();
//    _plane_renderer = new osg_planeRenderer();
//    _pointcloud_renderer = new osg_pointcloudRenderer();
osgController::osgController(AAssetManager * manager)
        :_asset_manager(manager) {
    _viewer = new osgViewer::Viewer();
    // use single thread: critical for mobile and web
    _viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

    _root = new Group;
    _sceneGroup = new Group;

    //TODO: NO idea why can not add realize
    _initialize_camera();

    _ar_controller = new arcoreController();
    _bgDrawable = new bgDrawable();
    _monitor = new perfMonitor();
}

osgController::~osgController() {
    delete(_viewer);
}
void osgController::_initialize_camera() {
    osg::ref_ptr<osg::Camera> mainCam = _viewer->getCamera();
    mainCam->setClearColor(osg::Vec4f(0.81, 0.77, 0.75,1.0));
    osg::Vec3d eye = osg::Vec3d(0,-5,0);
    osg::Vec3d center = osg::Vec3d(0,0,.0);
    osg::Vec3d up = osg::Vec3d(0,0,1);
    // set position and orientation of the viewer
    mainCam->setViewMatrixAsLookAt(eye,center,up); // usual up vector
    mainCam->setRenderOrder(osg::Camera::NESTED_RENDER);
    mainCam->setCullingMode( osg::CullSettings::NO_CULLING );
    //TODO:RE-IMPLEMENT MYSELF MANIPULATOR
//    _viewer->setCameraManipulator(new osgGA::TrackballManipulator);
//    _viewer->setCameraManipulator(new osgGA::FirstPersonManipulator());
//    _viewer->getCameraManipulator()->setHomePosition(eye,center,up,false);

}
void osgController::createDebugOSGSphere(osg::Vec3 pos) {
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Sphere(pos, 0.1f));
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

    osg::ref_ptr<osg::Uniform> lightPos = new osg::Uniform(
            "lightPosition", osg::Vec3() );
    lightPos->setUpdateCallback( new LightPosCallback );
    stateSet->addUniform( lightPos.get() );

    node->addDrawable(shape.get());
    _sceneGroup->addChild(node);
}

/*DEPRECATE:
 *  _camera_renderer->createNode(_asset_manager);
 * _root->addChild(_camera_renderer->createNode(_asset_manager));
 * _root->addChild(_plane_renderer->createNode(_asset_manager));
 * _root->addChild(_pointcloud_renderer->createNode(_asset_manager));
 * */
void osgController::onCreate(const char* res_path) {
    if(drawSphere)
        createDebugOSGSphere(osg::Vec3(.0f,0.1f,-0.5f));

    if(drawPoints){
        _pointcloudDrawable = new pointDrawable();
        _sceneGroup->addChild(_pointcloudDrawable->createDrawableNode(_asset_manager, &glStateStack));
    }

    if(drawObj){
        _object_renderer = new osg_objectRenderer();
        _sceneGroup->addChild(_object_renderer->createNode(_asset_manager, "models/andy.obj", "textures/andy.png"));
    }

    if(drawBackground){
        _root->addChild(_bgDrawable->createDrawableNode(_asset_manager, &glStateStack));
        //This will make sure camera always in the background
        _sceneGroup->getOrCreateStateSet()->setRenderBinDetails(2,"RenderBin");
        _sceneGroup->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
        _root->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    }else{
        _bgDrawable->createDrawableNode(_asset_manager, &glStateStack);
    }

    if(drawFreetype){
        string font = string(res_path) + string("calvrAssets/resources/arial.ttf");
        _textDrawable = new freetypeDrawable(font.c_str());
        _textDrawable->addTargetString("Menu Title",-0.8f, 0.5f, 0.005);
        _textDrawable->addTargetString("MenuButton",-0.2f, .0f, 0.005);
        _sceneGroup->addChild(_textDrawable->createDrawableNode(_asset_manager, &glStateStack));
    }

    _root->addChild(_sceneGroup);
    _viewer->setSceneData(_root.get());
}

void osgController::onViewChanged(int rot, int width, int height) {
    _viewer->setUpViewerAsEmbeddedInWindow(0,0,width,height);
    _ar_controller->onViewChanged(rot, width, height);
}

void osgController::onPause() {
    _ar_controller->onPause();
}

void osgController::onResume(void *env, void *context, void *activity) {
    _ar_controller->onResume(env, context, activity);
}

void osgController::onDrawFrame(bool btn_status_normal) {
    //must call this func before update ar session
    _ar_controller->onDrawFrame(_bgDrawable->GetTextureId());
    if(MOVE_OSG_CAM){
        glm::mat4 rotatemat = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(1.0,0,0))* _ar_controller->view_mat;
        osg::Matrixd* mat = new osg::Matrixd(glm::value_ptr(rotatemat));
        _viewer->getCamera()->setViewMatrix(*mat);

//        osg::Vec3d center,eye,up;
//        _viewer->getCamera()->getViewMatrixAsLookAt(eye,center,up);
//        _viewer->getCameraManipulator()->setHomePosition(eye,center,up);
//        _viewer->home();
//        LOGE("eye: === %f === %f === %f", eye[0], eye[1], eye[2]);
//        LOGE("center: === %f === %f === %f", center[0], center[1], center[2]);
//        LOGE("up: === %f === %f === %f", up[0], up[1], up[2]);
    }
    if(drawPoints)
        _ar_controller->renderPointClouds(_pointcloudDrawable);

    if(drawObj)
        _object_renderer->Draw(_ar_controller->proj_mat,_ar_controller->view_mat,
                               glm::translate(glm::mat4(), glm::vec3(.0f, 0.1f, 0.0f)), _color_correction, 1);

    if(drawBackground){
        float * transUV = _ar_controller->updateBackgroundRender();
        if(nullptr != transUV)
            _bgDrawable->updateOnFrame(transUV);
    }

    if(drawPlane){
        if(!_ar_controller->isTracking())
            return;
        _ar_controller->doLightEstimation();

        PlaneParams planes = _ar_controller->doPlaneDetection();
        if(_plane_num < planes.plane_color_map.size()){
            for(int i= _plane_num; i<planes.plane_color_map.size();i++){
                planeDrawable * pd = new planeDrawable();
                _sceneGroup->addChild(pd->createDrawableNode(_asset_manager, &glStateStack));
                _planeDrawables.push_back(pd);
            }
            _plane_num = planes.plane_color_map.size();
        }
        auto planeIt = planes.plane_color_map.begin();
        for(int i=0; i<_plane_num; i++,planeIt++)
            _planeDrawables[i]->updateOnFrame(_ar_controller->getSession(), planeIt->first,
                                              _ar_controller->proj_mat, _ar_controller->view_mat,
                                              planeIt->second);
    }

    /*NEARLY DEPRECATE*/
    //_ar_controller->updatePointCloudRenderer(_pointcloud_renderer);
    //GLuint textureId = _camera_renderer->GetTextureId(_viewer);
    //_camera_renderer->Draw(_ar_controller, btn_status_normal);

    _viewer->frame();
}
void osgController::onTouched(float x, float y) {}
void osgController::debug_loadScene(const char *filename) {
    osg::Node *scene = osgDB::readNodeFile(filename);
    if(nullptr == scene){
            LOGE("FAILED TO LOAD SCENE");
            return;
    }
    scene->getOrCreateStateSet()->setAttribute(
                osg_utils::createShaderProgram("shaders/naiveOSG.vert", "shaders/naiveOSG.frag", _asset_manager));
    _root->addChild(scene);
}