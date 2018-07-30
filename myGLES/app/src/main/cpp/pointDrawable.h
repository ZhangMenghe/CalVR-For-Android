//
// Created by menghe on 7/28/2018.
//

#ifndef MYGLES_POINTDRAWABLE_H
#define MYGLES_POINTDRAWABLE_H


#include <osg/Drawable>
#include "utils.h"
#include "osgController.h"
class pointDrawable: public osg::Drawable {
private:
    GLuint _shader_program;
    GLuint _attrib_vertices_;
    GLuint _uniform_view_mat;
    GLuint _uniform_proj_mat;
    glm::vec4 _default_color= glm::vec4(1.0, 0.5, 0.0, 1.0);
    float _default_size = 25.0f;

//    //point cloud data with 4 params (x,y,z, confidence)
//    const float pointCloudData[20] = {0.5f, -0.5f,  .0f, .0f,
//                                      -0.5f, -0.5f, .0f, .0f,
//                                      0.5f,  0.5f, .0f, .0f,
//                                      -0.5f, 0.5f, .0f, .0f,
//                                      0.0f, 0.0f, .0f, .0f};
    const float pointCloudData[8] = {1.0f, -1.0f,  .0f, .0f,
                                      -1.0f, -1.0f, .0f, .0f};
//                                      0.5f,  0.5f, .0f, .0f,
//                                      -0.5f, 0.5f, .0f, .0f,
//                                      0.0f, 0.0f, .0f, .0f};
    osgViewer::Viewer *_viewer;
public:
    pointDrawable(osgViewer::Viewer * viewer);
    void Initialization(AAssetManager * manager);
    void drawImplementation(osg::RenderInfo&) const;
};


#endif //MYGLES_POINTDRAWABLE_H
