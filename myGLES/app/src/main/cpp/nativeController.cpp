//
// Created by menghe on 6/29/2018.
//
#include <array>
#include "nativeController.h"


using namespace controller;
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
nativeController::nativeController(AAssetManager * assetManager)
        : _asset_manager(assetManager){
    _camera_renderer = new cameraRenderer();
    _plane_renderer = new planeRenderer();
    _pointcloud_renderer = new pointcloudRenderer();
}

nativeController::~nativeController() {
    if(_ar_session) {
        ArSession_destroy(_ar_session);
        ArFrame_destroy(_ar_frame);
    }
}

void nativeController::onTouched(float x, float y) {
    LOGE("================called from native onTouch================");
}
void nativeController::onCreate() {
    //On surface created
    _camera_renderer->Initialization(_asset_manager);
    _plane_renderer->Initialization(_asset_manager);
    _pointcloud_renderer->Initialization(_asset_manager);
}

void nativeController::onPause() {
    if(_ar_session)
        ArSession_pause(_ar_session);
}
void nativeController::onResume(void *env, void *context, void *activity) {
    if(nullptr == _ar_session){
        ArInstallStatus install_status;
        bool user_requested_install = !_install_requested;

        CHECK(ArCoreApk_requestInstall(env, activity, user_requested_install,&install_status) == AR_SUCCESS);

        switch (install_status) {
            case AR_INSTALL_STATUS_INSTALLED:
                break;
            case AR_INSTALL_STATUS_INSTALL_REQUESTED:
                _install_requested = true;
                return;
        }
        CHECK(ArSession_create(env, context, &_ar_session)==AR_SUCCESS);
        CHECK(_ar_session);
        ArFrame_create(_ar_session, &_ar_frame);
        CHECK(_ar_frame);
        ArSession_setDisplayGeometry(_ar_session, _displayRotation, _width, _height);
    }
    const ArStatus status = ArSession_resume(_ar_session);
    CHECK(status == AR_SUCCESS);
}
void nativeController::onDrawFrame() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(_ar_session == nullptr)
        return;

    //must call this func before update ar session
    ArSession_setCameraTextureName(_ar_session,_camera_renderer->GetTextureId());
    // Update session to get current frame and render camera background.
    if (ArSession_update(_ar_session, _ar_frame) != AR_SUCCESS) {
        LOGE("OnDrawFrame ArSession_update error");
    }

    ArCamera* camera;
    ArFrame_acquireCamera(_ar_session, _ar_frame, &camera);

    mat4 view_mat;
    mat4 proj_mat;

    ArCamera_getViewMatrix(_ar_session, camera, value_ptr(view_mat));
    ArCamera_getProjectionMatrix(_ar_session,camera, 0.1f, 100.0f, value_ptr(proj_mat));

    ArTrackingState cam_track_state;
    ArCamera_getTrackingState(_ar_session, camera, &cam_track_state);
    ArCamera_release(camera);

    _camera_renderer->Draw(_ar_session, _ar_frame);
    //not tracking anything
    if(cam_track_state != AR_TRACKING_STATE_TRACKING)
        return;

    ArLightEstimate* ar_light_estimate;
    ArLightEstimateState ar_light_estimate_state;
    ArLightEstimate_create(_ar_session, &ar_light_estimate);

    ArFrame_getLightEstimate(_ar_session, _ar_frame, ar_light_estimate);
    ArLightEstimate_getState(_ar_session, ar_light_estimate, &ar_light_estimate_state);
    if(ar_light_estimate_state == AR_LIGHT_ESTIMATE_STATE_VALID)
        ArLightEstimate_getColorCorrection(_ar_session, ar_light_estimate, _color_correction);
    ArLightEstimate_destroy(ar_light_estimate);

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

        _plane_renderer->Draw(_ar_session, ar_plane, proj_mat, view_mat,plane_color);
    }
    ArTrackableList_destroy(plane_list);
    plane_list = nullptr;

    //TODO:Render point clouds
    ArPointCloud * pointCloud;
    ArStatus  pointcloud_Status = ArFrame_acquirePointCloud(_ar_session, _ar_frame, &pointCloud);
    if(pointcloud_Status != AR_SUCCESS)
        return;
    _pointcloud_renderer->Draw(_ar_session, pointCloud, proj_mat*view_mat);
    ArPointCloud_release(pointCloud);
}
void nativeController::onViewChanged(int rot, int width, int height) {}