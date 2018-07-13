//This class public inherited from the osg::Texture2D and override method apply
//The aim is to support GL_TEXTURE_EXTERNAL_OES target for texture instead of GL_TEXTURE_2D
#ifndef MYGLES_TEXTUREAC_H
#define MYGLES_TEXTUREAC_H

#include <osg/Texture2D>

namespace osg{
    class TextureAC: public Texture2D {
    public:
        /** Bind the texture object. If the texture object hasn't already been
  * compiled, create the texture mipmap levels. */
        virtual void apply(State& state) const;

    };

}


#endif //MYGLES_TEXTUREAC_H
