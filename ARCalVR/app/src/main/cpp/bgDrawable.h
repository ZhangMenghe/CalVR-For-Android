//
// Created by menghe on 8/1/2018.
//

#ifndef MYGLES_BGDRAWABLE_H
#define MYGLES_BGDRAWABLE_H

#include <osg/BlendFunc>
#include <osg/MatrixTransform>
#include <cvrUtil/glesDrawable.h>

class bgDrawable: public cvr::glesDrawable {
private:
    GLuint _shader_program;
    GLuint _texture_id;

    GLuint _attrib_vertices;
    GLuint _attrib_uvs;

    GLuint _VAO;
    GLuint _VBO[2];

    const GLfloat _vertices[12] = {
            -1.0f, -1.0f, 0.0f, +1.0f, -1.0f, 0.0f,
            -1.0f, +1.0f, 0.0f, +1.0f, +1.0f, 0.0f,
    };
public:
    void Initialization(std::stack<cvr::glState>* stateStack);
    void updateOnFrame(const float * new_uvs);
    void drawImplementation(osg::RenderInfo&) const;
    // Returns the generated texture name for the GL_TEXTURE_EXTERNAL_OES target.
    GLuint GetTextureId(){return _texture_id;}
    osg::ref_ptr<osg::Geode> createDrawableNode(std::stack<cvr::glState>* stateStack){
        glNode = cvr::glesDrawable::createDrawableNode(stateStack);
        glNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
        glNode->getOrCreateStateSet()->setRenderBinDetails(1,"RenderBin");
        return glNode.get();
    }
};


#endif //MYGLES_BGDRAWABLE_H
