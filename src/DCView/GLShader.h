#ifndef DC_VIEW_GLSHADER_H
#define DC_VIEW_GLSHADER_H

#include "ViewDLL.h"

//Qt
class QString;

namespace DCView
	{
		class DC_VIEW_API GLShader
		{
		public:
			//构造函数
			GLShader();
			//析构函数
			virtual ~GLShader();

			//
			virtual bool FromFile(const QString& shaderBasePath, const QString& shaderBaseFilename);

			//加载程序
			virtual bool LoadProgram(const QString& vertShaderFile, const QString& fragShaderFile);

			//重置
			virtual void Reset();

			//启动
			virtual void Start();
			//停止
			virtual void Stop();

			//获取着色程序
			unsigned GetProgram();


			virtual void SetUniform1i(int loc, int value);
			virtual void SetUniform1f(int loc, float value);
			virtual void SetUniform4fv(int loc, float* value);

			virtual void SetUniform1i(const char* variable, int val);
			virtual void SetUniform2iv(const char* variable, int* p_val);
			virtual void SetUniform3iv(const char* variable, int* p_val);
			virtual void SetUniform4iv(const char* variable, int* p_val);
			virtual void SetTabUniform4iv(const char* uniform, int size, int* p_val);

			virtual void SetUniform1f(const char* variable, float val);
			virtual void SetUniform2fv(const char* variable, float* p_val);
			virtual void SetUniform3fv(const char* variable, float* p_val);
			virtual void SetUniform4fv(const char* variable, float* p_val);
			virtual void SetTabUniform1fv(const char* uniform, int size, float* p_val);
			virtual void SetTabUniform2fv(const char* uniform, int size, float* p_val);
			virtual void SetTabUniform3fv(const char* uniform, int size, float* p_val);
			virtual void SetTabUniform4fv(const char* uniform, int size, float* p_val);
			virtual void SetUniformMatrix4fv(const char* variable, float* p_val, bool transpose = false);

			virtual void SetAttrib4iv(int loc, int* p_val);
			virtual void SetAttrib1f(int loc, float val);
			virtual void SetAttrib2fv(int loc, float* p_val);
			virtual void SetAttrib3fv(int loc, float* p_val);
			virtual void SetAttrib4fv(int loc, float* p_val);

		protected:

			//从文件中加载
			static unsigned LoadShader(unsigned type, const QString& filename);

			//从内存中读取
			static QByteArray ReadShaderFile(const QString& filename);

			unsigned prog;
		};
	}
#endif
