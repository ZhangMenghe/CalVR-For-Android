//
// Created by menghe on 7/22/2018.
//

#include <osg/ShapeDrawable>

#include "gesture_controller.h"

void gesture_controller::createNode(osg_controller::osgController * osgApp, AAssetManager * manager) {
    LOGE("================called from create interaction node================");
    _geometry = new osg::Geometry();
    _node = new osg::Geode;

    _vertices = new osg::Vec3Array();
    _vertices->push_back(Vec3(0.5f, -0.5f,  .0f));
    _vertices->push_back(Vec3(-0.5f, -0.5f, .0f));
    _vertices->push_back(Vec3(0.5f,  0.5f, .0f));
    _vertices->push_back(Vec3(-0.5f, 0.5f, .0f));
    _vertices->push_back(Vec3(0.0f, 0.0f, .0f));

    _node->addDrawable(_geometry.get());

    _uniform_mvp_mat = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uMVP");
    _uniform_color = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "uColor");
    Uniform * _uniform_pointSize = new osg::Uniform(osg::Uniform::FLOAT, "uPointSize");

    _uniform_color->set(Vec4(1.0, 0.5, .0, 1.0));
    _uniform_pointSize->set(20.0f);
    _uniform_mvp_mat->set(*(new osg::RefMatrixf()));

    osg::StateSet * stateset = new osg::StateSet;
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateset->setMode(GL_VERTEX_PROGRAM_POINT_SIZE, osg::StateAttribute::ON);
    stateset->addUniform(_uniform_color);
    stateset->addUniform(_uniform_mvp_mat);
    stateset->addUniform(_uniform_pointSize);

    _geometry->setStateSet(stateset);
    _geometry->setDataVariance(osg::Object::DYNAMIC);
    _geometry->addPrimitiveSet(new DrawArrays(osg::PrimitiveSet::POINTS,0,_vertices->size()));

    Program * program = osg_utils::createShaderProgram("shaders/osgPoint.vert", "shaders/point.frag", manager);
    program->addBindAttribLocation("vPosition", _attribute_vpos);
    _geometry->setVertexAttribArray(_attribute_vpos, _vertices.get(), osg::Array::BIND_PER_VERTEX);
    _geometry->getOrCreateStateSet()->setAttribute(program);

    osgApp->addNode(_node.get());
    view = osgApp->getViewer();
}
