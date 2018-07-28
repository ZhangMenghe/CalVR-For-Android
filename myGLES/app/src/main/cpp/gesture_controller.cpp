//
// Created by menghe on 7/22/2018.
//

#include <osg/ShapeDrawable>

#include "gesture_controller.h"

void gesture_controller::createNode(osg_controller::osgController * osgApp, AAssetManager * manager) {
//    LOGE("================called from create interaction node================");
//    shape = new osg::ShapeDrawable;
//    shape->setShape(new osg::Sphere(osg::Vec3(.0f,.0f,.0f), 0.3f));
//
//    osg::ref_ptr<osg::Geode> node = new osg::Geode;
//    osg::Program * prog = osg_utils::createShaderProgram(shaderVertex, shaderFragment);
//    shape->getOrCreateStateSet()->setAttribute(prog);
//    node->addDrawable(shape.get());
//    osgApp->addNode(node.get());
    view = osgApp->getViewer();
}
