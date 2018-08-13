#ifndef MYGLES_FREETYPEDRAWABLE_H
#define MYGLES_FREETYPEDRAWABLE_H

#include <osg/Drawable>
#include <include/freetype/tttags.h>
#include "characterDrawable.h"
#include "glDrawable.h"
#include "utils.h"


class freetypeDrawable: public glDrawable{
private:
    GLuint _shader_program;
    const char* _fontFile;
    osg::ref_ptr<osg::Group> _string_root;
    std::map<GLchar, Character> _characters;

public:
    freetypeDrawable(const char* font);
    osg::ref_ptr<osg::Group> createDrawableNode(AAssetManager *manager,std::stack<utils::glState>* stateStack);
    void addTargetString(std::string text,float x, float y, float scale = 1);
    void Initialization(AAssetManager * manager,std::stack<utils::glState>* stateStack);
    void drawImplementation(osg::RenderInfo&) const;
};
#endif//MYGLES_FREETYPEDRAWABLE_H