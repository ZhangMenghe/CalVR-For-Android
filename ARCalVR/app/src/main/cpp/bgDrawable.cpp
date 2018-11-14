//
// Created by menghe on 8/1/2018.
//

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include "bgDrawable.h"
#include <cvrUtil/AndroidHelper.h>
#include <cvrUtil/AndroidStdio.h>

void bgDrawable::Initialization(){
    cvr::glesDrawable::Initialization();

    _shader_program = cvr::assetLoader::instance()->createGLShaderProgramFromFile("shaders/screenquad.vert", "shaders/screenquad.frag");
    if(!_shader_program)
        LOGE("Failed to create shader program");

    _attrib_vertices = glGetAttribLocation(_shader_program, "a_Position");
    _attrib_uvs = glGetAttribLocation(_shader_program, "a_TexCoord");


    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, _texture_id);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    //Generate VAO and bind
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    //Generate VBO and bind
    glGenBuffers(2, _VBO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, _vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(_attrib_vertices);
    glVertexAttribPointer(_attrib_vertices, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),0);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(_attrib_uvs);
    glVertexAttribPointer(_attrib_uvs, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(_shader_program);
    glUniform1i(glGetUniformLocation(_shader_program, "uTexture"), 0);
    glUseProgram(0);
}

void bgDrawable::updateOnFrame(const float * new_uvs){
    glBindBuffer(GL_ARRAY_BUFFER, _VBO[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 8* sizeof(float), new_uvs);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void bgDrawable::drawImplementation(osg::RenderInfo&) const{
    cvr::glStateStack::instance()->PushAllState();
    glUseProgram(_shader_program);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, _texture_id);
    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(0);
    cvr::glStateStack::instance()->PopAllState();
}