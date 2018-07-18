//
// Created by menghe on 7/18/2018.
//

#include <osg/Geometry>
#include <osg/Geode>
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
osg::ref_ptr<osg::Node> osg_pointcloudRenderer::createNode(AAssetManager *manager, arcoreController* ar) {
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Sphere(osg::Vec3(.0f,.0f,.0f), 1.0f));
    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
    _node = new osg::Geode;
    _node->addDrawable(shape.get());

    _uniform_color = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "uColor");
    _uniform_color->set(osg::Vec4(.0f, .0f, 1.0f, 1.0f));
    osg::Uniform* _uniform_mvp = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uMVP");
    _uniform_mvp->setUpdateCallback(new pointCallBack(ar));

    osg::StateSet* stateset = new osg::StateSet;
    stateset->addUniform(_uniform_color);
    stateset->addUniform(_uniform_mvp);
    _node->setStateSet(stateset);
    _node->getOrCreateStateSet()->setAttribute(
            osg_utils::createShaderProgram("shaders/osgPoint.vert", "shaders/point.frag", manager));


    /* osg::ref_ptr<osg::Vec3Array> _vertices = new osg::Vec3Array();
     _vertices->push_back(Vec3f(.0f, .0f, .0f));

     _geometry = new osg::Geometry();
     _node = new osg::Geode;

    _geometry->setDataVariance(osg::Object::DYNAMIC);
    _geometry->setUseDisplayList(false);
    _geometry->setVertexArray(_vertices.get());
    _geometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, (GLsizei)_vertices->size()));
    _node->addDrawable(_geometry.get());*/


//    osg::Uniform* _uniform_pointsize = new osg::Uniform(osg::Uniform::DOUBLE, "uPointSize");
//    _uniform_pointsize->set(_default_size);
//    osg::Uniform* _uniform_color = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "uColor");
//    _uniform_color->set(osg::Vec4(1.0f, 0.5f, .0f, 1.0f));


//    osg::StateSet* stateset = new osg::StateSet;
////    stateset->addUniform(_uniform_pointsize);
//    stateset->addUniform(_uniform_color);
//    stateset->addUniform(_uniform_mvp);
//
//    _node->setStateSet(stateset);
//    _node->getOrCreateStateSet()->setAttribute(
//            osg_utils::createShaderProgram("shaders/osgPoint.vert", "shaders/point.frag", manager));
    return _node.get();
}

void osg_pointcloudRenderer::Draw(arcoreController* ar) {
    if(!ar->updatePointCloudRenderer())
        return;

    /*osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>( _geometry->getVertexArray() );
    vertices->clear();
    for(int i=0;i<ar->num_of_points;i++)
        vertices->push_back(Vec3f(ar->pointCloudData[4*i], ar->pointCloudData[4*i+1], ar->pointCloudData[4*i+2]));

    vertices->dirty();
    _geometry->setVertexArray(vertices);

    _geometry->dirtyBound();*/
}