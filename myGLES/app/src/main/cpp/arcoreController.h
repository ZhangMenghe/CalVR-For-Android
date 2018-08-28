//
// Created by menghe on 7/18/2018.
//

#ifndef MYGLES_ARCORECONTROLLER_H
#define MYGLES_ARCORECONTROLLER_H

#include <cstdlib>
#include <unordered_map>
#include <array>
#include "arcore_c_api.h"
#include "arcore_utils.h"
#include "utils.h"
#include "osg_pointcloudRenderer.h"
#include "osg_planeRenderer.h"
#include "planeDrawable.h"

#include "pointDrawable.h"
namespace {
    constexpr size_t kMaxNumberOfAndroidsToRender = 20;
    constexpr int32_t kPlaneColorRgbaSize = 16;

    const glm::vec3 kWhite = {255, 255, 255};

    constexpr std::array<uint32_t, kPlaneColorRgbaSize> kPlaneColorRgba = {
            {0xFFFFFFFF, 0xF44336FF, 0xE91E63FF, 0x9C27B0FF, 0x673AB7FF, 0x3F51B5FF,
                    0x2196F3FF, 0x03A9F4FF, 0x00BCD4FF, 0x009688FF, 0x4CAF50FF, 0x8BC34AFF,
                    0xCDDC39FF, 0xFFEB3BFF, 0xFFC107FF, 0xFF9800FF}};

    inline glm::vec3 GetRandomPlaneColor() {
        const int32_t colorRgba = kPlaneColorRgba[std::rand() % kPlaneColorRgbaSize];
        return glm::vec3(((colorRgba >> 24) & 0xff) / 255.0f,
                         ((colorRgba >> 16) & 0xff) / 255.0f,
                         ((colorRgba >> 8) & 0xff) / 255.0f);
    }
}
typedef  struct{
    float intensity = 0.8f;
    float color_correction[4] = {1.f, 1.f, 1.f, 1.f};
}LightSrc;
typedef struct {
    std::unordered_map<ArPlane*, glm::vec3> plane_color_map;
    bool this_is_the_first_plane = true;
}PlaneParams;
class arcoreController {
private:
    ArSession * _ar_session = nullptr;
    ArFrame * _ar_frame = nullptr;//get frame state
    ArTrackingState cam_track_state;

    LightSrc _light;
    PlaneParams _planes;

    int _displayRotation = 0;
    int _width = 1;
    int _height = 1;
    bool _install_requested = false;
    int last_point_num = 0;

public:
    glm::mat4 view_mat;
    glm::mat4 proj_mat;
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

//            CHECK(ArCoreApk_requestInstall(env, activity, user_requested_install,&install_status) == AR_SUCCESS);

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
    float* updateBackgroundRender(){
        // UVs of the quad vertices (S, T)
        const GLfloat uvs[] = {
                0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        };
        int32_t geometry_changed = 0;
        ArFrame_getDisplayGeometryChanged(_ar_session, _ar_frame, &geometry_changed);
        if (geometry_changed != 0){
            ArFrame_transformDisplayUvCoords(_ar_session, _ar_frame, 8, uvs,
                                             transformed_camera_uvs);
            return transformed_camera_uvs;
        }
        return nullptr;
    }
    bool renderPointClouds(pointDrawable * drawable){
        ArPointCloud * pointCloud;
        ArStatus  pointcloud_Status = ArFrame_acquirePointCloud(_ar_session, _ar_frame, &pointCloud);
        if(pointcloud_Status != AR_SUCCESS)
            return false;

        int32_t num_of_points = 0;
        const float* pointCloudData;

        ArPointCloud_getNumberOfPoints(_ar_session, pointCloud, &num_of_points);
        if(num_of_points <= 0)
            return false;
//        last_point_num = num_of_points;

        //point cloud data with 4 params (x,y,z, confidence)
        ArPointCloud_getData(_ar_session, pointCloud, &pointCloudData);
//        for(int i=0;i<num_of_points;i++){
//            LOGE("%f, %f, %f ", pointCloudData[4*i], pointCloudData[4*i+1], pointCloudData[4*i+2]);
//        }
        drawable->updateVertices(pointCloudData, num_of_points);
        drawable->updateARMatrix(proj_mat*view_mat);
        ArPointCloud_release(pointCloud);
        return true;
    }
    /*bool updatePointCloudRenderer(osg_pointcloudRenderer * pc_renderer, osg_objectRenderer * objRenderer){
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

        float aaa[16] = {
                1, 0, 0, pointCloudData[0],
                0, 1, 0, pointCloudData[1],
                0, 0, 1, pointCloudData[2],
                0, 0, 0, 1
        };

        glm::mat4 pModel;

        memcpy( glm::value_ptr( pModel ), aaa, sizeof( aaa ) );

        objRenderer->Draw(proj_mat,view_mat, pModel, _light.color_correction,_light.intensity);

        ArPointCloud_release(pointCloud);
        return true;
    }*/
    bool isTracking(){
        return (cam_track_state == AR_TRACKING_STATE_TRACKING);
    }
    void doLightEstimation(){
        ArLightEstimate* ar_light_estimate;
        ArLightEstimateState ar_light_estimate_state;
        ArLightEstimate_create(_ar_session, &ar_light_estimate);

        ArFrame_getLightEstimate(_ar_session, _ar_frame, ar_light_estimate);
        ArLightEstimate_getState(_ar_session, ar_light_estimate, &ar_light_estimate_state);
        if(ar_light_estimate_state == AR_LIGHT_ESTIMATE_STATE_VALID){
            ArLightEstimate_getColorCorrection(_ar_session, ar_light_estimate, _light.color_correction);
            ArLightEstimate_getPixelIntensity(_ar_session, ar_light_estimate, &_light.intensity);
        }

        ArLightEstimate_destroy(ar_light_estimate);
    }

    PlaneParams doPlaneDetection(){
        int detectedPlaneNum;
        ArTrackableList* plane_list = nullptr;
        ArTrackableList_create(_ar_session, & plane_list);
        CHECK(plane_list!= nullptr);
        ArSession_getAllTrackables(_ar_session, AR_TRACKABLE_PLANE,plane_list);
        ArTrackableList_getSize(_ar_session, plane_list, &detectedPlaneNum);

        for(int i=0; i<detectedPlaneNum; i++){
            ArTrackable * ar_trackable = nullptr;
            ArTrackableList_acquireItem(_ar_session, plane_list, i, &ar_trackable);

            //cast down trackable to plane
            ArPlane * ar_plane = ArAsPlane(ar_trackable);

            //check the trackingstate, if not tracking, skip rendering
            ArTrackingState trackingState;
            ArTrackable_getTrackingState(_ar_session, ar_trackable, &trackingState);
            if(trackingState != AR_TRACKING_STATE_TRACKING)
                continue;

            //check if the plane contain the subsume plane, if so, skip to avoid overlapping
            ArPlane * subsume_plane;
            ArPlane_acquireSubsumedBy(_ar_session, ar_plane, &subsume_plane);
            if(subsume_plane != nullptr)
                continue;
            glm::vec3 plane_color;
            //Find if the plane already in the dic with specific color. Or add into dic
            auto iter = _planes.plane_color_map.find(ar_plane);
            if(iter!=_planes.plane_color_map.end()){
                plane_color = iter->second;
                ArTrackable_release(ar_trackable);
            }else{
                if(_planes.this_is_the_first_plane){
                    _planes.this_is_the_first_plane = false;
                    plane_color = glm::vec3(255, 255, 255);
                }else{
                    plane_color = GetRandomPlaneColor();
                }
                _planes.plane_color_map.insert({ar_plane, plane_color});
            }
//            drawable->updateARMatrix(proj_mat, view_mat);
//            drawable->updateVertices(_ar_session, ar_plane, plane_color);
        }
        ArTrackableList_destroy(plane_list);
        plane_list = nullptr;
        return _planes;
    }

};


#endif //MYGLES_ARCORECONTROLLER_H
