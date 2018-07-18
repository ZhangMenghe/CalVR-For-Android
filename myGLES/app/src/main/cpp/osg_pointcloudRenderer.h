//
// Created by menghe on 7/18/2018.
//

#ifndef MYGLES_OSG_POINTCLOUDRENDERER_H
#define MYGLES_OSG_POINTCLOUDRENDERER_H

#include <osg/Node>
#include "arcoreController.h"
#include "utils.h"

class osg_pointcloudRenderer {
private:
    osg::ref_ptr<osg::Geometry> _geometry;
    osg::ref_ptr<osg::Geode> _node;
    float _default_size = 5.0f;
    osg::Uniform* _uniform_color;
public:
    osg::ref_ptr<osg::Node> createNode(AAssetManager * manager, arcoreController* ar);
    void Draw(arcoreController * ar);
};


#endif //MYGLES_OSG_POINTCLOUDRENDERER_H
