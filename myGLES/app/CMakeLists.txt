cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall")
SET(REBUILD, NO)

# Choose between static or build from origin
#
IF(${REBUILD})
    #include(CMakeLists-buildFreetype.txt)
    include(CMakeLists-buildMXML.txt)
    include(CMakeLists-buildOSG.txt)
    include(CMakeLists-buildCALVR.txt)
ELSE()
    include(CMakeLists-staticFreetype.txt)
    include(CMakeLists-staticOSG.txt)
    include(CMakeLists-staticMXML.txt)
    #include(CMakeLists-staticCALVR.txt)
ENDIF()
include(CMakeLists-buildCALVR.txt)


add_library(arcore SHARED IMPORTED)
set_target_properties(arcore PROPERTIES IMPORTED_LOCATION
                      ${ARCORE_LIBPATH}/${ANDROID_ABI}/libarcore_sdk_c.so)

#Build a simple gl+ndk to draw triangle
#include(CMakeLists-glTriangle.txt)

#Build ARcore+gl+ndk project
#include(CMakeLists-arTemplate.txt)

#Build ARcore + osg + gl + ndk project
#include(CMakeLists-osgScene.txt)

#Build simple CalVR project
include(CMakeLists-simpleCalVR.txt)

