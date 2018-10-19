//
// Created by menghe on 8/1/2018.
//

#include "planeDrawable.h"
#include "arcore_utils.h"

using namespace arcore_utils;
using namespace glm;
void planeDrawable::_update_plane_vertices(const ArSession *arSession, const ArPlane*arPlane) {
    _vertices.clear();
    _triangles.clear();

    int32_t polygon_length;
    //get the number of elements(2*#vertives)
    ArPlane_getPolygonSize(arSession, arPlane, &polygon_length);

    if(polygon_length == 0){
        LOGE("NO valid plane polygon found");
        return;
    }
    const int32_t vertices_size = polygon_length/2;
    std::vector<vec2> raw_vertices(vertices_size);
    ArPlane_getPolygon(arSession, arPlane, value_ptr(raw_vertices.front()));

    //fill outter vertices
    for(int32_t i=0; i<vertices_size; i++)
        _vertices.push_back(vec3(raw_vertices[i].x, raw_vertices[i].y, .0f));

    //get model matrix
    ArPose * arPose;
    ArPose_create(arSession, nullptr,&arPose);
    ArPlane_getCenterPose(arSession, arPlane, arPose);
    ArPose_getMatrix(arSession, arPose, value_ptr(_model_mat));

    float raw_center_pose[7] = {.0f};
    ArPose_getPoseRaw(arSession, arPose, raw_center_pose);
    _planeCenter = glm::vec3(raw_center_pose[4], raw_center_pose[5], raw_center_pose[6]);

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

        _vertices.push_back(vec3(result_v.x, result_v.y, 1.0f));
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

void planeDrawable::Initialization(AAssetManager * manager,std::stack<utils::glState>* stateStack){
    glDrawable::Initialization(manager, stateStack);
    _shader_program = utils::CreateProgram("shaders/plane.vert", "shaders/plane.frag", manager);
    if(!_shader_program)
        LOGE("Failed to create shader program");

    //Init attribs & uniforms
    _attrib_vertices = glGetAttribLocation(_shader_program, "vPosition");
    _uniform_model_mat = glGetUniformLocation(_shader_program, "uModel");
    _uniform_normal_vec = glGetUniformLocation(_shader_program, "uNormal");
    _uniform_mvp_mat = glGetUniformLocation(_shader_program, "uMVP");
    _uniform_tex_sampler = glGetUniformLocation(_shader_program, "uTexture");
    _uniform_color = glGetUniformLocation(_shader_program, "uColor");

    //Init texture
    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(!utils::LoadPngFromAssetManager(GL_TEXTURE_2D, "textures/trigrid.png"))
        LOGE("Failed to load png image");
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Generate VAO and bind
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    //Generate VBO and bind
    glGenBuffers(1, &_VBO);

    //dynamic feed data
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * MAX_PLANE_VERTICES * 3, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(_attrib_vertices);
    glVertexAttribPointer(_attrib_vertices, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Generate EBO
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * MAX_PLANE_VERTICES*5, nullptr, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void planeDrawable::updateOnFrame(const ArSession *arSession, const ArPlane *arPlane,
                                  const glm::mat4 &projMat, const glm::mat4 &viewMat,
                                  const glm::vec3 &color) {
    _projMat = projMat;
    _viewMat = viewMat;

    _update_plane_vertices(arSession, arPlane);

    LOGE("DRAW PLANE: %d", _vertices.size());

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*_vertices.size()*3, _vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * _triangles.size(), _triangles.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glUseProgram(_shader_program);
    glUniform3f(_uniform_color, color.x, color.y, color.z);
    glUseProgram(0);
}

void planeDrawable::drawImplementation(osg::RenderInfo&) const{
    PushAllState();

//    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//    glEnable(GL_CULL_FACE);
//    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(_shader_program);
    glDepthMask(GL_FALSE);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(_uniform_tex_sampler, 0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);

    glUniformMatrix4fv(_uniform_mvp_mat, 1, GL_FALSE, glm::value_ptr(_projMat * _viewMat * _model_mat));
    glUniform3f(_uniform_normal_vec, _normal_vec.x, _normal_vec.y, _normal_vec.z);
    glUniformMatrix4fv(_uniform_model_mat, 1, GL_FALSE, glm::value_ptr(_model_mat));

    glBindVertexArray(_VAO);

    glDrawElements(GL_TRIANGLES, _triangles.size(), GL_UNSIGNED_SHORT, nullptr);
    glBindVertexArray(0);

    glUseProgram(0);
    checkGlError("Draw point cloud");
    glDepthMask(GL_TRUE);
    PopAllState();
}
//const float * const planeDrawable::getPlaneData(int & numOfVertices) const{
//    numOfVertices =  _vertices.size();
//    int memsize= numOfVertices* sizeof(glm::vec3);
//    float* data = (float*)malloc(memsize);
//    std::memcpy(data, _vertices.data(), memsize);
//    const float * ptr = &data[0];
//    return ptr;
//}