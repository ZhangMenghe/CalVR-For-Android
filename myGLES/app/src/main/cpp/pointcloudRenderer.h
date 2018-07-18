//
// Created by menghe on 7/5/2018.
//

#ifndef MYGLES_POINTCLOUDRENDERER_H
#define MYGLES_POINTCLOUDRENDERER_H


#include <GLES3/gl3.h>
#include "arcore_c_api.h"
#include "utils.h"

class pointcloudRenderer {
private:
    GLuint _shader_program;
    GLuint _attrib_vertices_;
    GLuint _uniform_mvp_mat;
    glm::vec4 _default_color= glm::vec4(1.0, 0.5, 0.0, 1.0);
    float _default_size = 5.0f;
public:
    void Initialization(AAssetManager * manager);
    void Draw(const ArSession * arSession, ArPointCloud* pointCloud, const glm::mat4 & mvpMat);
};


#endif //MYGLES_POINTCLOUDRENDERER_H
