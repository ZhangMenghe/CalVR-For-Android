//
// Created by menghe on 7/28/2018.
//

#include "pointDrawable.h"
#include "arcore_utils.h"
#include <stack>
#include <GLES3/gl3.h>
using namespace glm;
namespace {
    typedef struct glState_S {
        GLboolean depthTest, blend, cullFace;
        GLboolean dither, colorLogicOp, polygonOffsetLine, polygonOffsetFill;
        GLboolean polygonOffsetPoint, polygonSmooth, scissorTest, stencilTest;
    } glState;

    static std::stack<glState> stateStack;

    bool PushAllState(void)
    {
        glState state;

        state.blend = glIsEnabled(GL_BLEND);
        state.depthTest = glIsEnabled(GL_DEPTH_TEST);
        state.cullFace = glIsEnabled(GL_CULL_FACE);
        state.dither = glIsEnabled(GL_DITHER);
        state.polygonOffsetFill = glIsEnabled(GL_POLYGON_OFFSET_FILL);
        state.scissorTest = glIsEnabled(GL_SCISSOR_TEST);
        state.stencilTest = glIsEnabled(GL_STENCIL_TEST);

        stateStack.push(state);
        return true;
    }

    bool PopAllState(void)
    {
        if (!stateStack.empty()) {
            glState state = stateStack.top();

            if (state.blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
            if (state.depthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
            if (state.cullFace) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
            if (state.dither) glEnable(GL_DITHER); else glDisable(GL_DITHER);
            if (state.polygonOffsetFill) glEnable(GL_POLYGON_OFFSET_FILL); else glDisable(GL_POLYGON_OFFSET_FILL);
            if (state.scissorTest) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
            if (state.stencilTest) glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);

            stateStack.pop();
        }
        return true;
    }
}

void pointDrawable::Initialization(AAssetManager *manager) {
    _shader_program = utils::CreateProgram("shaders/pointDrawable.vert", "shaders/point.frag", manager);

    _attrib_vertices_ = glGetAttribLocation(_shader_program,"vPosition");
    _uniform_arMVP_mat =  glGetUniformLocation(_shader_program, "uarMVP");

    //Generate VAO and bind
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    //Generate VBO and bind
    glGenBuffers(1, &_VBO);

    //dynamic feed data
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * MAX_POINTS * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(_attrib_vertices_);
    glVertexAttribPointer(_attrib_vertices_, 4, GL_FLOAT, GL_FALSE, 0, 0);
    //static draw,debug only
//    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _point_num * 4, pointCloudData, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(_attrib_vertices_);
//    glVertexAttribPointer(_attrib_vertices_, 4, GL_FLOAT, GL_FALSE, 4* sizeof(float), 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(_shader_program);
    glUniform4fv(glGetUniformLocation(_shader_program, "uColor"), 1, value_ptr(_default_color));
    glUniform1f(glGetUniformLocation(_shader_program, "uPointSize"), _default_size);
    glUseProgram(0);

    checkGlError("Initialize point cloud renderer");
}

void pointDrawable::drawImplementation(osg::RenderInfo&) const{
    PushAllState();
    glUseProgram(_shader_program);
    glUniformMatrix4fv(_uniform_arMVP_mat, 1, GL_FALSE, value_ptr(_ar_mvp));

    glBindVertexArray(_VAO);
    glDrawArrays(GL_POINTS, 0, _point_num);
    glBindVertexArray(0);

    glUseProgram(0);
    checkGlError("Draw point cloud");

    PopAllState();
}