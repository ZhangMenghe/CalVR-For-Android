//
// Created by menghe on 7/12/2018.
//

#ifndef MYGLES_OSG_CAMERARENDERER_H
#define MYGLES_OSG_CAMERARENDERER_H

#include <android/asset_manager.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "TextureAC.h"
#include "osg_utils.h"
#include "arcoreController.h"

#include <string>
#include <osgViewer/ViewerBase>
#include <osgViewer/Viewer>

class osg_cameraRenderer {
private:
    osg::ref_ptr<osg::Geode> _bgNode;
    osg::TextureAC* _bgTexture;
    float _transformed_uvs[8];
    osg::ref_ptr<osg::Vec3Array> _vertices;
    osg::ref_ptr<osg::Vec2Array> _uvs;
    osg::ref_ptr<osg::Geometry> _bgGeo;
    osg::Uniform* _samUniform;

public:
    osg::ref_ptr<osg::Node> createNode(AAssetManager * manager);
    void Draw(arcoreController * arController, bool btn_status_normal = true);
    // Returns the generated texture name for the GL_TEXTURE_EXTERNAL_OES target.
    GLuint GetTextureId(osgViewer::Viewer* const viewer){
        osgViewer::ViewerBase::Contexts ctx;
        viewer->getContexts(ctx);
        Texture::TextureObject* textureObject = _bgTexture->getTextureObject(ctx[0]->getState()->getContextID());
        if(nullptr == textureObject){
            textureObject = _bgTexture->generateAndAssignTextureObject(ctx[0]->getState()->getContextID(), _bgTexture->getTextureTarget());
            textureObject->bind();
        }

//            textureObject = osg::Texture::generateTextureObject(_bgTexture, ctx[0]->getState()->getContextID(), _bgTexture->getTextureTarget());
        _bgTexture->textureID = textureObject->id();
        return textureObject->id();
    }
};


#endif //MYGLES_OSG_CAMERARENDERER_H
