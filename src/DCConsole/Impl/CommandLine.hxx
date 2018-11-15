#ifndef DC_CONSOLE_COMMANDLINE_H
#define DC_CONSOLE_COMMANDLINE_H

//Qt
#include "QString"
#include "QtWidgets/QLineEdit"

namespace DCConsole
{
	//命令行
	class CommandLine : public QLineEdit
	{
		Q_OBJECT

	public:
		explicit CommandLine(QWidget* pParent = nullptr);

		//设置命令
		void SetCommand(const QString& strCmd);

signals:
		//执行命令
		void Execute(const QString& strCmd);

		//焦点进入
		void FocusEntered();
		//焦点移出
		void FocusRemoved();

		//向上查找命令
		void SearchUp();
		//向下查找
		void SearchDown();

		//取消
		void Cancel();

		private slots:
			//解析命令
			void ParseCommand();
			void ParseCommand(const QString& strCmd);

	protected:
		//鼠标按键事件
		virtual void keyPressEvent(QKeyEvent* event);
		//焦点进入事件
		virtual void focusInEvent(QFocusEvent* event);
		//焦点移出事件
		virtual void focusOutEvent(QFocusEvent* event);
		//鼠标移出事件
		virtual void leaveEvent(QEvent* event);
	};
}

#endif