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
    include(CMakeLists-staticCALVR.txt)
ENDIF()
#include(CMakeLists-buildCALVR.txt)


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
#include(CMakeLists-simpleCalVR.txt)

add_library(osgScene
            SHARED
            src/main/cpp/utils.cpp
            src/main/cpp/perfMonitor.cpp
            src/main/cpp/arcore_utils.cpp
            src/main/cpp/osg_utils.cpp
            src/main/cpp/jni_interface_osg.cpp
            src/main/cpp/osgController.cpp
            src/main/cpp/arcoreController.cpp
            src/main/cpp/glDrawable.cpp
            src/main/cpp/bgDrawable.cpp
            src/main/cpp/pointDrawable.cpp
            src/main/cpp/planeDrawable.cpp
            src/main/cpp/characterDrawable.cpp
            src/main/cpp/freetypeDrawable.cpp
            src/main/cpp/osg_objectRenderer.cpp
            src/main/cpp/jni_interface_osg_gesture.cpp
            src/main/cpp/gesture_controller.cpp)

target_include_directories(osgScene PRIVATE
           src/main/cpp
            ${ANDROID_NDK}/sources/third_party/vulkan/src/libs/glm)

# add lib dependencies
target_link_libraries(osgScene
                      android
                      mediandk
                      log
                      freetype
                      arcore

                      #need for osg
                      osgPresentation               # For osg, the dependency relations:
                      osgWidget                     # osgPresentation: https://github.com/WeichenLiu/CalVR_for_Android/blob/master/Note/osgPresentation.png
                      osgUI                         # osgdb_osg: https://github.com/WeichenLiu/CalVR_for_Android/blob/master/Note/osgdb_osg.png
                      osgManipulator                # osgdb_serializers_osg:
                      osgAnimation                  # https://github.com/WeichenLiu/CalVR_for_Android/blob/master/Note/osgdb_serializers_osg.png
                      osgViewer                     # (These three are the "roots" of the trees)
                      osgVolume                     #
                      osgSim                        #
                      osgShadow                     #
                      osgTerrain                    #
                      osgParticle                   #
                      osgFX                         #
                      z                             #
                      osgText                       #
                      osgGA                         #
                      osgdb_osg                     #
                      osgdb_serializers_osg         #
                      osgDB                         #
                      osgUtil                       #
                      osg
                      OpenThreads

                      EGL
                      GLESv2
                      GLESv3)