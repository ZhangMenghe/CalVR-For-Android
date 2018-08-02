//
// Created by menghe on 7/12/2018.
//

#ifndef MYGLES_OSGCONTROLLER_H
#define MYGLES_OSGCONTROLLER_H
#include <unordered_map>
#include "osg_utils.h"

#include "osg_cameraRenderer.h"
#include "osg_planeRenderer.h"
#include "osg_pointcloudRenderer.h"
#include "osg_objectRenderer.h"
#include "arcoreController.h"


#include <osgGA/KeySwitchMatrixManipulator>
#include "planeDrawable.h"
#include "bgDrawable.h"
namespace osg_controller{
    class osgController {
    private:
        AAssetManager *const _asset_manager;
        osg::ref_ptr<bgDrawable> _bgDrawable;
        osg::ref_ptr<pointDrawable> _pointcloudDrawable;
        std::vector<osg::ref_ptr<planeDrawable>> _planeDrawables;

        int _plane_num = 0;
        std::stack<utils::glState> glStateStack;
        float _color_correction[4] = {1.f, 1.f, 1.f, 1.f};
        std::unordered_map<ArPlane*,  glm::vec3> _plane_color_map;

        osgViewer::Viewer * _viewer;
        osg::ref_ptr<osg::Group>  _root;

        arcoreController * _ar_controller;


        /*DEPRECATED
         * osg_cameraRenderer * _camera_renderer;
         * osg_pointcloudRenderer * _pointcloud_renderer;
         * osg_planeRenderer * _plane_renderer;
         * */

        osg_objectRenderer *_object_renderer;

        void createDebugOSGSphere(osg::Vec3 pos);
        void _initialize_camera();

    public:
        osgController(AAssetManager * manager);

        ~osgController();

        void onCreate();

        void onPause();

        void onResume(void * env, void* context, void* activity);

        void onDrawFrame(bool btn_status_normal);
        void onViewChanged(int rot, int width, int height);
        void onTouched(float x, float y);
        bool hasDetectedPlane(){ return  _plane_num > 0;}
        void debug_loadScene(const char* filename);
        void addNode(osg::ref_ptr<osg::Geode> node){_root->addChild(node);}
        osgViewer::Viewer * getViewer(){return _viewer;}
    };
}



#endif //MYGLES_OSGCONTROLLER_H
