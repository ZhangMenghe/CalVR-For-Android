//
// Created by menghe on 6/28/2018.
//

#ifndef MYGLES_UTILS_H
#define MYGLES_UTILS_H

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <android/asset_manager.h>
#include <android/log.h>
#include <errno.h>
#include <jni.h>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include <glm.hpp>
#include <string>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtx/quaternion.hpp"
#include "arcore_c_api.h"


#include <cvrUtil/AndroidStdio.h>

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
                                                    = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

namespace utils {
    typedef struct glState_S {
        GLboolean depthTest, blend, cullFace;
        GLboolean dither, colorLogicOp, polygonOffsetLine, polygonOffsetFill;
        GLboolean polygonOffsetPoint, polygonSmooth, scissorTest, stencilTest;
    } glState;

    // Check GL error, and abort if an error is encountered.
    //
    // @param operation, the name of the GL function call.
    void CheckGlError(const char* operation);

    GLuint CreateProgram(const char* pVertexSource,
                         const char* pFragmentSource);
    // Create a shader program ID.
    //
    // @param asset_manager, AAssetManager pointer.
    // @param vertex_shader_file_name, the vertex shader source file.
    // @param fragment_shader_file_name, the fragment shader source file.
    // @return a non-zero value if the shader is created successfully, otherwise 0.
    GLuint CreateProgram(const char* vertex_shader_file_name,
                         const char* fragment_shader_file_name,
                         AAssetManager* asset_manager);

    // Load a text file from assets folder.
    //
    // @param asset_manager, AAssetManager pointer.
    // @param file_name, path to the file, relative to the assets folder.
    // @param out_string, output string.
    // @return true if the file is loaded correctly, otherwise false.
        bool LoadTextFileFromAssetManager(const char* file_name,
                                      AAssetManager* asset_manager,
                                      std::string* out_file_text_string);

    // Load png file from assets folder and then assign it to the OpenGL target.
    // This method must be called from the renderer thread since it will result in
    // OpenGL calls to assign the image to the texture target.
    //
    // @param target, openGL texture target to load the image into.
    // @param path, path to the file, relative to the assets folder.
    // @return true if png is loaded correctly, otherwise false.
    bool LoadPngFromAssetManager(int target, const char* path,const char* className);
    bool LoadPngFromAssetManager(int target, const char* path);
    // Load obj file from assets folder from the app.
    //
    // @param asset_manager, AAssetManager pointer.
    // @param file_name, name of the obj file.
    // @param out_vertices, output vertices.
    // @param out_normals, output normals.
    // @param out_uv, output texture UV coordinates.
    // @param out_indices, output triangle indices.
    // @return true if obj is loaded correctly, otherwise false.
    bool LoadObjFile( AAssetManager* asset_manager,const char* file_name,
                     std::vector<GLfloat>* out_vertices,
                     std::vector<GLfloat>* out_normals,
                     std::vector<GLfloat>* out_uv,
                     std::vector<GLushort>* out_indices);

    void sendTextToJava(std::string str);

};


#endif //MYGLES_UTILS_H
