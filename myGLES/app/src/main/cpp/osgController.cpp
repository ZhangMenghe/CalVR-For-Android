//
// Created by menghe on 7/12/2018.
//

#include "osgController.h"
#include "utils.h"

using namespace osg_controller;

osgController::osgController() {
//    osg_utils::viewerInitialization(_viewer);
    _viewer = new Viewer;
    // use single thread: critical for mobile and web
    _viewer->setThreadingModel(ViewerBase::SingleThreaded);
    _viewer->setCameraManipulator(new osgGA::TrackballManipulator);
}
osgController::~osgController() {
    if(_ar_session) {
        ArSession_destroy(_ar_session);
        ArFrame_destroy(_ar_frame);
    }
    delete(_viewer);
}

void osgController::onCreate() {
    //TODO: this is just debug scene

}
void osgController::onViewChanged(int rot, int width, int height) {
    _viewer->setUpViewerAsEmbeddedInWindow(0,0,width,height);
    _displayRotation = rot;
    _width = width;
    _height = height;
    if (_ar_session != nullptr) {
        ArSession_setDisplayGeometry(_ar_session, _displayRotation, width, height);
    }
}
void osgController::onPause() {}
void osgController::onResume(void *env, void *context, void *activity) {}
void osgController::onDrawFrame(bool btn_status_normal) {
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
                osg_utils::createShaderProgram(osg_utils::vertexShader, osg_utils::fragmentShader));
    _viewer->setSceneData(scene);
}
