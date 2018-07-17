//
// Created by menghe on 7/13/2018.
//

#include "TextureAC.h"
#include <osg/State>
#include <osg/ContextData>

using namespace osg;
TextureAC::TextureAC():
        _textureWidth(0),
        _textureHeight(0),
        _numMipmapLevels(0)
{
    setUseHardwareMipMapGeneration(true);
}
TextureAC::TextureAC(const TextureAC& text,const CopyOp& copyop):
        Texture(text,copyop),
        _textureWidth(text._textureWidth),
        _textureHeight(text._textureHeight),
        _numMipmapLevels(text._numMipmapLevels)
{
    setImage(copyop(text._image.get()));
}
bool TextureAC::textureObjectValid(State& state) const
{
    TextureObject* textureObject = getTextureObject(state.getContextID());
    if (!textureObject) return false;

    // return true if image isn't assigned as we won't be override the value.
    if (!_image) return true;

    // compute the internal texture format, this set the _internalFormat to an appropriate value.
    computeInternalFormat();

    GLsizei new_width, new_height, new_numMipmapLevels;

    // compute the dimensions of the texture.
    computeRequiredTextureDimensions(state, *_image, new_width, new_height, new_numMipmapLevels);

    return textureObject->match(GL_TEXTURE_EXTERNAL_OES, new_numMipmapLevels, _internalFormat, new_width, new_height, 1, _borderWidth);
}
void TextureAC::apply(State &state) const {
    applyTexParameters(GL_TEXTURE_EXTERNAL_OES,state);
    const unsigned int contextID = state.getContextID();
    TextureObject * textureObject = getTextureObject(contextID);
    if(textureObject){

        bool textureObjectInvalidated = false;
        /*if(_subloadCallback.valid())
            textureObjectInvalidated = !_subloadCallback->textureObjectValid(*this, state);*/
        if (_image.valid() && getModifiedCount(contextID) != _image->getModifiedCount())
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

        /*if (_subloadCallback.valid())
        {
            applyTexParameters(GL_TEXTURE_EXTERNAL_OES,state);

            _subloadCallback->subload(*this,state);
        }*/
        if (_image.valid() && getModifiedCount(contextID) != _image->getModifiedCount())
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
    /*else if (_subloadCallback.valid())
    {
        _textureObjectBuffer[contextID] = _subloadCallback->generateTextureObject(*this, state);
        textureObject = _textureObjectBuffer[contextID].get();

        textureObject->bind();

        applyTexParameters(GL_TEXTURE_EXTERNAL_OES,state);

        if (_image.valid()) getModifiedCount(contextID) = _image->getModifiedCount();

        _subloadCallback->load(*this,state);

        textureObject->setAllocated(_numMipmapLevels,_internalFormat,_textureWidth,_textureHeight,1,_borderWidth);
    }*/
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
void TextureAC::setImage(Image* image)
{
    if (_image == image) return;

    if (_image.valid())
    {
        _image->removeClient(this);

        if (_image->requiresUpdateCall())
        {
            setUpdateCallback(0);
            setDataVariance(osg::Object::STATIC);
        }
    }

    _image = image;
    _modifiedCount.setAllElementsTo(0);

    if (_image.valid())
    {
        _image->addClient(this);

        if (_image->requiresUpdateCall())
        {
            setUpdateCallback(new Image::UpdateCallback());
            setDataVariance(osg::Object::DYNAMIC);
        }
    }
}
int TextureAC::compare(const StateAttribute& sa) const
{
    // check the types are equal and then create the rhs variable
    // used by the COMPARE_StateAttribute_Parameter macros below.
    COMPARE_StateAttribute_Types(TextureAC,sa)

    if (_image!=rhs._image) // smart pointer comparison.
    {
        if (_image.valid())
        {
            if (rhs._image.valid())
            {
                int result = _image->compare(*rhs._image);
                if (result!=0) return result;
            }
            else
            {
                return 1; // valid lhs._image is greater than null.
            }
        }
        else if (rhs._image.valid())
        {
            return -1; // valid rhs._image is greater than null.
        }
    }

    if (!_image && !rhs._image)
    {
        // no image attached to either Texture2D
        // but could these textures already be downloaded?
        // check the _textureObjectBuffer to see if they have been
        // downloaded

        int result = compareTextureObjects(rhs);
        if (result!=0) return result;
    }

    int result = compareTexture(rhs);
    if (result!=0) return result;

    // compare each parameter in turn against the rhs.
#if 1
    if (_textureWidth != 0 && rhs._textureWidth != 0)
    {
        COMPARE_StateAttribute_Parameter(_textureWidth)
    }
    if (_textureHeight != 0 && rhs._textureHeight != 0)
    {
        COMPARE_StateAttribute_Parameter(_textureHeight)
    }
#endif
    //COMPARE_StateAttribute_Parameter(_subloadCallback)

    return 0; // passed all the above comparison macros, must be equal.
}
void TextureAC::computeInternalFormat() const
{
    if (_image.valid()) computeInternalFormatWithImage(*_image);
    else computeInternalFormatType();
}
void TextureAC::copyTexImage2D(State& state, int x, int y, int width, int height )
{
    const unsigned int contextID = state.getContextID();

    if (_internalFormat==0) _internalFormat=GL_RGBA;

    // get the globj for the current contextID.
    TextureObject* textureObject = getTextureObject(contextID);

    if (textureObject)
    {
        if (width==(int)_textureWidth && height==(int)_textureHeight)
        {
            // we have a valid texture object which is the right size
            // so lets play clever and use copyTexSubImage2D instead.
            // this allows use to reuse the texture object and avoid
            // expensive memory allocations.
            copyTexSubImage2D(state,0 ,0, x, y, width, height);
            return;
        }
        // the relevant texture object is not of the right size so
        // needs to been deleted
        // remove previously bound textures.
        dirtyTextureObject();
        // note, dirtyTextureObject() dirties all the texture objects for
        // this texture, is this right?  Perhaps we should dirty just the
        // one for this context.  Note sure yet will leave till later.
        // RO July 2001.
    }


    // remove any previously assigned images as these are nolonger valid.
    _image = NULL;

    // switch off mip-mapping.
    //

    bool needHardwareMipMap = (_min_filter != LINEAR && _min_filter != NEAREST);
    bool hardwareMipMapOn = false;
    if (needHardwareMipMap)
    {
        hardwareMipMapOn = isHardwareMipmapGenerationEnabled(state);

        if (!hardwareMipMapOn)
        {
            // have to switch off mip mapping
            OSG_NOTICE<<"Warning: TextureAC::copyTexImage2D(,,,,) switch off mip mapping as hardware support not available."<<std::endl;
            _min_filter = LINEAR;
        }
    }

    _textureWidth = width;
    _textureHeight = height;

    _numMipmapLevels = 1;
    if (needHardwareMipMap)
    {
        for(int s=1; s<width || s<height; s <<= 1, ++_numMipmapLevels) {}
    }

    textureObject = generateAndAssignTextureObject(contextID,GL_TEXTURE_EXTERNAL_OES,_numMipmapLevels,_internalFormat,_textureWidth,_textureHeight,1,0);

    textureObject->bind();

    applyTexParameters(GL_TEXTURE_EXTERNAL_OES,state);


    GenerateMipmapMode mipmapResult = mipmapBeforeTexImage(state, hardwareMipMapOn);

    glCopyTexImage2D( GL_TEXTURE_EXTERNAL_OES, 0, _internalFormat, x, y, width, height, 0 );

    mipmapAfterTexImage(state, mipmapResult);

    textureObject->setAllocated(true);

    // inform state that this texture is the current one bound.
    state.haveAppliedTextureAttribute(state.getActiveTextureUnit(), this);
}

void TextureAC::copyTexSubImage2D(State& state, int xoffset, int yoffset, int x, int y, int width, int height )
{
    const unsigned int contextID = state.getContextID();

    if (_internalFormat==0) _internalFormat=GL_RGBA;

    // get the texture object for the current contextID.
    TextureObject* textureObject = getTextureObject(contextID);

    if (textureObject)
    {
        // we have a valid image
        textureObject->bind();

        applyTexParameters(GL_TEXTURE_EXTERNAL_OES,state);

        bool needHardwareMipMap = (_min_filter != LINEAR && _min_filter != NEAREST);
        bool hardwareMipMapOn = false;
        if (needHardwareMipMap)
        {
            hardwareMipMapOn = isHardwareMipmapGenerationEnabled(state);

            if (!hardwareMipMapOn)
            {
                // have to switch off mip mapping
                OSG_NOTICE<<"Warning: Texture2D::copyTexImage2D(,,,,) switch off mip mapping as hardware support not available."<<std::endl;
                _min_filter = LINEAR;
            }
        }

        GenerateMipmapMode mipmapResult = mipmapBeforeTexImage(state, hardwareMipMapOn);

        glCopyTexSubImage2D( GL_TEXTURE_EXTERNAL_OES, 0, xoffset, yoffset, x, y, width, height);

        mipmapAfterTexImage(state, mipmapResult);

        // inform state that this texture is the current one bound.
        state.haveAppliedTextureAttribute(state.getActiveTextureUnit(), this);

    }
    else
    {
        // no texture object already exsits for this context so need to
        // create it upfront - simply call copyTexImage2D.
        copyTexImage2D(state,x,y,width,height);
    }
}

void TextureAC::allocateMipmap(State& state) const
{
    const unsigned int contextID = state.getContextID();

    // get the texture object for the current contextID.
    TextureObject* textureObject = getTextureObject(contextID);

    if (textureObject && _textureWidth != 0 && _textureHeight != 0)
    {
        // bind texture
        textureObject->bind();

        // compute number of mipmap levels
        int width = _textureWidth;
        int height = _textureHeight;
        int numMipmapLevels = Image::computeNumberOfMipmapLevels(width, height);

        // we do not reallocate the level 0, since it was already allocated
        width >>= 1;
        height >>= 1;

        for( GLsizei k = 1; k < numMipmapLevels  && (width || height); k++)
        {
            if (width == 0)
                width = 1;
            if (height == 0)
                height = 1;

            glTexImage2D( GL_TEXTURE_EXTERNAL_OES, k, _internalFormat,
                          width, height, _borderWidth,
                          _sourceFormat ? _sourceFormat : _internalFormat,
                          _sourceType ? _sourceType : GL_UNSIGNED_BYTE, NULL);

            width >>= 1;
            height >>= 1;
        }

        // inform state that this texture is the current one bound.
        state.haveAppliedTextureAttribute(state.getActiveTextureUnit(), this);
    }
}