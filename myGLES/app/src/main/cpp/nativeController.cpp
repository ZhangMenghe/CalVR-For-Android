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
void nativeController::onDrawFrame() {}
void nativeController::onViewChanged(int rot, int width, int height) {}