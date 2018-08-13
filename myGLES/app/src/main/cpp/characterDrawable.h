#ifndef MYGLES_CHARACTERDRAWABLE_H
#define MYGLES_CHARACTERDRAWABLE_H

#include <osg/Drawable>
#include <include/freetype/tttags.h>
#include "glDrawable.h"
#include "utils.h"

struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    glm::ivec2 Advance;    // Horizontal offset to advance to next glyph
};
class characterDrawable: public glDrawable{
private:
    GLuint _VAO;
    GLuint _VBO[2];
    GLuint _TextureID;
    GLuint _shader_program;
    GLuint _attribute_vertex;
    GLuint _attribute_uv;

    int _pointNum = 4;
    const GLfloat _uvs[8] = {
            0.0, 0.0,
            1.0,0.0,
            0.0,1.0,
            1.1,1.1
    };
public:
    void setTargetCharacter(Character ch, float x, float y, float scale);
    void Initialization(AAssetManager * manager,std::stack<utils::glState>* stateStack);
    void drawImplementation(osg::RenderInfo&) const;
};
#endif//MYGLES_FREETYPEDRAWABLE_H