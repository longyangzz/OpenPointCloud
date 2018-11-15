#ifndef DC_CONSOLE_UI_CONSOLE_H
#define DC_CONSOLE_UI_CONSOLE_H

//Qt
#include "QtWidgets/QLabel"
#include "QtWidgets/QLineEdit"
#include "QtWidgets/QPlainTextEdit"
#include "QtWidgets/QToolButton"
#include "QtWidgets/QGridLayout"
#include "QtWidgets/QHBoxLayout"
#include "QtWidgets/QSpacerItem"

//Self
#include "CommandLine.hxx"

namespace DCConsole
{
	class MessageBrower : public QTextEdit
	{
		Q_OBJECT
	public:
		explicit MessageBrower(QWidget* pParent = nullptr);

		private slots:
			//更新高度
			void UpdateHeight();

			void ClearMessage();
	};

	class UI_Console
	{
	public:
		//命令输入栏
		CommandLine* cmdLine;
		//命令提示栏
		QLabel* cmdPrompt;
		//命令图标
		QToolButton* tbtnIcon;

		//清空图标
		QToolButton* tbtnClear;

		//命令消息框
		MessageBrower* msgBrower;

		void SetupUi(QWidget* pWidget)
		{
			//创建栅格布局管理器
			QGridLayout* pGrid = new QGridLayout(pWidget);
			pGrid->setContentsMargins(0, 0, 0, 0);
			pGrid->setSpacing(0);

			//创建水平布局管理器
			{
				QHBoxLayout* pHlyt = new QHBoxLayout;
				
				//创建清空toolbutton
				{
					tbtnClear = new QToolButton;
					tbtnClear->setIcon(QIcon(":/image/Resources/clear.png"));

					//设置固定大小
					tbtnClear->setFixedSize(30, 20);
					//设置背景色
					tbtnClear->setStyleSheet("background-color: rgb(188, 199, 216);");
					//设置大小样式
					QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
					sizePolicy.setHorizontalStretch(0);
					sizePolicy.setVerticalStretch(0);
					tbtnClear->setSizePolicy(sizePolicy);

					//设置为平坦
					tbtnClear->setAutoRaise(true);

					//添加到水平布局管理器
					pHlyt->addWidget(tbtnClear);

					QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
				
					pHlyt->addItem(horizontalSpacer);
				}

				//将水平布局管理器添加到栅格布局管理器
				pGrid->addLayout(pHlyt, 0, 0);
			}
			

			//创建命令消息框
			{
				msgBrower = new MessageBrower;
				//不可编辑
				msgBrower->setReadOnly(true);
				//设置大小样式
				msgBrower->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

				pGrid->addWidget(msgBrower, 1, 0);
			}


			//创建水平布局管理器
			{
				QHBoxLayout* pHlyt = new QHBoxLayout;

				//创建命令图标
				{
					tbtnIcon = new QToolButton;
					//设置为弹出菜单模式
					tbtnIcon->setPopupMode(QToolButton::MenuButtonPopup);
					//设置固定大小
					tbtnIcon->setFixedSize(30, 20);
					//设置为平坦
					tbtnIcon->setAutoRaise(true);

					//添加到水平布局管理器
					pHlyt->addWidget(tbtnIcon);
				}

				//创建命令提示栏
				{
					cmdPrompt = new QLabel;
					//设置固定高度
					cmdPrompt->setFixedHeight(20);
					//设置文本格式
					cmdPrompt->setTextFormat(Qt::RichText);
					//设置交互模式
					cmdPrompt->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
					//设置默认为隐藏
					//cmdPrompt->setVisible(false);
					//设置背景色为白色
					cmdPrompt->setStyleSheet("background-color: rgb(255, 255, 255);");
					//设置大小样式
					cmdPrompt->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

					//添加到水平布局管理器
					pHlyt->addWidget(cmdPrompt);
				}

				//创建命令输入栏
				{
					cmdLine = new CommandLine;
					//设置固定高度
					cmdLine->setFixedHeight(20);
					//设置无框架
					cmdLine->setFrame(false);
					//设置大小样式
					cmdLine->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

					//添加到水平布局管理器
					pHlyt->addWidget(cmdLine);
				}

				//将水平布局管理器添加到栅格布局管理器
				pGrid->addLayout(pHlyt, 2, 0);
			}
		}
	};
}

#endif