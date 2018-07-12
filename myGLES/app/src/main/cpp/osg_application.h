//
// Created by menghe on 7/11/2018.
//

#ifndef MYGLES_OSG_APPLICATION_H
#define MYGLES_OSG_APPLICATION_H

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>

#include "osg_utils.h"
#include "utils.h"

using namespace osg;
using namespace osgViewer;

class osgApplication{
private:
    Viewer * _viewer;
    void Initialization(){
        _viewer = new Viewer;
        // use single thread: critical for mobile and web
        _viewer->setThreadingModel(ViewerBase::SingleThreaded);
        _viewer->setCameraManipulator(new osgGA::TrackballManipulator);
    }

public:
    osgApplication(){Initialization();}

    void loadScene(const char* filename){
        Node * scene =  osgDB::readNodeFile(filename);
        if(nullptr == scene){
            LOGE("FAILED TO LOAD SCENE");
            return;
        }
        scene->getOrCreateStateSet()->setAttribute(
                osg_utils::createShaderProgram(osg_utils::vertexShader, osg_utils::fragmentShader));
        _viewer->setSceneData(scene);
    }
    void setupWindow(int width, int height){
        _viewer->setUpViewerAsEmbeddedInWindow(0,0,width,height);
    }
    void Draw(){
        _viewer->frame();
    }
    void onMouseMove(float x, float y){
        _viewer->getEventQueue()->mouseMotion(x,y);
    }
    void onMousePress(bool down, float x, float y){
        if(down){
            _viewer->getEventQueue()->mouseButtonPress(x, y, 2);
        }else{
            _viewer->getEventQueue()->mouseButtonRelease(x,y,2);
        }
    }
};
#endif //MYGLES_OSG_APPLICATION_H
