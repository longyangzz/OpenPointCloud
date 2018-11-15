#include "QFileDialog"

//金字塔分割
#include "TileSplit/GridSplitStrategy.h"
#include "TileSplit/GeneratorContext.h"

#include "DlgFileSplit.h"

DlgFileSplit::DlgFileSplit(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	CreateConnection();
}

DlgFileSplit::~DlgFileSplit()
{

}

void DlgFileSplit::CreateConnection()
{
	connect(ui.pbtnInputfilename, SIGNAL(clicked()), this, SLOT(OpenInputFile()));
	connect(ui.pbtnOutDir, SIGNAL(clicked()), this, SLOT(OpenOutDir()));

	//确定取消
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(Ok()));
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void DlgFileSplit::OpenInputFile()
{
	//选择输入文件
	QString fileName = QFileDialog::getOpenFileName(this);
	ui.pbtnInputfilename->setText(fileName);
}

void DlgFileSplit::OpenOutDir()
{
	QString outDir = QFileDialog::getExistingDirectory(this);
	ui.pbtnOutDir->setText(outDir);
}

void DlgFileSplit::Ok()
{
	//参数
	QString inputFilename = ui.pbtnInputfilename->text();
	QDir outDir = ui.pbtnOutDir->text();
	unsigned depth = ui.spbxPyraDepth->text().toUInt();

	unsigned boxSample = ui.spbxBoxSample->text().toUInt();
	unsigned levelSample = ui.spbxLevelSample->text().toUInt();
	bool isAll = !ui.cmbxAllLevel->currentIndex();

	//执行分块
	DC::Pyra::GeneratorContext context(inputFilename, depth, outDir, isAll);
	DC::Pyra::GridSplitStrategy* gridStrategy = new DC::Pyra::GridSplitStrategy;
	gridStrategy->SetBoxSample(boxSample);
	gridStrategy->SetLevelSample(levelSample);
	context.SetSplitStrategy(gridStrategy);
	context.Generate();

	//关闭对话框
	accept();
}