//
// Created by weichen on 2018/7/9.
//

#ifndef MYGLES_ANCHORRENDERER_H
#define MYGLES_ANCHORRENDERER_H

#include <GLES3/gl3.h>
#include "arcore_c_api.h"
#include "utils.h"

// The anchors at which we are drawing android models using given colors.
struct ColoredAnchor {
    ArAnchor* anchor;
    glm::vec4 color;
};

class anchorRenderer {
private:
    GLuint _shader_program;
    GLuint _attrib_vertices_;
    GLuint _uniform_mvp_mat;
    GLuint _uniform_color;
    GLuint _uniform_point_size;
    glm::vec4 _position=glm::vec4(0,0,0,1);
    float _default_size= 50.0f;

public:
    void Initialization(AAssetManager * manager);
    void Draw(const glm::vec4 & color, const glm::mat4 & mvpMat);
};


#endif //MYGLES_ANCHORRENDERER_H
