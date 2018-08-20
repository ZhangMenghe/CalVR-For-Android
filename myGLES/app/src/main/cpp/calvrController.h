//
// Created by menghe on 8/3/2018.
//

#ifndef MYGLES_CALVRCONTROLLER_H
#define MYGLES_CALVRCONTROLLER_H

#include <android/asset_manager.h>
#include <osgViewer/Viewer>
#include <cvrKernel/CVRViewer.h>
#include <cvrKernel/InteractionManager.h>
#include "SpatialViz.h"
namespace controller{
    class calvrController {
    private:
//        cvr::CalVR * calvr;
        AAssetManager *const _asset_manager;
        osg::ref_ptr<osg::Group>  _root;

        int _screenWidth;
        int _screenHeight;

        ///////////////////////////////
//        osgViewer::Viewer * _viewer;
        cvr::CVRViewer * _viewer;
        cvr::SceneManager * _scene;
        cvr::ConfigManager * _config;
        cvr::MenuManager * _menu;
        cvr::InteractionManager * _interactionManager;
        cvr::TrackingManager * _tracking;
        cvr::Navigation * _navigation;
        SpatialViz* _spatialViz;

        void createDebugOSGSphere(osg::Vec3 pos);
        void initialize_camera();
        void setupDefaultEnvironment(const char* root_path);
        osg::Vec3f screenToWorld(float x, float y);

    public:
        calvrController(AAssetManager *assetManager);

        void onCreate(const char * calvr_path);

        void onDrawFrame();

        void onViewChanged(int rot, int width, int height);

        void onResourceLoaded(const char* path);

        void onTouch(float x, float y);

        void onDoubleTouch(float x, float y);

        void onTouchMove(float srcx, float srcy, float destx, float desty);
    };
}



#endif //MYGLES_CALVRCONTROLLER_H
