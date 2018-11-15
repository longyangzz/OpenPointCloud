#include "DCView/GLShader.h"
using namespace DCView;

//C++±ê×¼¿â
#include <assert.h>
#include <string.h>
#include <stdio.h>

//Qt
#include "QtCore/QString"
#include "QtCore/QFile"
#include "QtCore/QByteArray"

#include "DCCore/DCGL.h"

GLShader::GLShader()
{
    prog = 0;
}

GLShader::~GLShader()
{
    Reset();
}

void GLShader::Reset()
{
    if (prog>0)
        glDeleteProgram(prog);
    prog = 0;
}

unsigned GLShader::GetProgram()
{
    return prog;
}

void GLShader::Start()
{
    glUseProgram(prog);
}

void GLShader::Stop()
{
    glUseProgram(0);
}

bool GLShader::FromFile(const QString& shaderBasePath, const QString& shaderBaseFilename)
{
	if (shaderBasePath.isEmpty() || shaderBaseFilename.isEmpty())
	{
		//error = "Missing input argument for ccShader::fromFile";
		return false;
	}

	QString vertFilename = QString("%1/%2.vert").arg(shaderBasePath).arg(shaderBaseFilename);
	QString fragFilename = QString("%1/%2.frag").arg(shaderBasePath).arg(shaderBaseFilename);

	return LoadProgram(vertFilename,fragFilename);
}
//*/

bool GLShader::LoadProgram(const QString& vertexShaderFile, const QString& pixelShaderFile)
{
    Reset();
    assert(prog == 0);

    //GL ids
    GLuint vs = 0, ps = 0;

    //we load shader files
    if(!vertexShaderFile.isEmpty())
    {
        vs = LoadShader(GL_VERTEX_SHADER, vertexShaderFile);
        if(!vs)
            return false;
    }
    if(!pixelShaderFile.isEmpty())
    {
        ps = LoadShader(GL_FRAGMENT_SHADER, pixelShaderFile);
        if(!ps)
        {
            if(glIsShader(vs))
                glDeleteShader(vs);
            return false;
        }
    }

    //we create an empty GL program
    prog = glCreateProgram();

    //we attach loaded shaders to it
    if(vs)
        glAttachShader(prog, vs);
    if(ps)
        glAttachShader(prog, ps);

    //we link them alltogether
    glLinkProgram(prog);

    //we check for success
    GLint linkStatus = GL_TRUE;
    glGetProgramiv(prog, GL_LINK_STATUS, &linkStatus);
    if(linkStatus != GL_TRUE)
    {
        GLint logSize = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logSize);
        char* log = new char[logSize+1];
        if(log)
        {
            memset(log, '\0', logSize + 1);
            glGetProgramInfoLog(prog, logSize, &logSize, log);

            //ccConsole::Error("Can't create shader program! (%s)", log);

            delete[] log;
        }
        //else ccConsole::Error("Can't create shader program! (unable to get GL log)");

        glDeleteProgram(prog);
        prog = 0;
    }

    // even if program creation was successful, we don't need the shaders anymore
    if(vs)
        glDeleteShader(vs);
    if(ps)
        glDeleteShader(ps);

    return true;
}

QByteArray GLShader::ReadShaderFile(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QFile::ReadOnly))
	{
		//failed to open the file
		return QByteArray();
	}

	return file.readAll();
}

GLuint GLShader::LoadShader(GLenum type, const QString& filename)
{
	//load shader file
	QByteArray shaderFileContent = ReadShaderFile(filename);
	if (shaderFileContent.isEmpty())
	{
		return 0;
	}

	//create shader
	GLuint shader = glCreateShader(type);
	if (!glIsShader(shader))
	{
		return 0;
	}

	const GLchar* shaderStr = static_cast<const GLchar*>(shaderFileContent.data());
	glShaderSource(shader, 1, &shaderStr, NULL);

	glCompileShader(shader);

	//we must check compilation result
	GLint compileStatus = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if(compileStatus != GL_TRUE)
	{
		GLint infoLogLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* strInfoLog = new GLchar[infoLogLength+1];
		if (strInfoLog)
		{
			glGetShaderInfoLog(shader, infoLogLength, 0, strInfoLog);
			delete[] strInfoLog;
			strInfoLog = 0;
		}
		else
		{
		}

		glDeleteShader(shader);
		shader = 0;
	}

	return shader;
}

void GLShader::SetUniform1i(int loc, int value)
{
	glUniform1i(loc,value);
}

void GLShader::SetUniform1f(int loc, float value)
{
	glUniform1f(loc,value);
}

void GLShader::SetUniform4fv(int loc, float* value)
{
	glUniform4fv(loc, 1, value);
}

void GLShader::SetUniform1i(const char* uniform, int value)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform1i(loc,value);
}

void GLShader::SetUniform2iv(const char* uniform, int* val)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform2iv(loc,1,val);
}

void GLShader::SetUniform3iv(const char* uniform, int* val)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform3iv(loc,1,val);
}

void GLShader::SetUniform4iv(const char* uniform, int* val)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform4iv(loc,1,val);
}

void GLShader::SetTabUniform4iv(const char* uniform, int size, int* val)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform4iv(loc,size,val);
}

void GLShader::SetUniform1f(const char* uniform, float value)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform1f(loc,value);
}

void GLShader::SetUniform2fv(const char* uniform, float* val)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform2fv(loc,1,val);
}

void GLShader::SetUniform3fv(const char* uniform, float* val)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform3fv(loc,1,val);
}

void GLShader::SetUniform4fv(const char* uniform, float* val)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform4fv(loc,1,val);
}

void GLShader::SetTabUniform1fv(const char* uniform, int size, float* val)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform1fv(loc,size,val);
}

void GLShader::SetTabUniform2fv(const char* uniform, int size, float* val)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform2fv(loc,size,val);
}

void GLShader::SetTabUniform3fv(const char* uniform, int size, float* val)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform3fv(loc,size,val);
}

void GLShader::SetTabUniform4fv(const char* uniform, int size, float* val)
{
	int loc = glGetUniformLocation(prog,uniform);
	glUniform4fv(loc,size,val);
}

void GLShader::SetUniformMatrix4fv(const char* uniform, float* val, bool transpose)
{
	int loc	= glGetUniformLocation(prog,uniform);
	glUniformMatrix4fv(loc,1,transpose,val);
}

void GLShader::SetAttrib4iv(int loc, int* val)
{
	glVertexAttrib4iv(loc, val);
}

void GLShader::SetAttrib1f(int loc, float value)
{
	glVertexAttrib1f(loc,value);
}

void GLShader::SetAttrib2fv(int loc, float* val)
{
	glVertexAttrib2fv(loc, val);
}

void GLShader::SetAttrib3fv(int loc, float* val)
{
	glVertexAttrib3fv(loc, val);
}

void GLShader::SetAttrib4fv(int loc, float* val)
{
	glVertexAttrib4fv(loc, val);
}