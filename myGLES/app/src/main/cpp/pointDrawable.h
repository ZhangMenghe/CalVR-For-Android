//
// Created by menghe on 7/28/2018.
//

#ifndef MYGLES_POINTDRAWABLE_H
#define MYGLES_POINTDRAWABLE_H

#include <osg/Drawable>
#include "utils.h"
#include "glDrawable.h"
#define MAX_POINTS 100
class pointDrawable: public glDrawable {
private:
    GLuint _VAO;
    GLuint _VBO;
    GLuint _shader_program;

    GLuint _attrib_vertices;
    GLuint _uniform_arMVP_mat;

    glm::vec4 _default_color= glm::vec4(1.0, 0.5, 0.0, 1.0);
    float _default_size = 20.0f;
    glm::mat4 _ar_mvp = glm::mat4();

    int _point_num = 0;
    float *pointCloudData;

public:
    void updateVertices(const float *newData, int num_of_points){
        _point_num = num_of_points;
        size_t mem_size =  _point_num * 4 * sizeof(float);
        pointCloudData = (float*)malloc(mem_size);
        memcpy(pointCloudData, newData, mem_size);

        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, mem_size, pointCloudData);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void updateARMatrix(glm::mat4 mvp){
        _ar_mvp = mvp;
    }
    void Initialization(AAssetManager * manager,std::stack<utils::glState>* stateStack);
    void drawImplementation(osg::RenderInfo&) const;
};

#endif //MYGLES_POINTDRAWABLE_H
