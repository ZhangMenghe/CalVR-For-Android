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