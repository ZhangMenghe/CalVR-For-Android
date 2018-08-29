//
// Created by menghe on 8/3/2018.
//

#ifndef MYGLES_ALL_CONTROLLER_H
#define MYGLES_ALL_CONTROLLER_H

#include <android/asset_manager.h>
#include <osgViewer/Viewer>
#include <cvrKernel/CVRViewer.h>
#include <cvrKernel/InteractionManager.h>
#include "SpatialViz.h"
#include "bgDrawable.h"
#include "arcoreController.h"
#include <MenuBasics.h>
namespace controller{
    class allController {
    private:
        AAssetManager *const _asset_manager;
        osg::ref_ptr<osg::Group>  _root;

        int _screenWidth;
        int _screenHeight;
        float _screen_ratio;

        cvr::CVRViewer * _viewer;
        cvr::SceneManager * _scene;
        cvr::ConfigManager * _config;
        cvr::MenuManager * _menu;
        cvr::InteractionManager * _interactionManager;
        cvr::ComController * _communication;
        cvr::TrackingManager * _tracking;
        cvr::Navigation * _navigation;
        SpatialViz* _spatialViz;
        MenuBasics* _menuBasics;

        /////////////////
        osg::ref_ptr<bgDrawable> _bgDrawable;
        osg::ref_ptr<osg::Group>  _sceneGroup;
        arcoreController * _ar_controller;
        std::stack<utils::glState> glStateStack;


        ////
        bool debug_flag = true;
        float _distance = 1000;
        /// \param pos
        void createDebugOSGSphere(osg::Vec3 pos);
        void initialize_camera();
        void setupDefaultEnvironment(const char* root_path);
        osg::Vec3f screenToWorld(float x, float y);
        void commonMouseEvent(cvr::MouseInteractionEvent * mie,
                              int pointer_num, float x, float y);

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

        void onTouchMove(int pointer_num, float destx, float desty);
    };
}



#endif //MYGLES_CALVRCONTROLLER_H
