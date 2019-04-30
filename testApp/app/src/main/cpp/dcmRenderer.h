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
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    glm::vec3 _eyePos, _worldUp, _Front;

    const float NEAR_PLANE = 0.01f;
    const float FAR_PLANE = 1000.0f;
    const float FOV = 45.0f;

    void updateCameraVector(){
        _viewMat = glm::lookAt(_eyePos, glm::vec3(0,0,0), _worldUp);
    }
public:
    Camera(){
        _worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        _eyePos = glm::vec3(0.0f, 0.f, 3.0f);
        _Front = glm::vec3(0.0f, 0.0f, -1.0f);
        updateCameraVector();
    }
    void setProjMat(int screen_width, int screen_height){
        float screen_ratio = ((float)screen_width) / screen_height;
        _projMat = glm::perspective(FOV, screen_ratio, NEAR_PLANE, FAR_PLANE);
    }
    glm::mat4 getProjMat(){return _projMat;}
    glm::mat4 getViewMat(){return _viewMat;}
    glm::vec3 getCameraPosition(){return _eyePos;}
};
class dcmImage{
public:
    GLubyte * data;
    float location;

    dcmImage(GLubyte * _data, float _location):
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


    void addImage(GLubyte * img, float location);
    void initDCMIProperty(size_t w, size_t h, int thickness){
        volume_size = glm::vec3(w*CONVERT_UNIT, h*CONVERT_UNIT ,thickness*CONVERT_UNIT);
        img_width = w; img_height = h;
    }
    void assembleTexture();
    void onViewCreated();
    void onViewChange(int w, int h){
        glViewport(0, 0, w, h);
        _camera->setProjMat(w,h);
        _screen_w = w; _screen_h = h;
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void onDraw();



    void onSingleTouchDown(float x, float y){
        Mouse_old = glm::fvec2(x, y);
    }
    void onTouchMove(float x, float y){
        //Camera::instance()->Rotate_Camera(x - Mouse_old.x, Mouse_old.y - y);
        float xoffset = x - Mouse_old.x, yoffset = Mouse_old.y - y;
        Mouse_old = glm::fvec2(x, y);
        xoffset*= MOUSE_ROTATE_SENSITIVITY;
        yoffset*= MOUSE_ROTATE_SENSITIVITY;
        if(switcher_move){
            if(fabsf(xoffset / _screen_w) > fabsf(yoffset/_screen_h))
                _modelMat = glm::rotate(_modelMat, xoffset, glm::vec3(0,1,0));
                //rotate around y-axis
//        else{
//
//            _modelMat = glm::rotate(_modelMat, -yoffset, glm::vec3(1,0,0));
//        }
        }else{
            adjustParam[adjustIdx] += xoffset * adjustParam_origin[adjustIdx] * 0.01f;
            LOGE("==== ID: %d, NOW:%f", adjustIdx, adjustParam[adjustIdx]);
        }
    }
    void onDoubleTouch(int id, float x, float y){
        if(id == 1)
            switcher_move = !switcher_move;
        else
            adjustIdx = (adjustIdx+1)%3;
    }
protected:
    const float CONVERT_UNIT = 0.001f;
    const int UI_SIZE = sizeof(GLubyte);
    size_t img_width, img_height, dimensions;

    unsigned int volume_texid;
    std::vector<dcmImage *> images_;
private:
    glm::fvec2 Mouse_old = glm::fvec2(.0);
    float _screen_w, _screen_h;
    const float MOUSE_ROTATE_SENSITIVITY = 0.005f;
    const float adjustParam_origin[3] = {200.0f, 0.95f, 400.0f};
    float adjustParam[3] = {200.0f, 0.95f, 400.0f};
    int adjustIdx = 0;
    bool switcher_move = false;


    GLuint* m_puTextureIDs;
    GLuint* m_VAOs;

    glm::vec3 volume_size;
    void initGeometry();
    void initGeometry_texturebased();
    void initGeometry_Naive();

    void onNaiveDraw();
    void onTexturebasedDraw();
    void onRaycastDraw();
};


#endif
