//
// Created by menghe on 7/28/2018.
//

#ifndef MYGLES_POINTDRAWABLE_H
#define MYGLES_POINTDRAWABLE_H

#include <osg/Drawable>
#include "utils.h"
#define MAX_POINTS 100
class pointDrawable: public osg::Drawable {
private:
    GLuint _VAO;
    GLuint _VBO;
    GLuint _shader_program;
    GLuint _attrib_vertices_;
    GLuint _uniform_view_mat;
    GLuint _uniform_proj_mat;


    glm::vec4 _default_color= glm::vec4(1.0, 0.5, 0.0, 1.0);
    float _default_size = 20.0f;
    glm::mat4 _ar_camMat = glm::mat4();
    osg::Camera * _osgCamera;
//    //point cloud data with 4 params (x,y,z, confidence)
//    float pointCloudData[72] = {0.002609,	0.140262,	0.206912,	 .0f,
//    -0.026641,	0.165463,	0.191488,	 .0f,
//    -0.069134,	0.057006,	0.240065,	 .0f,
//    -0.050232,	0.057391,	0.239645,	 .0f,
//    0.045289	,0.16386,	0.199531,	 .0f,
//    -0.071337,	0.075365,	0.234478,	 .0f,
//    0.051582,	0.077414,	0.244598,	 .0f,
//    0.026819,	0.16393,	0.199223,	 .0f,
//    -0.003276,	0.099483,	0.233154,	 .0f,
//    -0.021424,	0.162992,	0.196775,	 .0f,
//    0.05243,	0.150938,	0.213602,	 .0f,
//    -0.045399,	0.183242,	0.184112,	 .0f,
//    0.050976,	0.185227,	0.188771,	 .0f,
//    0.060612,	0.134743,	0.208531,	 .0f,
//    0.022608,	0.042545,	0.255272,	 .0f,
//    -0.063322,	0.0664,	0.237355,	 .0f,
//    0.027872,	0.042821,	0.257776,	 .0f,
//    -0.084796,	0.023097,	0.249748,	 .0f
//    };
    int _point_num = 0;
//    const float pointCloudData[20] = {0.5f, -0.5f,  .0f, .0f,
//                                      -0.5f, -0.5f, .0f, .0f,
//                                      0.5f,  0.5f, .0f, .0f,
//                                      -0.5f, 0.5f, .0f, .0f,
//                                      0.0f, 0.0f, .0f, .0f};
    float *pointCloudData;
    float arcamcx, arcamcy;
public:
    void updateVertices(){
//        pointCloudData = (float*)malloc(8* sizeof(float));
//        memset(pointCloudData, 0, 8*sizeof(float));
//
//        pointCloudData[0] = 1.0f;pointCloudData[1] = -1.0f;
//        pointCloudData[4] = -1.0f;pointCloudData[5] = -1.0f;
//
//        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
//        glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * _point_num * sizeof(GLfloat), pointCloudData);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void updateVertices(const float *newData, int num_of_points){
         _point_num = num_of_points;
        size_t mem_size =  _point_num * 4 * sizeof(float);
        pointCloudData = (float*)malloc(mem_size);
        memcpy(pointCloudData, newData, mem_size);

        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, mem_size, pointCloudData);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void quatToMatrix(glm::mat4 &m, const float * quat){
        double sqw = quat[3]*quat[3];
        double sqx = quat[0]*quat[0];
        double sqy = quat[1]*quat[1];
        double sqz = quat[2]*quat[2];

        // invs (inverse square length) is only required if quaternion is not already normalised
        double invs = 1 / (sqx + sqy + sqz + sqw);
        m[0][0] = ( sqx - sqy - sqz + sqw)*invs ; // since sqw + sqx + sqy + sqz =1/invs*invs
        m[1][1] = (-sqx + sqy - sqz + sqw)*invs ;
        m[2][2] = (-sqx - sqy + sqz + sqw)*invs ;

        double tmp1 = quat[0]*quat[1];
        double tmp2 = quat[2]*quat[3];
        m[1][0] = 2.0 * (tmp1 + tmp2)*invs ;
        m[0][1] = 2.0 * (tmp1 - tmp2)*invs ;

        tmp1 = quat[0]*quat[2];
        tmp2 = quat[1]*quat[3];
        m[2][0] = 2.0 * (tmp1 - tmp2)*invs ;
        m[0][2] = 2.0 * (tmp1 + tmp2)*invs ;
        tmp1 = quat[1]*quat[2];
        tmp2 = quat[0]*quat[3];
        m[2][1] = 2.0 * (tmp1 + tmp2)*invs ;
        m[1][2] = 2.0 * (tmp1 - tmp2)*invs ;
    }
    void updateARView(const float * ar_camera_pose){
        quatToMatrix(_ar_camMat, ar_camera_pose);
        _ar_camMat[0][3] = ar_camera_pose[4];
        _ar_camMat[1][3] = -ar_camera_pose[6];
        _ar_camMat[2][3] = ar_camera_pose[5];
        _ar_camMat[3][3] = 1;
//        _osgCamera->setViewMatrixAsLookAt(osg::Vec3f(_ar_camMat[0][3],_ar_camMat[1][3],_ar_camMat[2][3]),
//                                          center,up);
    }
    void Initialization(AAssetManager * manager,osg::Camera * osgCamera);
    void drawImplementation(osg::RenderInfo&) const;
};

#endif //MYGLES_POINTDRAWABLE_H
