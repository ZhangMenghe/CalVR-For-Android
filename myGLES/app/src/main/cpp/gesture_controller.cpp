//
// Created by menghe on 7/22/2018.
//

#include <osg/ShapeDrawable>

#include "gesture_controller.h"

void gesture_controller::createSphere(osg_controller::osgController * osgApp) {
    LOGE("================called from create sphere================");
//    shape = new osg::ShapeDrawable;
//    sphere = new osg::Sphere(osg::Vec3(.0f,.0f,.0f), 0.3f);
//    shape->setShape(sphere);
//    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
//    node = new osg::Geode;
//    node->addDrawable(shape.get());
////    osg::Vec3Array* normals = new osg::Vec3Array;
////    normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
////    shape->setNormalArray(normals, osg::Array::BIND_OVERALL);
//    osgApp->addNode(node.get());
//    _osgApp = osgApp;
}

void gesture_controller::onTouched(float x, float y) {
    LOGE("================called from native onTouch================%f,%f",x, y );
//    Sphere * sp= (Sphere *)node->getDrawable(0)->getShape();
//    Vec3 center = sp->getCenter();
////    sphere->setCenter(osg::Vec3(-0.5f,.0f,.0f));
//
//    sp->setCenter(osg::Vec3(-0.5f,.0f,.0f));
//    shape->dirtyDisplayList();
}