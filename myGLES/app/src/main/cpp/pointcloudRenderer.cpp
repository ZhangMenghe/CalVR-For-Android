//
// Created by menghe on 7/5/2018.
//

#include "pointcloudRenderer.h"
#include "arcore_utils.h"

void pointcloudRenderer::Initialization(AAssetManager *manager) {
    _shader_program = utils::CreateProgram("shaders/point.vert", "shaders/point.frag", manager);
    CHECK(_shader_program);
    _attrib_vertices_ = glGetAttribLocation(_shader_program,"vPosition");
    _uniform_mvp_mat = glGetUniformLocation(_shader_program, "uMVP");
    _default_color = glm::vec4(1.0, 0.5, 0.0, 1.0);
    glUseProgram(_shader_program);
    glUniform4fv(glGetUniformLocation(_shader_program, "uColor"), 1, value_ptr(_default_color));
    glUseProgram(0);
    checkGlError("Initialize point cloud renderer");
}
void pointcloudRenderer::Draw(const ArSession *arSession, ArPointCloud *pointCloud,
                              const mat4 &mvpMat) {
    glUseProgram(_shader_program);

    int32_t num_of_points = 0;
    ArPointCloud_getNumberOfPoints(arSession, pointCloud, &num_of_points);

    if(num_of_points <= 0)
        return;

    //point cloud data with 4 params (x,y,z, confidence)
    const float * pointCloudData;
    ArPointCloud_getData(arSession, pointCloud, &pointCloudData);

    glUniformMatrix4fv(_uniform_mvp_mat, 1, GL_FALSE, value_ptr(mvpMat));

    glEnableVertexAttribArray(_attrib_vertices_);
    glVertexAttribPointer(_attrib_vertices_, 4, GL_FLOAT, GL_FALSE, 0, pointCloudData);
    glDrawArrays(GL_POINTS, 0, num_of_points);
    glUseProgram(0);
    checkGlError("Draw point cloud");
}