//C++标准库
#include <algorithm>

//Qt标准库;
#include <QFile>
#include <QTextStream>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QScrollBar>
#include <QRegExp>
#include "QtGui/QColor"

#include "DlgOpenAsciiFile.h"

DataTypePerColumn DlgOpenAsciiFile::dataHeaders[12] = {IGNORED, COORD_X, COORD_Y, COORD_Z, 
	RGB_RED, RGB_GREEN, RGB_BLUE, GRAY, SCALAR, NORMAL_X, NORMAL_Y, NORMAL_Z};

DlgOpenAsciiFile::DlgOpenAsciiFile(QString fileName, QWidget *parent)
	: QDialog(parent)
	, m_fileName(fileName)
	, m_separator(',')
	, m_columnNumber(0)
	, m_readLines(0)
	, m_skippedLines(0)
	, m_maxCloudSize(3.0)
	, m_isSettedColumnType(false)
{
	ui.setupUi(this);

	ui.pushBtnOk->setFocus();

	//设置图标
	/*QIcon icon;
	icon.addFile(QString::fromUtf8(":/MainWindow/Resources/images/lpcp.ico"), QSize(), QIcon::Normal, QIcon::Off);
	this->setWindowIcon(icon);*/

	//设置文件名文本框的值
	ui.txtFileName->setText(m_fileName);
	
	//设置默认分隔符列表
	QList<QChar> separators;
	separators << QChar(' ');
	separators << QChar('\t');
	separators << QChar(',');
	separators << QChar(';');

	//尝试所有的默认分隔符，匹配文件中的分隔符
	for (unsigned i = 0; i < separators.size(); ++i)
	{
		ui.txtSeparator->setText(separators[i]); //此过程会刷新表格
		//直到表格的列数超过2为止，因为坐标最低为两维
		if (GetDataColumnNumber() > 2)
		{
			
			break;
		}
	}
}

DlgOpenAsciiFile::~DlgOpenAsciiFile()
{

}

//生成文本预览表格的内容;
//第一个参数为要添加内容的表格窗体，第二个参数是要添加的文本内容;
//第三个是文本的分隔符;
void DlgOpenAsciiFile::AddDataToTable(QStringList lines,
	QChar separator)
{
	bool isValid = true;
	//从第一行开始添加文本，因为第一行显示选择下拉列表;
	for (int i = 0; i < lines.size(); ++i)
	{
		//使用分隔符分割每行文本;
		QStringList cellStings = lines[i].trimmed().split(separator, QString::SkipEmptyParts);

		//将文本添加到文本预览表格的单元格中;
		for (int j = 0; j < cellStings.size(); ++j)
		{
			//创建表格条目对象;
			QTableWidgetItem *item = new QTableWidgetItem(cellStings[j], QTableWidgetItem::Type);

			//设置背景色，隔行换色，突出显示;
			if (i % 2 == 0)
			{
				item->setBackgroundColor(QColor(180, 180, 180));
			}

			//如果文本块存在非法字符，则设置背景为红色;
			//非法字符为除数字0-9和小数点之外的所有字符;
			if (!cellStings[j].contains(QRegExp("^\-?[0-9]*\.[0-9]*(e|E)?\-?[0-9]*$")) && m_header[j] != IGNORED)
			{
				isValid = false;
				item->setBackgroundColor(QColor(255, 0, 0));
			}

			//将表格条目添加到单元格中;
			ui.tblAsciiFile->setItem(i + 1, j, item);
		}
	}
	//设置确定按钮的可操作性
	ui.pushBtnOk->setEnabled(isValid);
}

//为文本预览表格窗体添加数据类型选择的下拉列表;
//参数为文本数据的列数;
void DlgOpenAsciiFile::AddDataTypeComboBox(int columnNumber)
{
	m_dataType.clear();
	//每个下拉列表的对象名
	QStringList typeList = QStringList() << tr("Ignore") << tr("XCoord") << tr("YCoord") << tr("ZCoord")
							<< tr("Red") << tr("Green") << tr("Blue") << tr("Gray") << tr("XNORMAL") << tr("YNORMAL") << tr("ZNORMAL");
	for (unsigned i = 11; i < columnNumber; ++i)
	{
		typeList.push_back(tr("Scalar"));
	}
	//通过循环创建选择下拉列表;
	for (int i = 0; i < columnNumber; ++i)
	{
		//创建表格中数据结构选择下拉列表;
		QComboBox *dataType = new QComboBox;

		//下拉列表中添加内容;
		dataType->addItem(tr("Ignore"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/xCoord.png"), tr("XCoord"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/yCoord.png"), tr("YCoord"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/zCoord.png"), tr("ZCoord"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/rgb.png"), tr("Red"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/rgb.png"), tr("Green"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/rgb.png"), tr("Blue"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/gray.png"), tr("Gray"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/scalar.png"), tr("Scalar"), Qt::DisplayRole);

		dataType->addItem(QIcon(":/logo/Resource/xnormal.png"), tr("XNORMAL"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/logo/Resource/ynormal.png"), tr("YNORMAL"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/logo/Resource/znormal.png"), tr("ZNORMAL"), Qt::DisplayRole);

		//设置默认值
		dataType->setCurrentIndex(m_header[i]);
		dataType->setObjectName(typeList[i]);

		//将下拉列表添加到单元格中;
		ui.tblAsciiFile->setCellWidget(0, i, dataType);

		//连接信号与槽;
		connect(dataType, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeColumnType(int)));

		m_dataType.push_back(dataType);

	}
}

//改变列数据类型
void DlgOpenAsciiFile::ChangeColumnType(int index)
{
	QObject* objectSender = sender();

	if (!objectSender)
	{
		return;
	}
	//转换为QComboBox
	QComboBox* cmbxSender = static_cast<QComboBox*>(objectSender);
	//判断是否出现冲突，与存在的类型重复
	//如果重复则进行交换
	auto repetition = std::find_if(m_dataType.cbegin(), m_dataType.cend(), [cmbxSender](QComboBox* cmbxTemp)
							{return (cmbxTemp->currentIndex() == cmbxSender->currentIndex() && cmbxTemp != cmbxSender);});
	//查找当前发送信号的对象在容器中的位置
	unsigned pos = 0;
	std::find_if(m_dataType.cbegin(), m_dataType.cend(), [cmbxSender, &pos](QComboBox* cmbxTemp)->bool
		{++pos;return (cmbxTemp == cmbxSender);});
	//交换重复的类型，如果选择的是忽略则不交换
	if (repetition != m_dataType.cend() && (*repetition)->currentIndex() != IGNORED && (*repetition)->currentIndex() != SCALAR)
	{
		(*repetition)->setCurrentIndex(m_header[pos - 1]);
	}
	//更新列数据类型
	//先清空数据
	m_header.clear();
	//再更新数据
	for (auto i = m_dataType.cbegin(); i != m_dataType.cend(); ++i)
	{
		m_header.push_back(dataHeaders[(*i)->currentIndex()]);
	}

	m_isSettedColumnType = true;

	UpdateTable();
}

//点击分隔符按钮
void DlgOpenAsciiFile::ClickedSeparator()
{
	/*if (!m_columnsCount)
		return;*/

	//we get the signal sender
	QObject* obj = sender();
	if (!obj)
		return;

	//it should be a QToolButton (could we test this?)
	QToolButton* btnSeparator = static_cast<QToolButton*>(obj);

	uchar newSeparator = 0;
	if (btnSeparator == ui.toolBtnSpace)
		newSeparator = uchar(32);
	else if (btnSeparator == ui.toolBtnTab)
		newSeparator = uchar(9);
	else if (btnSeparator == ui.toolBtnComma)
		newSeparator = uchar(44);
	else if (btnSeparator == ui.toolBtnSemicolon)
		newSeparator = uchar(59);

	if (newSeparator > 0 && GetSeparator() != newSeparator)
		ui.txtSeparator->setText(QChar(newSeparator));
}

//获取文本;
QStringList DlgOpenAsciiFile::LoadDataFromFile()
{
	//获取文件设备()
	QFile file(m_fileName);

	//打开文件;
	file.open(QIODevice::ReadOnly | QIODevice::Text);

	QTextStream in(&file);
	//文件中的数据
	QStringList fileData;
	//记录读入数据的字节数
	unsigned number = 500;
	//通过计数器忽略跳过的行数
	unsigned count = 0;
	while(!in.atEnd())
	{
		if (++count <= m_skippedLines)
		{
			QString line = in.readLine();
			continue;
		}
		//最多预读500行数据
		if (fileData.size() == MAX_LINES_OF_ASCII_FILE)
		{
			break;
		}
		//读取文件的每行内容;
		QString line = in.readLine();
		number = line.size() > number ? number : line.size();

		fileData.push_back(line);
	}

	//
	m_readLines = file.size() / number;

	//关闭文件;
	file.close();

	//通过文件的行数设置最大可跳过的数目
	//确保最终至少能保留一行数据
	ui.spinBoxSkipLines->setMaximum(count - 1);

	//返回数据
	return fileData;
}

//分析数据的列数;
unsigned DlgOpenAsciiFile::GetDataColumnNumber(const QStringList& lines, const QChar& separator)
{
	unsigned columnNumber = 0;
	unsigned count = 0;
	for (auto i = lines.constBegin(); i != lines.constEnd(); ++i, ++count)
	{
		//最多分析前50行
		if (count == 50)
		{
			break;
		}
		//将文本使用分隔符分离，先去除数据两端的空格;
		QStringList list = (*i).trimmed().split(separator, QString::SkipEmptyParts);
		//columnNumber = MAX(columnNumber, list.count());
		columnNumber = columnNumber > list.count() ? columnNumber : list.count();
	}

	//返回数据的列数;
	return columnNumber;
}

//获取数据列类型
DataColumnType DlgOpenAsciiFile::GetDataColumnType()
{
	DataColumnType header;

	//确保列数不为0
	if (m_columnNumber > 0)
	{
		//循环确定每个下拉列表的值
		for(int i = 0; i < m_columnNumber; ++i)
		{
			//获取下拉列表
			QComboBox* cur = static_cast<QComboBox*>(ui.tblAsciiFile->cellWidget(0, i));
			
			if (IGNORED == cur->currentIndex())
			{
			}
			else if (COORD_X == cur->currentIndex())
			{
				header.xCoordIndex = i;
				++header.m_validTypeNumbers;
			} 
			else if (COORD_Y == cur->currentIndex())
			{
				header.yCoordIndex = i;
				++header.m_validTypeNumbers;
			}
			else if (COORD_Z == cur->currentIndex())
			{
				header.zCoordIndex = i;
				++header.m_validTypeNumbers;
			}
			else if (RGB_RED == cur->currentIndex())
			{
				header.rgbRedIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasRGBColor = true;
			}
			else if (RGB_GREEN == cur->currentIndex())
			{
				header.rgbGreenIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasRGBColor = true;
			}
			else if (RGB_BLUE == cur->currentIndex())
			{
				header.rgbBlueIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasRGBColor = true;
			}
			else if (GRAY == cur->currentIndex())
			{
				header.grayIndex = i;
				++header.m_validTypeNumbers;
			}
			else if (SCALAR == cur->currentIndex())
			{
				header.scalarIndex.push_back(i);
				++header.m_validTypeNumbers;
			}
			else if (NORMAL_X == cur->currentIndex())
			{
				header.xNormalIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasNormals = true;
			}
			else if (NORMAL_Y == cur->currentIndex())
			{
				header.yNormalIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasNormals = true;
			}
			else if (NORMAL_Z == cur->currentIndex())
			{
				header.zNormalIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasNormals = true;
			}

		}
	}
	

	return header;
}

//获取文本的分隔符()
QChar DlgOpenAsciiFile::GetSeparator(const QString& line)
{
	//读取字符串中每个字符的地址;
	QChar *ch = const_cast<QString&>(line).data();

	//定义标识，如果发现非数字(不包括小数点)，则存在分隔符;
	//否则不存在分隔符;
	bool flag = false;

	while(!ch->isNull())
	{
		//判断字符是否是数字或小数点或负号或空字符;
		if (ch->isNumber() || *ch == QChar(45)/*负号;*/ 
			|| *ch == QChar(46)/*小数点;*/ || *ch == QChar(0)/*'\0'*/)
		{
			flag = false;
		} 
		else
		{
			flag = true;
			break;
		}

		//将指针移到存放下一个字符地址;
		++ch;
	}

	if (flag)
	{
		//返回寻找到的分隔符;
		return *ch;
	} 
	else
	{
		//默认逗号为分隔符()
		return ',';
	}
}

//设置默认的列数据类型
void DlgOpenAsciiFile::SetDefaultColumnType()
{
	//情况列数据类型
	m_header.clear();

	//分析默认的列数据类型
	switch (GetDataColumnNumber())
	{
		case 1:
			m_header.push_back(IGNORED);
			break;
		case 2:
			m_header.push_back(COORD_X);
			m_header.push_back(COORD_Y);
			break;
		case 3:
			m_header.push_back(COORD_X);
			m_header.push_back(COORD_Y);
			m_header.push_back(COORD_Z);
			break;
		case 6:
			m_header.push_back(COORD_X);
			m_header.push_back(COORD_Y);
			m_header.push_back(COORD_Z);
			m_header.push_back(RGB_RED);
			m_header.push_back(RGB_GREEN);
			m_header.push_back(RGB_BLUE);
			break;
// 		case 4:
// 			m_header.push_back(COORD_X);
// 			m_header.push_back(COORD_Y);
// 			m_header.push_back(COORD_Z);
// 			m_header.push_back(SCALAR);
// 			break;
// 		case 5:
// 			m_header.push_back(COORD_X);
// 			m_header.push_back(COORD_Y);
// 			m_header.push_back(COORD_Z);
// 			m_header.push_back(SCALAR);
// 			m_header.push_back(SCALAR);
// 			break;
		default:
			m_header.push_back(COORD_X);
			m_header.push_back(COORD_Y);
			m_header.push_back(COORD_Z);
// 			m_header.push_back(RGB_RED);
// 			m_header.push_back(RGB_GREEN);
// 			m_header.push_back(RGB_BLUE);
			for (unsigned i = 3; i != GetDataColumnNumber(); ++i)
			{
				//m_header.push_back(SCALAR);
				m_header.push_back(IGNORED);
			}
			break;
	}
}

//设置最大点数
void DlgOpenAsciiFile::SetMaxCloudSize()
{
	m_maxCloudSize = ui.dblSpinBoxMaxPoints->value();
}

//设置分隔符
void DlgOpenAsciiFile::SetSeparator()
{
	//设置分隔符
	//只有当分隔符变化时才更新窗体，并将数据类型设置为默认
	if (m_separator != ui.txtSeparator->text()[0])
	{
		m_separator = ui.txtSeparator->text()[0];
		m_isSettedColumnType = false;
		//显示分隔符的ASCII码
		ui.lblAsciiCode->setText(tr("ASCII code %1").arg(int(m_separator.toLatin1())));
		//刷新表
		UpdateTable();
	}
}

//设置跳过的行数
void DlgOpenAsciiFile::SetSkippedLines()
{
	m_skippedLines = ui.spinBoxSkipLines->value();
	m_readLines -= m_skippedLines;
	//更新表数据
	UpdateTable();
}

//添加文本预览表格窗体，并进行相应的设置;
void DlgOpenAsciiFile::SetTableWidget(int rowNumber, int columnNumber)
{
	//设置文本预览表格的单元格不可操作;
	ui.tblAsciiFile->setEditTriggers(QAbstractItemView::NoEditTriggers);

	//设置行数，比实际数据多一行，用来放置数据类型选择下拉列表;
	ui.tblAsciiFile->setRowCount(rowNumber);

	//设置列数;
	ui.tblAsciiFile->setColumnCount(columnNumber);

	//竖直滚动条的大小;
	//QSize scrSize = ui.tblAsciiFile->verticalScrollBar()->size();
	//计算每列表格的最小宽度;
	//确保窗体中可见的表格列数最多为5；
// 	int minSize = 0;
// 	const int MAXCOLUMN = 5;
// 	if (columnNumber <= MAXCOLUMN)
// 	{
// 		minSize = ui.tblAsciiFile->size().width() / columnNumber;
// 	} 
// 	else
// 	{
// 		minSize = ui.tblAsciiFile->size().width() / MAXCOLUMN;
// 	}
	const unsigned MINSIZE = 100;
	//设置列单元格的最小宽度;
	/*ui.tblAsciiFile->horizontalHeader()->setMinimumSectionSize(minSize);*/
	ui.tblAsciiFile->horizontalHeader()->setMinimumSectionSize(MINSIZE);
	//设置列单元格的默认宽度;
	/*ui.tblAsciiFile->horizontalHeader()->setDefaultSectionSize(minSize);*/
	ui.tblAsciiFile->horizontalHeader()->setDefaultSectionSize(MINSIZE);
}

//更新数据表格
void DlgOpenAsciiFile::UpdateTable()
{
	//ui.tblAsciiFile->set Enabled(false);
	//判断文件名是否为空
	if (m_fileName.isEmpty())
	{
		ui.tblAsciiFile->clear();
		return;
	}
	else
	{
		//如果文件不存在则返回上层函数
		if (!QFile::exists(m_fileName))
		{
			return;
		}
	}

	//判断是否存在分隔符
	if (m_separator == 0)
	{
		ui.lblAsciiCode->setText(tr("Enter a valid character!"));
		ui.pushBtnOk->setEnabled(false);
		ui.tblAsciiFile->clear();
		return;
	} 
	else
	{
		//检验分割符是否为数字、小数点或负号
		if (m_separator.isNumber() || m_separator == 45/*负号;*/ 
			|| m_separator == 46/*小数点;*/ )
		{
			ui.lblAsciiCode->setText(tr("Enter a legal character!"));
			ui.pushBtnOk->setEnabled(false);
			ui.tblAsciiFile->clear();
			return;
		}
		else
		{
			ui.pushBtnOk->setEnabled(true);
		}
	}

	//获取文件数据
	QStringList fileData = LoadDataFromFile();

	//计算文本的行数;
	int rowNumber = fileData.size() + 1;

	//计算文本的列数;
	m_columnNumber = GetDataColumnNumber(fileData, m_separator);

	//添加文本预览表格窗体，并进行相应的设置;
	SetTableWidget(rowNumber, m_columnNumber);

	//设置默认的列数据类型
	if (!m_isSettedColumnType)
	{
		SetDefaultColumnType();
	}

	//为文本预览表格窗体添加数据类型选择的下拉列表;
	AddDataTypeComboBox(m_columnNumber);

	//生成数据表格;
	AddDataToTable(fileData, m_separator);
}

int DlgOpenAsciiFile::GetSampleRate() const
{
	return ui.cmbxSamplingRate->currentIndex() + 1;
}