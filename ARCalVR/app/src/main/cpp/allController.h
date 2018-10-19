#ifndef ARCALVR_ALL_CONTROLLER_H
#define ARCALVR_ALL_CONTROLLER_H

#include <string>
#include <android/asset_manager.h>
#include <osgViewer/Viewer>
#include <cvrKernel/CVRViewer.h>
#include <cvrKernel/InteractionManager.h>

class allController {
protected:

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

    float getFPS();
};



#endif
