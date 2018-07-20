//
// Created by menghe on 7/18/2018.
//

#ifndef MYGLES_ARCORECONTROLLER_H
#define MYGLES_ARCORECONTROLLER_H

#include <cstdlib>
#include "arcore_c_api.h"
#include "arcore_utils.h"
#include "utils.h"
#include "osg_pointcloudRenderer.h"

class arcoreController {
private:
    ArSession * _ar_session = nullptr;
    ArFrame * _ar_frame = nullptr;//get frame state

    int _displayRotation = 0;
    int _width = 1;
    int _height = 1;
    bool _install_requested = false;

    glm::mat4 view_mat;
    glm::mat4 proj_mat;
public:

    float transformed_camera_uvs[8];

    ~arcoreController(){
        if(_ar_session) {
            ArSession_destroy(_ar_session);
            ArFrame_destroy(_ar_frame);
        }
    }
    ArSession * getSession(){ return  _ar_session;}
    void onViewChanged(int rot, int width, int height){
        _displayRotation = rot;
        _width = width;
        _height = height;
        if (_ar_session != nullptr) {
            ArSession_setDisplayGeometry(_ar_session, rot, width, height);
        }
    }
    void onPause(){
        if(_ar_session)
            ArSession_pause(_ar_session);
    }
    void onResume(void *env, void *context, void *activity){
        if(nullptr == _ar_session){
            ArInstallStatus install_status;
            bool user_requested_install = !_install_requested;

            CHECK(ArCoreApk_requestInstall(env, activity, user_requested_install,&install_status) == AR_SUCCESS);

            switch (install_status) {
                case AR_INSTALL_STATUS_INSTALLED:
                    break;
                case AR_INSTALL_STATUS_INSTALL_REQUESTED:
                    _install_requested = true;
                    return;
            }
            CHECK(ArSession_create(env, context, &_ar_session)==AR_SUCCESS);
            CHECK(_ar_session);
            ArFrame_create(_ar_session, &_ar_frame);
            CHECK(_ar_frame);
            ArSession_setDisplayGeometry(_ar_session, _displayRotation, _width, _height);
        }
        const ArStatus status = ArSession_resume(_ar_session);
        CHECK(status == AR_SUCCESS);
    }
    void onDrawFrame(GLuint textureId);
    bool updateBackgroundRender(const GLfloat kUvs[]){
        int32_t geometry_changed = 0;
        ArFrame_getDisplayGeometryChanged(_ar_session, _ar_frame, &geometry_changed);
        if (geometry_changed != 0){
            ArFrame_transformDisplayUvCoords(_ar_session, _ar_frame, 8, kUvs,
                                             transformed_camera_uvs);
            return true;
        }
        return false;
    }
    bool updatePointCloudRenderer(osg_pointcloudRenderer * pc_renderer){
        ArPointCloud * pointCloud;
        ArStatus  pointcloud_Status = ArFrame_acquirePointCloud(_ar_session, _ar_frame, &pointCloud);
        if(pointcloud_Status != AR_SUCCESS)
            return false;

        int32_t num_of_points = 0;
        const float* pointCloudData;

        ArPointCloud_getNumberOfPoints(_ar_session, pointCloud, &num_of_points);
        if(num_of_points <= 0)
            return false;
        //point cloud data with 4 params (x,y,z, confidence)
        ArPointCloud_getData(_ar_session, pointCloud, &pointCloudData);
        pc_renderer->Draw(proj_mat*view_mat, num_of_points, pointCloudData);
        ArPointCloud_release(pointCloud);
        return true;
    }

};


#endif //MYGLES_ARCORECONTROLLER_H
