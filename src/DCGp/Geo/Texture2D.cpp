#include "Texture2D.h"

using namespace DCGp;

Texture2D::Texture2D()
	: Texture()
    , _textureWidth(0)
    , _textureHeight(0)
	, _image(nullptr)
{

}

Texture2D::Texture2D(QImage* image)
	: Texture()
    , _textureWidth(0)
    , _textureHeight(0)
{
    setImage(image);
}


Texture2D::~Texture2D()
{
	if (_image)
	{
		delete _image;
		_image = nullptr;
	}
	
}

void Texture2D::setImage(QImage* image)
{
    if (_image == image) return;

    _image = image;
}

void Texture2D::Apply(State& state) const
{

    if (_textureObject)
    {
        _textureObject->bind();
    }
    else if (_image)
    {
		//! 根据image初始化纹理参数

        // compute the internal texture format, this set the _internalFormat to an appropriate value.
        computeInternalFormat();

		//生成纹理对象
		_textureObject = GenerateTextureObject(this, GL_TEXTURE_2D);

        _textureObject->bind();

		//! 应用参数设置
       ApplyTexParameters(_textureObject->target(), state);

	   //! 加载照片数据
	   ApplyTexImage2D_load(state, _textureObject->target(), _image, _image->width(), _image->height(), 1);
    }
    else
    {
        glBindTexture( GL_TEXTURE_2D, 0 );
    }
}

void Texture2D::computeInternalFormat() const
{
    
}
