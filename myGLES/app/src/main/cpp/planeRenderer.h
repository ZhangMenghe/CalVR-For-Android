//
// Created by menghe on 7/2/2018.
//

#ifndef MYGLES_PLANERENDERER_H
#define MYGLES_PLANERENDERER_H

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <android/asset_manager.h>

#include "utils.h"

#include <arcore_c_api.h>
#include <vector>

using namespace glm;
using namespace std;

class planeRenderer {
private:
    vector<vec3> _vertices;
    vector<GLushort> _triangles;
    mat4 _model_mat = mat4(1.0f);
    vec3 _normal_vec = vec3(.0f);

    GLuint _texture_id;
    GLuint _shader_program;

    GLint _attrib_vertices;
    GLint _uniform_mvp_mat;
    GLint _uniform_tex_sampler;
    GLint _uniform_normal_vec;
    GLint _uniform_model_mat;
    GLint _uniform_color;


    void _update_plane_vertices(const ArSession * arSession, const ArPlane * arPlane);

public:
    void Initialization(AAssetManager * manager);
    void Draw(const ArSession * arSession, const ArPlane * arPlane,
              const mat4 & projMat, const mat4 & viewMat,
              const vec3 & color);
    mat4 getModelMat(){return _model_mat;}
};


#endif //MYGLES_PLANERENDERER_H
