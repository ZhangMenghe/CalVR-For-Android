//
// Created by menghe on 7/12/2018.
//

#include "osgController.h"
#include "utils.h"
#include "arcore_utils.h"

#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <array>

using namespace osg_controller;
using namespace osg;
using namespace std;
using namespace glm;

osgController::osgController(AAssetManager * manager)
        :_asset_manager(manager) {
    _viewer = new osgViewer::Viewer();
    // use single thread: critical for mobile and web
    _viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
    _viewer->setCameraManipulator(new osgGA::TrackballManipulator());
    _root = new Group;
    //TODO: NO idea why can not add realize
//    _viewer->realize();

    _viewer->addEventHandler(new osgViewer::StatsHandler);
    _viewer->addEventHandler(
            new osgGA::StateSetManipulator(
                    _viewer->getCamera()->getOrCreateStateSet()));
    _viewer->addEventHandler(new osgViewer::ThreadingHandler);
    _viewer->addEventHandler(new osgViewer::LODScaleHandler);

    _manipulator = new osgGA::KeySwitchMatrixManipulator;
    _manipulator->addMatrixManipulator('1', "Trackball",
                                       new osgGA::TrackballManipulator());
//    _viewer->setCameraManipulator(_manipulator.get());

    _viewer->getViewerStats()->collectStats("scene", true);

    _ar_controller = new arcoreController();
    _camera_renderer = new osg_cameraRenderer();
    _plane_renderer = new osg_planeRenderer();
    _pointcloud_renderer = new osg_pointcloudRenderer();
    _object_renderer = new osg_objectRenderer();
}

osgController::~osgController() {
    delete(_viewer);
}

void osgController::onCreate() {
//    _camera_renderer->createNode(_asset_manager);
    _root->addChild(_camera_renderer->createNode(_asset_manager));
//    _root->addChild(_plane_renderer->createNode(_asset_manager));
//    _root->addChild(_pointcloud_renderer->createNode(_asset_manager));
//    _root->addChild(_object_renderer->createNode(_asset_manager, "models/andy.obj", "textures/andy.png"));

    osgViewer::Viewer::Windows windows;
    _viewer->getWindows(windows);
    for (osgViewer::Viewer::Windows::iterator itr = windows.begin();
         itr != windows.end(); ++itr) {
        (*itr)->getState()->setUseModelViewAndProjectionUniforms(true);
        (*itr)->getState()->setUseVertexAttributeAliasing(true);
    }

    _viewer->setSceneData(_root.get());
    _manipulator->getNode();
    _viewer->home();

    _viewer->getDatabasePager()->clear();
    _viewer->getDatabasePager()->registerPagedLODs(_root.get());
    _viewer->getDatabasePager()->setUpThreads(3, 1);
    _viewer->getDatabasePager()->setTargetMaximumNumberOfPageLOD(2);
    _viewer->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(true, true);

    _viewer->getCamera()->setCullingMode(
            _viewer->getCamera()->getCullingMode() &
            ~osg::CullSettings::SMALL_FEATURE_CULLING);
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
    GLuint textureId = _camera_renderer->GetTextureId(_viewer);

    _ar_controller->onDrawFrame(textureId);

    _camera_renderer->Draw(_ar_controller, btn_status_normal);

//    if(!_ar_controller->isTracking())
//        return;
//    _ar_controller->doLightEstimation();
//    _ar_controller->doPlaneDetection(_plane_renderer);

//    _ar_controller->updatePointCloudRenderer(_pointcloud_renderer, _object_renderer);

//    const float mcolor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
//    _object_renderer->Draw(glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), mcolor, 1);

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