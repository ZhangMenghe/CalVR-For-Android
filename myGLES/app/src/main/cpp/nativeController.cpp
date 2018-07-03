//
// Created by menghe on 6/29/2018.
//

#include "nativeController.h"


using namespace controller;

nativeController::nativeController(AAssetManager * assetManager)
        : _asset_manager(assetManager){
    _plane_renderer = new planeRenderer();
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
    _background_renderer->InitializeGlContent(_asset_manager);
    _plane_renderer->Initialization(_asset_manager);
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

    CHECK(_ar_session);

    //must call this func before update ar session
    ArSession_setCameraTextureName(_ar_session,_background_renderer->GetTextureId());
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

    _background_renderer->Draw(_ar_session, _ar_frame);
    //not tracking anything
    if(cam_track_state != AR_TRACKING_STATE_TRACKING)
        return;
}
void nativeController::onViewChanged(int rot, int width, int height) {}