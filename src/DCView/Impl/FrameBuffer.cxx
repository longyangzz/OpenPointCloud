#include "FrameBuffer.hxx"
using namespace DCView;

//#include "GpShader.h"
//#include "FBOTools.hxx"

#include <assert.h>

static GLenum s_buffers[] = {GL_COLOR_ATTACHMENT0_EXT,
                                GL_COLOR_ATTACHMENT1_EXT,
                                GL_COLOR_ATTACHMENT2_EXT,
                                GL_COLOR_ATTACHMENT3_EXT};

FrameBuffer::FrameBuffer()
{
    //FBO size
    width = 0;
    height = 0;

    //textures
    depthTexture=0;
	colorTextures[0] = 0;
	colorTextures[1] = 0;
	colorTextures[2] = 0;
	colorTextures[3] = 0;

    //FBO GL ID
    fboId=0;
}

FrameBuffer::~FrameBuffer()
{
    Reset();
}

void FrameBuffer::Reset()
{
    if (depthTexture>0)
        glDeleteTextures(1,&depthTexture);
    depthTexture=0;

    for (int i=0;i<4;++i)
        if (colorTextures[i])
        {
            glDeleteTextures(1,colorTextures+i);
            colorTextures[i]=0;
        }

    if (fboId>0)
        glDeleteFramebuffersEXT(1, &fboId);
    fboId=0;

    width=height=0;
}

bool FrameBuffer::Initialize(unsigned w, unsigned h)
{
    //we check if FBO extension is supported by video card
//     if (!DcGpFBOTools::CheckExtension("GL_EXT_framebuffer_object"))
//         return false;

    //to support reinit
    Reset();

    width=w;
    height=h;

    // create a framebuffer object
    glGenFramebuffersEXT(1, &fboId);

    return true;
}

void FrameBuffer::Start()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fboId);
}

void FrameBuffer::Stop()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
}

GLuint FrameBuffer::GetID()
{
    return fboId;
}

GLuint FrameBuffer::GetColorTexture(unsigned i)
{
    assert(i<4);
    return colorTextures[i];
}

GLuint FrameBuffer::GetDepthTexture()
{
    return depthTexture;
}

bool FrameBuffer::InitTexture(unsigned index,
                                        GLint internalformat,
                                        GLenum format,
                                        GLenum type,
                                        GLint minMagFilter /*= GL_LINEAR*/,
                                        GLenum target /*= GL_TEXTURE_2D*/)
{
    if (index>=4)
        //wrong index
        return false;

    if (fboId==0)
    {
        //ccConsole::Warning("[FBO::initTexture] Internal error: FBO not yet initialized!");
        return false;
    }

    Start();

    if (glIsTexture(colorTextures[index]))
        glDeleteTextures(1,colorTextures+index);

    glGenTextures(1,colorTextures+index);
    glBindTexture(target,colorTextures[index]);

    /*INITIAL VERSION
    glTexImage2D(target,0,GL_RGBA,width,height,0,GL_BGRA,GL_UNSIGNED_INT_8_8_8_8_REV,NULL);
    glTexParameteri(target,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(target, 0);
    //*/
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, minMagFilter );
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minMagFilter );
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(target,0,internalformat,width,height,0,format,type,0);

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,target,colorTextures[index],0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,colorTextures[index],0);

    glBindTexture(target, 0);

    bool success = false;
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch (status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
            success = true;
			break;
		default:
            //ccConsole::Warning("[FBO] Color texture %i init. error: %i",index+1,status);
            break;
	}

    Stop();

    return success;
}

bool FrameBuffer::InitTextures(unsigned count,
                                        GLint internalformat,
                                        GLenum format,
                                        GLenum type,
                                        GLint minMagFilter /*= GL_LINEAR*/,
                                        GLenum target /*= GL_TEXTURE_2D*/)
{
    assert(count<5);

    if (fboId==0)
    {
        //ccConsole::Warning("[FBO::initTextures] Internal error: FBO not yet initialized!");
        return false;
    }

	for(unsigned i=0;i<count;++i)
        InitTexture(i,internalformat,format,type,minMagFilter,target);

    return true;
}

bool FrameBuffer::InitDepth(GLint wrapParam /*=GL_CLAMP_TO_BORDER*/,
                                    GLenum internalFormat /*=GL_DEPTH_COMPONENT24*/,
                                    GLint minMagFilter /*= GL_NEAREST*/,
                                    GLenum target/*=GL_TEXTURE_2D*/)
{
    if (fboId==0)
    {
        //ccConsole::Warning("[FBO::initDepth] Internal error: FBO not yet initialized!");
        return false;
    }

    Start();

    if (glIsTexture(depthTexture))
        glDeleteTextures(1,&depthTexture);

    glGenTextures(1,&depthTexture);
    glBindTexture(target,depthTexture);

    /* INITIAL VERSION
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,width,height,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    //*/
	float border[]={1.0,1.0,1.0,1.0};
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapParam);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapParam);
	glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(target, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minMagFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, minMagFilter);
	glTexImage2D(target,0,internalFormat,width,height,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,0);

    //glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,depthTexture,0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,target,depthTexture,0);

    glBindTexture(target, 0);

    bool success = false;
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch (status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
            //ccConsole::Print("[FBO] Depth init. ok");
            success = true;
			break;
		default:
            //ccConsole::Warning("[FBO] Depth texture init. error: %i",status);
            break;
	}

    Stop();

	return success;
}

void FrameBuffer::SetDrawBuffers(GLsizei n, const GLenum* buffers)
{
    assert(n>0 && n<5);
	glDrawBuffers(n,buffers);
	glReadBuffer(GL_NONE);
}

void FrameBuffer::SetDrawBuffers1()
{
    glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
}

void FrameBuffer::SetDrawBuffersN(GLsizei n)
{
    SetDrawBuffers(n,s_buffers);
}
