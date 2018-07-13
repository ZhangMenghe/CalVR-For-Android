# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(OSG_SRC_DIR "${CMAKE_SOURCE_DIR}/../../..")
# Specify critical OpenSceneGraph build variables.
#SET(OPENGL_PROFILE "GLES2" CACHE STRING "OpenGL variant to use")
#SET(DYNAMIC_OPENTHREADS OFF CACHE BOOL "Link OpenThreads statically")
#SET(DYNAMIC_OPENSCENEGRAPH OFF CACHE BOOL "Link OpenSceneGraph statically")

# Build and reference OpenSceneGraph.
set(OSG_SOURCE_DIR "${OSG_SRC_DIR}/OpenSceneGraph-3.6.2")
set(OSG_BUILD_DIR "${OSG_SRC_DIR}/OpenSceneGraph-3.6.2/build/${ANDROID_ABI}")
#file(MAKE_DIRECTORY ${OSG_BUILD_DIR})
#add_subdirectory(${OSG_SOURCE_DIR} ${OSG_BUILD_DIR})


include_directories(${OSG_SOURCE_DIR}/include)
include_directories(${OSG_BUILD_DIR}/include)

add_library(osg STATIC IMPORTED)
set_target_properties(osg PROPERTIES IMPORTED_LOCATION
                      ${OSG_BUILD_DIR}/lib/libosgd.a)
add_library(osgViewer STATIC IMPORTED)
set_target_properties(osgViewer PROPERTIES IMPORTED_LOCATION
                      ${OSG_BUILD_DIR}/lib/libosgViewerd.a)
add_library(osgText STATIC IMPORTED)
set_target_properties(osgText PROPERTIES IMPORTED_LOCATION
                      ${OSG_BUILD_DIR}/lib/libosgTextd.a)

add_library(osgGA STATIC IMPORTED)
set_target_properties(osgGA PROPERTIES IMPORTED_LOCATION
                      ${OSG_BUILD_DIR}/lib/libosgGAd.a)

add_library(osgDB STATIC IMPORTED)
set_target_properties(osgDB PROPERTIES IMPORTED_LOCATION
                      ${OSG_BUILD_DIR}/lib/libosgDBd.a)

add_library(osgdb_osg STATIC IMPORTED)
set_target_properties(osgdb_osg PROPERTIES IMPORTED_LOCATION
                      ${OSG_BUILD_DIR}/lib/libosgdb_osgd.a)

add_library(osgdb_serializers_osg STATIC IMPORTED)
set_target_properties(osgdb_serializers_osg PROPERTIES IMPORTED_LOCATION
                      ${OSG_BUILD_DIR}/lib/libosgdb_serializers_osgd.a)

add_library(osgUtil STATIC IMPORTED)
set_target_properties(osgUtil PROPERTIES IMPORTED_LOCATION
                      ${OSG_BUILD_DIR}/lib/libosgUtild.a)

add_library(OpenThreads STATIC IMPORTED)
set_target_properties(OpenThreads PROPERTIES IMPORTED_LOCATION
                      ${OSG_BUILD_DIR}/lib/libOpenThreadsd.a)

add_library(osgShadow STATIC IMPORTED)
set_target_properties(osgShadow PROPERTIES IMPORTED_LOCATION
                      ${OSG_BUILD_DIR}/lib/libosgShadowd.a)

add_library(arcore SHARED IMPORTED)
set_target_properties(arcore PROPERTIES IMPORTED_LOCATION
                      ${ARCORE_LIBPATH}/${ANDROID_ABI}/libarcore_sdk_c.so)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp )
add_library(glTriangle
            SHARED
            src/main/cpp/drawTriangle.cpp)
add_library(osgScene
            SHARED
            src/main/cpp/utils.cpp
            src/main/cpp/arcore_utils.cpp
            src/main/cpp/osg_utils.cpp
            src/main/cpp/jni_interface_osg.cpp
            src/main/cpp/osgController.cpp
            src/main/cpp/TextureAC.cpp
            src/main/cpp/osg_cameraRenderer.cpp)
add_library(arTemplate
            SHARED
            src/main/cpp/drawPrimitive.cpp
            src/main/cpp/jni_interface.cpp
            src/main/cpp/nativeController.cpp
            src/main/cpp/objRenderer.cpp
            src/main/cpp/planeRenderer.cpp
            src/main/cpp/cameraRenderer.cpp
            src/main/cpp/pointcloudRenderer.cpp
            src/main/cpp/arcore_utils.cpp
            src/main/cpp/anchorRenderer.cpp
            src/main/cpp/utils.cpp)
target_include_directories(osgScene PRIVATE
           src/main/cpp
            ${ANDROID_NDK}/sources/third_party/vulkan/src/libs/glm)
target_include_directories(arTemplate PRIVATE
           src/main/cpp
            ${ANDROID_NDK}/sources/third_party/vulkan/src/libs/glm)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )
# add lib dependencies
target_link_libraries(glTriangle
                      android
                      log
                      EGL
                      GLESv2)
# add lib dependencies
target_link_libraries(osgScene
                      android
                      mediandk
                      log
                      EGL
                      GLESv2
                      arcore
                      #need for osg
                      osgViewer
                      osgDB
                      z
                      osgdb_osg
                      osgdb_serializers_osg
                      osgGA
                      osgShadow
                      osgText
                      osgUtil
                      osg
                      OpenThreads)
target_link_libraries(arTemplate
                      android
                      mediandk
                      log
                      EGL
                      GLESv2
                      GLESv3
                      arcore
                      )