//
// Created by menghe on 7/18/2018.
//

#ifndef MYGLES_OSG_POINTCLOUDRENDERER_H
#define MYGLES_OSG_POINTCLOUDRENDERER_H

#include <osg/Node>
#include "arcore_c_api.h"
#include "utils.h"

class osg_pointcloudRenderer {
private:
    osg::ref_ptr<osg::Geometry> _geometry;
    osg::ref_ptr<osg::Geode> _node;
    float _default_size = 5.0f;
public:
    osg::ref_ptr<osg::Node> createNode(AAssetManager * manager);
    void Draw(const ArSession * arSession, ArPointCloud* pointCloud, const glm::mat4 & mvpMat);
};


#endif //MYGLES_OSG_POINTCLOUDRENDERER_H
