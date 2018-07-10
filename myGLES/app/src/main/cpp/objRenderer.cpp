//
// Created by menghe on 7/5/2018.
//

#include "objRenderer.h"
#include "arcore_utils.h"

const glm::vec4 kLightDirection(0.0f, 1.0f, 0.0f, 0.0f);

void objRenderer::Initialization(AAssetManager *manager, const char*obj_file_name, const char *png_file_name) {
    _shader_program = utils::CreateProgram("shaders/object.vert", "shaders/object.frag", manager);
    CHECK(_shader_program);

    _attrib_vertices = glGetAttribLocation(_shader_program, "vPosition");
    _attrib_uvs = glGetAttribLocation(_shader_program, "vTexcoord");
    _attrib_normals = glGetAttribLocation(_shader_program, "vNormal");

    _uniform_mv_mat = glGetUniformLocation(_shader_program, "uMV");
    _uniform_mvp_mat = glGetUniformLocation(_shader_program, "uMVP");
    _uniform_texture_sampler = glGetUniformLocation(_shader_program, "uSampler");
    _uniform_material = glGetUniformLocation(_shader_program, "uMaterialParams");
    _uniform_lighting_param = glGetUniformLocation(_shader_program, "uLightingParams");
    _uniform_color_correction_param = glGetUniformLocation(_shader_program, "uColorCorrection");

    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    if (!utils::LoadPngFromAssetManager(GL_TEXTURE_2D, png_file_name)) {
        LOGE("Could not load png texture for planes.");
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    utils::LoadObjFile(manager, obj_file_name, &_vertices, &_normals, &_uvs, &_indices);

    //Generate VAO and bind
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    //Generate VBO and bind
    glGenBuffers(3, _VBO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(_attrib_vertices);
    glVertexAttribPointer(_attrib_vertices, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),0);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _uvs.size(), _uvs.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(_attrib_uvs);
    glVertexAttribPointer(_attrib_uvs, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);


    glBindBuffer(GL_ARRAY_BUFFER, _VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _normals.size(), _normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(_attrib_normals);
    glVertexAttribPointer(_attrib_normals, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),0);


    //Generate EBO
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * _indices.size(), _indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void objRenderer::Draw(const mat4 &projMat, const mat4 &viewMat, const mat4 &modelMat,
                       const float *color_correction, float light_intensity) {
    glUseProgram(_shader_program);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(_uniform_texture_sampler, 0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);

    mat4 mv_mat = viewMat * modelMat;
    glUniformMatrix4fv(_uniform_mv_mat, 1,GL_FALSE,value_ptr(mv_mat));
    glUniformMatrix4fv(_uniform_mvp_mat, 1, GL_FALSE, value_ptr(projMat * mv_mat));

    vec4 view_light_direction = normalize(mv_mat * kLightDirection);
    glUniform4f(_uniform_lighting_param, view_light_direction[0],
                view_light_direction[1], view_light_direction[2], light_intensity);
    glUniform4f(_uniform_material,
                _shader_params.ambient, _shader_params.diffuse,
                _shader_params.specular, _shader_params.specular_alpha);
    glUniform4f(_uniform_color_correction_param, color_correction[0],color_correction[1],
                color_correction[2],color_correction[3]);

    glBindVertexArray(_VAO);

    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_SHORT, nullptr);

    glBindVertexArray(0);
    glUseProgram(0);
    /*glEnableVertexAttribArray(_attrib_vertices);
    glVertexAttribPointer(_attrib_vertices, 3, GL_FLOAT, GL_FALSE, 0,
                          _vertices.data());

    glEnableVertexAttribArray(_attrib_normals);
    glVertexAttribPointer(_attrib_normals, 3, GL_FLOAT, GL_FALSE, 0,
                          _normals.data());

    glEnableVertexAttribArray(_attrib_uvs);
    glVertexAttribPointer(_attrib_uvs, 2, GL_FLOAT, GL_FALSE, 0, _uvs.data());

    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_SHORT,
                   _indices.data());

    glDisableVertexAttribArray(_attrib_vertices);
    glDisableVertexAttribArray(_attrib_uvs);
    glDisableVertexAttribArray(_attrib_normals);

    glUseProgram(0);*/
}