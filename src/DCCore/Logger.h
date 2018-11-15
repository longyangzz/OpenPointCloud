#ifndef DCCORE_LOGGER_H
#define DCCORE_LOGGER_H

#include "DCCoreDLL.h"

//Qt
#include "QString"

namespace DCCore
{
	#define LOG_NAME "DCLWLog"
	//日志记录器
	class DCCORE_EXPORT Logger
	{
	public:

		//析构函数
		virtual ~Logger() {};

		//返回一个唯一的日志
		static Logger* Instance();

		//注册日志
		static void Register(Logger* pInstance);

		//输出正常信息
		static void Message(const QString& rsMsg);
		static void MessageDebug(const QString& rsMsg);

		//输出警告信息
		static void Warning(const QString& rsWarning);
		static void WarningDebug(const QString& rsWarning);

		//输出错误信息
		static void Error(const QString& rsError);
		static void ErrorDebug(const QString& rsError);

		//! 日志文件的文件管理
		static void CreateNewLogFile(const QString logFile = LOG_NAME);

		static void Writeln(const QString aValue);

	protected:

		//信息级别
		enum LogLevel
		{
			eMessageLog				=	0x00001000	,
			eWarningLog				=	0x00002000	,
			eErrorLog				=	0x00004000
		};

		//显示信息
		void virtual ShowMessage(const QString& msg, LogLevel level = eMessageLog) = 0;

		static QString logFileName;
	};
}
#endif
