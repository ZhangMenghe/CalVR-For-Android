//
// Created by menghe on 7/2/2018.
//

#ifndef MYGLES_ARCORE_UTILS_H
#define MYGLES_ARCORE_UTILS_H

#include <glm.hpp>

#include <media/NdkImage.h>

#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtx/quaternion.hpp"

#include "arcore_c_api.h"
using namespace glm;

#ifndef CHECK
#define CHECK(condition)                                                   \
  if (!(condition)) {                                                      \
    LOGE("*** CHECK FAILED at %s:%d: %s", __FILE__, __LINE__, #condition); \
    abort();                                                               \
  }
#endif  // CHECK

namespace arcore_utils {

    vec3 getPlaneNormal(const ArSession & arSession, const ArPose & arPose);

    // Calculate the normal distance to plane from cameraPose, the given planePose
    // should have y axis parallel to plane's normal, for example plane's center
    // pose or hit test pose.
    float calculateDistanceToPlane(const ArSession& ar_session,
                                   const ArPose& plane_pose,
                                   const ArPose& camera_pose);
    // Get transformation matrix from ArAnchor.
    void getTransformMatrixFromAnchor(const ArAnchor& ar_anchor,
                                      ArSession* ar_session,
                                      glm::mat4* out_model_mat);
    bool getNdkImageProperties(const AImage* ndk_image, int32_t* out_format,
                               int32_t* out_width, int32_t* out_height,
                               int32_t* out_plane_num, int32_t* out_stride);
};


#endif //MYGLES_ARCORE_UTILS_H
