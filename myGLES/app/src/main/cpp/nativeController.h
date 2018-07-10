//
// Created by menghe on 6/29/2018.
//

#ifndef MYGLES_NATIVECONTROLLER_H
#define MYGLES_NATIVECONTROLLER_H

#include <unordered_map>

#include "utils.h"
#include "arcore_utils.h"
#include "arcore_c_api.h"
#include "anchorRenderer.h"
#include "planeRenderer.h"
#include "cameraRenderer.h"
#include "pointcloudRenderer.h"
#include "objRenderer.h"
namespace controller {
    class nativeController {
    private:
        ArSession * _ar_session = nullptr;
        ArFrame * _ar_frame = nullptr;//get frame state



        std::vector<ColoredAnchor> _anchors;

        AAssetManager *const _asset_manager;
        int _plane_num = 0;
        bool _install_requested = false;
        bool _this_is_the_first_plane = true;

        int _displayRotation = 0;
        int _width = 1;
        int _height = 1;
        float _color_correction[4] = {1.f, 1.f, 1.f, 1.f};

        unordered_map<ArPlane*, vec3> _plane_color_map;

        planeRenderer * _plane_renderer;
        cameraRenderer * _camera_renderer;
        pointcloudRenderer * _pointcloud_renderer;
        objRenderer * _obj_renderer;
        anchorRenderer * _anchor_renderer;

    public:
        nativeController(AAssetManager *assetManager);

        ~nativeController();

        void onCreate();

        void onPause();

        void onResume(void * env, void* context, void* activity);

        void onDrawFrame(bool btn_status_normal);
        void onViewChanged(int rot, int width, int height);
        void onTouched(float x, float y);
        bool hasDetectedPlane(){ return  _plane_num > 0;}

    };
}



#endif //MYGLES_NATIVECONTROLLER_H
