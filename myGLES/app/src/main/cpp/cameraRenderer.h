//
// Created by menghe on 7/4/2018.
//

#ifndef MYGLES_CAMERARENDERER_H
#define MYGLES_CAMERARENDERER_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>

#include <memory>
#include <vector>

#include "utils.h"

#include "arcore_c_api.h"


using namespace glm;
using namespace std;

class cameraRenderer {
private:
    GLuint _shader_program;
    GLuint _texture_id;
    GLuint _texture_overlay_id;

    GLuint _attrib_vertices;
    GLuint _attrib_uvs;
    GLuint _uniform_status_normal;
//    GLuint _uniform_texture;

    float _transformed_uvs[8];
    std::unique_ptr<uint8_t[]> processed_image_bytes_grayscale_;
    int cpu_image_buffer_size_ = 0;

//    bool _uvs_initialized = false;

//    void _update_texture_coords(int32_t image_width, int32_t image_height,
//                                float screen_aspect_ratio,
//                                int display_rotation);
public:
    void Initialization(AAssetManager * manager);
    void Draw(ArSession* session, ArFrame* frame, bool btn_status_normal = true);
    // Returns the generated texture name for the GL_TEXTURE_EXTERNAL_OES target.
    GLuint GetTextureId(){return _texture_id;}
};


#endif //MYGLES_CAMERARENDERER_H
