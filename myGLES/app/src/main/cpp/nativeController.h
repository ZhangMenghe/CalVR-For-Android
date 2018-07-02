//
// Created by menghe on 6/29/2018.
//

#ifndef MYGLES_NATIVECONTROLLER_H
#define MYGLES_NATIVECONTROLLER_H

#include "utils.h"
#include "arcore_c_api.h"
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
        AAssetManager *const _asset_manager;
        int _plane_num = 0;


    };
}



#endif //MYGLES_NATIVECONTROLLER_H
