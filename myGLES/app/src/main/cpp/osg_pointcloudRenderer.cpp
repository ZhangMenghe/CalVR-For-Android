//
// Created by menghe on 7/18/2018.
//

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Point>
#include <osg/LineWidth>
#include <osg/PointSprite>
#include <osg/BlendFunc>
#include <osg/ShapeDrawable>
#include "osg_pointcloudRenderer.h"
#include "osg_utils.h"

class pointCallBack:public osg::UniformCallback{
protected: arcoreController * _ar;
public:
    pointCallBack(arcoreController * ar):_ar(ar){}
    virtual void operator()(Uniform *uf, NodeVisitor *nv){
        uf->set(Matrixf(glm::value_ptr(_ar->getMVP())));
        uf->dirty();
    }
};

osg::ref_ptr<osg::Geode> osg_pointcloudRenderer::createNode(AAssetManager *manager, arcoreController* ar) {
    _geometry = new osg::Geometry();
    _node = new osg::Geode;

    _node->addDrawable(_geometry.get());
    _geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    _geometry->getOrCreateStateSet()->setMode(GL_VERTEX_PROGRAM_POINT_SIZE, osg::StateAttribute::ON);
    Program * program = osg_utils::createShaderProgram("shaders/osgPoint.vert", "shaders/point.frag", manager);
    program->addBindAttribLocation("vPosition", _attribute_vpos);

    _geometry->getOrCreateStateSet()->setAttribute(program);
    return _node.get();
}

void osg_pointcloudRenderer::Draw(arcoreController* ar) {
//    if(!ar->updatePointCloudRenderer())
//        return;
    osg::ref_ptr<osg::Vec3Array> _vertices = new osg::Vec3Array();

    _vertices->push_back(Vec3(0.5f, -0.5f,  .0f));
    _vertices->push_back(Vec3(-0.5f, -0.5f, .0f));
    _vertices->push_back(Vec3(0.5f,  0.5f, .0f));
    _vertices->push_back(Vec3(-0.5f, 0.5f, .0f));
    _vertices->push_back(Vec3(0.0f, 0.0f, .0f));
    _geometry->addPrimitiveSet(new DrawArrays(osg::PrimitiveSet::POINTS, 0, _vertices->size()));
    _geometry->setVertexAttribArray(_attribute_vpos, _vertices.get(), osg::Array::BIND_PER_VERTEX);
}