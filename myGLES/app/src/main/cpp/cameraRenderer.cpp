//
// Created by menghe on 7/4/2018.
//
#include <type_traits>
#include "cameraRenderer.h"

namespace {
    // Positions of the quad vertices in clip space (X, Y, Z).
    const GLfloat kVertices[] = {
            -1.0f, -1.0f, 0.0f, +1.0f, -1.0f, 0.0f,
            -1.0f, +1.0f, 0.0f, +1.0f, +1.0f, 0.0f,
    };

    // UVs of the quad vertices (S, T)
    const GLfloat kUvs[] = {
            0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };
    constexpr int kSobelEdgeThreshold = 128 * 128;

    bool GetNdkImageProperties(const AImage* ndk_image, int32_t* out_format,
                               int32_t* out_width, int32_t* out_height,
                               int32_t* out_plane_num, int32_t* out_stride) {
        if (ndk_image == nullptr) {
            return false;
        }
        media_status_t status = AImage_getFormat(ndk_image, out_format);
        if (status != AMEDIA_OK) {
            return false;
        }

        status = AImage_getWidth(ndk_image, out_width);
        if (status != AMEDIA_OK) {
            return false;
        }

        status = AImage_getHeight(ndk_image, out_height);
        if (status != AMEDIA_OK) {
            return false;
        }

        status = AImage_getNumberOfPlanes(ndk_image, out_plane_num);
        if (status != AMEDIA_OK) {
            return false;
        }

        status = AImage_getPlaneRowStride(ndk_image, 0, out_stride);
        if (status != AMEDIA_OK) {
            return false;
        }

        return true;
    }

    bool DetectEdge(const AImage* ndk_image, int32_t width, int32_t height,
                    int32_t stride, uint8_t* output_pixels) {
        if (ndk_image == nullptr || output_pixels == nullptr) {
            return false;
        }

        uint8_t* input_pixels = nullptr;
        int length = 0;
        media_status_t status =
                AImage_getPlaneData(ndk_image, 0, &input_pixels, &length);
        if (status != AMEDIA_OK) {
            return false;
        }

        // Detect edges.
        for (int j = 1; j < height - 1; j++) {
            for (int i = 1; i < width - 1; i++) {
                // Offset of the pixel at [i, j] of the input image.
                int offset = (j * stride) + i;

                // Neighbour pixels around the pixel at [i, j].
                int a00 = input_pixels[offset - width - 1];
                int a01 = input_pixels[offset - width];
                int a02 = input_pixels[offset - width + 1];
                int a10 = input_pixels[offset - 1];
                int a12 = input_pixels[offset + 1];
                int a20 = input_pixels[offset + width - 1];
                int a21 = input_pixels[offset + width];
                int a22 = input_pixels[offset + width + 1];

                // Sobel X filter:
                //   -1, 0, 1,
                //   -2, 0, 2,
                //   -1, 0, 1
                int x_sum = -a00 - (2 * a10) - a20 + a02 + (2 * a12) + a22;

                // Sobel Y filter:
                //    1, 2, 1,
                //    0, 0, 0,
                //   -1, -2, -1
                int y_sum = a00 + (2 * a01) + a02 - a20 - (2 * a21) - a22;

                if ((x_sum * x_sum) + (y_sum * y_sum) > kSobelEdgeThreshold) {
                    output_pixels[(j * width) + i] = static_cast<uint8_t>(0xFF);
                } else {
                    output_pixels[(j * width) + i] = static_cast<uint8_t>(0x1F);
                }
            }
        }
        return true;
    }
}
void cameraRenderer::Initialization(AAssetManager *manager) {
    _shader_program = utils::CreateProgram("shaders/screenquad.vert", "shaders/screenquad.frag", manager);
    if(!_shader_program)
        LOGE("Failed to create shader program");

    _attrib_vertices = glGetAttribLocation(_shader_program, "a_Position");
    _attrib_uvs = glGetAttribLocation(_shader_program, "a_TexCoord");
    _uniform_status_normal = glGetUniformLocation(_shader_program, "uStatus_normal");

    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, _texture_id);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &_texture_overlay_id);
    glBindTexture(GL_TEXTURE_2D, _texture_overlay_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glUseProgram(_shader_program);
    glUniform1i(glGetUniformLocation(_shader_program, "uTexture_normal"), 0);
    glUniform1i(glGetUniformLocation(_shader_program, "uTexture_special"), 1);
    glUseProgram(0);

}
void cameraRenderer::Draw(ArSession *session, ArFrame *frame, bool btn_status_normal) {
    static_assert(std::extent<decltype(kUvs)>::value == 8,
                  "Incorrect kUvs length");
    static_assert(std::extent<decltype(kVertices)>::value == 12,
                  "Incorrect kVertices length");
    // If display rotation changed (also includes view size change), we need to
    // re-query the uv coordinates for the on-screen portion of the camera image.
    int32_t geometry_changed = 0;
    ArFrame_getDisplayGeometryChanged(session, frame, &geometry_changed);
    if (geometry_changed != 0) {
        ArFrame_transformDisplayUvCoords(session, frame, 8, kUvs,
                                         _transformed_uvs);
    }

    // special mode: grab current image
    if(!btn_status_normal) {
        ArImage *ar_image;
        const AImage *ndk_image = nullptr;
        ArStatus status =
                ArFrame_acquireCameraImage(session, frame, &ar_image);
        if (status == AR_SUCCESS) {
            ArImage_getNdkImage(ar_image, &ndk_image);
            ArImage_release(ar_image);
        } else {
            LOGE("ComputerVisionApplication::OnDrawFrame acquire camera image not ready.");
        }
        if (ndk_image == nullptr)
            return;
        int32_t format = 0, width = 0, height = 0, num_plane = 0, stride = 0;
        if (GetNdkImageProperties(ndk_image, &format, &width, &height, &num_plane,
                                  &stride)) {
            if (format == AIMAGE_FORMAT_YUV_420_888) {
                if (width > 0 || height > 0 || num_plane > 0 || stride > 0) {
                    if (processed_image_bytes_grayscale_ == nullptr ||
                        stride * height > cpu_image_buffer_size_) {
                        cpu_image_buffer_size_ = stride * height;
                        processed_image_bytes_grayscale_ =
                                std::unique_ptr<uint8_t[]>(new uint8_t[cpu_image_buffer_size_]);
                    }
                    DetectEdge(ndk_image, width, height, stride,
                               processed_image_bytes_grayscale_.get());
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, _texture_overlay_id);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE,
                                 GL_UNSIGNED_BYTE, processed_image_bytes_grayscale_.get());
                }
            } else {
                LOGE("Expected image in YUV_420_888 format.");
            }
        }
    }

    glUseProgram(_shader_program);
    // No need to test or write depth, the screen quad has arbitrary depth, and is
    // expected to be drawn first.
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, _texture_id);

    glEnableVertexAttribArray(_attrib_vertices);
    glVertexAttribPointer(_attrib_vertices, 3, GL_FLOAT, GL_FALSE, 0,
                          kVertices);

    glEnableVertexAttribArray(_attrib_uvs);
    glVertexAttribPointer(_attrib_uvs, 2, GL_FLOAT, GL_FALSE, 0,
                          _transformed_uvs);

    glUniform1i(_uniform_status_normal, (int)btn_status_normal);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(0);

    // Restore the depth state for further drawing.
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    utils::CheckGlError("backgroundRenderer::Draw() error");
}