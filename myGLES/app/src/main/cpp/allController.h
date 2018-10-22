//
// Created by menghe on 8/3/2018.
//

#ifndef MYGLES_ALL_CONTROLLER_H
#define MYGLES_ALL_CONTROLLER_H

#include <string>
#include <android/asset_manager.h>
#include <osgViewer/Viewer>
#include <cvrKernel/CVRViewer.h>
#include <cvrKernel/InteractionManager.h>
#include "bgDrawable.h"
#include "arcoreController.h"
#include "strokeDrawable.h"
#include "perfMonitor.h"
#include "osg_objectRenderer.h"

namespace controller{
    class allController {
    protected:
        //Utils
        AAssetManager *const _asset_manager;
        arcoreController * _ar_controller;
        perfMonitor * _fpsMonitor;

        cvr::CalVR *_CalVR;
        // CalVR
//        cvr::CVRViewer * _viewer;
//        cvr::SceneManager * _scene;
//        cvr::ConfigManager * _config;
//        cvr::MenuManager * _menu;
//        cvr::InteractionManager * _interactionManager;
//        cvr::ComController * _communication;
//        cvr::TrackingManager * _tracking;
//        cvr::Navigation * _navigation;
//        cvr::PluginManager * _plugins;

        // AUX OSG Node & drawable
        osg::ref_ptr<osg::Group>  _root, _sceneGroup;
        std::stack<utils::glState> _glStateStack;
        osg::ref_ptr<bgDrawable> _bgDrawable;
        osg::ref_ptr<strokeDrawable> _strokeDrawable;
        osg::ref_ptr<pointDrawable> _pointcloudDrawable;
        std::vector<osg::ref_ptr<planeDrawable>> _planeDrawables;
        std::vector<osg_objectRenderer *>_object_renderers;
        std::vector<glm::mat4>_andy_Model_Mats;
        // Other factors
        int _screenWidth, _screenHeight;
        float _touchX, _touchY;
        int _plane_num = 0;
        int _andy_num = 0;
        float _color_correction[4] = {1.f, 1.f, 1.f, 1.f};

        ref_ptr<osg::Geode> createDebugOSGSphere(osg::Vec3 pos);

        void initialize_camera();

        void setupDefaultEnvironment(const char* root_path);

        void commonMouseEvent(cvr::MouseInteractionEvent * mie,
                              int pointer_num, float x, float y);
        void DrawRay();

        void DrawRealWorld();

        void onSingleFingerDoubleTouch(float x, float y);

    public:
        allController(AAssetManager *assetManager);

        ~allController();

        void onCreate(const char * calvr_path);

        void onPause();

        void onResume(void * env, void* context, void* activity);

        void onDrawFrame();

        void onViewChanged(int rot, int width, int height);

        void onResourceLoaded(const char* path);

        void onSingleTouchDown(int pointer_num, float x, float y);

        void onSingleTouchUp(int pointer_num, float x, float y);

        void onDoubleTouch(int pointer_num, float x, float y);

        void onTouchMove(int pointer_num, float x, float y);

        float getFPS(){return _fpsMonitor->Update();}
    };
}



#endif //MYGLES_CALVRCONTROLLER_H
