#include "freetypeDrawable.h"
#include <include/ft2build.h>
#include <include/freetype/freetype.h>
freetypeDrawable::freetypeDrawable(const char* font):_fontFile(font) {}


void freetypeDrawable::Initialization(AAssetManager *manager,
                                      std::stack<utils::glState> *stateStack) {
    glDrawable::Initialization(manager, stateStack);
    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        LOGE("Freetype library load fail");

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, _fontFile, 0, &face))
        LOGE("Freetype Face creation fail");

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            LOGE("FAIL TO LOAD GLYPH");
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_ALPHA,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0,
                     GL_ALPHA,
                     GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                glm::ivec2(face->glyph->advance.x, face->glyph->advance.y)
        };
        _characters[c] = character;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

osg::ref_ptr<osg::Group> freetypeDrawable::createDrawableNode(AAssetManager *manager,std::stack<utils::glState>* stateStack){
    Initialization(manager, stateStack);
    _string_root = new osg::Group;
    setUseDisplayList(false);
    return _string_root;
}

void freetypeDrawable::addTargetString(std::string text, float x, float y, float scale) {
    osg::ref_ptr<osg::Group> single_str = new osg::Group;
    for ( std::string::iterator it=text.begin(); it!=text.end(); ++it){
        Character ch = _characters[*it];
        characterDrawable * chaDrawable = new characterDrawable();
        single_str->addChild(chaDrawable->createDrawableNode(_asset_manager, _stateStack));
        chaDrawable->setTargetCharacter(ch, x, y, scale);
        x += (ch.Advance.x>> 6) * scale;
//        y += (ch.Advance.y >> 6) * scale;
    }
    _string_root->addChild(single_str);
}

void freetypeDrawable::drawImplementation(osg::RenderInfo&) const{}