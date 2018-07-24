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
    osg::Sphere *sphere;
    osg::ref_ptr<osg::ShapeDrawable> shape;
    osgViewer::Viewer * view;
public:
    osg::ref_ptr<osg::Geode> node;
    void createSphere(osg_controller::osgController * osgApp);
    void onMove(float x, float y)
    {
        view->getEventQueue()->mouseMotion(x, y);
    }
    void onTouched(bool down, float x, float y)
    {
        if (down)
            view->getEventQueue()->mouseButtonPress(x, y, 2 /* MMB */);
        else
            view->getEventQueue()->mouseButtonRelease(x, y, 2 /* MMB */);
    }
};


#endif //MYGLES_GESTURE_CONTROLLER_H
