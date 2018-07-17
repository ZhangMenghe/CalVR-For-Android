//
// Created by menghe on 7/17/2018.
//

#ifndef MYGLES_OSG_PLANERENDERER_H
#define MYGLES_OSG_PLANERENDERER_H
#include <GLES2/gl2.h>
#include <osg/PrimitiveSet>
#include <android/asset_manager.h>
#include "osg_utils.h"
#include "utils.h"
#include <arcore_c_api.h>
#include <vector>
using namespace std;
using namespace osg;
class osg_planeRenderer {
private:
    osg::ref_ptr<osg::Geode> _node;
    osg::ref_ptr<osg::Geometry> _geometry;
//    osg::Texture2D* _planeTexture;
    GLuint _textureId = 0;
    osg::ref_ptr<osg::Vec3Array> _vertices;
    DrawElementsUShort* _triangles;

    osg::Uniform* _uniform_tex_sampler;
    osg::Uniform* _uniform_model_mat;
    osg::Uniform* _uniform_mvp_mat;
    osg::Uniform* _uniform_normal_vec;
    osg::Uniform* _uniform_color;

    glm::mat4 _model_mat = glm::mat4(1.0f);
    glm::vec3 _normal_vec = glm::vec3(.0f);

    void _update_plane_vertices(const ArSession * arSession, const ArPlane * arPlane);

public:
    osg::ref_ptr<osg::Node> createNode(AAssetManager * manager);
    void Draw(const ArSession * arSession, const ArPlane * arPlane,
              const glm::mat4 & projMat, const  glm::mat4 & viewMat,
              const  glm::vec3 & color);
    glm::mat4 getModelMat(){return _model_mat;}

};


#endif //MYGLES_OSG_PLANERENDERER_H
