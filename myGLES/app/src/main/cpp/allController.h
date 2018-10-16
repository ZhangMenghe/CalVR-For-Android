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

namespace controller{
    class allController {
    private:
        AAssetManager *const _asset_manager;
        osg::ref_ptr<osg::Group>  _root;

        int _screenWidth;
        int _screenHeight;

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

        float _touchX, _touchY;
        /// param pos
        ref_ptr<osg::Geode> createDebugOSGSphere(osg::Vec3 pos);

        void initialize_camera();
        void setupDefaultEnvironment(const char* root_path);

        void commonMouseEvent(cvr::MouseInteractionEvent * mie,
                              int pointer_num, float x, float y);
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
