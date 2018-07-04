//
// Created by menghe on 7/4/2018.
//

#ifndef MYGLES_CAMERARENDERER_H
#define MYGLES_CAMERARENDERER_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>

#include "utils.h"

#include <arcore_c_api.h>
#include <vector>

using namespace glm;
using namespace std;

class cameraRenderer {
private:
    GLuint _shader_program;
    GLuint _texture_id;

    GLuint _attrib_vertices;
    GLuint _attrib_uvs;
    GLuint _uniform_texture;

    float _transformed_uvs[8];
    bool _uvs_initialized = false;
public:
    void Initialization(AAssetManager * manager);
    void Draw(const ArSession* session, const ArFrame* frame);
    // Returns the generated texture name for the GL_TEXTURE_EXTERNAL_OES target.
    GLuint GetTextureId(){return _texture_id;}
};


#endif //MYGLES_CAMERARENDERER_H
