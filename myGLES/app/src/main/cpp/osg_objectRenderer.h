//
// Created by menghe on 7/18/2018.
//

#ifndef MYGLES_OSG_OBJECTRENDERER_H
#define MYGLES_OSG_OBJECTRENDERER_H

#include <osg/ShapeDrawable>
#include <osg/ref_ptr>
#include <osg/Node>
#include <osg/Shape>
#include "osg_utils.h"

class osg_objectRenderer {
public:
    osg::ref_ptr<osg::Node> createNode(){
            osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
            shape->setShape(new osg::Sphere(osg::Vec3(.0f,.0f,.0f), 1.0f));
            shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
            osg::ref_ptr<osg::Geode> node = new osg::Geode;
            node->addDrawable(shape.get());
            return node.get();
    }
};


#endif //MYGLES_OSG_OBJECTRENDERER_H
