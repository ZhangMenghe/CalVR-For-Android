//
// Created by menghe on 7/13/2018.
//

#include "TextureAC.h"
#include <osg/State>
#include <osg/ContextData>
#include <GLES2/gl2ext.h>

using namespace osg;

void TextureAC::apply(State &state) {
    const unsigned int contextID = state.getContextID();
    TextureObject * textureObject = getTextureObject(contextID);
    if(textureObject){
        textureID = textureObject->id();
        bool textureObjectInvalidated = false;
        if(_subloadCallback.valid())
            textureObjectInvalidated = !_subloadCallback->textureObjectValid(*this, state);
        else if (_image.valid() && getModifiedCount(contextID) != _image->getModifiedCount())
            textureObjectInvalidated = !textureObjectValid(state);
        if (textureObjectInvalidated){
            _textureObjectBuffer[contextID]->release();
            _textureObjectBuffer[contextID] = 0;
            textureObject = 0;
        }
    }

    if (textureObject)
    {
        textureObject->bind();

        if (_subloadCallback.valid())
        {
            applyTexParameters(GL_TEXTURE_EXTERNAL_OES,state);

            _subloadCallback->subload(*this,state);
        }
        else if (_image.valid() && getModifiedCount(contextID) != _image->getModifiedCount())
        {
            // update the modified tag to show that it is up to date.
            getModifiedCount(contextID) = _image->getModifiedCount();

            applyTexParameters(GL_TEXTURE_EXTERNAL_OES,state);

            applyTexImage2D_subload(state,GL_TEXTURE_EXTERNAL_OES,_image.get(),
                                    _textureWidth, _textureHeight, _internalFormat, _numMipmapLevels);
        }
        else if (_readPBuffer.valid())
        {
            _readPBuffer->bindPBufferToTexture(GL_FRONT);
        }

        if (getTextureParameterDirty(state.getContextID()))
            applyTexParameters(GL_TEXTURE_EXTERNAL_OES,state);

    }
    else if (_subloadCallback.valid())
    {
        _textureObjectBuffer[contextID] = _subloadCallback->generateTextureObject(*this, state);
        textureObject = _textureObjectBuffer[contextID].get();

        textureObject->bind();

        applyTexParameters(GL_TEXTURE_EXTERNAL_OES,state);

        if (_image.valid()) getModifiedCount(contextID) = _image->getModifiedCount();

        _subloadCallback->load(*this,state);

        textureObject->setAllocated(_numMipmapLevels,_internalFormat,_textureWidth,_textureHeight,1,_borderWidth);
    }
    else if (_image.valid() && _image->data())
    {

        // keep the image around at least till we go out of scope.
        osg::ref_ptr<osg::Image> image = _image;

        // compute the internal texture format, this set the _internalFormat to an appropriate value.
        computeInternalFormat();

        // compute the dimensions of the texture.
        computeRequiredTextureDimensions(state,*image,_textureWidth, _textureHeight, _numMipmapLevels);

        textureObject = generateAndAssignTextureObject(contextID,GL_TEXTURE_EXTERNAL_OES,_numMipmapLevels,_internalFormat,_textureWidth,_textureHeight,1,_borderWidth);

        textureObject->bind();

        applyTexParameters(GL_TEXTURE_EXTERNAL_OES,state);

        // update the modified tag to show that it is up to date.
        getModifiedCount(contextID) = image->getModifiedCount();

        if (textureObject->isAllocated() && image->supportsTextureSubloading())
        {
            //OSG_NOTICE<<"Reusing texture object"<<std::endl;
            applyTexImage2D_subload(state,GL_TEXTURE_EXTERNAL_OES,image.get(),
                                    _textureWidth, _textureHeight, _internalFormat, _numMipmapLevels);
        }
        else
        {
            //OSG_NOTICE<<"Creating new texture object"<<std::endl;
            applyTexImage2D_load(state,GL_TEXTURE_EXTERNAL_OES,image.get(),
                                 _textureWidth, _textureHeight, _numMipmapLevels);

            textureObject->setAllocated(true);
        }

        // unref image data?
        if (isSafeToUnrefImageData(state) && image->getDataVariance()==STATIC)
        {
            TextureAC* non_const_this = const_cast<TextureAC*>(this);
            non_const_this->_image = NULL;
        }
    }
    else if ( (_textureWidth!=0) && (_textureHeight!=0) && (_internalFormat!=0) )
    {
        textureObject = generateAndAssignTextureObject(contextID,GL_TEXTURE_EXTERNAL_OES,_numMipmapLevels,_internalFormat,_textureWidth,_textureHeight,1,_borderWidth);

        textureObject->bind();

        applyTexParameters(GL_TEXTURE_EXTERNAL_OES,state);

        // no image present, but dimensions at set so lets create the texture
        glTexImage2D( GL_TEXTURE_EXTERNAL_OES, 0, _internalFormat,
                      _textureWidth, _textureHeight, _borderWidth,
                      _sourceFormat ? _sourceFormat : _internalFormat,
                      _sourceType ? _sourceType : GL_UNSIGNED_BYTE,
                      0);

        if (_readPBuffer.valid())
        {
            _readPBuffer->bindPBufferToTexture(GL_FRONT);
        }

    }
    else
    {
        glBindTexture( GL_TEXTURE_EXTERNAL_OES, 0 );
    }

    // if texture object is now valid and we have to allocate mipmap levels, then
    if (textureObject != 0 && _texMipmapGenerationDirtyList[contextID])
    {
        generateMipmap(state);
    }
}