//
// Created by menghe on 7/17/2018.
//
#include <type_traits>
#include <osg/Texture2D>
#include "arcore_utils.h"
#include "osg_planeRenderer.h"
using namespace arcore_utils;

void osg_planeRenderer::_update_plane_vertices(const ArSession *arSession, const ArPlane*arPlane) {
    _vertices->clear();
    _triangles->clear();

    int32_t polygon_length;
    //get the number of elements(2*#vertives)
    ArPlane_getPolygonSize(arSession, arPlane, &polygon_length);

    if(polygon_length == 0){
        LOGE("NO valid plane polygon found");
        return;
    }
    const int32_t vertices_size = polygon_length/2;
    vector<vec2> raw_vertices(vertices_size);
    ArPlane_getPolygon(arSession, arPlane, value_ptr(raw_vertices.front()));

    //fill outter vertices
    for(int32_t i=0; i<vertices_size; i++)
        _vertices->push_back(Vec3(raw_vertices[i].x, raw_vertices[i].y, .0f));

    //get model matrix
    ArPose * arPose;
    ArPose_create(arSession, nullptr, &arPose);
    ArPlane_getCenterPose(arSession, arPlane, arPose);
    ArPose_getMatrix(arSession, arPose, value_ptr(_model_mat));

    _normal_vec = getPlaneNormal(*arSession, *arPose);

    // Feather distance 0.2 meters.
    const float kFeatherLength = 0.2f;
    // Feather scale over the distance between plane center and vertices.
    const float kFeatherScale = 0.2f;

    // Fill vertex 4 to 7, with alpha set to 1.
    for (int32_t i = 0; i < vertices_size; ++i) {
        // Vector from plane center to current point.
        vec2 v = raw_vertices[i];
        const float scale =
                1.0f - std::min((kFeatherLength / glm::length(v)), kFeatherScale);
        const vec2 result_v = scale * v;

        _vertices->push_back(Vec3(result_v.x, result_v.y, 1.0f));
    }

    const int32_t vertices_length = _vertices->size();
    const int32_t half_vertices_length = vertices_length / 2;

    // Generate triangle (4, 5, 6) and (4, 6, 7).
    for (int i = half_vertices_length + 1; i < vertices_length - 1; ++i) {
        _triangles->push_back(half_vertices_length);
        _triangles->push_back(i);
        _triangles->push_back(i + 1);
    }

    // Generate triangle (0, 1, 4), (4, 1, 5), (5, 1, 2), (5, 2, 6),
    // (6, 2, 3), (6, 3, 7), (7, 3, 0), (7, 0, 4)
    for (int i = 0; i < half_vertices_length; ++i) {
        _triangles->push_back(i);
        _triangles->push_back((i + 1) % half_vertices_length);
        _triangles->push_back(i + half_vertices_length);

        _triangles->push_back(i + half_vertices_length);
        _triangles->push_back((i + 1) % half_vertices_length);
        _triangles->push_back((i + half_vertices_length + 1) % half_vertices_length +
                             half_vertices_length);
    }
}

osg::ref_ptr<osg::Node> osg_planeRenderer::createNode(AAssetManager *manager) {
    _node = new osg::Geode();
    _geometry = new osg::Geometry();
    _vertices = new osg::Vec3Array();
    _triangles = new DrawElementsUShort(PrimitiveSet::TRIANGLES);
    Texture2D * _planeTexture = new osg::Texture2D();

    _geometry->addPrimitiveSet(_triangles);
    _node->addDrawable(_geometry.get());

    _planeTexture->setFilter(osg::Texture::FilterParameter::MIN_FILTER, osg::Texture::FilterMode::LINEAR_MIPMAP_LINEAR);
    _planeTexture->setFilter(osg::Texture::FilterParameter::MAG_FILTER, osg::Texture::FilterMode::LINEAR);
    _planeTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    _planeTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

    _planeTexture->setDataVariance(osg::Object::DYNAMIC);

    if(!utils::LoadPngFromAssetManager(GL_TEXTURE_2D, "textures/trigrid.png"))
        LOGE("Failed to load png image");
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    _uniform_tex_sampler = new osg::Uniform(osg::Uniform::SAMPLER_2D, "uTexture");
    _uniform_mvp_mat = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uMVP");
    _uniform_model_mat = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uModel");
    _uniform_normal_vec = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "uNormal");
    _uniform_color = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "uColor");

    _uniform_tex_sampler->set(0);

    osg::StateSet * stateset = new osg::StateSet;
    stateset->addUniform(_uniform_tex_sampler);
    stateset->addUniform(_uniform_mvp_mat);
    stateset->addUniform(_uniform_model_mat);
    stateset->addUniform(_uniform_normal_vec);
    stateset->addUniform(_uniform_color);

    stateset->setTextureAttributeAndModes(0, _planeTexture, osg::StateAttribute::ON);

    _node->setStateSet(stateset);
    _node->getOrCreateStateSet()->setAttribute(
            osg_utils::createShaderProgram("shaders/plane.vert", "shaders/plane.frag", manager));
    return _node.get();
}
void osg_planeRenderer::Draw(const ArSession *arSession, const ArPlane *arPlane,
                             const mat4 &projMat, const mat4 &viewMat, const vec3 &color) {
    _update_plane_vertices(arSession, arPlane);
    _geometry->setVertexArray(_vertices);

    _uniform_model_mat->set(Matrixd(value_ptr(projMat)));
    _uniform_mvp_mat->set(Matrixd(value_ptr(projMat * viewMat * _model_mat)));
    _uniform_normal_vec->set(Vec3(_normal_vec.x, _normal_vec.y, _normal_vec.z));
    _uniform_color->set(Vec3(color.x, color.y, color.z));
}