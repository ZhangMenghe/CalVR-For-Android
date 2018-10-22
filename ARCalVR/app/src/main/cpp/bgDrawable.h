#ifndef BACKGROUND_DRAWABLE_H
#define BACKGROUND_DRAWABLE_H

#include <osg/BlendFunc>
#include <osg/MatrixTransform>
#include <cvrUtil/glesDrawable.h>
namespace cvr {
    class bgDrawable : public cvr::glesDrawable {
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
        void Initialization();
    public:
        bgDrawable(std::stack<cvr::glState> *stateStack);

        osg::ref_ptr<osg::Geode> createDrawableNode();

        // Returns the generated texture name for the GL_TEXTURE_EXTERNAL_OES target.
        GLuint GetTextureId() { return _texture_id; }

        void updateOnFrame(float *new_uvs);

        void drawImplementation(osg::RenderInfo &) const;




    };
}

#endif
