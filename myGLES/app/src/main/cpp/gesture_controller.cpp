//
// Created by menghe on 7/22/2018.
//

#include <osg/ShapeDrawable>

#include "gesture_controller.h"

void gesture_controller::createSphere(osg_controller::osgController * osgApp) {
    LOGE("================called from create sphere================");
    osg::ref_ptr<osg::Vec3Array> _vertices = new osg::Vec3Array();
    _vertices->push_back(Vec3(-0.5f,-0.5f,.0f));
    _vertices->push_back(Vec3(0.5f, -0.5f, .0f));
    _vertices->push_back(Vec3(0.5f, 0.5f, .0f));
    _vertices->push_back(Vec3(-0.5f, 0.5f, .0f));
    _vertices->push_back(Vec3(.0f, .0f, 1.0f));
    osg::ref_ptr<osg::Vec4Array> _colors = new osg::Vec4Array();
    for(int i=0;i<5;i++)
        _colors->push_back(Vec4(1.0f, .0f, .0f,1.0f));

    osg::ref_ptr<osg::Geometry> _geometry = new osg::Geometry();
    osg::ref_ptr<osg::Geode> _node = new osg::Geode;

    _geometry->setVertexArray(_vertices.get());
    _geometry->setColorArray(_colors, osg::Array::BIND_PER_VERTEX);
    GLushort idxLoops0[4] = {
            3, 2, 1, 0 };
    GLushort idxLoops1[3] = {
            0,1,4 };
    _geometry->addPrimitiveSet(
            new DrawElementsUShort(osg::PrimitiveSet::QUADS,
                                   4,
                                   idxLoops0));
    _geometry->addPrimitiveSet(
            new DrawElementsUShort(osg::PrimitiveSet::TRIANGLES,
                                   3,
                                   idxLoops1));

    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();

    normals->push_back(Vec3(-1.0f,-1.0f, .0f)); // left front
    normals->push_back(Vec3( 1.0f,-1.0f, .0f)); // right front
    normals->push_back(Vec3( 1.0f, 1.0f, .0f)); // right back
    normals->push_back( Vec3(-1.0f, 1.0f, .0f)); // left back
    normals->push_back( Vec3( .0f, .0f, 1.0f)); // peak
    _geometry->setNormalArray(normals);
    _geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

    _node->addDrawable(_geometry.get());

    osgApp->addNode(_node.get());
    view = osgApp->getViewer();
//    _osgApp = osgApp;
}

//void gesture_controller::onTouched(float x, float y) {
//    LOGE("================called from native onTouch================%f,%f",x, y );
////    Sphere * sp= (Sphere *)node->getDrawable(0)->getShape();
////    Vec3 center = sp->getCenter();
//////    sphere->setCenter(osg::Vec3(-0.5f,.0f,.0f));
////
////    sp->setCenter(osg::Vec3(-0.5f,.0f,.0f));
////    shape->dirtyDisplayList();
//}