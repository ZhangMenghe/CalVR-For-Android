//
// Created by menghe on 7/12/2018.
//

#include "osg_utils.h"
using namespace osg_utils;

void osg_utils::setupCamera(Camera *camera, GraphicsContext *context,
                 int width, int height,
                 double fovy, double zNear, double zFar) {
    camera->setGraphicsContext(context);
    camera->setViewport(new Viewport(0, 0, width, height));
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    float aspect = static_cast<float>(width) / static_cast<float >(height);
    camera->setProjectionMatrixAsPerspective(fovy, aspect, zNear, zFar);
}

Program *osg_utils::createShaderProgram(const char *vertShader, const char *fragShader) {
    Shader * vs = new Shader(Shader::VERTEX, vertShader);
    Shader * fs = new Shader(Shader::FRAGMENT, fragShader);

    Program *program = new Program;
    program->addShader(vs);
    program->addShader(fs);
    return program;
}