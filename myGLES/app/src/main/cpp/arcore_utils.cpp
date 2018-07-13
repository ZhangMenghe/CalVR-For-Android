//
// Created by menghe on 7/2/2018.
//

#include "arcore_utils.h"


using namespace arcore_utils;

vec3 arcore_utils::getPlaneNormal(const ArSession &arSession, const ArPose &arPose) {
    float plane_pose_raw[7] = {.0f};
    ArPose_getPoseRaw(&arSession, &arPose, plane_pose_raw);
    quat plane_quaternion(plane_pose_raw[3], plane_pose_raw[0],
                          plane_pose_raw[1], plane_pose_raw[2]);
    // Get normal vector, normal is defined to be positive Y-position in local
    // frame.
    return rotate(plane_quaternion, vec3(0., 1.f, 0.));
}

float arcore_utils::calculateDistanceToPlane(const ArSession& ar_session,
                               const ArPose& plane_pose,
                               const ArPose& camera_pose) {
    float plane_pose_raw[7] = {0.f};
    ArPose_getPoseRaw(&ar_session, &plane_pose, plane_pose_raw);
    glm::vec3 plane_position(plane_pose_raw[4], plane_pose_raw[5],
                             plane_pose_raw[6]);
    glm::vec3 normal = getPlaneNormal(ar_session, plane_pose);

    float camera_pose_raw[7] = {0.f};
    ArPose_getPoseRaw(&ar_session, &camera_pose, camera_pose_raw);
    glm::vec3 camera_P_plane(camera_pose_raw[4] - plane_position.x,
                             camera_pose_raw[5] - plane_position.y,
                             camera_pose_raw[6] - plane_position.z);
    return glm::dot(normal, camera_P_plane);
}

void arcore_utils::getTransformMatrixFromAnchor(const ArAnchor& ar_anchor,
                                  ArSession* ar_session,
                                  glm::mat4* out_model_mat) {
    if (out_model_mat == nullptr) {
        //LOGE("util::GetTransformMatrixFromAnchor model_mat is null.");
        return;
    }
    ArPose* pose_;
    ArPose_create(ar_session, nullptr, &pose_);
    ArAnchor_getPose(ar_session, &ar_anchor, pose_);
    ArPose_getMatrix(ar_session, pose_,
                     glm::value_ptr(*out_model_mat));
    ArPose_destroy(pose_);
}
bool arcore_utils::getNdkImageProperties(const AImage *ndk_image, int32_t *out_format,
                                         int32_t *out_width, int32_t *out_height,
                                         int32_t *out_plane_num, int32_t *out_stride) {
    if (ndk_image == nullptr) {
        return false;
    }
    media_status_t status = AImage_getFormat(ndk_image, out_format);
    if (status != AMEDIA_OK) {
        return false;
    }

    status = AImage_getWidth(ndk_image, out_width);
    if (status != AMEDIA_OK) {
        return false;
    }

    status = AImage_getHeight(ndk_image, out_height);
    if (status != AMEDIA_OK) {
        return false;
    }

    status = AImage_getNumberOfPlanes(ndk_image, out_plane_num);
    if (status != AMEDIA_OK) {
        return false;
    }

    status = AImage_getPlaneRowStride(ndk_image, 0, out_stride);
    if (status != AMEDIA_OK) {
        return false;
    }

    return true;
}