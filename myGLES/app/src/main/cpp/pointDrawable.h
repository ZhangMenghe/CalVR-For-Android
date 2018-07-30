//
// Created by menghe on 7/28/2018.
//

#ifndef MYGLES_POINTDRAWABLE_H
#define MYGLES_POINTDRAWABLE_H


#include <osg/Drawable>
#include "utils.h"

class pointDrawable: public osg::Drawable {
private:
    GLuint _VAO;
    GLuint _VBO;
    GLuint _shader_program;
    GLuint _attrib_vertices_;
    GLuint _uniform_view_mat;
    GLuint _uniform_proj_mat;
    int _point_num = 2;
    glm::vec4 _default_color= glm::vec4(1.0, 0.5, 0.0, 1.0);
    float _default_size = 25.0f;
    osg::Camera * _osgCamera;
//    //point cloud data with 4 params (x,y,z, confidence)
//    const float pointCloudData[20] = {0.5f, -0.5f,  .0f, .0f,
//                                      -0.5f, -0.5f, .0f, .0f,
//                                      0.5f,  0.5f, .0f, .0f,
//                                      -0.5f, 0.5f, .0f, .0f,
//                                      0.0f, 0.0f, .0f, .0f};

//                                      0.5f,  0.5f, .0f, .0f,
//                                      -0.5f, 0.5f, .0f, .0f,
//                                      };
    float pointCloudData[8] = {.0f};
public:


    void updateVertices(){
        pointCloudData[0] = 1.0f;pointCloudData[1] = -1.0f;
        pointCloudData[4] = -1.0f;pointCloudData[5] = -1.0f;

        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * _point_num * sizeof(GLfloat), pointCloudData);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void Initialization(AAssetManager * manager,osg::Camera * osgCamera);
    void drawImplementation(osg::RenderInfo&) const;
};


#endif //MYGLES_POINTDRAWABLE_H
