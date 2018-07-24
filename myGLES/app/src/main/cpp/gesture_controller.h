//
// Created by menghe on 7/22/2018.
//

#ifndef MYGLES_GESTURE_CONTROLLER_H
#define MYGLES_GESTURE_CONTROLLER_H

#include <osg/Geode>
#include "utils.h"
#include "osgController.h"
#include <osgGA/GUIEventHandler>
//viewer->getEventQueue()->userEvent(data)
struct TimerInfo : public osg::Referenced{
    TimerInfo(unsigned int c ):_count(c){}
    unsigned int _count;
};

//handler is used for both processing the timer object
//and handling timer events.
//addEvent() take GUIEventAdapter pointer
//GUIActionAdapter allows the handler to ask the GUI to take some acton
// in response to an incoming event. In most cases, this can actually be considered as the viewer object.
class TimerHandler:public osgGA::GUIEventHandler{
public:
    TimerHandler():_switch(nullptr),_count(0){}
    TimerHandler(osg::Switch * sw):_switch(sw), _count(0){}
    virtual bool handle(const osgGA::GUIEventAdapter & ea,
                              osgGA::GUIActionAdapter & aa){
        switch(ea.getEventType()){
            // automatically emitted every frame
                //Use to manage internal counter
            case osgGA::GUIEventAdapter::FRAME:
                _count ++;
                if(_count % 100 == 0){
                    osgViewer::Viewer * viewer = dynamic_cast<osgViewer::Viewer *>(&aa);
                    if(viewer){
                        viewer->getEventQueue()->userEvent(new TimerInfo(_count));
                    }
                }
                break;
            case osgGA::GUIEventAdapter::USER:
                const TimerInfo * timer = dynamic_cast<const TimerInfo*>(ea.getUserData());
                LOGE("Timer event at: %d ",timer->_count);
//                if(_gesture!= nullptr)
//                    _gesture->changePointColor();
                if(_switch!= nullptr && _switch.valid()){
                    _switch->setValue(0, !_switch->getValue(0));
                    _switch->setValue(1, !_switch->getValue(1));
                }
                break;
        }
        return false;
    }

protected:

    osg::ref_ptr<osg::Switch> _switch;
    unsigned int _count;
};

class gesture_controller {
private:
    osg::ref_ptr<osg::Vec3Array> _vertices;
    osg::ref_ptr<osg::Geometry> _geometry;
    osg::ref_ptr<osg::Geode> _node;
    Uniform * _uniform_color;
    GLuint _attribute_vpos = 1;
    osg::Uniform* _uniform_mvp_mat;
    osgViewer::Viewer * view;
    osg::Vec4f color1 = osg::Vec4f(1.0f, 0.5f, .0f, 1.0f);
    osg::Vec4f color2 = osg::Vec4f(.0f, 1.0f, 0.5f, 1.0f);
    bool isColor1 = true;
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
        changePointColor();
        view->addEventHandler(new TimerHandler());

        //Button numbering is 1 for left mouse button, 2 for middle, 3 for right.
        if (down)
            view->getEventQueue()->mouseButtonPress(x, y, 1);
        else
            view->getEventQueue()->mouseButtonRelease(x, y, 1);
    }
    void changePointColor(){
        if(isColor1)
            _uniform_color->set(color2);
        else
            _uniform_color->set(color1);
        isColor1 = !isColor1;
    }
};


#endif //MYGLES_GESTURE_CONTROLLER_H
