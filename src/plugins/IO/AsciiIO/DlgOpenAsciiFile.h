#ifndef DLGOPENASCIIFILE_H
#define DLGOPENASCIIFILE_H

//C++标准库
#include <vector>

//Qt标准库;
#include <QString>
#include <QStringList>
#include <QList>
#include <QtWidgets/QComboBox>
#include "QtWidgets/QDialog"

#include "ui_DlgOpenAsciiFile.h"

using std::vector;

//数据每列类型
enum DataTypePerColumn
{
	IGNORED = 0,
	COORD_X,
	COORD_Y,
	COORD_Z,
	RGB_RED,
	RGB_GREEN,
	RGB_BLUE,
	GRAY,
	SCALAR, 
	NORMAL_X,
	NORMAL_Y,
	NORMAL_Z
};


struct DataColumnType
{
	int xCoordIndex;
	int yCoordIndex;
	int zCoordIndex;
	int rgbRedIndex;
	int rgbGreenIndex;
	int rgbBlueIndex;
	int grayIndex;
	QList<unsigned> scalarIndex;
	int xNormalIndex;
	int yNormalIndex;
	int zNormalIndex;

	//是否存在坐标值
	bool m_hasRGBColor;
	//是否存在向量值
	bool m_hasNormals;
	//有效类型数量
	unsigned m_validTypeNumbers;


	//初始化
	DataColumnType()
		: xCoordIndex(-1)
		, yCoordIndex(-1)
		, zCoordIndex(-1)
		, rgbRedIndex(-1)
		, rgbGreenIndex(-1)
		, rgbBlueIndex(-1)
		, grayIndex(-1)
		, m_validTypeNumbers(0)
		, m_hasNormals(false)
		, m_hasRGBColor(false)
		, xNormalIndex(-1)
		, yNormalIndex(-1)
		, zNormalIndex(-1)
	{};

	//是否拥有RGB颜色
	bool HasRGBColors() const 
	{
		return m_hasRGBColor;
	}
	//是否拥有向量
	bool HasNormals() const 
	{
		return m_hasNormals;
	}
	//获取有效的类型数量
	unsigned ValidTypeNumber() const
	{
		return m_validTypeNumbers;
	}
};

//Ascii文件预览最大行数
const int MAX_LINES_OF_ASCII_FILE = 500;

class DlgOpenAsciiFile : public QDialog
{
	Q_OBJECT

public:
	//默认构造函数;
	DlgOpenAsciiFile(QString fileName, QWidget *parent = 0);

	//析构函数;
	~DlgOpenAsciiFile();

	//成员函数;

	//获取列数
	unsigned GetDataColumnNumber() {return m_columnNumber;}
	//获取数据行数
	unsigned GetDataLines() {return m_readLines;}
	//获取数据列类型
	DataColumnType GetDataColumnType();
	//获取最大点数
	double GetMaxCloudSize() {return m_maxCloudSize * 1000000;}
	//获取分隔符;
	QChar GetSeparator() {return m_separator;}
	//获取跳过的行数;
	unsigned GetSkippedLines() {return m_skippedLines;}

	//获取采样率
	int GetSampleRate() const;

private:
	Ui::DlgOpenAsciiFile ui;
	//成员变量;

	//文件名;
	QString m_fileName;
	//分隔符;
	QChar m_separator;
	//数据的列数
	unsigned m_columnNumber;
	//跳过的行数
	unsigned m_skippedLines;
	//文件所要读入的行数
	unsigned m_readLines;
	//每个点云最大点数量
	double m_maxCloudSize;
	//是否手动设置了数据类型
	bool m_isSettedColumnType;
	//数据类型下拉列表
	vector<QComboBox*> m_dataType;
	vector<DataTypePerColumn> m_header;
	//头信息
	static DataTypePerColumn dataHeaders[12];
	//成员函数;

	//生成文本预览表格的内容;
	void AddDataToTable(QStringList lines, QChar separator);
	//为文本预览表格窗体添加数据类型选择的下拉列表;
	void AddDataTypeComboBox(int columnNumber);
	//使用分隔符对预览文本进行分块，返回数据块的个数;
	unsigned GetDataColumnNumber(const QStringList&, const QChar&);
	//获取分隔符;
	QChar GetSeparator(const QString&);
	////获取数据;
	QStringList LoadDataFromFile();
	//设置默认的列数据类型
	void SetDefaultColumnType();
	//添加文本预览表格窗体，并进行相应的设置;
	void SetTableWidget(int rowNumber, int columnNumber);
	//更新表数据
	void UpdateTable();

//定义槽;
private slots:
	//改变列的数据类型
	void ChangeColumnType(int);
	//点击空格、tab、逗号、分号分隔符;
	void ClickedSeparator();
	//生成数据结构;
	void SetMaxCloudSize();
	//设置分隔符
	void SetSeparator();
	//设置跳过的行;
	void SetSkippedLines();
	////获取数据的类型;
		//void getDataType(QComboBox *dataType);

protected:


	//成员函数;

	//重写重绘方法;
	//void paintEvent(QPaintEvent *event);
};

#endif // DLGOPENASCIIFILE_H
