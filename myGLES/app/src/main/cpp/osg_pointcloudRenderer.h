//
// Created by menghe on 7/18/2018.
//

#ifndef MYGLES_OSG_POINTCLOUDRENDERER_H
#define MYGLES_OSG_POINTCLOUDRENDERER_H

#include <osg/Node>
//#include "arcoreController.h"
#include "utils.h"
#include <osg/PrimitiveSet>

class osg_pointcloudRenderer {
private:
    osg::ref_ptr<osg::Vec3Array> _vertices;
    osg::ref_ptr<osg::Geometry> _geometry;
    osg::ref_ptr<osg::Geode> _node;
    osg::DrawArrays* _drawArray;
    GLuint _attribute_vpos = 1;
    osg::Uniform* _uniform_mvp_mat;
public:
    osg::ref_ptr<osg::Geode> createNode(AAssetManager * manager);
    void Draw(glm::mat4 mvp, int num_of_points, const float * pointCloudData);
};


#endif //MYGLES_OSG_POINTCLOUDRENDERER_H
