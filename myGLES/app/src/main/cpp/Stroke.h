#ifndef STROKE_H
#define STROKE_H

#include <osg/Geometry>

namespace cvr{
class Stroke : public osg::Geometry
{
private:
    osg::Vec4f _startColor = osg::Vec4f(1.0f, .0f, .0f, 1.0f);
    osg::Vec4f _endColor = osg::Vec4f(1.0f, .0f, .0f, 1.0f);
    float _lineWidth = 20.0f;
    osg::Matrixd* _transform = new osg::Matrixd();
public:
    Stroke(osg::Vec3f start, osg::Vec3f end);
    void setColor(const osg::Vec4& c);
    void setBegin(float a, float b);
    void setEnd(float c, float d);
    void setBeginEnd(float a, float b, float c, float d);
}; // class
} // namespace

#endif // STROKE_H
