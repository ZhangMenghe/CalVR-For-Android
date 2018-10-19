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
    cvr::ARcoreHelper::instance()->setMVPmat(proj_mat * view_mat);

    ArPose* camera_pose = nullptr;
    ArPose_create(_ar_session, nullptr, &camera_pose);
    ArCamera_getPose(_ar_session, camera, camera_pose);

    ArPose_getPoseRaw(_ar_session, camera_pose, camera_pose_raw);

    ArCamera_getTrackingState(_ar_session, camera, &cam_track_state);
    ArCamera_release(camera);
}

void arcoreController::updateHitTest(float x, float y){
    if(!_ar_frame || !_ar_session) return;
    ArHitResultList* hit_result_list = nullptr;
    ArHitResultList_create(_ar_session, &hit_result_list);
    CHECK(hit_result_list);
    ArFrame_hitTest(_ar_session, _ar_frame, x, y, hit_result_list);

    int32_t hit_result_list_size = 0;
    ArHitResultList_getSize(_ar_session, hit_result_list,
                            &hit_result_list_size);

    ArHitResult* ar_hit_result = nullptr;

    for (int32_t i = 0; i < hit_result_list_size; ++i) {
        ArHitResult *ar_hit = nullptr;
        ArHitResult_create(_ar_session, &ar_hit);
        ArHitResultList_getItem(_ar_session, hit_result_list, i, ar_hit);

        if (ar_hit == nullptr) {
            LOGE("HelloArApplication::OnTouched ArHitResultList_getItem error");
            return;
        }

        ArTrackable *ar_trackable = nullptr;
        ArHitResult_acquireTrackable(_ar_session, ar_hit, &ar_trackable);
        ArTrackableType ar_trackable_type = AR_TRACKABLE_NOT_VALID;
        ArTrackable_getType(_ar_session, ar_trackable, &ar_trackable_type);
        ///////////////ONLY CHECK PLANE/////////////////////////////
        if(ar_trackable_type != AR_TRACKABLE_PLANE)
            continue;

        ArPose* hit_pose = nullptr;
        ArPose_create(_ar_session, nullptr, &hit_pose);
        ArHitResult_getHitPose(_ar_session, ar_hit, hit_pose);
        int32_t in_polygon = 0;
        ArPlane* ar_plane = ArAsPlane(ar_trackable);
        ArPlane_isPoseInPolygon(_ar_session, ar_plane, hit_pose, &in_polygon);

        // Use hit pose and camera pose to check if hittest is from the
        // back of the plane, if it is, no need to create the anchor.
        ArPose* camera_pose = nullptr;
        ArPose_create(_ar_session, nullptr, &camera_pose);
        ArCamera* ar_camera;
        ArFrame_acquireCamera(_ar_session, _ar_frame, &ar_camera);
        ArCamera_getPose(_ar_session, ar_camera, camera_pose);
        ArCamera_release(ar_camera);
        float normal_distance_to_plane = arcore_utils::calculateDistanceToPlane(
                *_ar_session, *hit_pose, *camera_pose);

        ArPose_destroy(hit_pose);
        ArPose_destroy(camera_pose);

        if (!in_polygon || normal_distance_to_plane < 0) {
            continue;
        }
        ar_hit_result = ar_hit;
        break;
    }
    ////////////////////////////
    if(ar_hit_result){
        ArAnchor* anchor = nullptr;
        if (ArHitResult_acquireNewAnchor(_ar_session, ar_hit_result, &anchor) !=
            AR_SUCCESS) {
            LOGE(
                    "HelloArApplication::OnTouched ArHitResult_acquireNewAnchor error");
            return;
        }

        ArTrackingState tracking_state = AR_TRACKING_STATE_STOPPED;
        ArAnchor_getTrackingState(_ar_session, anchor, &tracking_state);
        if (tracking_state != AR_TRACKING_STATE_TRACKING) {
            ArAnchor_release(anchor);
            return;
        }

        _anchors.push_back(anchor);
//        ArHitResult_destroy(ar_hit_result);
//        ar_hit_result = nullptr;
//
//        ArHitResultList_destroy(hit_result_list);
//        hit_result_list = nullptr;
    }
}