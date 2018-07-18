//
// Created by menghe on 7/18/2018.
//

#include <osg/Geometry>
#include <osg/Geode>
#include "osg_pointcloudRenderer.h"
#include "osg_utils.h"
class pointCallBack:public osg::UniformCallback{
public:
//    pointCallBack()
};
osg::ref_ptr<osg::Node> osg_pointcloudRenderer::createNode(AAssetManager *manager) {
     osg::ref_ptr<osg::Vec3Array> _vertices = new osg::Vec3Array();
     _vertices->push_back(Vec3f(.0f, .0f, .0f));

     _geometry = new osg::Geometry();
     _node = new osg::Geode;

    _geometry->setDataVariance(osg::Object::DYNAMIC);
    _geometry->setUseDisplayList(false);
    _geometry->setVertexArray(_vertices.get());
    _geometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, (GLsizei)_vertices->size()));
    _node->addDrawable(_geometry.get());


//    osg::Uniform* _uniform_pointsize = new osg::Uniform(osg::Uniform::DOUBLE, "uPointSize");
//    _uniform_pointsize->set(_default_size);
    osg::Uniform* _uniform_color = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "uColor");
    _uniform_color->set(osg::Vec4(1.0f, 0.5f, .0f, 1.0f));
    osg::Uniform* _uniform_mvp = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uMVP");
//    _uniform_mvp->setUpdateCallback(new pointCallBack());

    osg::StateSet* stateset = new osg::StateSet;
//    stateset->addUniform(_uniform_pointsize);
    stateset->addUniform(_uniform_color);
    stateset->addUniform(_uniform_mvp);

    _node->setStateSet(stateset);
    _node->getOrCreateStateSet()->setAttribute(
            osg_utils::createShaderProgram("shaders/osgPoint.vert", "shaders/point.frag", manager));
    return _node.get();
}

void osg_pointcloudRenderer::Draw(const ArSession *arSession, ArPointCloud *pointCloud,
                                  const glm::mat4 &mvpMat) {
    return;
    int32_t num_of_points = 0;
    ArPointCloud_getNumberOfPoints(arSession, pointCloud, &num_of_points);

    if(num_of_points <= 0)
        return;

    //point cloud data with 4 params (x,y,z, confidence)
    const float * pointCloudData;
    ArPointCloud_getData(arSession, pointCloud, &pointCloudData);

    osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>( _geometry->getVertexArray() );
    vertices->clear();
    for(int i=0;i<num_of_points;i++)
        vertices->push_back(Vec3f(pointCloudData[4*i], pointCloudData[4*i+1], pointCloudData[4*i+2]));

    vertices->dirty();
    _geometry->setVertexArray(vertices);

    osg::StateSet* stateset = _node->getStateSet();
    Uniform * umvp = stateset->getUniform("uMVP");
    umvp->set(Matrixf(glm::value_ptr(mvpMat)));
    umvp->dirty();

    _geometry->dirtyBound();
}