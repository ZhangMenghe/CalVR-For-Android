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


#define DEFAULT_CLICK_OFFSET -80
#define DEFAULT_MENU_OFFSET 300

namespace controller{
    class allController {
    private:
        AAssetManager *const _asset_manager;
        osg::ref_ptr<osg::Group>  _root;

        int _screenWidth;
        int _screenHeight;
//        float _screen_ratio;

        cvr::CVRViewer * _viewer;
        cvr::SceneManager * _scene;
        cvr::ConfigManager * _config;
        cvr::MenuManager * _menu;
        cvr::InteractionManager * _interactionManager;
        cvr::ComController * _communication;
        cvr::TrackingManager * _tracking;
        cvr::Navigation * _navigation;
        cvr::PluginManager * _plugins;

        /////////////////
        osg::ref_ptr<bgDrawable> _bgDrawable;
        osg::ref_ptr<osg::Group>  _sceneGroup;

        arcoreController * _ar_controller;
        std::stack<utils::glState> glStateStack;
        osg::ref_ptr<strokeDrawable> _strokeDrawable;
        osg::ref_ptr<pointDrawable> _cameraPoseDrawable;

        bool _pointerBntDown = false;
        float _touchX, _touchY;
        osg::Uniform * _uniform_mvp;
        /// param pos
        ref_ptr<osg::Geode> createDebugOSGSphere(osg::Vec3 pos);
        ref_ptr<osg::Geode> createPointingStick(osg::Vec3f pos);

        void initialize_camera();
        void setupDefaultEnvironment(const char* root_path);
//        osg::Vec3f screenToWorld(float x, float y);

        void commonMouseEvent(cvr::MouseInteractionEvent * mie,
                              int pointer_num, float x, float y, float offset);
        void DrawRay();

    public:
        allController(AAssetManager *assetManager);

        ~allController();
        void onCreate(const char * calvr_path);

        void onPause();

        void onResume(void * env, void* context, void* activity);

        void onDrawFrame(bool moveCam);

        void onViewChanged(int rot, int width, int height);

        void onResourceLoaded(const char* path);

        void onSingleTouchDown(int pointer_num, float x, float y);

        void onSingleTouchUp(int pointer_num, float x, float y);

        void onDoubleTouch(int pointer_num, float x, float y);

        void onTouchMove(int pointer_num, float x, float y);
    };
}



#endif //MYGLES_CALVRCONTROLLER_H
