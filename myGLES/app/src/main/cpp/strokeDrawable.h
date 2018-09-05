//
// Created by menghe on 7/28/2018.
//

#ifndef MYGLES_STROKEDRAWABLE_H
#define MYGLES_STROKEDRAWABLE_H

#include <osg/Drawable>
#include "utils.h"
#include "glDrawable.h"

class strokeDrawable: public glDrawable {
private:
    GLuint _VAO;
    GLuint _VBO[2];
    GLuint _shader_program;

    GLuint _attrib_vertices;
    GLuint _attrib_offsets;
    GLuint _uniform_arMVP_mat;

    glm::vec4 _default_color= glm::vec4(1.0, 0.5, 0.0, 1.0);
    float _default_size = 30.0f;
    float _default_line_width = 10.0f;
    glm::mat4 _ar_mvp = glm::mat4();

    float strokeData[8];
    float offsetData[4] = {.0};

public:
    void setStrokePoints(const float *start_pos, const float* end_pos, const float * startOffset){
        for(int i=0;i<3;i++){
            strokeData[i] = start_pos[i];
            strokeData[4 + i] = end_pos[i];
        }
        strokeData[3] = 1.0; strokeData[7] = 1.0f;

        offsetData[0] = startOffset[0];
        offsetData[1] = startOffset[1];

        glBindBuffer(GL_ARRAY_BUFFER, _VBO[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(float), strokeData);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, _VBO[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(float), offsetData);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void updateARMatrix(glm::mat4 mvp){
        _ar_mvp = mvp;
    }

    void Initialization(AAssetManager * manager, std::stack<utils::glState>* stateStack);

    void drawImplementation(osg::RenderInfo&) const;
};

#endif
