#include "freetypeDrawable.h"
#include <include/ft2build.h>
#include <include/freetype/freetype.h>
freetypeDrawable::freetypeDrawable(const char* font):_fontFile(font) {}


void freetypeDrawable::Initialization(AAssetManager *manager,
                                      std::stack<utils::glState> *stateStack) {
    glDrawable::Initialization(manager, stateStack);
    _shader_program = utils::CreateProgram("shaders/text.vert", "shaders/text.frag", manager);
    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        LOGE("Freetype library load fail");

    // Load font as face

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
                (GLuint)face->glyph->advance.x
        };
        Characters[c] = character;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    _attribute_vertex = glGetAttribLocation(_shader_program,"vPosition");
    _attribute_uv = glGetAttribLocation(_shader_program,"vTexcoord");

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    glGenBuffers(2, _VBO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _pointNum * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(_attribute_vertex);
    glVertexAttribPointer(_attribute_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _pointNum * 2, _uvs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(_attribute_uv);
    glVertexAttribPointer(_attribute_uv, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint proj_mat =  glGetUniformLocation(_shader_program, "uProj");
    GLuint text_color = glGetUniformLocation(_shader_program, "texColor");

    glUseProgram(_shader_program);
    glUniformMatrix4fv(proj_mat, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
    glUniform3f(text_color, 1.0, .0, .0);
    glUniform1i(glGetUniformLocation(_shader_program, "uTexture"), 0);
    glUseProgram(0);
}

void freetypeDrawable::setTargetString(std::string text, float x, float y, float scale) {
    //delete above to recover characters as pre-load
    Character ch = Characters[text[0]];
    _TextureID = ch.TextureID;
    GLfloat xpos = x + ch.Bearing.x * scale;
    GLfloat ypos = -y-ch.Bearing.y * scale;//y - (ch.Size.y - ch.Bearing.y)*scale;
    GLfloat w = ch.Size.x * scale;
    GLfloat h = ch.Size.y * scale;
    const GLfloat vertices[] = {
            xpos,     -ypos,
            xpos + w, -ypos,
            xpos,     -ypos - h,
            xpos + w, -ypos - h
    };
    glBindBuffer(GL_ARRAY_BUFFER, _VBO[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) *_pointNum * 2, vertices); // Be sure to use glBufferSubData and not glBufferData
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void freetypeDrawable::drawImplementation(osg::RenderInfo&) const{
    PushAllState();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(_shader_program);

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(_VAO);
    glBindTexture(GL_TEXTURE_2D, _TextureID);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, _pointNum);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D,0);

    glUseProgram(0);
    checkGlError("Draw point cloud");
    PopAllState();
}