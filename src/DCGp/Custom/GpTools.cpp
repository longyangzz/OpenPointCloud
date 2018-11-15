#include "GpTools.h"

//Common
#include "DCCore/Const.h"
#include "DCCore/Logger.h"

#include "DCGp/GpMatrix.h"

//*********** OPENGL TEXTURES ***********//

void DcGp::DcGpTools::DisplayTexture2DPosition(GLuint tex, int x, int y, int w, int h, unsigned char alpha/*=255*/)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	glColor4ub(255, 255, 255, alpha);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0,0.0);
	glVertex2i(x, y+h);
	glTexCoord2f(0.0,1.0);
	glVertex2i(x, y);
	glTexCoord2f(1.0,1.0);
	glVertex2i(x+w, y);
	glTexCoord2f(1.0,0.0);
	glVertex2i(x+w, y+h);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);
}

void DcGp::DcGpTools::DisplayTexture2D(GLuint tex, int w, int h, unsigned char alpha/*=255*/)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	float halfW = static_cast<float>(w) * 0.5;
	float halfH = static_cast<float>(h)*0.5;

	glColor4ub(255, 255, 255, alpha);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-halfW, -halfH);
	glTexCoord2f(1.0, 0.0);
	glVertex2f( halfW, -halfH);
	glTexCoord2f(1.0, 1.0);
	glVertex2f( halfW,  halfH);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(-halfW,  halfH);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void DcGp::DcGpTools::DisplayTexture2DCorner(GLuint tex, int w, int h, unsigned char alpha/*=255*/)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	glColor4ub(255, 255, 255, alpha);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(0, 0);
	glTexCoord2f(1.0, 0.0);
	glVertex2f( w, 0);
	glTexCoord2f(1.0, 1.0);
	glVertex2f( w,  h);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(0,  h);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

#define READ_PIXELS_PER_LINE
void DcGp::DcGpTools::SaveTextureToArray(unsigned char* data, GLuint texID, unsigned w, unsigned h)
{
	assert(data);

#ifdef READ_PIXELS_PER_LINE
	//to avoid memory issues, we read line by line
	for (int i=0;i<(int)h;++i)
	{
		glReadPixels(0,i,w,1,GL_BGRA,GL_UNSIGNED_BYTE,data+((int)h-1-i)*(int)w*4);
		DcGpTools::CatchGLError("DcGpTools::SaveTextureToArray");
	}
#else
	//GLint width,height,format,align;
	//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
	//glGetIntegerv(GL_PACK_ALIGNMENT,&align);

	//we grab texture data
	glBindTexture(GL_TEXTURE_2D, texID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
#endif
}

bool DcGp::DcGpTools::SaveTextureToFile(const char* filename, GLuint texID, unsigned w, unsigned h)
{
	QImage temp(w,h,QImage::Format_ARGB32);
	GLubyte* data = temp.bits();

	if (!data)
	{
		//ccLog::Error("[DcGpTools::saveTextureToFile] Not enough memory!");
		return false;
	}

	SaveTextureToArray(data,texID,w,h);

	//we save image
#ifdef READ_PIXELS_PER_LINE
	temp.save(filename);
#else
	temp.mirrored(false,true).save(filename);
#endif

	return true;
}

//*********** OPENGL MATRICES ***********//

void DcGp::DcGpTools::MultGLMatrices(const float* A, const float* B, float* dest)
{
	//we backup actual matrix...
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadMatrixf(A);
	glMultMatrixf(B);
	glGetFloatv(GL_MODELVIEW_MATRIX, dest);

	//... and restore it
	glPopMatrix();
}

void DcGp::DcGpTools::TransposeGLMatrix(const float* A, float* dest)
{
	unsigned char i,j;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			dest[(i<< 2) + j] = A[(j << 2) + i];
}

DcGp::DcGpMatrix DcGp::DcGpTools::GenerateGLRotationMatrixFromVectors(const float* sourceVec, const float* destVec)
{
	//we compute scalar prod between the two vectors
	float ps = DCVector3D::VDot(sourceVec,destVec);

	//we bound result (in case vecors are not exactly unit)
	if (ps > 1.0f)
		ps = 1.0f;
	else if (ps < -1.0f)
		ps = -1.0f;

	//we deduce angle from scalar prod
	float angle_deg = acos(ps) * DC_RAD_TO_DEG;

	//we compute rotation axis with scalar prod
	float axis[3];
	DCVector3D::VCross(sourceVec, destVec, axis);

	//we eventually compute the rotation matrix with axis and angle
	return GenerateGLRotationMatrixFromAxisAndAngle(axis, angle_deg);
}

DcGp::DcGpMatrix DcGp::DcGpTools::GenerateGLRotationMatrixFromAxisAndAngle(const float* axis, float angle_deg)
{
	//we backup actual matrix...
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	glRotatef(angle_deg, axis[0], axis[1], axis[2]);
	DcGpMatrix mat;
	glGetFloatv(GL_MODELVIEW_MATRIX, mat.Data());

	//... and restore it
	glPopMatrix();

	return mat;
}

DcGp::DcGpMatrix DcGp::DcGpTools::GenerateViewMat(ViewOrientation orientation)
{
	GLfloat eye[3] = {0.0f, 0.0f, 0.0f};
	GLfloat top[3] = {0.0f, 0.0f, 0.0f};


	//we look at (0,0,0) by default
	switch (orientation)
	{
	case eTopView:
		eye[2] = 1.0f;
		top[1] = 1.0f;
		break;
	case eBottomView:
		eye[2] = -1.0f;
		top[1] = -1.0f;
		break;
	case eFrontView:
		eye[1] = -1.0f;
		top[2] = 1.0f;
		break;
	case eBackView:
		eye[1] = 1.0f;
		top[2] = 1.0f;
		break;
	case eLeftView:
		eye[0] = -1.0f;
		top[2] = 1.0f;
		break;
	case eRightView:
		eye[0] = 1.0f;
		top[2] = 1.0f;
		break;
	case eISOView1:
		eye[0] = -1.0f;
		eye[1] = -1.0f;
		eye[2] = 1.0f;
		top[0] = 1.0f;
		top[1] = 1.0f;
		top[2] = 1.0f;
		break;
	case eISOView2:
		eye[0] = 1.0f;
		eye[1] = 1.0f;
		eye[2] = 1.0f;
		top[0] = -1.0f;
		top[1] = -1.0f;
		top[2] = 1.0f;
		break;
	}

	DcGpMatrix result;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(eye[0], eye[1] ,eye[2], 0.0, 0.0, 0.0, top[0], top[1], top[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, result.Data());
	result.Data()[14] = 0.0; //annoying value (?!)
	glPopMatrix();

	return result;
}

bool DcGp::DcGpTools::CatchGLError(QString error)
{
	GLenum err = glGetError();

	//see http://www.opengl.org/sdk/docs/man/xhtml/glGetError.xml
	switch(err)
	{
	case GL_NO_ERROR:
		return false;
		break;
	case GL_INVALID_ENUM:
		DCCore::Logger::Warning(QObject::tr("[%1] OpenGL error: invalid enumerator").arg(error));
		break;
	case GL_INVALID_VALUE:
		DCCore::Logger::Warning(QObject::tr("[%1] OpenGL error: invalid value").arg(error));
		break;
	case GL_INVALID_OPERATION:
		DCCore::Logger::Warning(QObject::tr("[%1] OpenGL error: invalid operation").arg(error));
		break;
	case GL_STACK_OVERFLOW:
		DCCore::Logger::Error(QObject::tr("[%1] OpenGL error: stack overflow").arg(error));
		break;
	case GL_STACK_UNDERFLOW:
		DCCore::Logger::Error(QObject::tr("[%1] OpenGL error: stack underflow").arg(error));
		break;
	case GL_OUT_OF_MEMORY:
		DCCore::Logger::Error(QObject::tr("[%1] OpenGL error: out of memory").arg(error));
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		DCCore::Logger::Warning(QObject::tr("[%1] OpenGL error: invalid framebuffer operation").arg(error));
		break;
	}

	return true;
}

void DcGp::DcGpTools::MakeLightsNeutral()
{
	GLint maxLightCount;
	glGetIntegerv(GL_MAX_LIGHTS, &maxLightCount);
	for (int i = 0; i < maxLightCount; ++i)
	{
		if (glIsEnabled(GL_LIGHT0+i))
		{
			float diffuse[4];
			float ambiant[4];
			float specular[4];


			glGetLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
			glGetLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambiant);
			glGetLightfv(GL_LIGHT0 + i, GL_SPECULAR, specular);

			diffuse[0] = diffuse[1] = diffuse[2] = (diffuse[0] + diffuse[1] + diffuse[2]) / 3.0f;		//gray 'mean' value
			ambiant[0] = ambiant[1] = ambiant[2] = (ambiant[0] + ambiant[1] + ambiant[2]) / 3.0f;		//gray 'mean' value
			specular[0] = specular[1] = specular[2] = (specular[0] + specular[1] + specular[2]) / 3.0f;	//gray 'mean' value

			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
			glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambiant);
			glLightfv(GL_LIGHT0 + i, GL_SPECULAR, specular);
		}
	}
}
