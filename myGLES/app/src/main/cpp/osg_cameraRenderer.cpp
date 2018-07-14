//
// Created by menghe on 7/12/2018.
//

#include "osg_cameraRenderer.h"
#include "arcore_utils.h"

using namespace osg;
namespace {
    // Positions of the quad vertices in clip space (X, Y, Z).
    const GLfloat kVertices[] = {
            .0, .0f, .0f, 1.0f, .0f, .0f,
            1.0f, .0f, 1.0f, .0f, .0f, 1.0f
    };

    // UVs of the quad vertices (S, T)
    const GLfloat kUvs[] = {
           0.0f, 0.0f, 1.0f, 0.0f,1.0f, 1.0f, 0.0f, 1.0f
    };

}

osg::ref_ptr<osg::Node> osg_cameraRenderer::createNode(AAssetManager *manager) {
    osg::ref_ptr<osg::Geode> _bgNode = new osg::Geode();
    osg::ref_ptr<osg::Geometry> _bgGeo = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> _vertices = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec2Array> _uvs = new osg::Vec2Array();

    for(int i=0; i<4; i++){
        _vertices->push_back(Vec3(kVertices[3*i], kVertices[3*i+1], kVertices[3*i+2]));
        _uvs ->push_back(Vec2(kUvs[2*i], kUvs[2*i+1]));
    }
    _bgGeo->setVertexArray(_vertices.get());
    _bgGeo->setTexCoordArray(0, _uvs.get());


    //创建颜色数组
    osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array();
    //添加数据
    vc->push_back(osg::Vec4(1, 0, 0, 1));
    vc->push_back(osg::Vec4(0, 1, 0, 1));
    vc->push_back(osg::Vec4(0, 0, 1, 1));
    vc->push_back(osg::Vec4(1, 1, 0, 1));

    //设置颜色数组
    _bgGeo->setColorArray(vc.get());
    //设置颜色的绑定方式为单个顶点
    _bgGeo->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    //创建法线数组
    osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array();
    //添加法线
    nc->push_back(osg::Vec3(0, -1, 0));
    //设置法线
    _bgGeo->setNormalArray(nc.get());
    //设置法绑定为全部顶点
    _bgGeo->setNormalBinding(osg::Geometry::BIND_OVERALL);
    //添加图元

    _bgGeo->addPrimitiveSet(new DrawArrays(PrimitiveSet::QUADS, 0, 4));

    _bgNode->addDrawable(_bgGeo.get());

    osg::StateSet* stateset = new osg::StateSet;
    osg::Texture2D* _bgTexture = new osg::Texture2D();
//    _bgTexture->setFilter(osg::Texture::FilterParameter::MIN_FILTER, osg::Texture::FilterMode::LINEAR);
//    _bgTexture->setFilter(osg::Texture::FilterParameter::MAG_FILTER, osg::Texture::FilterMode::LINEAR);
    _bgTexture->setDataVariance(osg::Object::DYNAMIC);

    _bgTexture->setImage(osgDB::readImageFile(_filename));
    osg::Uniform* samUniform = new osg::Uniform(osg::Uniform::SAMPLER_2D, "uTexture");
    samUniform->set(0);
    stateset->addUniform(samUniform);
    stateset->setTextureAttributeAndModes(0, _bgTexture, osg::StateAttribute::ON);

    _bgNode->setStateSet(stateset);
    _bgNode->getOrCreateStateSet()->setAttribute(
            osg_utils::createShaderProgram("shaders/osgTexture.vert", "shaders/osgTexture.frag", manager));
    return _bgNode.get();
}

void osg_cameraRenderer::Draw(ArSession *session, ArFrame *frame, bool btn_status_normal) {
    static float grey = 0.5f;
//    grey += 0.01f;
//    if (grey > 1.0f) {
//        grey = 0.0f;
//    }
    glClearColor(grey, grey, grey, 1.0f);

//    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //_bgTexture->setImage(osgDB::readImageFile(_filename));
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
