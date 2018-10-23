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
        perfMonitor * _fpsMonitor;

        cvr::CalVR *_CalVR;

        // AUX OSG Node & drawable
        osg::ref_ptr<osg::Group>  _root, _sceneGroup;
        std::stack<utils::glState> _glStateStack;
        osg::ref_ptr<bgDrawable> _bgDrawable;

        // Other factors
        int _screenWidth, _screenHeight;
        float _touchX, _touchY;

        ref_ptr<osg::Geode> createDebugOSGSphere(osg::Vec3 pos);


        void commonMouseEvent(cvr::MouseInteractionEvent * mie,
                              int pointer_num, float x, float y);
        void DrawRay();

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
