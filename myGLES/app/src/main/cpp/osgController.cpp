//
// Created by menghe on 7/12/2018.
//

#include "osgController.h"
#include "utils.h"
#include "arcore_utils.h"
#include "osg_utils.h"
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

osgController::osgController(AAssetManager * manager)
        :_asset_manager(manager) {
    _viewer = new osgViewer::Viewer();
    // use single thread: critical for mobile and web
    _viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

    _root = new Group;
    //TODO: NO idea why can not add realize
//
    _initialize_camera();

    _ar_controller = new arcoreController();
    _camera_renderer = new osg_cameraRenderer();
    _plane_renderer = new osg_planeRenderer();
    _pointcloud_renderer = new osg_pointcloudRenderer();
    _object_renderer = new osg_objectRenderer();
}

osgController::~osgController() {
    delete(_viewer);
}
void osgController::_initialize_camera() {
    osg::ref_ptr<osg::Camera> mainCam = _viewer->getCamera();
    mainCam->setClearColor(osg::Vec4f(0.81, 0.77, 0.75,1.0));
    osg::Vec3d eye = osg::Vec3d(0.000275, -1.5,-0.019077);
    osg::Vec3d center = osg::Vec3d(0,0,.0);
    osg::Vec3d up = osg::Vec3d(0,0,1);
    // set position and orientation of the viewer
    mainCam->setViewMatrixAsLookAt(eye,center,up); // usual up vector

    //TODO:RE-IMPLEMENT MYSELF MANIPULATOR
    _viewer->setCameraManipulator(new osgGA::TrackballManipulator);
//    _viewer->setCameraManipulator(new osgGA::FirstPersonManipulator());
    _viewer->getCameraManipulator()->setHomePosition(eye,center,up,false);;
}
void osgController::createDebugOSGPrimitive() {
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Sphere(osg::Vec3(.0f,.0f,.0f), 0.05f));
    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
    osg::ref_ptr<osg::Geode> node = new osg::Geode;

    Program * program = osg_utils::createShaderProgram("shaders/lightingOSG.vert","shaders/lightingOSG.frag",_asset_manager);

    osg::StateSet * stateSet = node->getOrCreateStateSet();
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
    _root->addChild(node);
}
void osgController::createDebugGLDrawable() {
    _glDrawable = new pointDrawable();
    _glDrawable->Initialization(_asset_manager, _viewer->getCamera());
    osg::ref_ptr<osg::Geode> glNode = new osg::Geode;
    glNode->addDrawable(_glDrawable.get());
    _glDrawable->setUseDisplayList(false);
    _root->addChild(glNode);
}
void osgController::onCreate() {
//    createDebugOSGPrimitive();
//    createDebugGLDrawable();


    _camera_renderer->createNode(_asset_manager);

//    _root->addChild(_camera_renderer->createNode(_asset_manager));
//    _root->addChild(_plane_renderer->createNode(_asset_manager));
//    _root->addChild(_pointcloud_renderer->createNode(_asset_manager));
//    _root->addChild(_object_renderer->createNode(_asset_manager, "models/andy.obj", "textures/andy.png"));
    createDebugGLDrawable();
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
void osgController::debug_tryDynamicDrawable(pointDrawable *drawable) {
    drawable->updateVertices();

}
void osgController::onDrawFrame(bool btn_status_normal) {
    //must call this func before update ar session
    GLuint textureId = _camera_renderer->GetTextureId(_viewer);

    _ar_controller->onDrawFrame(textureId);
    _ar_controller->renderPointClouds(_glDrawable);
//    _camera_renderer->Draw(_ar_controller, btn_status_normal);

//    if(!_ar_controller->isTracking())
//        return;
//    _ar_controller->doLightEstimation();
//    _ar_controller->doPlaneDetection(_plane_renderer);

//    _ar_controller->updatePointCloudRenderer(_pointcloud_renderer);

//    const float mcolor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
//    _object_renderer->Draw(glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), mcolor, 1);
//    debug_tryDynamicDrawable(_glDrawable);
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