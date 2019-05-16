#ifndef DCM_RENDERER_H
#define DCM_RENDERER_H

#include <vector>
#include <unordered_map>
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
#include "perfMonitor.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/vec4.hpp>
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
enum Face{
    FRONT=0,BACK,LEFT,RIGHT,UP,BOTTOM
};
typedef std::pair<glm::vec3, int> Polygon;
typedef std::unordered_map<Face, std::vector<int>> PolygonMap;
class Camera{
    glm::mat4 _viewMat, _projMat;
    glm::vec3 _eyePos, _worldUp, _Front, _center;

    const float NEAR_PLANE = 0.01f;
    const float FAR_PLANE = 1000.0f;
    const float FOV = 45.0f;

    void updateCameraVector(){
        _viewMat = glm::lookAt(_eyePos,_center,_worldUp);
    }
public:
    Camera(){
        _worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        _eyePos = glm::vec3(0.0f, 0.f, 3.0f);
        _Front = glm::vec3(0.0f, 0.0f, -1.0f);
        _center = glm::vec3(_eyePos.x + _Front.x, _eyePos.y + _Front.y, _eyePos.z + _Front.z);
        updateCameraVector();
    }
    void setProjMat(int screen_width, int screen_height){
        float screen_ratio = ((float)screen_width) / screen_height;
        _projMat = glm::perspective(FOV, screen_ratio, NEAR_PLANE, FAR_PLANE);
    }
    glm::mat4 getProjMat(){return _projMat;}
    glm::mat4 getViewMat(){return _viewMat;}
    glm::vec3 getCameraPosition(){return _eyePos;}
    glm::vec3 getViewDirection(){return _Front;}

    void rotateCamera(int axis, glm::vec4 center, float offset){
        glm::mat4 modelMat = glm::mat4(1.0);
        glm::vec3 rotateAxis = glm::vec3(0,1,0);
        if(axis != 3)
            rotateAxis = glm::vec3(1,0,0);
        modelMat = glm::translate(modelMat, glm::vec3(-center.x, -center.y, -center.z));
        modelMat = glm::rotate(modelMat, offset, rotateAxis);
        modelMat = glm::translate(modelMat, glm::vec3(center.x, center.y, center.z));
        _eyePos = glm::vec3(modelMat * glm::vec4(_eyePos, 1.0));
        _center = glm::vec3(center);
        updateCameraVector();
    }
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
    const int MAX_VERTEX_NUM = 8;
    const int MAX_INDICE_NUM = 36;
    glm::mat4 _modelMat;

    GLuint VAO,VBO[2], EBO;
    GLuint mProgram;

    EGLContext mEglContext;
    AAssetManager* _asset_manager;
    Camera* _camera;

    int indices_num_, vertices_num_;

    const GLfloat sVertex[24] = {//World					//Color
            -0.5f,-0.5f,0.5f,		//x0, y0, z1, //0.0f,0.0f,0.0f,	//v0
            0.5f,-0.5f,0.5f,		//x1,y0,z1, //1.0f,0.0f,0.0f,	//v1
            0.5f,0.5f,0.5f,			//x1, y1, z1,//1.0f,1.0f,0.0f,	//v2
            -0.5f,0.5f,0.5f,		//x0,y1,z1, //0.0f,1.0f,0.0f,	//v3
            -0.5f,-0.5f,-0.5f,		//x0,y0,z0,//0.0f,0.0f,1.0f,	//v4
            0.5f,-0.5f,-0.5f,		//x1,y0,z0,//1.0f,0.0f,1.0f,	//v5
            0.5f,0.5f,-0.5f,		//x1,y1,z0, //1.0f,1.0f,1.0f,	//v6
            -0.5f,0.5f,-0.5f,		//x0,y1,z0//0.0f,1.0f,1.0f,	//v7
    };
    const GLuint sIndices[36] = { 0,1,2,0,2,3,	//front
                                 4,6,7,4,5,6,	//back
                                 4,0,3,4,3,7,	//left
                                 1,5,6,1,6,2,	//right
                                 3,2,6,3,6,7,	//top
                                 4,5,1,4,1,0,	//bottom
    };
    GLfloat* vertices_;
    GLuint* indices_;
};


class dcmVolumeRender:public cubeRenderer{
public:
    dcmVolumeRender(AAssetManager *assetManager);


    void addImage(GLubyte * img, float location);
    void initDCMIProperty(size_t w, size_t h, int thickness){
//        volume_size = glm::vec3(w*CONVERT_UNIT, h*CONVERT_UNIT ,thickness*CONVERT_UNIT);
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
            if(fabsf(xoffset / _screen_w) > fabsf(yoffset/_screen_h)){
//                if(swithcer_render_texture)
                    _modelMat = glm::rotate(_modelMat, xoffset, glm::vec3(0,1,0));
//                else
//                    _camera->rotateCamera(3, glm::vec4(_modelMat[3]), xoffset);
            }else{
//                if(swithcer_render_texture)
                    _modelMat = glm::rotate(_modelMat, -yoffset, glm::vec3(1,0,0));
//                else
//                    _camera->rotateCamera(2, glm::vec4(_modelMat[3]), -yoffset);
            }
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
    bool changeRender(){
        render_mode = static_cast<RENDERER >((render_mode+1)%2);
        return (render_mode == RAYCAST);
    }
    void onSwitchersSet(int idx, bool isSet){
        if(idx == 0) use_color_tranfer = isSet;
        else if(idx == 1) use_lighting = isSet;
    }
    void onParamsSet(int idx, float value){
//        if(idx < 0)
//            setZpos(value);
//        else
            adjustParam[idx] = value;
    }
    bool getOriginalChecked(int idx){
        return (idx == 0)? use_color_tranfer:use_lighting;
    }
    float getOriginalValue(int idx){return adjustParam_origin[idx];}
    float getFPS(){ return fps_monitor_.Update();}
protected:
    const float CONVERT_UNIT = 0.001f;
    const int UI_SIZE = sizeof(GLubyte);
    size_t img_width, img_height, dimensions;

    unsigned int volume_texid, trans_texid;
    std::vector<dcmImage *> images_;

    float* cplane_points_;//[18] = {.0f};
    int cplane_points_num_ = 0;

private:
    perfMonitor fps_monitor_;
    enum RENDERER{
        RAYCAST = 0,
        TEXTURE_BASED
    };
    const float scale_inv = 0.15f;
    glm::fvec2 Mouse_old = glm::fvec2(.0);
    float _screen_w, _screen_h;
    const float MOUSE_ROTATE_SENSITIVITY = 0.005f;
    const float adjustParam_origin[3] = {500.0f, 0.6f, 350.0f};
    float adjustParam[3]= {500.0f, 0.6f, 350.0f};
    int adjustIdx = 0;
    bool switcher_move = true;
    bool use_color_tranfer = false, use_lighting = false;
    RENDERER render_mode = TEXTURE_BASED;


    GLuint* m_VAOs;
    GLuint VAO_PLANE, VBO_PLANE;

    glm::vec3 stepsize_;
    GLuint program_texture, program_ray, program_plane;

    std::vector<Polygon> polygon;
    PolygonMap polygon_map;
    void setting_1D_texture();

    void initGeometry();
    void initGeometry_texturebased();
    void initGeometry_Naive();

    void onNaiveDraw();
    void onTexturebasedDraw();
    void onRaycastDraw();

    void draw_intersect_plane();
    void updateVBOData();
    void setZpos(float nz);
    void updateGeometry(std::vector<Polygon> polygon, PolygonMap polygon_map, std::vector<int> rpoints);
};


#endif
