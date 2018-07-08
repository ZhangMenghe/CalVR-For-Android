//
// Created by menghe on 7/5/2018.
//

#ifndef MYGLES_POINTCLOUDRENDERER_H
#define MYGLES_POINTCLOUDRENDERER_H


#include <GLES3/gl3.h>
#include "arcore_c_api.h"
#include "utils.h"

using namespace glm;

class pointcloudRenderer {
private:
    GLuint _shader_program;
    GLuint _attrib_vertices_;
    GLuint _uniform_mvp_mat;
    glm::vec4 _default_color;
public:
    void Initialization(AAssetManager * manager);
    void Draw(const ArSession * arSession, ArPointCloud* pointCloud, const mat4 & mvpMat);
};


#endif //MYGLES_POINTCLOUDRENDERER_H
