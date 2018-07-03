//
// Created by menghe on 7/2/2018.
//

#include "planeRenderer.h"
#include "arcore_utils.h"
#include "utils.h"
using namespace arcore_utils;
void planeRenderer::_update_plane_vertices(const ArSession &arSession, const ArPlane &arPlane) {
    _vertices.clear();
    _triangles.clear();

    int32_t polygon_length;
    //get the number of elements(2*#vertives)
    ArPlane_getPolygonSize(&arSession, &arPlane, &polygon_length);

    if(polygon_length == 0){
        LOGE("NO valid plane polygon found");
        return;
    }
    const int32_t vertices_size = polygon_length/2;
    vector<vec2> raw_vertices(vertices_size);
    ArPlane_getPolygon(&arSession, &arPlane, value_ptr(raw_vertices.front()));

    //fill outter vertices
    for(int32_t i=0; i<vertices_size; i++)
        _vertices.push_back(vec3(raw_vertices[i].x, raw_vertices[i].y, .0f));

    //get model matrix
    ArPose * arPose;
    ArPose_create(&arSession, nullptr,&arPose);
    ArPlane_getCenterPose(&arSession, &arPlane, arPose);
    ArPose_getMatrix(&arSession, arPose, value_ptr(_model_mat));

    _normal_vec = getPlaneNormal(arSession, *arPose);

    // Feather distance 0.2 meters.
    const float kFeatherLength = 0.2f;
    // Feather scale over the distance between plane center and vertices.
    const float kFeatherScale = 0.2f;

    // Fill vertex 4 to 7, with alpha set to 1.
    for (int32_t i = 0; i < vertices_size; ++i) {
        // Vector from plane center to current point.
        vec2 v = raw_vertices[i];
        const float scale =
                1.0f - std::min((kFeatherLength / glm::length(v)), kFeatherScale);
        const vec2 result_v = scale * v;

        _vertices.push_back(vec3(result_v.x, result_v.y, 1.0f));
    }

    const int32_t vertices_length = _vertices.size();
    const int32_t half_vertices_length = vertices_length / 2;

    // Generate triangle (4, 5, 6) and (4, 6, 7).
    for (int i = half_vertices_length + 1; i < vertices_length - 1; ++i) {
        _triangles.push_back(half_vertices_length);
        _triangles.push_back(i);
        _triangles.push_back(i + 1);
    }

    // Generate triangle (0, 1, 4), (4, 1, 5), (5, 1, 2), (5, 2, 6),
    // (6, 2, 3), (6, 3, 7), (7, 3, 0), (7, 0, 4)
    for (int i = 0; i < half_vertices_length; ++i) {
        _triangles.push_back(i);
        _triangles.push_back((i + 1) % half_vertices_length);
        _triangles.push_back(i + half_vertices_length);

        _triangles.push_back(i + half_vertices_length);
        _triangles.push_back((i + 1) % half_vertices_length);
        _triangles.push_back((i + half_vertices_length + 1) % half_vertices_length +
                             half_vertices_length);
    }
}
void planeRenderer::Initialization(AAssetManager *manager) {
    _shader_program = utils::CreateProgram("shaders/plane.vert", "shaders/plane.frag", manager);
    if(!_shader_program)
        LOGE("Failed to create shader program");

    //Init attribs & uniforms
    _attrib_vertices = glGetAttribLocation(_shader_program, "vPosition");
    _uniform_model_mat = glGetUniformLocation(_shader_program, "uModel");
    _uniform_normal_vec = glGetUniformLocation(_shader_program, "uNormal");
    _uniform_mvp_mat = glGetUniformLocation(_shader_program, "uMVP");
    _uniform_tex_sampler = glGetUniformLocation(_shader_program, "uTexture");
    _uniform_color = glGetUniformLocation(_shader_program, "uColor");

    //Init texture
    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(!utils::LoadPngFromAssetManager(GL_TEXTURE_2D, "textures/trigrid.png"))
        LOGE("Failed to load png image");
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void planeRenderer::Draw(const ArSession &arSession, const ArPlane &arPlane, const mat4 &projMat,
                         const mat4 &viewMat, const vec3 &color) {
    _update_plane_vertices(arSession, arPlane);
    glUseProgram(_shader_program);
    glDepthMask(GL_FALSE);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(_uniform_tex_sampler, 0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);

    glUniformMatrix4fv(_uniform_mvp_mat, 1, false, value_ptr(projMat * viewMat * _model_mat));
    glUniform3f(_uniform_normal_vec, _normal_vec.x, _normal_vec.y, _normal_vec.z);
    glUniformMatrix4fv(_uniform_model_mat, 1, false, value_ptr(_model_mat));
    glUniform3f(_uniform_color, color.x, color.y, color.z);

    glEnableVertexAttribArray(_attrib_vertices);
    glVertexAttribPointer(_attrib_vertices, 3, GL_FLOAT, GL_FALSE, 0, _vertices.data());
    glDrawElements(GL_TRIANGLES, _triangles.size(), GL_UNSIGNED_SHORT, _triangles.data());

    glUseProgram(_shader_program);
    glDepthMask(GL_TRUE);
}