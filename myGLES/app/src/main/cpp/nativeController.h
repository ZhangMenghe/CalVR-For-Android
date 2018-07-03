//
// Created by menghe on 6/29/2018.
//

#ifndef MYGLES_NATIVECONTROLLER_H
#define MYGLES_NATIVECONTROLLER_H

#include "utils.h"
#include "arcore_utils.h"
#include "arcore_c_api.h"
#include "planeRenderer.h"
namespace controller {
    class nativeController {
    public:
        nativeController();

        nativeController(AAssetManager *assetManager);

        ~nativeController();

        void onCreate();

        void onPause();

        void onResume(void * env, void* context, void* activity);

        void onDrawFrame();

        void onViewChanged(int rot, int width, int height);
        void onTouched(float x, float y);

        bool hasDetectedPlane(){return _plane_num>0;}
    private:
        ArSession * _ar_session = nullptr;
        ArFrame * _ar_frame = nullptr;//get frame state
        AAssetManager *const _asset_manager;
        int _plane_num = 0;
        bool _install_requested = false;

        int _displayRotation = 0;
        int _width = 1;
        int _height = 1;

        planeRenderer * _plane_renderer;


    };
}



#endif //MYGLES_NATIVECONTROLLER_H
