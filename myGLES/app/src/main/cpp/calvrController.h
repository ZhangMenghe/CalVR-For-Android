//
// Created by menghe on 8/3/2018.
//

#ifndef MYGLES_CALVRCONTROLLER_H
#define MYGLES_CALVRCONTROLLER_H

#include <android/asset_manager.h>
#include <osgViewer/Viewer>
#include <cvrKernel/CVRViewer.h>

namespace controller{
    class calvrController {
    private:
//        cvr::CalVR * calvr;
        AAssetManager *const _asset_manager;
//        osgViewer::Viewer * _viewer;
        cvr::CVRViewer * _viewer;
        osg::ref_ptr<osg::Group>  _root;

        cvr::SceneManager * _scene;
        cvr::ConfigManager * _config;
        cvr::MenuManager * _menu;

        void createDebugOSGSphere(osg::Vec3 pos);
        void initialize_camera();
        void setupDefaultEnvironment(const char* root_path);

    public:
        calvrController(AAssetManager *assetManager);

        void onCreate(const char * calvr_path);

        void onDrawFrame();

        void onViewChanged(int rot, int width, int height);

        void onResourceLoaded(const char* path);
    };
}



#endif //MYGLES_CALVRCONTROLLER_H
