//
// Created by menghe on 7/22/2018.
//

#ifndef MYGLES_GESTURE_CONTROLLER_H
#define MYGLES_GESTURE_CONTROLLER_H

#include <osg/Geode>
#include "utils.h"
#include "osgController.h"
class gesture_controller {
public:
    osg::ref_ptr<osg::Geode> node;
    void createSphere(osg_controller::osgController * osgApp);
    void onTouched(float x, float y);

};


#endif //MYGLES_GESTURE_CONTROLLER_H
