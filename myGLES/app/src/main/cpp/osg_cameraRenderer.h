//
// Created by menghe on 7/12/2018.
//

#ifndef MYGLES_OSG_CAMERARENDERER_H
#define MYGLES_OSG_CAMERARENDERER_H


#include <android/asset_manager.h>
#include <osg/GL>
#include <osg/TextureRectangle>
#include "TextureAC.h"
#include "arcore_c_api.h"
#include "osg_utils.h"
class osg_cameraRenderer {
private:
    ref_ptr<TextureRectangle> _bgTexture;
    ref_ptr<Geode> _bgNode;
    ref_ptr<Image> _bgImg;

    osg::ref_ptr<osg::Geometry> _texturedQuad;


public:
    void Initialization(AAssetManager * manager, Group* root);
    void Draw(ArSession* session, ArFrame* frame, bool btn_status_normal = true);
    // Returns the generated texture name for the GL_TEXTURE_EXTERNAL_OES target.
    GLuint GetTextureId(GraphicsContext * gc){return _bgTexture->getTextureObject(gc->getState()->getContextID())->id();}
    Node * GetGeode(){return _bgNode;}
};


#endif //MYGLES_OSG_CAMERARENDERER_H
