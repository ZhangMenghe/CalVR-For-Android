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

    const float NEAR_PLANE = 0.01f;
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
    const int MAX_DIV_X = 50;
    const int MAX_DIV_Y = 50;
    const int MAX_DIV_Z = 50;

    const long MAX_VERTEX_NUM = MAX_DIV_X * MAX_DIV_Y * MAX_DIV_Z * 8;
    const long MAX_INDICE_NUM =  MAX_DIV_X * MAX_DIV_Y * MAX_DIV_Z * 36;
    const int VAO_DATA_LEN = 3;

    glm::mat4 _modelMat;

    GLuint VAO,VBO[2], EBO;
    GLuint mProgram;

    EGLContext mEglContext;
    AAssetManager* _asset_manager;
    Camera* _camera;

    int indices_num_, vertices_num_;

    const GLfloat sVertex[24] = {//World					//Color
            -0.5f,-0.5f,0.5f, //0.0f,0.0f,1.0f,		//x0, y0, z1, //	//v0
            0.5f,-0.5f,0.5f, //1.0f,0.0f,1.0f,		//x1,y0,z1, //	//v1
            0.5f,0.5f,0.5f,	//1.0f,1.0f,1.0f,		//x1, y1, z1,//	//v2
            -0.5f,0.5f,0.5f,//0.0f,1.0f,1.0f,		//x0,y1,z1, //	//v3
            -0.5f,-0.5f,-0.5f,//0.0f,0.0f,0.0f,	//x0,y0,z0,//	//v4
            0.5f,-0.5f,-0.5f,//1.0f,0.0f,0.0f,		//x1,y0,z0,//	//v5
            0.5f,0.5f,-0.5f,//1.0f,1.0f,0.0f,	//x1,y1,z0, //	//v6
            -0.5f,0.5f,-0.5f,//0.0f,1.0f,0.0f,		//x0,y1,z0//	//v7
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

        if(operate_entity == OP_CUT_SPHERE){
            adjust_cut_sphere_pos(x, y);
            return;
        }

        if(fabsf(xoffset / _screen_w) > fabsf(yoffset/_screen_h)){
            if(operate_entity == OP_CAMERA)
                _camera->rotateCamera(3, glm::vec4(_modelMat[3]), xoffset);
            else
                _modelMat = glm::rotate(_modelMat, xoffset, glm::vec3(0,1,0));


        }else{
            if(operate_entity == OP_CAMERA)
                _camera->rotateCamera(2, glm::vec4(_modelMat[3]), -yoffset);
            else
                _modelMat = glm::rotate(_modelMat, -yoffset, glm::vec3(1,0,0));
        }
    }
    void onDoubleTouch(int id, float x, float y){
        if(id == 0)//single finger
            gl_draw_mode_id = (gl_draw_mode_id+1)%3;
        else if(id == 1)
            adjust_cut_mode = static_cast<ADJUST_CUTTING >((adjust_cut_mode+ 1) % (int)CT_NR_ITEMS);
        else if(id == 2)//debug!!!!! three fingers
            operate_entity = static_cast<OPERATE_ENTITY >((operate_entity + 1) % (int)OP_NR_ITEMS);
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
            if(adjust_cut_mode == ADJ_CUT_PLANE)
                setCuttingPlane(value);//should be 0-1
            else
                setCuttingSphereRadius(value);
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
    enum ADJUST_CUTTING{
        ADJ_CUT_PLANE = 0,
        ADJ_CUT_SPHERE,
        CT_NR_ITEMS
    };
    enum OPERATE_ENTITY{
        OP_CAMERA = 0,
        OP_CUT_SPHERE,
        OP_NR_ITEMS
    };
    GLenum RenderMode[3] = {GL_TRIANGLES, GL_POINTS,GL_LINES};
    int gl_draw_mode_id = 0;

    const float scale_inv = 0.15f;
    const glm::vec3 scale_origin = glm::vec3(1.2f, -1.2f, 0.6f);

    glm::fvec2 Mouse_old = glm::fvec2(.0);
    float _screen_w, _screen_h;
    const float MOUSE_ROTATE_SENSITIVITY = 0.005f;
    const float adjustParam_origin[4] = {100.0f, 0.7f, 250.0f, 0.3f};
    float adjustParam[4]= {100.0f, 0.7f, 250.0f, 0.3f};

    bool use_color_tranfer = false, use_lighting = false, use_interpolation = false;
    RENDERER render_mode = RAYCAST;
    bool use_simple_cube = false;

    bool rotate_model = false;//toggle between rotate model and camera


    //////all about cutting...
    glm::vec3 last_cutting_norm = glm::vec3(FLT_MAX), start_cutting;
    float cutting_length;
    bool is_cutting = true, is_in_deeper = false;
    glm::vec3 current_plane_normal_ = glm::vec3(1.0);
    glm::vec3 current_plane_point_ = glm::vec3(1000.0);
    glm::vec3 sphere_center_ = glm::vec3(1000.0);
    float sphere_radius = 0.2f;

    glm::vec3 ori_cut_pieces = glm::vec3(10,10,5);
    ADJUST_CUTTING adjust_cut_mode = ADJ_CUT_PLANE;
    OPERATE_ENTITY operate_entity = OP_CAMERA;
    const float sphere_raius_max  = 0.8f;

    void adjust_cut_sphere_pos(float x_screen, float y_screen){
        float x = x_screen/_screen_w, y = 1.0f - y_screen/_screen_h;

    }
    //////////

    GLuint* m_VAOs;
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
    void setCuttingSphereRadius(float percent){
        sphere_radius = sphere_raius_max * percent;
    }
    void updateCuttingPlane(glm::vec3 p, glm::vec3 p_norm);
    void updateCuttingPlane_gpu(glm::vec3 p, glm::vec3 p_norm);
    void updateGeometry(std::vector<Polygon> polygon, PolygonMap polygon_map, std::vector<int> rpoints);
    void updateTexCoords(GLfloat* vertices, glm::vec3 p);
    typedef std::pair<glm::vec3, int> vPair;
    struct CmpClass // class comparing vertices in the set
    {
        bool operator() (const vPair& p1, const vPair& p2) const{
            return p1.first.x == p2.first.x && p1.first.y == p2.first.y &&p1.first.z == p2.first.z;
        }
    };

    void dense_the_cube(int dx, int dy, int dz);
};


#endif
