//
// Created by menghe on 7/11/2018.
//

#ifndef MYGLES_OSG_UTILS_H
#define MYGLES_OSG_UTILS_H

#include <osg/Camera>

using namespace osg;
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
                     double fovy = 30, double zNear = 1, double zFar = 1000) {
        camera->setGraphicsContext(context);
        camera->setViewport(new Viewport(0, 0, width, height));
        camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        float aspect = static_cast<float>(width) / static_cast<float >(height);
        camera->setProjectionMatrixAsPerspective(fovy, aspect, zNear, zFar);
    }

    Program *createShaderProgram(const char *vertShader, const char *fragShader) {
        Shader * vs = new Shader(Shader::VERTEX, vertShader);
        Shader * fs = new Shader(Shader::FRAGMENT, fragShader);

        Program *program = new Program;
        program->addShader(vs);
        program->addShader(fs);
        return program;
    }
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
