#ifndef DC_VIEW_FRAMEBUFFER_H
#define DC_VIEW_FRAMEBUFFER_H

//Self
#include "DCCore/DCGL.h"

namespace DCView
	{
		const GLenum FBO_COLORS[] = {GL_COLOR_ATTACHMENT0_EXT,
			GL_COLOR_ATTACHMENT1_EXT,
			GL_COLOR_ATTACHMENT2_EXT,
			GL_COLOR_ATTACHMENT3_EXT};

		//帧缓冲对象
		class FrameBuffer
		{
		public:
			FrameBuffer();
			~FrameBuffer();

			//初始化
			bool Initialize(unsigned w, unsigned h);

			//重置
			void Reset();

			//启动帧缓冲
			void Start();
			//停止帧缓冲
			void Stop();

			//初始化纹理
			bool InitTexture(unsigned index,
				GLint internalformat,
				GLenum format,
				GLenum type,
				GLint minMagFilter = GL_LINEAR,
				GLenum target = GL_TEXTURE_2D);

			bool InitTextures(unsigned count,
				GLint internalformat,
				GLenum format,
				GLenum type,
				GLint minMagFilter = GL_LINEAR,
				GLenum target = GL_TEXTURE_2D);

			//初始化深度
			bool InitDepth(GLint wrapParam = GL_CLAMP_TO_BORDER,
				GLenum internalFormat = GL_DEPTH_COMPONENT24,
				GLint minMagFilter = GL_NEAREST,
				GLenum textureTarget = GL_TEXTURE_2D);

			//void bindAll();

			void SetDrawBuffers(GLsizei n, const GLenum* buffers);	//GLenum buffers[n]	= {GL_COLOR_ATTACHMENT0_EXT,GL_COLOR_ATTACHMENT1_EXT};

			void SetDrawBuffers1();
			void SetDrawBuffersN(GLsizei n); //n=1..4

			GLuint GetID();
			GLuint GetColorTexture(unsigned i);
			GLuint GetDepthTexture();

		protected:

			unsigned width,height;

			//! Depth texture GL ID
			GLuint depthTexture;

			//! Color textures GL IDs
			GLuint colorTextures[4];

			GLuint fboId;
		};
	}

#endif
