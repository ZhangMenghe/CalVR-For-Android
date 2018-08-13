#include "characterDrawable.h"
#include <include/freetype/freetype.h>
void characterDrawable::Initialization(AAssetManager *manager,
                                      std::stack<utils::glState> *stateStack) {
    glDrawable::Initialization(manager, stateStack);
    _shader_program = utils::CreateProgram("shaders/text.vert", "shaders/text.frag", manager);
    _attribute_vertex = glGetAttribLocation(_shader_program,"vPosition");
    _attribute_uv = glGetAttribLocation(_shader_program,"vTexcoord");

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    glGenBuffers(2, _VBO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _pointNum * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(_attribute_vertex);
    glVertexAttribPointer(_attribute_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _pointNum * 2, _uvs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(_attribute_uv);
    glVertexAttribPointer(_attribute_uv, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint proj_mat =  glGetUniformLocation(_shader_program, "uProj");
    GLuint text_color = glGetUniformLocation(_shader_program, "texColor");

    glUseProgram(_shader_program);
    glUniformMatrix4fv(proj_mat, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
    glUniform3f(text_color, 1.0, .0, .0);
    glUniform1i(glGetUniformLocation(_shader_program, "uTexture"), 0);
    glUseProgram(0);
}

void characterDrawable::setTargetCharacter(Character ch, float x, float y, float scale) {
    _TextureID = ch.TextureID;
    GLfloat xpos = x + ch.Bearing.x * scale;
    GLfloat ypos = -y-ch.Bearing.y * scale;//y - (ch.Size.y - ch.Bearing.y)*scale;
    GLfloat w = ch.Size.x * scale;
    GLfloat h = ch.Size.y * scale;
    const GLfloat vertices[] = {
            xpos,     -ypos,
            xpos + w, -ypos,
            xpos,     -ypos - h,
            xpos + w, -ypos - h
    };
    glBindBuffer(GL_ARRAY_BUFFER, _VBO[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) *_pointNum * 2, vertices); // Be sure to use glBufferSubData and not glBufferData
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void characterDrawable::drawImplementation(osg::RenderInfo&) const{
    PushAllState();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(_shader_program);

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(_VAO);
    glBindTexture(GL_TEXTURE_2D, _TextureID);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, _pointNum);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D,0);

    glUseProgram(0);
    checkGlError("Draw point cloud");
    PopAllState();
}