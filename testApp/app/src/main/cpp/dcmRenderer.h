#ifndef DCM_RENDERER_H
#define DCM_RENDERER_H

#include <vector>

// Include the latest possible header file( GL version header )
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>

#include "AndroidHelper.h"
#include <glm/glm.hpp>
//typedef struct mdcmImage{
//    unsigned int* data;
//    float sizex, sizey, sizez;
//    float location;
//
//    mdcmImage(unsigned int * _data, float _location, float _sizex, float _sizey, float _sizez){
//        data = _data;
//        size
//    }
//}dcmImage;
class Camera{
    glm::mat4 _viewMat, _projMat;
    glm::vec3 _eyePos;
public:
    Camera(){
        _eyePos = glm::vec3(0,0,0);
        _viewMat = glm::mat4(1.0);
        _projMat = glm::mat4(1.0);
    }
    glm::mat4 getProjMat(){return _projMat;}
    glm::mat4 getViewMat(){return _viewMat;}
    glm::vec3 getCameraPosition(){return _eyePos;}
};
class dcmImage{
public:
    unsigned int* data;
    float location;

    dcmImage(unsigned int * _data, float _location):
    data(_data), location(_location){}

};
class cubeRenderer{
public:
    cubeRenderer(AAssetManager *assetManager):
    _asset_manager(assetManager),
    mEglContext(eglGetCurrentContext()),
    mProgram(0){
        _camera = new Camera();
        _modelMat = glm::mat4(1.0f);
    }
protected:
    glm::mat4 _modelMat;

    GLuint VAO,VBO[2], EBO;
    GLuint mProgram;

    EGLContext mEglContext;
    AAssetManager* _asset_manager;
    Camera* _camera;
};


class dcmVolumeRender:public cubeRenderer{
public:
    dcmVolumeRender(AAssetManager *assetManager);
    void initGeometry(bool origin);
    void initGeometry();

    void addImage(unsigned int* img, float location);
    void initDCMIProperty(size_t w, size_t h, int thickness){
        volume_size = glm::vec3(w*CONVERT_UNIT, h*CONVERT_UNIT ,thickness*CONVERT_UNIT);
        img_width = w; img_height = h;
    }
    void assembleTexture();
    void onViewChange(int w, int h){
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void onDraw();
    void onNaiveDraw();
//    {
//        glClear(GL_COLOR_BUFFER_BIT);}

protected:
    const float CONVERT_UNIT = 0.001f;
    const int UI_SIZE = sizeof(unsigned int);
    size_t img_width, img_height, dimensions;

    unsigned int volume_texid;
    std::vector<dcmImage *> images_;
private:
    glm::vec3 volume_size;
};


#endif
