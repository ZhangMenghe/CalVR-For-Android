//
// Created by menghe on 7/2/2018.
//

#ifndef MYGLES_ARCORE_UTILS_H
#define MYGLES_ARCORE_UTILS_H

#include <glm.hpp>
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

};


#endif //MYGLES_ARCORE_UTILS_H
