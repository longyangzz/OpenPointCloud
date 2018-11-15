#include "CommandLine.hxx"
using namespace DCConsole;

//Qt
#include "QtGui/QKeyEvent"
#include "QtGui/QFocusEvent"

CommandLine::CommandLine(QWidget* pParent /* = nullptr */)
	: QLineEdit(pParent)
{

	//创建信号与槽的连接
	connect(this, SIGNAL(returnPressed()),
		this, SLOT(ParseCommand()));
	connect(this, SIGNAL(textChanged(const QString&)),
		this, SLOT(ParseCommand(const QString&)));
}

//设置命令
void CommandLine::SetCommand(const QString& strCmd)
{
	setText(strCmd);
}

//解析命令
void CommandLine::ParseCommand()
{
	//获取命令内容
	QString strCmd = text();

	//如果命令不为空，则执行命令
	//清空命令
	clear();

	//发送命令
	emit Execute(strCmd);
}

void CommandLine::ParseCommand(const QString& strCmd)
{
	//判断命令是否以空格结束，若是则执行命令
	if (strCmd.contains(" "))
	{
		//清空命令
		clear();

		//发送命令
		QString strCmd1 = strCmd;
		strCmd1.remove(" ");
		emit Execute(strCmd1);
	}
}

//鼠标按键事件
void CommandLine::keyPressEvent(QKeyEvent* event)
{
	//判断是否按下Esc，如果按下，则发送取消信号
	if (event->key() == Qt::Key_Escape)
	{
		emit Cancel();
	}
	//向上查找
	else if (event->key() == Qt::Key_Up)
	{
		emit SearchUp();
	}
	//向下查找
	else if (event->key() == Qt::Key_Down)
	{
		emit SearchDown();
	}

	QLineEdit::keyPressEvent(event);
}

//焦点进入事件
void CommandLine::focusInEvent(QFocusEvent* event)
{
	//发送焦点进入信号
	emit FocusEntered();

	QLineEdit::focusInEvent(event);
}

//焦点移出事件
void CommandLine::focusOutEvent(QFocusEvent* event)
{
	//发送焦点移出的信号
	emit FocusRemoved();

	QLineEdit::focusOutEvent(event);
}

//鼠标移出事件
void CommandLine::leaveEvent(QEvent* event)
{
	//清除焦点
	//clearFocus();

	//发送焦点移出的信号
	//emit FocusRemoved();

//	QLineEdit::leaveEvent(event);
}