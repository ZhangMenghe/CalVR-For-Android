//
// Created by menghe on 7/18/2018.
//

#include "arcoreController.h"
using namespace glm;

void arcoreController::onDrawFrame(GLuint textureId) {
    if(_ar_session == nullptr)
        return;

    ArSession_setCameraTextureName(_ar_session, textureId);
    // Update session to get current frame and render camera background.
    if (ArSession_update(_ar_session, _ar_frame) != AR_SUCCESS) {
        LOGE("OnDrawFrame ArSession_update error");
    }
    ArCamera* camera;
    ArFrame_acquireCamera(_ar_session, _ar_frame, &camera);

    ArCamera_getViewMatrix(_ar_session, camera, value_ptr(view_mat));
    ArCamera_getProjectionMatrix(_ar_session,camera, 0.1f, 100.0f, value_ptr(proj_mat));

    ArPose* camera_pose = nullptr;
    ArPose_create(_ar_session, nullptr, &camera_pose);
    ArCamera_getPose(_ar_session, camera, camera_pose);
    float camera_pose_raw[7] = {0.f};
    ArPose_getPoseRaw(_ar_session, camera_pose, camera_pose_raw);
//    LOGE("ROTATION:====== %f === %f ==== %f === %f======", camera_pose_raw[0], camera_pose_raw[1], camera_pose_raw[2], camera_pose_raw[3]);
//    LOGE("POSITION:====== %f === %f ==== %f === ", camera_pose_raw[4], camera_pose_raw[5], camera_pose_raw[6]);

    ArCamera_getTrackingState(_ar_session, camera, &cam_track_state);
    ArCamera_release(camera);
}