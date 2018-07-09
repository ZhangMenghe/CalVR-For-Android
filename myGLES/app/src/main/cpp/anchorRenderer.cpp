//
// Created by weichen on 2018/7/9.
//

#include "anchorRenderer.h"
#include "arcore_utils.h"

void anchorRenderer::Initialization(AAssetManager * manager){
    _shader_program = utils::CreateProgram("shaders/point.vert", "shaders/point.frag", manager);
    CHECK(_shader_program);
    _attrib_vertices_ = glGetAttribLocation(_shader_program,"vPosition");
    _uniform_mvp_mat = glGetUniformLocation(_shader_program, "uMVP");
    _uniform_color = glGetUniformLocation(_shader_program, "uColor");
    _position = glm::vec4(0,0,0,1);
    glEnableVertexAttribArray(_attrib_vertices_);
    glVertexAttribPointer(_attrib_vertices_, 4, GL_FLOAT, GL_FALSE, 0, value_ptr(_position));
    checkGlError("Initialize anchor renderer");
}

void anchorRenderer::Draw(const ArSession * arSession, const glm::vec4 & color, const glm::mat4 & mvpMat){
    glUseProgram(_shader_program);

    glUniformMatrix4fv(_uniform_mvp_mat, 1, GL_FALSE, value_ptr(mvpMat));
    glUniform4fv(_uniform_color, 1, value_ptr(color));
    glDrawArrays(GL_POINTS, 0, 1);

    glUseProgram(0);
    checkGlError("Draw anchor");
}