#ifndef MYGLES_FREETYPEDRAWABLE_H
#define MYGLES_FREETYPEDRAWABLE_H

#include <osg/Drawable>
#include <include/freetype/tttags.h>
#include "glDrawable.h"
#include "utils.h"

struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};
class freetypeDrawable: public glDrawable{
private:
    GLuint _VAO;
    GLuint _VBO[2];
    GLuint _TextureID;
    GLuint _shader_program;
    const char* _fontFile;
    GLuint _attribute_vertex;
    GLuint _attribute_uv;
    GLuint _uniform_tex_sampler;
    FT_Face face;
    int _pointNum = 4;
//    const GLfloat _uvs[12] = {
//            0.0, 0.0,
//            0.0, 1.0,
//            1.0, 1.0,
//            0.0, 0.0,
//            1.0, 1.0,
//            1.0, 0.0
//    };
    const GLfloat _uvs[8] = {
            0.0, 0.0,
            1.0,0.0,
            0.0,1.0,
            1.1,1.1
    };
    std::map<GLchar, Character> Characters;
public:
    freetypeDrawable(const char* font);
    void setTargetString(std::string text,float x, float y, float scale = 1);
    void Initialization(AAssetManager * manager,std::stack<utils::glState>* stateStack);
    void drawImplementation(osg::RenderInfo&) const;
};
#endif//MYGLES_FREETYPEDRAWABLE_H