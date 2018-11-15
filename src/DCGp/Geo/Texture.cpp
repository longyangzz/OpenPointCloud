#include "Texture.h"

using namespace DCGp;

Texture::Texture()
	: _textureObject(nullptr)
{

}

Texture::~Texture()
{
	if (_textureObject)
	{
		delete _textureObject;
		_textureObject = nullptr;
	}
}

DCGp::TextureObject* Texture::GenerateTextureObject(const Texture* texture, GLenum target)
{
	GLuint id;
	glGenTextures( 1L, &id);

	TextureObject* to = new TextureObject(const_cast<Texture*>(texture),id, target);

	return to;
}

void Texture::ApplyTexParameters(GLenum target, State& state) const
{
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//glTexImage2D(GL_TEXTURE_2D, 0, 3, tximg.width(), tximg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());
}

void Texture::ApplyTexImage2D_load(State& state, GLenum target, const QImage* image, GLsizei width, GLsizei height,GLsizei numMipmapLevels) const
{
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image->width(), image->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->bits());
}
