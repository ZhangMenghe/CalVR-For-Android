//
// Created by menghe on 7/22/2018.
//

#include <osg/ShapeDrawable>

#include "gesture_controller.h"

void gesture_controller::createSphere(osg_controller::osgController * osgApp) {
    LOGE("================called from create sphere================");
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Sphere(osg::Vec3(.0f,.0f,.0f), 1.0f));
    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
    node = new osg::Geode;
    node->addDrawable(shape.get());
    osgApp->addNode(node.get());
}

void gesture_controller::onTouched(float x, float y) {
    LOGE("================called from native onTouch================%f,%f",x, y );
}