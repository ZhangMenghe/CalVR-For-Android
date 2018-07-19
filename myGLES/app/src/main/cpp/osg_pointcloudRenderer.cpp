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

    _uniform_mvp_mat = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uMVP");
    Uniform * _uniform_color = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "uColor");
    _uniform_color->set(Vec4(1.0, 0.5, .0, 1.0));

    osg::StateSet * stateset = new osg::StateSet;
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateset->setMode(GL_VERTEX_PROGRAM_POINT_SIZE, osg::StateAttribute::ON);
    stateset->addUniform(_uniform_color);
    _geometry->setStateSet(stateset);
    _geometry->setDataVariance(osg::Object::DYNAMIC);

    _drawArray = new DrawArrays(osg::PrimitiveSet::POINTS);
    _drawArray->setFirst(0);
    _geometry->addPrimitiveSet(_drawArray);
    Program * program = osg_utils::createShaderProgram("shaders/osgPoint.vert", "shaders/point.frag", manager);
    program->addBindAttribLocation("vPosition", _attribute_vpos);

    _geometry->getOrCreateStateSet()->setAttribute(program);
    return _node.get();
}

void osg_pointcloudRenderer::Draw(arcoreController* ar) {
    if(!ar->updatePointCloudRenderer())
        return;
    _uniform_mvp_mat->set(osg::Matrixf(glm::value_ptr(ar->getMVP())));
    _vertices = new osg::Vec3Array();
//    for(int i=0;i<ar->num_of_points;i++)
//        _vertices->push_back(Vec3(ar->pointCloudData[4*i], ar->pointCloudData[4*i+1], ar->pointCloudData[4*i+2]));
    _vertices->clear();
    _vertices->push_back(Vec3(0.5f, -0.5f,  .0f));
    _vertices->push_back(Vec3(-0.5f, -0.5f, .0f));
    _vertices->push_back(Vec3(0.5f,  0.5f, .0f));
    _vertices->push_back(Vec3(-0.5f, 0.5f, .0f));
    _vertices->push_back(Vec3(0.0f, 0.0f, .0f));


    _drawArray->setCount(_vertices->size());
    _geometry->setVertexAttribArray(_attribute_vpos, _vertices.get(), osg::Array::BIND_PER_VERTEX);
}