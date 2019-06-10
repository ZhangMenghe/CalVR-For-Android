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
    glm::vec3 _eyePos, _up,  _center;

    const float NEAR_PLANE = 2.f;//as close as possible
    const float FAR_PLANE = 1000.0f;
    const float FOV = 45.0f;
    const glm::vec3 ORI_CAM_POS = glm::vec3(0.0f, .0f, 3.0f);
    const glm::vec3 ORI_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 ORI_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);

    void updateCameraVector(){
        _viewMat = glm::lookAt(_eyePos, _center, _up);
    }
public:
    Camera(){
        _up = ORI_UP; _eyePos = ORI_CAM_POS;
        _center = ORI_CAM_POS + ORI_FRONT;
        updateCameraVector();
    }
    void setProjMat(int screen_width, int screen_height){
        float screen_ratio = ((float)screen_width) / screen_height;
        _projMat = glm::perspective(FOV, screen_ratio, NEAR_PLANE, FAR_PLANE);
    }
    glm::mat4 getProjMat(){return _projMat;}
    glm::mat4 getViewMat(){return _viewMat;}
    glm::vec3 getCameraPosition(){return _eyePos;}
    glm::vec3 getCameraPosition(glm::mat4 modelMat){
        glm::mat4 inv_model = glm::inverse(modelMat);
        glm::vec4 eye_model = inv_model * glm::vec4(_eyePos.x, _eyePos.y, _eyePos.z, 1.0f);
        float inv_w = 1.0f / eye_model.w;
        return glm::vec3(eye_model.x * inv_w, eye_model.y * inv_w, eye_model.z * inv_w);
    }
    glm::vec3 getViewCenter(){return _center;}
    glm::vec3 getViewDirection(){return glm::normalize(_center - _eyePos);}

    void rotateCamera(int axis, glm::vec4 center, float offset){
        glm::vec3 rotateAxis = (axis==3)?glm::vec3(0,1,0):glm::vec3(1,0,0);
        glm::mat4 modelMat = glm::mat4(1.0);

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
    const int MAX_VERTEX_NUM = 15;
    const int MAX_INDICE_NUM = 90;
    const int VAO_DATA_LEN = 6;
    const size_t SLICE_DENSITY =3 ;
    glm::mat4 _modelMat;

    GLuint VAO,VBO[2], EBO;
    GLuint mProgram;

    EGLContext mEglContext;
    AAssetManager* _asset_manager;
    Camera* _camera;

    int indices_num_, vertices_num_;

    const GLfloat sVertex[48] = {//World					//Color
            -0.5f,-0.5f,0.5f, 0.0f,0.0f,1.0f,		//x0, y0, z1, //	//v0
            0.5f,-0.5f,0.5f, 1.0f,0.0f,1.0f,		//x1,y0,z1, //	//v1
            0.5f,0.5f,0.5f,	1.0f,1.0f,1.0f,		//x1, y1, z1,//	//v2
            -0.5f,0.5f,0.5f,0.0f,1.0f,1.0f,		//x0,y1,z1, //	//v3
            -0.5f,-0.5f,-0.5f,0.0f,0.0f,0.0f,	//x0,y0,z0,//	//v4
            0.5f,-0.5f,-0.5f,1.0f,0.0f,0.0f,		//x1,y0,z0,//	//v5
            0.5f,0.5f,-0.5f,1.0f,1.0f,0.0f,	//x1,y1,z0, //	//v6
            -0.5f,0.5f,-0.5f,0.0f,1.0f,0.0f,		//x0,y1,z0//	//v7
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
        if(fabsf(xoffset / _screen_w) > fabsf(yoffset/_screen_h)){
            if(rotate_model)
                _modelMat = glm::rotate(_modelMat, xoffset, glm::vec3(0,1,0));
            else
                _camera->rotateCamera(3, glm::vec4(_modelMat[3]), xoffset);
        }else{
            if(rotate_model)
                _modelMat = glm::rotate(_modelMat, -yoffset, glm::vec3(1,0,0));
            else
                _camera->rotateCamera(2, glm::vec4(_modelMat[3]), -yoffset);
        }
    }
    void onDoubleTouch(int id, float x, float y){
    }
    bool changeRender(){
        render_mode = static_cast<RENDERER >((render_mode+1)%2);
        return (render_mode == RAYCAST);
    }
    void onSwitchersSet(int idx, bool isSet){
        switch(idx){
            case 0:
                use_color_tranfer = isSet;
                return;
            case 1:
                use_lighting = isSet;
                return;
            case 2:
                use_interpolation = isSet;
                return;
            case 3:
                use_simple_cube = isSet;
                return;
            case 4:
                render_mode = isSet? TEXTURE_BASED:RAYCAST;
                return;
            default:
                break;
        }
    }
    void onParamsSet(int idx, float value){
        if(idx < 0){
            if(render_mode == TEXTURE_BASED)
                setCuttingPlane_texturebased(value);
            else
                setCuttingPlane(value);//should be 0-1
        }

        else
            adjustParam[idx] = value;
    }
    bool getOriginalChecked(int idx){
        switch(idx) {
            case 0:
                return use_color_tranfer;
            case 1:
                return use_lighting;
            case 2:
                return use_interpolation;
            case 3:
                return use_simple_cube;
            case 4:
                return (render_mode == TEXTURE_BASED);
            default:
                return false;
        }
    }
    float getOriginalValue(int idx){
        return adjustParam_origin[idx];
    }
    float getFPS(){ return fps_monitor_.Update();}
protected:
    const float CONVERT_UNIT = 0.001f;
    const int UI_SIZE = sizeof(GLubyte);
    size_t img_width, img_height, dimensions;

    unsigned int volume_texid, trans_texid;
    std::vector<dcmImage *> images_;

    float* cplane_points_ = nullptr;
    int cplane_points_num_ = 0;

private:
    perfMonitor fps_monitor_;
    enum RENDERER{
        RAYCAST = 0,
        TEXTURE_BASED
    };
    GLenum RenderMode[3] = {GL_TRIANGLES, GL_POINTS,GL_LINES};
    int gl_draw_mode_id = 0;

    const float scale_inv = 0.3f;
    const size_t NEED_SLCIES = 90;
    const glm::vec3 scale_origin = glm::vec3(1.2f, -1.2f, 0.6f);

    glm::fvec2 Mouse_old = glm::fvec2(.0);
    float _screen_w, _screen_h;
    const float MOUSE_ROTATE_SENSITIVITY = 0.005f;
    const float adjustParam_origin[4] = {400.0f, 0.9f, 250.0f, 0.3f};
    float adjustParam[4]= {400.0f, 0.9f, 250.0f, 0.3f};

    float adjust_opacities[3] = {1.0f, 1.0f, .0f};

    bool use_color_tranfer = false, use_lighting = false, use_interpolation = false;
    RENDERER render_mode = TEXTURE_BASED;
    bool use_simple_cube = false;

    bool rotate_model = false;//toggle between rotate model and camera


    glm::vec3 last_cutting_norm = glm::vec3(FLT_MAX), start_cutting;
    float cutting_length;
    bool is_cutting = true, is_in_deeper = false;
    int slice_start_idx = 0;
    std::vector<GLuint> m_VAOs;
    GLuint close_VAO;

    GLuint VAO_PLANE, VBO_PLANE;

    glm::vec3 stepsize_, volume_size;
    GLuint program_texture, program_ray, program_plane;

    std::vector<Polygon> polygon;
    PolygonMap polygon_map;
    void setting_1D_texture();

    void initGeometry();
    void initGeometry_texturebased();
    void initGeometry_Naive();

    void onNaiveDraw();
    void onTexturebasedDraw();
    void onTexturebasedDraw_dense();
    void onRaycastDraw();

    void restore_original_cube();
    void draw_intersect_plane();
    void updateVBOData();
    void setCuttingPlane(float percent = .0f);
    void setCuttingPlane_texturebased(float percent = .0f);
    void updateCuttingPlane(glm::vec3 p, glm::vec3 p_norm);
    void updateGeometry(std::vector<Polygon> polygon, PolygonMap polygon_map, std::vector<int> rpoints);
    void updateTexCoords(GLfloat* vertices, glm::vec3 p);
};


#endif
