#include "UI_Console.hxx"
using namespace DCConsole;

MessageBrower::MessageBrower(QWidget* pParent /* = nullptr */)
	: QTextEdit(pParent)
{
	////设置固定高度
	//setMinimumHeight(document()->size().height());

	////连接文档内容变化的信号
	//connect(document(), SIGNAL(contentsChanged()),
	//	this, SLOT(UpdateHeight()));
}

//更新高度
void MessageBrower::UpdateHeight()
{
	//检测高度是否变化
	QTextDocument* pDoc = dynamic_cast<QTextDocument*>(sender());
	//文档高度
	int heightDoc = pDoc->size().height();

	//高度发生变化则重置高度
	if (heightDoc != height())
	{
		setMinimumHeight(heightDoc);
	}
}

void MessageBrower::ClearMessage()
{
	clear();
}