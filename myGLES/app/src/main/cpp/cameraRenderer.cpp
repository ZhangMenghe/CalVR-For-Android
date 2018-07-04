//
// Created by menghe on 7/4/2018.
//
#include <type_traits>
#include "cameraRenderer.h"
namespace {
    // Positions of the quad vertices in clip space (X, Y, Z).
    const GLfloat kVertices[] = {
            -1.0f, -1.0f, 0.0f, +1.0f, -1.0f, 0.0f,
            -1.0f, +1.0f, 0.0f, +1.0f, +1.0f, 0.0f,
    };

    // UVs of the quad vertices (S, T)
    const GLfloat kUvs[] = {
            0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };
}
void cameraRenderer::Initialization(AAssetManager *manager) {
    _shader_program = utils::CreateProgram("shaders/screenquad.vert", "shaders/screenquad.frag", manager);
    if(!_shader_program)
        LOGE("Failed to create shader program");

    _attrib_vertices = glGetAttribLocation(_shader_program, "a_Position");
    _attrib_uvs = glGetAttribLocation(_shader_program, "a_TexCoord");
    _uniform_texture = glGetUniformLocation(_shader_program, "sTexture");

    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, _texture_id);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
void cameraRenderer::Draw(const ArSession *session, const ArFrame *frame) {
    static_assert(std::extent<decltype(kUvs)>::value == 8,
                  "Incorrect kUvs length");
    static_assert(std::extent<decltype(kVertices)>::value == 12,
                  "Incorrect kVertices length");

    // If display rotation changed (also includes view size change), we need to
    // re-query the uv coordinates for the on-screen portion of the camera image.
    int32_t geometry_changed = 0;
    ArFrame_getDisplayGeometryChanged(session, frame, &geometry_changed);
    if (geometry_changed != 0 || !_uvs_initialized) {
        ArFrame_transformDisplayUvCoords(session, frame, 8, kUvs,
                                         _transformed_uvs);
        _uvs_initialized = true;
    }

    glUseProgram(_shader_program);
    glDepthMask(GL_FALSE);

    glUniform1i(_uniform_texture, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, _texture_id);

    glEnableVertexAttribArray(_attrib_vertices);
    glVertexAttribPointer(_attrib_vertices, 3, GL_FLOAT, GL_FALSE, 0,
                          kVertices);

    glEnableVertexAttribArray(_attrib_uvs);
    glVertexAttribPointer(_attrib_uvs, 2, GL_FLOAT, GL_FALSE, 0,
                          _transformed_uvs);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(0);
    glDepthMask(GL_TRUE);
    utils::CheckGlError("backgroundRenderer::Draw() error");
}