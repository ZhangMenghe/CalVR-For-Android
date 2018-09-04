//
// Created by menghe on 7/28/2018.
//

#include "strokeDrawable.h"
#include "arcore_utils.h"
#include <stack>
#include <GLES3/gl3.h>
using namespace glm;

void strokeDrawable::Initialization(AAssetManager *manager,std::stack<utils::glState>* stateStack) {
    glDrawable::Initialization(manager, stateStack);
    _shader_program = utils::CreateProgram("shaders/pointDrawable.vert", "shaders/point.frag", manager);

    _attrib_vertices = glGetAttribLocation(_shader_program,"vPosition");
    _uniform_arMVP_mat =  glGetUniformLocation(_shader_program, "uarMVP");

    //Generate VAO and bind
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    //Generate VBO and bind
    glGenBuffers(1, &_VBO);

    //dynamic feed data
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(_attrib_vertices);
    glVertexAttribPointer(_attrib_vertices, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(_shader_program);
    glLineWidth(_default_line_width);
    glUniform4fv(glGetUniformLocation(_shader_program, "uColor"), 1, value_ptr(_default_color));
    glUniform1f(glGetUniformLocation(_shader_program, "uPointSize"), _default_size);
    glUseProgram(0);

    checkGlError("Initialize point cloud renderer");
}

void strokeDrawable::drawImplementation(osg::RenderInfo &) const {
    PushAllState();
    glUseProgram(_shader_program);
    glUniformMatrix4fv(_uniform_arMVP_mat, 1, GL_FALSE, value_ptr(_ar_mvp));

    glBindVertexArray(_VAO);
    glDrawArrays(GL_LINES, 0, 2);
    glDrawArrays(GL_POINTS, 0, 2);
    glBindVertexArray(0);

    glUseProgram(0);
    checkGlError("Draw point cloud");

    PopAllState();
}