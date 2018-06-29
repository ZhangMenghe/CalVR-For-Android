//
// Created by menghe on 6/28/2018.
//

#include "utils.h"
#include <jni.h>
#include <string>
#include <GLES2/gl2.h>

using namespace std;
namespace utils{
    void CheckGlError(const char* operation) {
        bool anyError = false;
        for (GLint error = glGetError(); error; error = glGetError()) {
            LOGE("after %s() glError (0x%x)\n", operation, error);
            anyError = true;
        }
        if (anyError) {
            abort();
        }
    }
    GLuint LoadShader(GLenum shaderType, const char* pSource) {
        GLuint shader = glCreateShader(shaderType);
        if (shader) {
            glShaderSource(shader, 1, &pSource, NULL);
            glCompileShader(shader);
            GLint compiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled) {
                GLint infoLen = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
                if (infoLen) {
                    char* buf = (char*) malloc(infoLen);
                    if (buf) {
                        glGetShaderInfoLog(shader, infoLen, NULL, buf);
                        free(buf);
                    }
                    glDeleteShader(shader);
                    shader = 0;
                }
            }
        }
        return shader;
    }

    GLuint CreateProgram(const char* pVertexSource, const char* pFragmentSource) {
        GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, pVertexSource);
        if (!vertexShader) {
            return 0;
        }

        GLuint pixelShader = LoadShader(GL_FRAGMENT_SHADER, pFragmentSource);
        if (!pixelShader) {
            return 0;
        }

        GLuint program = glCreateProgram();
        if (program) {
            glAttachShader(program, vertexShader);

            glAttachShader(program, pixelShader);

            glLinkProgram(program);
            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
            if (linkStatus != GL_TRUE) {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                if (bufLength) {
                    char* buf = (char*) malloc(bufLength);
                    if (buf) {
                        glGetProgramInfoLog(program, bufLength, NULL, buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program);
                program = 0;
            }
        }
        return program;
    }

    GLuint CreateProgram(const char* vertex_shader_file_name,
                         const char* fragment_shader_file_name,
                         AAssetManager* asset_manager) {
        std::string VertexShaderContent;
        if (!LoadTextFileFromAssetManager(vertex_shader_file_name, asset_manager,
                                          &VertexShaderContent)) {
            LOGE("Failed to load file: %s", vertex_shader_file_name);
            return 0;
        }

        std::string FragmentShaderContent;
        if (!LoadTextFileFromAssetManager(fragment_shader_file_name, asset_manager,
                                          &FragmentShaderContent)) {
            LOGE("Failed to load file: %s", fragment_shader_file_name);
            return 0;
        }

        GLuint vertexShader =
                LoadShader(GL_VERTEX_SHADER, VertexShaderContent.c_str());
        if (!vertexShader) {
            return 0;
        }

        GLuint fragment_shader =
                LoadShader(GL_FRAGMENT_SHADER, FragmentShaderContent.c_str());
        if (!fragment_shader) {
            return 0;
        }

        GLuint program = glCreateProgram();
        if (program) {
            glAttachShader(program, vertexShader);
            CheckGlError("hello_ar::util::glAttachShader");
            glAttachShader(program, fragment_shader);
            CheckGlError("hello_ar::util::glAttachShader");
            glLinkProgram(program);
            GLint link_status = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &link_status);
            if (link_status != GL_TRUE) {
                GLint buf_length = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buf_length);
                if (buf_length) {
                    char* buf = reinterpret_cast<char*>(malloc(buf_length));
                    if (buf) {
                        glGetProgramInfoLog(program, buf_length, nullptr, buf);
                        LOGE("hello_ar::util::Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program);
                program = 0;
            }
        }
        return program;
    }

    bool LoadTextFileFromAssetManager(const char* file_name,
                                      AAssetManager* asset_manager,
                                      std::string* out_file_text_string) {
        // If the file hasn't been uncompressed, load it to the internal storage.
        // Note that AAsset_openFileDescriptor doesn't support compressed
        // files (.obj).
        AAsset* asset =
                AAssetManager_open(asset_manager, file_name, AASSET_MODE_STREAMING);
        if (asset == nullptr) {
            LOGE("Error opening asset %s", file_name);
            return false;
        }

        off_t file_size = AAsset_getLength(asset);
        out_file_text_string->resize(file_size);
        int ret = AAsset_read(asset, &out_file_text_string->front(), file_size);

        if (ret <= 0) {
            LOGE("Failed to open file: %s", file_name);
            AAsset_close(asset);
            return false;
        }

        AAsset_close(asset);
        return true;
    }
}
