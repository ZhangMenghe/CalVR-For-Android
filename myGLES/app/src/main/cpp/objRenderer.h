//
// Created by menghe on 7/5/2018.
//

#ifndef MYGLES_OBJRENDERER_H
#define MYGLES_OBJRENDERER_H

#include <vector>
#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include "arcore_c_api.h"

#include "utils.h"

using namespace std;
using namespace glm;

typedef struct{
    float ambient = .0f;
    float diffuse = 2.0f;
    float specular = 0.5f;
    float specular_alpha = 6.0f;
}objParams;
class objRenderer {
private:
    objParams _shader_params;

    vector<GLfloat> _vertices;
    vector<GLfloat > _uvs;
    vector<GLfloat > _normals;

    vector<GLushort > _indices;

    GLuint _texture_id;
    GLuint _shader_program;
    GLuint _attrib_vertices;
    GLuint _attrib_uvs;
    GLuint _attrib_normals;

    GLuint _VAO;
    GLuint _VBO[3];
    GLuint _EBO;

    GLuint _uniform_mv_mat;
    GLuint _uniform_mvp_mat;
    GLuint _uniform_texture_sampler;
    GLuint _uniform_lighting_param;
    GLuint _uniform_material;
    GLuint _uniform_color_correction_param;
public:
    void Initialization(AAssetManager * manager, const char*obj_file_name, const char *png_file_name);
    void Draw(const mat4 & projMat, const mat4 & viewMat, const mat4 & modelMat,
              const float * color_correction, float light_intensity);
};


#endif //MYGLES_OBJRENDERER_H
