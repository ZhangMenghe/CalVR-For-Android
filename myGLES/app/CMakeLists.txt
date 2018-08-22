cmake_minimum_required(VERSION 3.4.1)
#SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=C++1z -Wall")
# for more detailed linker message
SET( GCC_COVERAGE_LINK_FLAGS    "-v")
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
SET( CMAKE_STATIC_LINKER_FLAGS  "${CMAKE_STATIC_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

SET(REBUILD, NO)

SET(KERNEL_LIBRARIES
    cvrKernel                     # For cvr, Kernel > Menu > Util
    cvrMenu                       # The order of Input, Config, Collaborative is unsure (current order works)
    cvrUtil                       #
    cvrInput                      #
    cvrConfig                     #
    cvrCollaborative              #
    mxml                          # mxml must be included before cvr
    ${log-lib}                    #
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
    #osgdb_tiff
    #osgdb_png
    #osgdb_jpeg
    #osgdb_osg                     #
    osgdb_serializers_osg         #
    osgDB                         #
    osgUtil                       #
    osg                           #
    OpenThreads                   #
    #tiff
    #png
    #jpeg
    #zlib
    EGL                           #
    GLESv2                        #
)

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
#include(CMakeLists-simpleCalVR.txt)

add_library(simpleCalVR
            SHARED
            src/main/cpp/utils.cpp
            src/main/cpp/osg_utils.cpp
            src/main/cpp/jni_interface_calvr.cpp
            src/main/cpp/calvrController.cpp
            src/main/cpp/SpatialViz.cpp)

target_include_directories(simpleCalVR PRIVATE
           src/main/cpp
            ${ANDROID_NDK}/sources/third_party/vulkan/src/libs/glm)

# add lib dependencies
target_link_libraries(simpleCalVR
                      MenuBasics
                      cvrKernel                     # For cvr, Kernel > Menu > Util
                      cvrMenu                       # The order of Input, Config, Collaborative is unsure (current order works)
                      cvrUtil                       #
                      cvrInput                      #
                      cvrConfig                     #
                      cvrCollaborative              #
                      android
                      mediandk
                      log
                      mxml
                      freetype
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