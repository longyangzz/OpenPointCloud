#ifndef MPGP_TEXTURE_H
#define MPGP_TEXTURE_H

#include "StateAttribute.h"

#include "DCGp/DcGpDLL.h"

namespace DCGp
{
	class TextureObject;
	class State;
	class Texture : public StateAttribute
	{
	public:
		Texture();
		~Texture();

		StateAttribute::Type GetType() const {return TEXTURE;}

		//设置纹理参数
		void ApplyTexParameters(GLenum target, State& state) const;

		//! 加载照片数据
		void ApplyTexImage2D_load(State& state, GLenum target, const QImage* image, GLsizei width, GLsizei height,GLsizei numMipmapLevels) const;

		static TextureObject* GenerateTextureObject(const Texture* texture, GLenum target);

		TextureObject* GetTextureObject() {return _textureObject;}
	protected:
		mutable TextureObject* _textureObject;
	};

	class DCGP_API TextureObject
	{
	public:

		inline TextureObject(Texture* texture, GLuint id, GLenum target):
			_id(id),
			_texture(texture),
			_target(target)
		{

		}

		virtual ~TextureObject()
		{
			_texture = nullptr;
			if (_id)
			{
				glDeleteTextures(1, &_id);
			}
			
		}

		inline GLenum id() const { return _id; }
		inline GLenum target() const { return _target; }

		void bind()
		{
			glBindTexture(_target, _id);
		}

		inline void setTexture(Texture* texture) { _texture = texture; }
		inline Texture* getTexture() const { return _texture; }

		GLuint              _id;
		GLenum              _target;
		Texture*            _texture;

	};
}

#endif // TEXTURE_H
