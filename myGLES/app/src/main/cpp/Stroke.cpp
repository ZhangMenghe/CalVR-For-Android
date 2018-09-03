#include <osg/LineWidth>
#include "Stroke.h"
using namespace cvr;
Stroke::Stroke(osg::Vec3f start_pos, osg::Vec3f end_pos)
        :osg::Geometry(){
    osg::Vec3Array* vertices = new osg::Vec3Array;
    vertices->push_back(start_pos);
    vertices->push_back(end_pos);

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(_startColor);
    colors->push_back(_endColor);

    this->setVertexArray(vertices);
    this->setColorArray(colors);
    this->setColorBinding(osg::Geometry::BIND_OVERALL);
    this->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, 2));

    this->getOrCreateStateSet()->setAttribute(new osg::LineWidth(_lineWidth));
    this->setDataVariance(osg::Object::DYNAMIC);
    this->setUseDisplayList(false);
    this->setUseVertexBufferObjects(true);
}

void Stroke::setColor(const osg::Vec4 &c)
{
    osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(this->getColorArray());
    colors->front() = c;
    colors->dirty();
}

void Stroke::setBegin(float a, float b)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    (*verts)[0] = osg::Vec3(a,0,b);
    verts->dirty();
}

void Stroke::setEnd(float c, float d)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    (*verts)[1] = osg::Vec3(c,0,d);
    verts->dirty();
}

void Stroke::setBeginEnd(float a, float b, float c, float d)
{
    osg::Vec3Array* verts = static_cast<osg::Vec3Array*>(this->getVertexArray());
    if (verts->size() == 0) {
        verts->push_back(osg::Vec3(a,0,b));
        verts->push_back(osg::Vec3(c,0,d));
    }
    else {
        (*verts)[0] = osg::Vec3(a,0,b);
        (*verts)[1] = osg::Vec3(c,0,d);
    }

    verts->dirty();
}
