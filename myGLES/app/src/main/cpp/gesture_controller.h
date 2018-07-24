//
// Created by menghe on 7/22/2018.
//

#ifndef MYGLES_GESTURE_CONTROLLER_H
#define MYGLES_GESTURE_CONTROLLER_H

#include <osg/Geode>
#include "utils.h"
#include "osgController.h"
class gesture_controller {
private:
    osg::ref_ptr<osg::Vec3Array> _vertices;
    osg::ref_ptr<osg::Geometry> _geometry;
    osg::ref_ptr<osg::Geode> _node;
    Uniform * _uniform_color;
    GLuint _attribute_vpos = 1;
    osg::Uniform* _uniform_mvp_mat;
    osgViewer::Viewer * view;

public:
    void createNode(osg_controller::osgController * osgApp,
                      AAssetManager * manager);
    void onMove(float x, float y)
    {
        LOGE("================called from onMove================");
        view->getEventQueue()->mouseMotion(x, y);
    }
    void onTouched(bool down, float x, float y)
    {
        LOGE("================called from onTouch================");
        osg::RefMatrixf * mat = new osg::RefMatrixf();
        _uniform_mvp_mat->set(*mat);
        _uniform_color->set(Vec4f(.0f,1.0f,.0f,1.0f));
        //Button numbering is 1 for left mouse button, 2 for middle, 3 for right.
        if (down)
            view->getEventQueue()->mouseButtonPress(x, y, 1);
        else
            view->getEventQueue()->mouseButtonRelease(x, y, 1);
    }
};


#endif //MYGLES_GESTURE_CONTROLLER_H
