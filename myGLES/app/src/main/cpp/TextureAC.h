//This class public inherited from the osg::Texture2D and override method apply
//The aim is to support GL_TEXTURE_EXTERNAL_OES target for texture instead of GL_TEXTURE_2D
#ifndef MYGLES_TEXTUREAC_H
#define MYGLES_TEXTUREAC_H

#include <osg/Texture>
#include <GLES2/gl2ext.h>

namespace osg{
    class TextureAC: public Texture {
    protected:
        /** Subloaded images can have different texture and image sizes. */
        mutable GLsizei _textureWidth, _textureHeight;
        ref_ptr<Image> _image;
        /** Number of mipmap levels created. */
        mutable GLsizei _numMipmapLevels;
        typedef buffered_value<unsigned int> ImageModifiedCount;
        mutable ImageModifiedCount _modifiedCount;
        void allocateMipmap(State& state) const;
        virtual void computeInternalFormat() const;
    public:
        TextureAC();
        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        TextureAC(const TextureAC& text,const CopyOp& copyop=CopyOp::SHALLOW_COPY);
        /** Sets the texture image. */
        void setImage(Image* image);
        inline unsigned int& getModifiedCount(unsigned int contextID) const
        {
            // get the modified count for the current contextID.
            return _modifiedCount[contextID];
        }
        /** Return true of the TextureObject assigned to the context associate with osg::State object is valid.*/
        bool textureObjectValid(State& state) const;
        /** Bind the texture object. If the texture object hasn't already been
  * compiled, create the texture mipmap levels. */
        virtual void apply(State& state) const;
        /** Return -1 if *this < *rhs, 0 if *this==*rhs, 1 if *this>*rhs. */
        virtual int compare(const StateAttribute& rhs) const;
        virtual GLenum getTextureTarget() const { return GL_TEXTURE_EXTERNAL_OES; }
        /** return true if the texture image data has been modified and the associated GL texture object needs to be updated.*/
        virtual bool isDirty(unsigned int contextID) const { return (_image.valid() && _image->getModifiedCount()!=_modifiedCount[contextID]); }
        /** Sets the texture image, ignoring face. */
        virtual void setImage(unsigned int, Image* image) { setImage(image); }

        template<class T> void setImage(unsigned int, const ref_ptr<T>& image) { setImage(image.get()); }

        /** Gets the texture image, ignoring face. */
        virtual Image* getImage(unsigned int) { return _image.get(); }

        /** Gets the const texture image, ignoring face. */
        virtual const Image* getImage(unsigned int) const { return _image.get(); }

        /** Gets the number of images that can be assigned to the Texture. */
        virtual unsigned int getNumImages() const { return 1; }
        /** Sets the texture width and height. If width or height are zero,
          * calculate the respective value from the source image size. */
        inline void setTextureSize(int width, int height) const
        {
            _textureWidth = width;
            _textureHeight = height;
        }

        void setTextureWidth(int width) { _textureWidth=width; }
        void setTextureHeight(int height) { _textureHeight=height; }
        virtual osg::Object* cloneType() const{ return new TextureAC();}
        virtual osg::Object* clone(const CopyOp& copyop) const{return new TextureAC(*this, copyop);}
        virtual int getTextureWidth() const { return _textureWidth; }
        virtual int getTextureHeight() const { return _textureHeight; }
        virtual int getTextureDepth() const { return 1; }
        void copyTexImage2D(State& state, int x, int y, int width, int height );
        void copyTexSubImage2D(State& state, int xoffset, int yoffset, int x, int y, int width, int height );
//        class OSG_EXPORT SubloadCallback : public Referenced
//        {
//        public:
//
//            virtual bool textureObjectValid(const Texture2D& texture, State& state) const
//            {
//                return texture.textureObjectValid(state);
//            }
//
//            virtual osg::ref_ptr<TextureObject> generateTextureObject(const Texture2D& texture, State& state) const
//            {
//                return osg::Texture::generateTextureObject(&texture, state.getContextID(), GL_TEXTURE_EXTERNAL_OES);
//            }
//
//            virtual void load(const Texture2D& texture,State& state) const = 0;
//            virtual void subload(const Texture2D& texture,State& state) const = 0;
//        };
        GLuint textureID;
    };

}


#endif //MYGLES_TEXTUREAC_H
