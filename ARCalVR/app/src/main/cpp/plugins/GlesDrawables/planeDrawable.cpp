#include <cvrUtil/AndroidHelper.h>
#include <cvrUtil/AndroidStdio.h>
#include "planeDrawable.h"
void planeDrawable::_update_plane_vertices() {
    _vertices.clear();
    _triangles.clear();

    for(int32_t i=0; i<_vertices_num; i++)
        _vertices.push_back(osg::Vec3f(raw_vertices[2*i], raw_vertices[2*i+1], .0f));

    // Feather distance 0.2 meters.
    const float kFeatherLength = 0.2f;
    // Feather scale over the distance between plane center and vertices.
    const float kFeatherScale = 0.2f;

    // Fill vertex 4 to 7, with alpha set to 1.
    for (int32_t i = 0; i < _vertices_num; ++i) {
        const float scale =
                1.0f - std::min((kFeatherLength /
                        sqrt(raw_vertices[2*i]*raw_vertices[2*i]
                                                       +raw_vertices[2*i+1]*raw_vertices[2*i+1] )),
                                kFeatherScale);

        _vertices.push_back(osg::Vec3f(raw_vertices[2*i] * scale,
                                       raw_vertices[2*i+1] * scale,
                                       1.0f));
    }

    const int32_t vertices_length = _vertices.size();
    const int32_t half_vertices_length = vertices_length / 2;

    // Generate triangle (4, 5, 6) and (4, 6, 7).
    for (int i = half_vertices_length + 1; i < vertices_length - 1; ++i) {
        _triangles.push_back(half_vertices_length);
        _triangles.push_back(i);
        _triangles.push_back(i + 1);
    }

    // Generate triangle (0, 1, 4), (4, 1, 5), (5, 1, 2), (5, 2, 6),
    // (6, 2, 3), (6, 3, 7), (7, 3, 0), (7, 0, 4)
    for (int i = 0; i < half_vertices_length; ++i) {
        _triangles.push_back(i);
        _triangles.push_back((i + 1) % half_vertices_length);
        _triangles.push_back(i + half_vertices_length);

        _triangles.push_back(i + half_vertices_length);
        _triangles.push_back((i + 1) % half_vertices_length);
        _triangles.push_back((i + half_vertices_length + 1) % half_vertices_length +
                             half_vertices_length);
    }
}

void planeDrawable::Initialization(){
    cvr::glesDrawable::Initialization();
    _shader_program = cvr::assetLoader::instance()->createGLShaderProgramFromFile("shaders/plane.vert", "shaders/plane.frag");

    if(!_shader_program)
        LOGE("Failed to create shader program");

    _attrib_vertices = glGetAttribLocation(_shader_program, "vPosition");
    _uniform_model_mat = glGetUniformLocation(_shader_program, "uModel");
    _uniform_normal_vec = glGetUniformLocation(_shader_program, "uNormal");
    _uniform_mvp_mat = glGetUniformLocation(_shader_program, "uMVP");
    _uniform_tex_sampler = glGetUniformLocation(_shader_program, "uTexture");
    _uniform_color = glGetUniformLocation(_shader_program, "uColor");


    std::string fhead(getenv("CALVR_RESOURCE_DIR"));
    osg::Image * img = osgDB::readImageFile(fhead+"textures/trigrid.png");
    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0,
                 img->getPixelFormat(),
                 img->s(),img->t(),0,
                 img->getPixelFormat(), img->getDataType(),
                 img->getDataPointer());


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void planeDrawable::updateOnFrame(ArPlane* plane, osg::Vec3f color) {
    if(cvr::ARCoreManager::instance()->getPlaneData(plane, raw_vertices,
                                                    _model_mat, _normal_vec,
                                                    _vertices_num))
        _update_plane_vertices();

    _view_proj_mat = cvr::ARCoreManager::instance()->getMVPMatrix();

    _update_plane_vertices();

    glUseProgram(_shader_program);
    glUniform3f(_uniform_color, color.x(), color.y(), color.z());
    glUseProgram(0);
}

void planeDrawable::drawImplementation(osg::RenderInfo&) const{
    cvr::glStateStack::instance()->PushAllState();

    glUseProgram(_shader_program);
    glDepthMask(GL_FALSE);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(_uniform_tex_sampler, 0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniformMatrix4fv(_uniform_mvp_mat, 1, GL_FALSE, _view_proj_mat.ptr());
    glUniform3f(_uniform_normal_vec, _normal_vec.x(), _normal_vec.y(), _normal_vec.z());
    glUniformMatrix4fv(_uniform_model_mat, 1, GL_FALSE, _model_mat.ptr());

    glEnableVertexAttribArray(_attrib_vertices);
    glVertexAttribPointer(_attrib_vertices, 3, GL_FLOAT, GL_FALSE, 0, _vertices.data());
    glDrawElements(GL_TRIANGLES, _triangles.size(), GL_UNSIGNED_SHORT,  _triangles.data());

    glUseProgram(0);
    glDepthMask(GL_TRUE);
    cvr::glStateStack::instance()->PopAllState();
}