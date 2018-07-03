//
// Created by menghe on 7/3/2018.
//

#ifndef MYGLES_BACKGROUNDRENDERER_H
#define MYGLES_BACKGROUNDRENDERER_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>

#include "utils.h"

#include <arcore_c_api.h>
#include <vector>

class backgroundRenderer {
public:
    // Sets up OpenGL state.  Must be called on the OpenGL thread and before any
    // other methods below.
    void InitializeGlContent(AAssetManager* asset_manager);

    // Draws the background image.  This methods must be called for every ArFrame
    // returned by ArSession_update() to catch display geometry change events.
    void Draw(const ArSession* session, const ArFrame* frame);

    // Returns the generated texture name for the GL_TEXTURE_EXTERNAL_OES target.
    GLuint GetTextureId() const;

private:
    static constexpr int kNumVertices = 4;

    GLuint shader_program_;
    GLuint texture_id_;

    GLuint attribute_vertices_;
    GLuint attribute_uvs_;
    GLuint uniform_texture_;

    float transformed_uvs_[kNumVertices * 2];
    bool uvs_initialized_ = false;
};


#endif //MYGLES_BACKGROUNDRENDERER_H
