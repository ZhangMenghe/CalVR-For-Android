//
// Created by menghe on 6/28/2018.
//

#include "utils.h"
#include <jni.h>
#include <string>
#include <sstream>
#include <GLES3/gl3.h>
#include "jni_interface.h"

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

        return CreateProgram(VertexShaderContent.c_str(), FragmentShaderContent.c_str());
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
    bool LoadPngFromAssetManager(int target, const char* path) {
        JNIEnv* env = GetJniEnv();
        //if()
        // Put all the JNI values in a structure that is statically initalized on the
        // first call to this method.  This makes it thread safe in the unlikely case
        // of multiple threads calling this method.
        static struct JNIData {
            jclass helper_class;
            jmethodID load_image_method;
            jmethodID load_texture_method;
        } jniIds = [env]() -> JNIData {
            constexpr char kHelperClassName[] =
                    "com/example/menghe/mygles/JniInterfaceOSG";
            constexpr char kLoadImageMethodName[] = "loadImage";
            constexpr char kLoadImageMethodSignature[] =
                    "(Ljava/lang/String;)Landroid/graphics/Bitmap;";
            constexpr char kLoadTextureMethodName[] = "loadTexture";
            constexpr char kLoadTextureMethodSignature[] =
                    "(ILandroid/graphics/Bitmap;)V";
            jclass helper_class = env->FindClass(kHelperClassName);
            if (helper_class) {
                helper_class = static_cast<jclass>(env->NewGlobalRef(helper_class));
                jmethodID load_image_method = env->GetStaticMethodID(
                        helper_class, kLoadImageMethodName, kLoadImageMethodSignature);
                jmethodID load_texture_method = env->GetStaticMethodID(
                        helper_class, kLoadTextureMethodName, kLoadTextureMethodSignature);
                return {helper_class, load_image_method, load_texture_method};
            }
            LOGE("utils::Could not find Java helper class %s",
                 kHelperClassName);
            return {};
        }();

        if (!jniIds.helper_class) {
            return false;
        }

        jstring j_path = env->NewStringUTF(path);

        jobject image_obj = env->CallStaticObjectMethod(
                jniIds.helper_class, jniIds.load_image_method, j_path);

        if (j_path) {
            env->DeleteLocalRef(j_path);
        }

        env->CallStaticVoidMethod(jniIds.helper_class, jniIds.load_texture_method,
                                  target, image_obj);
        return true;
    }

    bool LoadPngFromAssetManager(int target, const char* path, const char* className) {
        JNIEnv* env = GetJniEnv();
        jclass helper_class;
        jmethodID load_image_method;
        jmethodID load_texture_method;

        char kHelperClassName[] =
                "com/example/menghe/mygles/";
        strcat(kHelperClassName, className);
        constexpr char kLoadImageMethodName[] = "loadImage";
        constexpr char kLoadImageMethodSignature[] =
                "(Ljava/lang/String;)Landroid/graphics/Bitmap;";
        constexpr char kLoadTextureMethodName[] = "loadTexture";
        constexpr char kLoadTextureMethodSignature[] =
                "(ILandroid/graphics/Bitmap;)V";


        helper_class = env->FindClass(kHelperClassName);
        if (helper_class) {
            helper_class = static_cast<jclass>(env->NewGlobalRef(helper_class));
            load_image_method = env->GetStaticMethodID(
                    helper_class, kLoadImageMethodName, kLoadImageMethodSignature);
            load_texture_method = env->GetStaticMethodID(
                    helper_class, kLoadTextureMethodName, kLoadTextureMethodSignature);

        }else{
            LOGE("utils::Could not find Java helper class %s",
                 kHelperClassName);
        }
        if (!helper_class)
            return false;

        jstring j_path = env->NewStringUTF(path);

        jobject image_obj = env->CallStaticObjectMethod(
                helper_class, load_image_method, j_path);

        if (j_path) {
            env->DeleteLocalRef(j_path);
        }

        env->CallStaticVoidMethod(helper_class, load_texture_method,
                                  target, image_obj);
        return true;
    }

    bool LoadObjFile( AAssetManager* asset_manager,const char* file_name,
                     std::vector<GLfloat>* out_vertices,
                     std::vector<GLfloat>* out_normals,
                     std::vector<GLfloat>* out_uv,
                     std::vector<GLushort>* out_indices) {
        std::vector<GLfloat> temp_positions;
        std::vector<GLfloat> temp_normals;
        std::vector<GLfloat> temp_uvs;
        std::vector<GLushort> vertex_indices;
        std::vector<GLushort> normal_indices;
        std::vector<GLushort> uv_indices;

        std::string file_buffer;
        bool read_success = LoadTextFileFromAssetManager(file_name,asset_manager, &file_buffer);
        if (!read_success) {
            return false;
        }
        std::stringstream file_string_stream(file_buffer);

        while (!file_string_stream.eof()) {
            char line_header[128];
            file_string_stream.getline(line_header, 128);

            if (line_header[0] == 'v' && line_header[1] == 'n') {
                // Parse vertex normal.
                GLfloat normal[3];
                int matches = sscanf(line_header, "vn %f %f %f\n", &normal[0], &normal[1],
                                     &normal[2]);
                if (matches != 3) {
                    LOGE("Format of 'vn float float float' required for each normal line");
                    return false;
                }

                temp_normals.push_back(normal[0]);
                temp_normals.push_back(normal[1]);
                temp_normals.push_back(normal[2]);
            } else if (line_header[0] == 'v' && line_header[1] == 't') {
                // Parse texture uv.
                GLfloat uv[2];
                int matches = sscanf(line_header, "vt %f %f\n", &uv[0], &uv[1]);
                if (matches != 2) {
                    LOGE("Format of 'vt float float' required for each texture uv line");
                    return false;
                }

                temp_uvs.push_back(uv[0]);
                temp_uvs.push_back(uv[1]);
            } else if (line_header[0] == 'v') {
                // Parse vertex.
                GLfloat vertex[3];
                int matches = sscanf(line_header, "v %f %f %f\n", &vertex[0], &vertex[1],
                                     &vertex[2]);
                if (matches != 3) {
                    LOGE("Format of 'v float float float' required for each vertice line");
                    return false;
                }

                temp_positions.push_back(vertex[0]);
                temp_positions.push_back(vertex[1]);
                temp_positions.push_back(vertex[2]);
            } else if (line_header[0] == 'f') {
                // Actual faces information starts from the second character.
                char* face_line = &line_header[1];

                unsigned int vertex_index[4];
                unsigned int normal_index[4];
                unsigned int texture_index[4];

                std::vector<char*> per_vert_info_list;
                char* per_vert_info_list_c_str;
                char* face_line_iter = face_line;
                while ((per_vert_info_list_c_str =
                                strtok_r(face_line_iter, " ", &face_line_iter))) {
                    // Divide each faces information into individual positions.
                    per_vert_info_list.push_back(per_vert_info_list_c_str);
                }

                bool is_normal_available = false;
                bool is_uv_available = false;
                for (int i = 0; i < per_vert_info_list.size(); ++i) {
                    char* per_vert_info;
                    int per_vert_infor_count = 0;

                    bool is_vertex_normal_only_face =
                            (strstr(per_vert_info_list[i], "//") != nullptr);

                    char* per_vert_info_iter = per_vert_info_list[i];
                    while ((per_vert_info =
                                    strtok_r(per_vert_info_iter, "/", &per_vert_info_iter))) {
                        // write only normal and vert values.
                        switch (per_vert_infor_count) {
                            case 0:
                                // Write to vertex indices.
                                vertex_index[i] = atoi(per_vert_info);  // NOLINT
                                break;
                            case 1:
                                // Write to texture indices.
                                if (is_vertex_normal_only_face) {
                                    normal_index[i] = atoi(per_vert_info);  // NOLINT
                                    is_normal_available = true;
                                } else {
                                    texture_index[i] = atoi(per_vert_info);  // NOLINT
                                    is_uv_available = true;
                                }
                                break;
                            case 2:
                                // Write to normal indices.
                                if (!is_vertex_normal_only_face) {
                                    normal_index[i] = atoi(per_vert_info);  // NOLINT
                                    is_normal_available = true;
                                    break;
                                }
                                [[clang::fallthrough]];
                                // Intentionally falling to default error case because vertex
                                // normal face only has two values.
                            default:
                                // Error formatting.
                                LOGE(
                                        "Format of 'f int/int/int int/int/int int/int/int "
                                                "(int/int/int)' "
                                                "or 'f int//int int//int int//int (int//int)' required for "
                                                "each face");
                                return false;
                        }
                        per_vert_infor_count++;
                    }
                }

                int vertices_count = per_vert_info_list.size();
                for (int i = 2; i < vertices_count; ++i) {
                    vertex_indices.push_back(vertex_index[0] - 1);
                    vertex_indices.push_back(vertex_index[i - 1] - 1);
                    vertex_indices.push_back(vertex_index[i] - 1);

                    if (is_normal_available) {
                        normal_indices.push_back(normal_index[0] - 1);
                        normal_indices.push_back(normal_index[i - 1] - 1);
                        normal_indices.push_back(normal_index[i] - 1);
                    }

                    if (is_uv_available) {
                        uv_indices.push_back(texture_index[0] - 1);
                        uv_indices.push_back(texture_index[i - 1] - 1);
                        uv_indices.push_back(texture_index[i] - 1);
                    }
                }
            }
        }

        bool is_normal_available = (!normal_indices.empty());
        bool is_uv_available = (!uv_indices.empty());

        if (is_normal_available && normal_indices.size() != vertex_indices.size()) {
            LOGE("Obj normal indices does not equal to vertex indices.");
            return false;
        }

        if (is_uv_available && uv_indices.size() != vertex_indices.size()) {
            LOGE("Obj UV indices does not equal to vertex indices.");
            return false;
        }

        for (unsigned int i = 0; i < vertex_indices.size(); i++) {
            unsigned int vertex_index = vertex_indices[i];
            out_vertices->push_back(temp_positions[vertex_index * 3]);
            out_vertices->push_back(temp_positions[vertex_index * 3 + 1]);
            out_vertices->push_back(temp_positions[vertex_index * 3 + 2]);
            out_indices->push_back(i);

            if (is_normal_available) {
                unsigned int normal_index = normal_indices[i];
                out_normals->push_back(temp_normals[normal_index * 3]);
                out_normals->push_back(temp_normals[normal_index * 3 + 1]);
                out_normals->push_back(temp_normals[normal_index * 3 + 2]);
            }

            if (is_uv_available) {
                unsigned int uv_index = uv_indices[i];
                out_uv->push_back(temp_uvs[uv_index * 2]);
                out_uv->push_back(temp_uvs[uv_index * 2 + 1]);
            }
        }

        return true;
    }





}

