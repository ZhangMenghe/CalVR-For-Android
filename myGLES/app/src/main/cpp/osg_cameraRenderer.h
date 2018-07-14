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
#include <string>
class osg_cameraRenderer {
private:

    std::string _filename;
public:
    osg::ref_ptr<osg::Node> createNode(AAssetManager * manager);
    void Draw(ArSession* session, ArFrame* frame, bool btn_status_normal = true);
    // Returns the generated texture name for the GL_TEXTURE_EXTERNAL_OES target.
    //GLuint GetTextureId(GraphicsContext * gc){return _bgTexture->getTextureObject(gc->getState()->getContextID())->id();}
    void setImage(const char* name){_filename = std::string(name);}
};


#endif //MYGLES_OSG_CAMERARENDERER_H
