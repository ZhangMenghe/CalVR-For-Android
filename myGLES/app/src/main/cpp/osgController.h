//
// Created by menghe on 7/12/2018.
//

#ifndef MYGLES_OSGCONTROLLER_H
#define MYGLES_OSGCONTROLLER_H

#include "osg_utils.h"
#include "arcore_c_api.h"

namespace osg_controller{
    class osgController {
    private:
        osgViewer::Viewer * _viewer;

        ArSession * _ar_session = nullptr;
        ArFrame * _ar_frame = nullptr;//get frame state

        int _displayRotation = 0;
        int _width = 1;
        int _height = 1;
        int _plane_num = 0;

    public:
        osgController();

        ~osgController();

        void onCreate();

        void onPause();

        void onResume(void * env, void* context, void* activity);

        void onDrawFrame(bool btn_status_normal);
        void onViewChanged(int rot, int width, int height);
        void onTouched(float x, float y);
        bool hasDetectedPlane(){ return  _plane_num > 0;}
        void debug_loadScene(const char* filename);
    };
}



#endif //MYGLES_OSGCONTROLLER_H
