//
// Created by menghe on 7/12/2018.
//

#include "osg_cameraRenderer.h"
#include "arcore_utils.h"

using namespace osg;
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
    //compute texture image , taken from osgspotlight

    osg::Image* createSpotLightImage(const osg::Vec4& centerColour, const osg::Vec4& backgroudColour, unsigned int size, float power)
    {
        osg::Image* image = new osg::Image;
        image->allocateImage(size, size, 1,
                             GL_RGBA, GL_UNSIGNED_BYTE);


        float mid = (float(size) - 1)*0.5f;
        float div = 2.0f / float(size);
        for (unsigned int r = 0; r < size; ++r)
        {
            unsigned char* ptr = image->data(0, r, 0);
            for (unsigned int c = 0; c < size; ++c)
            {
                float dx = (float(c) - mid)*div;
                float dy = (float(r) - mid)*div;
                float pr = powf(1.0f - sqrtf(dx*dx + dy*dy), power);
                if (pr < 0.0f) pr = 0.0f;
                osg::Vec4 color = centerColour*pr + backgroudColour*(1.0f - pr);
                *ptr++ = (unsigned char)((color[0])*255.0f);
                *ptr++ = (unsigned char)((color[1])*255.0f);
                *ptr++ = (unsigned char)((color[2])*255.0f);
                *ptr++ = (unsigned char)((color[3])*255.0f);
            }
        }
        return image;
    }

}

void osg_cameraRenderer::Initialization(AAssetManager * manager, Group*root) {
    _bgNode = new Geode;
//    root->addChild(_bgNode);
    auto stateset = _bgNode->getOrCreateStateSet();

    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    {
        _bgImg = createSpotLightImage(osg::Vec4(1, 0, 0, 1), osg::Vec4(0.5, 0, 0, 0.0), 32, 0.7);

       _texturedQuad = osg::createTexturedQuadGeometry(
                osg::Vec3(0.f, 0.f, 0.f),
                osg::Vec3(1.0f, 0.f, 0.f),
                osg::Vec3(0.f, 1.0f, 0.f),
                0.f,
                0.f,
                _bgImg->s(),
                _bgImg->t());
        _bgTexture = new osg::TextureRectangle(_bgImg);
        _bgTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
        _bgTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
        _bgTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
        _bgTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
        _bgTexture->setDataVariance(osg::Object::DYNAMIC);

        _texturedQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0, _bgTexture, osg::StateAttribute::ON);
        _texturedQuad->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
        _bgNode->addDrawable(_texturedQuad);
    }


//    _bgTexture = new TextureAC();
//    _bgTexture->setFilter(osg::Texture::FilterParameter::MIN_FILTER, osg::Texture::FilterMode::LINEAR);
//    _bgTexture->setFilter(osg::Texture::FilterParameter::MAG_FILTER, osg::Texture::FilterMode::LINEAR);
//    _bgTexture->setDataVariance(osg::Object::DYNAMIC);

}

void osg_cameraRenderer::Draw(ArSession *session, ArFrame *frame, bool btn_status_normal) {
    static float grey = 0.5f;
//    grey += 0.01f;
//    if (grey > 1.0f) {
//        grey = 0.0f;
//    }
    glClearColor(grey, grey, grey, 1.0f);

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    _bgTexture->setImage(_bgImg);
    // If display rotation changed (also includes view size change), we need to
    // re-query the uv coordinates for the on-screen portion of the camera image.
    /*int32_t geometry_changed = 0;
    ArFrame_getDisplayGeometryChanged(session, frame, &geometry_changed);
    if (geometry_changed != 0) {
        ArFrame_transformDisplayUvCoords(session, frame, 8, kUvs,
                                         _transformed_uvs);
    }
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
    if (arcore_utils::getNdkImageProperties(ndk_image, &format, &width, &height, &num_plane,
                              &stride)) {
        if (format == AIMAGE_FORMAT_YUV_420_888) {
            if (width > 0 || height > 0 || num_plane > 0 || stride > 0) {
                ref_ptr<Image> osgImg = new Image;
                uint8_t* input_pixels = nullptr;
                int length = 0;
                media_status_t status =
                        AImage_getPlaneData(ndk_image, 0, &input_pixels, &length);
                if (status != AMEDIA_OK)
                    return;
                osgImg->setImage(width, height, 3,
                                 GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
                                 (unsigned char*)input_pixels,
                                 osg::Image::AllocationMode::NO_DELETE, 1);
                _bgTexture->setImage(osgImg);
            }
        }
    }*/

}
