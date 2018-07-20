//
// Created by menghe on 7/18/2018.
//

#include "osg_objectRenderer.h"
#include <osg/Texture2D>
using namespace osg;

ref_ptr<Geode> osg_objectRenderer::createNode(AAssetManager * manager, const char* obj_file_name, const char* png_file_name){
//    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
//    shape->setShape(new osg::Sphere(osg::Vec3(.0f,.0f,.0f), 1.0f));
//    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
//    osg::ref_ptr<osg::Geode> node = new osg::Geode;
//    node->addDrawable(shape.get());
//    return node.get();

    _geometry = new osg::Geometry();
    _node = new osg::Geode;
    _node->addDrawable(_geometry.get());

    ref_ptr<Vec3Array> vertices = new Vec3Array();
    ref_ptr<Vec3Array> normals = new Vec3Array();

    ref_ptr<Vec2Array> uvs = new Vec2Array();

    std::vector<GLfloat> _vertices;
    std::vector<GLfloat > _uvs;
    std::vector<GLfloat > _normals;
    std::vector<GLushort > _indices;

    utils::LoadObjFile(manager, obj_file_name, &_vertices, &_normals, &_uvs, &_indices);


    for(int i=0; i<_uvs.size()/2; i++){
        vertices->push_back(Vec3f(_vertices[3*i], _vertices[3*i+1], _vertices[3*i+2]));
        normals->push_back(Vec3f(_normals[3*i], _normals[3*i+1], _normals[3*i+2]));
        uvs->push_back(Vec2f(_uvs[2*i], _uvs[2*i+1]));
    }

    _geometry->setVertexArray(vertices.get());
    _geometry->setNormalArray(normals.get());
    _geometry->setTexCoordArray(0, uvs.get());
    _geometry->addPrimitiveSet(new DrawElementsUShort(GL_TRIANGLES, _indices.size(), _indices.data()));
    _geometry->setUseVertexBufferObjects(true);
    _geometry->setUseDisplayList(false);

    Texture2D* objTexture = new Texture2D();
    objTexture->setFilter(osg::Texture::FilterParameter::MIN_FILTER, osg::Texture::FilterMode::LINEAR_MIPMAP_LINEAR);
    objTexture->setFilter(osg::Texture::FilterParameter::MAG_FILTER, osg::Texture::FilterMode::LINEAR);
    objTexture->setWrap(Texture::WRAP_T, Texture::REPEAT);
    objTexture->setWrap(Texture::WRAP_S, Texture::REPEAT);

    if (!utils::LoadPngFromAssetManager(GL_TEXTURE_2D, png_file_name)) {
        LOGE("Could not load png texture for planes.");
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    osg::Uniform* samUniform =new osg::Uniform(osg::Uniform::SAMPLER_2D, "uTexture");
    osg::Uniform * uniform_material = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "uMaterialParams");
    samUniform->set(0);
    uniform_material->set(Vec4f(_shader_params.ambient, _shader_params.diffuse,
                                _shader_params.specular, _shader_params.specular_alpha));

    _uniform_mvp = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uMVP");
    _uniform_mv = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uMV");

    _uniform_light = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "uLightingParams");
    _uniform_color_correct = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "uColorCorrection");

    osg::StateSet* stateset = new osg::StateSet;
    stateset->addUniform(samUniform);
    stateset->addUniform(uniform_material);
    stateset->addUniform(_uniform_mvp);
    stateset->addUniform(_uniform_mv);
    stateset->addUniform(_uniform_light);
    stateset->addUniform(_uniform_color_correct);

    stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    stateset->setTextureAttributeAndModes(0, objTexture, osg::StateAttribute::ON);
    Program * program = osg_utils::createShaderProgram("shaders/osgObject.vert", "shaders/osgObject.frag", manager);

    GLuint _attribute_vNormal = 1;
    program->addBindAttribLocation("vNormal", 1);

    _geometry->setVertexAttribArray(_attribute_vNormal, normals.get(), osg::Array::BIND_PER_VERTEX);

    stateset->setAttribute(program);

    _geometry->setStateSet(stateset);

    return _node.get();
}

void osg_objectRenderer::Draw(const glm::mat4 &projMat,
                                                  const glm::mat4 &viewMat,
                                                  const glm::mat4 &modelMat,
                                                  const float *color_correction,
                                                  float light_intensity) {
    glm::mat4 mv = viewMat * modelMat;
    glm::vec4 v_light = glm::normalize(mv * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

    _uniform_mv->set(*(new RefMatrixf(glm::value_ptr(mv))));
    _uniform_mvp->set(*(new RefMatrixf(glm::value_ptr(projMat* mv))));
    _uniform_light->set(Vec4f(v_light[0], v_light[1], v_light[2], light_intensity));
    _uniform_color_correct->set(Vec4f(color_correction[0], color_correction[1], color_correction[2], color_correction[3]));

}