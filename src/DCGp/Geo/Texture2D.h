#ifndef MPGP_TEXTURE2D_H
#define MPGP_TEXTURE2D_H

#include "StateAttribute.h"

#include "Texture.h"
#include "DCGp/DcGpDLL.h"

class QImage;
namespace DCGp 
{
class TextureObject;

class DCGP_API Texture2D : public Texture
{
	Q_OBJECT
    public :
        
        Texture2D();

        Texture2D(QImage* image);

		virtual ~Texture2D();

        virtual GLenum getTextureTarget() const { return GL_TEXTURE_2D; }

        /** Sets the texture image. */
        void setImage(QImage* image);

        /** Gets the texture image. */
        QImage* getImage() { return _image; }

        /** Gets the const texture image. */
        inline const QImage* getImage() const { return _image; }



        /** Sets the texture width and height. If width or height are zero,
          * calculate the respective value from the source image size. */
        inline void setTextureSize(int width, int height) const
        {
            _textureWidth = width;
            _textureHeight = height;
        }

        void setTextureWidth(int width) { _textureWidth=width; }
        void setTextureHeight(int height) { _textureHeight=height; }

        virtual int getTextureWidth() const { return _textureWidth; }
        virtual int getTextureHeight() const { return _textureHeight; }
        virtual int getTextureDepth() const { return 1; }

        virtual void Apply(State& state) const;
	public:

        virtual void computeInternalFormat() const;
        
        QImage* _image;

        /** Subloaded images can have different texture and image sizes. */
        mutable GLsizei _textureWidth, _textureHeight;
};

}

#endif
