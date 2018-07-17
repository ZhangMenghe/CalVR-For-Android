//
// Created by menghe on 7/11/2018.
//

#ifndef MYGLES_OSG_UTILS_H
#define MYGLES_OSG_UTILS_H

#include <osg/Camera>
#include <osgDB/ReadFile>

#include "utils.h"

// BEGIN FEATURE PLUGINS_STATIC
// Initialize OSG plugins when OpenSceneGraph is built
// as a static library.
USE_OSGPLUGIN(osg2)
USE_SERIALIZER_WRAPPER_LIBRARY(osg)
// END   FEATURE PLUGINS_STATIC

using namespace osg;

namespace osg_utils {
    void setupCamera(Camera *camera, GraphicsContext *context,
                     int width, int height,
                     double fovy = 30, double zNear = 1, double zFar = 1000);

    Program *createShaderProgram(const char *vertShader, const char *fragShader);

    Program* createShaderProgram(const char* vertex_shader_file_name,
                           const char* fragment_shader_file_name,
                           AAssetManager* asset_manager);
}
#endif //MYGLES_OSG_UTILS_H
