//
// Created by menghe on 8/1/2018.
//

#ifndef MYGLES_BGDRAWABLE_H
#define MYGLES_BGDRAWABLE_H

#include "glDrawable.h"

class bgDrawable: public glDrawable {
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
    // UVs of the quad vertices (S, T)
    const GLfloat _uvs[8] = {
            0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };
public:
    void Initialization(AAssetManager * manager,std::stack<utils::glState>* stateStack);
    void updateOnFrame(float * new_uvs);
    void drawImplementation(osg::RenderInfo&) const;
    // Returns the generated texture name for the GL_TEXTURE_EXTERNAL_OES target.
    GLuint GetTextureId(){return _texture_id;}
    osg::ref_ptr<osg::Geode> createDrawableNode(AAssetManager *manager,std::stack<utils::glState>* stateStack){
        osg::ref_ptr<osg::Geode> glNode = glDrawable::createDrawableNode(manager, stateStack);
        osg::StateSet * stateset = glNode->getOrCreateStateSet();
        stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
        stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        return glNode.get();
    }
};


#endif //MYGLES_BGDRAWABLE_H
