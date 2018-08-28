cmake_minimum_required(VERSION 3.4.1)
#SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=C++1z -Wall")
# for more detailed linker message
SET( GCC_COVERAGE_LINK_FLAGS    "-v")
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
SET( CMAKE_STATIC_LINKER_FLAGS  "${CMAKE_STATIC_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
SET(OSG_CPP_EXCEPTIONS_AVAILABLE "Yes")

SET(REBUILD, NO)

SET(KERNEL_LIBRARIES
    cvrKernel                     # For cvr, Kernel > Menu > Util
    cvrMenu                       # The order of Input, Config, Collaborative is unsure (current order works)
    cvrUtil                       #
    cvrInput                      #
    cvrConfig                     #
    cvrCollaborative              #
    mxml                          # mxml must be included before cvr
    log                   #
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
    osgdb_tiff
    osgdb_rgb
    osgdb_png
    osgdb_jpeg
    osgdb_osg                     #
    osgdb_serializers_osg         #
    osgDB                         #
    osgUtil                       #
    osg                           #
    OpenThreads                   #
    tiff
    png
    jpeg
    #zlib
    EGL                           #
    GLESv2                        #
    GLESv3
)

IF(${REBUILD})
     include(CMakeLists-buildDependencies.txt)
    #include(CMakeLists-buildFreetype.txt)
     include(CMakeLists-buildMXML.txt)
     include(CMakeLists-buildOSG.txt)
     include(CMakeLists-buildCALVR.txt)
ELSE()
    include(CMakeLists-staticDependencies.txt)
    include(CMakeLists-staticOSG.txt)
    include(CMakeLists-staticMXML.txt)
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
include(CMakeLists-osgScene.txt)

#Build simple CalVR project
#include(CMakeLists-simpleCalVR.txt)

