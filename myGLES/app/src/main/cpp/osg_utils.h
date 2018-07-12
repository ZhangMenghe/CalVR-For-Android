//
// Created by menghe on 7/11/2018.
//

#ifndef MYGLES_OSG_UTILS_H
#define MYGLES_OSG_UTILS_H

#include <osg/Camera>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>

// BEGIN FEATURE PLUGINS_STATIC
// Initialize OSG plugins when OpenSceneGraph is built
// as a static library.
USE_OSGPLUGIN(osg2)
USE_SERIALIZER_WRAPPER_LIBRARY(osg)
// END   FEATURE PLUGINS_STATIC

using namespace osg;
using namespace osgViewer;
namespace osg_utils {

    // Fragment shader to display everything in red colour.
    static const char fragmentShader[] =
            "void main() {                             \n"
                    "  gl_FragColor = vec4(0.5, 0.3, 0.3, 1.0);\n"
                    "}                                         \n";
// Geometry shader to pass geometry vertices to fragment shader.
    static const char vertexShader[] =
            "void main() {                                              \n"
                    "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  \n"
                    "}                                                          \n";

    void setupCamera(Camera *camera, GraphicsContext *context,
                     int width, int height,
                     double fovy = 30, double zNear = 1, double zFar = 1000);

    Program *createShaderProgram(const char *vertShader, const char *fragShader);

    /*Program* createShaderProgram(const char* vertex_shader_file_name,
                           const char* fragment_shader_file_name,
                           AAssetManager* asset_manager) {
        std::string VertexShaderContent;
        if (!utils::LoadTextFileFromAssetManager(vertex_shader_file_name, asset_manager,
                                          &VertexShaderContent)) {
            LOGE("Failed to load file: %s", vertex_shader_file_name);
            return 0;
        }

        std::string FragmentShaderContent;
        if (!utils::LoadTextFileFromAssetManager(fragment_shader_file_name, asset_manager,
                                          &FragmentShaderContent)) {
            LOGE("Failed to load file: %s", fragment_shader_file_name);
            return 0;
        }

        return createShaderProgram(VertexShaderContent.c_str(), FragmentShaderContent.c_str());
    }*/
}
#endif //MYGLES_OSG_UTILS_H
