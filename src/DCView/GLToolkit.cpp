#include "DCView/GLToolkit.h"
using namespace DCView;

//DCCore
#include "DCCore/DCGL.h"
#include "DCCore/Algorithm.h"

void GLToolkit::RenderTexture2D(GLuint uTextureID, int iWidth, int iHeight)
{
	//启用纹理
	glEnable(GL_TEXTURE_2D);
	//绑定纹理
	glBindTexture(GL_TEXTURE_2D, uTextureID);

	float halfW = (float)iWidth*0.5f;
	float halfH = (float)iHeight*0.5f;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-halfW, -halfH);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f( halfW, -halfH);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f( halfW,  halfH);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-halfW,  halfH);
	glEnd();

	//取消纹理绑定
	glBindTexture(GL_TEXTURE_2D, 0);
	//停用纹理
	glDisable(GL_TEXTURE_2D);
}

void GLToolkit::RenderTexture2DCorner(GLuint uTextureID, int iWidth, int iHeight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, uTextureID);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex2i(0, 0);
	glTexCoord2f(1.0, 0.0);
	glVertex2i(iWidth, 0);
	glTexCoord2f(1.0, 1.0);
	glVertex2i(iWidth, iHeight);
	glTexCoord2f(0.0, 1.0);
	glVertex2i(0, iHeight);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

//*********** OPENGL EXTENSIONS ***********//
bool GLToolkit::InitGlew()
{
	// GLEW initialization
	glewExperimental = GL_TRUE;
	GLenum code = glewInit();
	if(code != GLEW_OK)
		return false;

	return true;
}

//扩展是否有效
bool GLToolkit::IsValidExtension(const char* pExtName)
{
	if (!InitGlew())
		return false;

	return (glewIsExtensionSupported(pExtName) > 0);
}

//检测shader的可用性
bool GLToolkit::CheckShadersAvailability()
{
	return IsValidExtension("GL_ARB_shading_language_100") &&
		IsValidExtension("GL_ARB_shader_objects") &&
		IsValidExtension("GL_ARB_vertex_shader") &&
		IsValidExtension("GL_ARB_fragment_shader");
}

//检测FBO的可用性
bool GLToolkit::CheckFBOAvailability()
{
	return IsValidExtension("GL_EXT_framebuffer_object");
}

//捕获最近的OpenGL错误
QString GLToolkit::CatchError()
{
	//获取OpenGL错误代码
	GLenum errorCode = glGetError();

	//错误信息
	QString strError;

	//see http://www.opengl.org/sdk/docs/man/xhtml/glGetError.xml
	switch(errorCode)
	{
	case GL_NO_ERROR:
		strError = "";
		break;
	//枚举值无效
	case GL_INVALID_ENUM:
		strError = tr("OpenGL error: invalid enumerator");
		break;
	//值无效
	case GL_INVALID_VALUE:
		strError = tr("OpenGL error: invalid value");
		break;
	//无效操作
	case GL_INVALID_OPERATION:
		strError = tr("OpenGL error: invalid operation");
		break;
	//栈溢出
	case GL_STACK_OVERFLOW:
		strError = tr("OpenGL error: stack overflow");
		break;
	//栈下溢
	case GL_STACK_UNDERFLOW:
		strError = tr("OpenGL error: stack underflow");
		break;
	//内存不足
	case GL_OUT_OF_MEMORY:
		strError = tr("OpenGL error: out of memory");
		break;
	//无效的帧缓冲对象
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		strError = tr("OpenGL error: invalid framebuffer operation");
		break;
	default:
		break;
	}

	return strError;
}

//计算旋转矩阵
Matrix_4_4 GLToolkit::CalculateRotationMatrix(const Vector_3& vSource, 
	const Vector_3& vDest)
{
	//we compute scalar prod between the two vectors
	float ps = vSource.Dot(vDest);

	//we bound result (in case vecors are not exactly unit)
	if (ps > 1.0f)
		ps = 1.0f;
	else if (ps < -1.0f)
		ps = -1.0f;

	//计算旋转角度
	float fAngle = DCCore::RadianToAngle(acos(ps));

	//计算旋转轴
	Vector_3 vAxis = vSource.Cross(vDest);

	//根据坐标轴和旋转角度计算旋转矩阵
	return CalculateRotationMatrix(vAxis, fAngle);
}
Matrix_4_4 GLToolkit::CalculateRotationMatrix(const Vector_3& vAxis, 
	float fAngle)
{
	//we backup actual matrix...
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	glRotatef(fAngle, vAxis.x, vAxis.y, vAxis.z);
	Matrix_4_4 matrix;
	glGetDoublev(GL_MODELVIEW_MATRIX, matrix.data());

	//... and restore it
	glPopMatrix();

	return matrix;
}

//生成视图矩阵
Matrix_4_4 GLToolkit::GenerateViewMatrix(ViewMode mode)
{
	GLdouble eye[3] = {0.0, 0.0, 0.0};
	GLdouble top[3] = {0.0, 0.0, 0.0};

	//we look at (0,0,0) by default
	switch (mode)
	{
	case eTopViewMode:
		eye[2] = 1.0;
		top[1] = 1.0;
		break;
	case eBottomViewMode:
		eye[2] = -1.0;
		top[1] = -1.0;
		break;
	case eFrontViewMode:
		eye[1] = -1.0;
		top[2] = 1.0;
		break;
	case eBackViewMode:
		eye[1] = 1.0;
		top[2] = 1.0;
		break;
	case eLeftViewMode:
		eye[0] = -1.0;
		top[2] = 1.0;
		break;
	case eRightViewMode:
		eye[0] = 1.0;
		top[2] = 1.0;
		break;
	}

	Matrix_4_4 matrix;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(eye[0], eye[1] ,eye[2], 0.0, 0.0, 0.0, top[0], top[1], top[2]);
	glGetDoublev(GL_MODELVIEW_MATRIX, matrix.data());
	matrix.data()[14] = 0.0; //annoying value (?!)
	glPopMatrix();

	return matrix;
}