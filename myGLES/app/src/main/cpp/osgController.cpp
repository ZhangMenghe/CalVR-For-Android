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
namespace {
    constexpr size_t kMaxNumberOfAndroidsToRender = 20;
    constexpr int32_t kPlaneColorRgbaSize = 16;

    const glm::vec3 kWhite = {255, 255, 255};

    constexpr std::array<uint32_t, kPlaneColorRgbaSize> kPlaneColorRgba = {
            {0xFFFFFFFF, 0xF44336FF, 0xE91E63FF, 0x9C27B0FF, 0x673AB7FF, 0x3F51B5FF,
                    0x2196F3FF, 0x03A9F4FF, 0x00BCD4FF, 0x009688FF, 0x4CAF50FF, 0x8BC34AFF,
                    0xCDDC39FF, 0xFFEB3BFF, 0xFFC107FF, 0xFF9800FF}};

    inline glm::vec3 GetRandomPlaneColor() {
        const int32_t colorRgba = kPlaneColorRgba[std::rand() % kPlaneColorRgbaSize];
        return glm::vec3(((colorRgba >> 24) & 0xff) / 255.0f,
                         ((colorRgba >> 16) & 0xff) / 255.0f,
                         ((colorRgba >> 8) & 0xff) / 255.0f);
    }
}

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
    _viewer->setCameraManipulator(_manipulator.get());

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
//    _root->addChild(_camera_renderer->createNode(_asset_manager));
//    _root->addChild(_plane_renderer->createNode(_asset_manager));
//    _root->addChild(_pointcloud_renderer->createNode(_asset_manager));
    _root->addChild(_object_renderer->createNode(_asset_manager, "models/andy.obj", "textures/andy.png"));

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
//    GLuint textureId = _camera_renderer->GetTextureId(_viewer);
//
//    _ar_controller->onDrawFrame(textureId);
//
//    _camera_renderer->Draw(_ar_controller, btn_status_normal);

/*


    //Render plane
    // get trackable PLANES
    ArTrackableList* plane_list = nullptr;
    ArTrackableList_create(_ar_session, & plane_list);
    CHECK(plane_list!= nullptr);
    ArSession_getAllTrackables(_ar_session, AR_TRACKABLE_PLANE,plane_list);
    ArTrackableList_getSize(_ar_session, plane_list, &_plane_num);

    for(int i=0; i<_plane_num; i++){
        ArTrackable * ar_trackable = nullptr;
        ArTrackableList_acquireItem(_ar_session, plane_list, i, &ar_trackable);

        //cast down trackable to plane
        ArPlane * ar_plane = ArAsPlane(ar_trackable);

        //check the trackingstate, if not tracking, skip rendering
        ArTrackingState trackingState;
        ArTrackable_getTrackingState(_ar_session, ar_trackable, &trackingState);
        if(trackingState != AR_TRACKING_STATE_TRACKING)
            continue;

        //check if the plane contain the subsume plane, if so, skip to avoid overlapping
        ArPlane * subsume_plane;
        ArPlane_acquireSubsumedBy(_ar_session, ar_plane, &subsume_plane);
        if(subsume_plane != nullptr)
            continue;
        vec3 plane_color;
        //Find if the plane already in the dic with specific color. Or add into dic
        auto iter = _plane_color_map.find(ar_plane);
        if(iter!=_plane_color_map.end()){
            plane_color = iter->second;
            ArTrackable_release(ar_trackable);
        }else{
            if(_this_is_the_first_plane){
                _this_is_the_first_plane = false;
                plane_color = kWhite;
            }else{
                plane_color = GetRandomPlaneColor();
            }
            _plane_color_map.insert({ar_plane, plane_color});
        }

        _plane_renderer->Draw(_ar_session, _viewer, ar_plane, proj_mat, view_mat,plane_color);
    }

    ArTrackableList_destroy(plane_list);
    plane_list = nullptr;*/

//    _ar_controller->updatePointCloudRenderer(_pointcloud_renderer);
    _viewer->frame();
    return;


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