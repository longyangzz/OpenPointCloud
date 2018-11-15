//C++
#include <set>

//Qt
#include "QFontMetrics"
#include "QObject"

//DcGp
#include "DCGp/Custom/GpScalarField.h"
#include "DCGp/GpBasicTypes.h"

//自定义
#include "ViewRenderingTool.h"
#include "Viewer.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))

using namespace MPViewer;

void ViewRenderingTool::DrawColorBar(const DcGp::DcGpDrawContext& context )
{
	DcGp::DcGpScalarField* scalarField = context.sfColorScaleToDisplay;

	if (!scalarField || !scalarField->GetColorScale())
	{
		return;
	}

	//判断当前context中的窗体是否存在
	Viewer* win = static_cast<Viewer* >(context._win);
	if (!win)
	{
		return;
	}

	//创建set容器,传入当前标量场的最小和最大值
	std::set<double> keyValues = scalarField->GetScaleRange();
	//keyValues.insert(238.0);
	//keyValues.insert(239.0);

	//字体设置
	QFontMetrics strMetrics(win->font());

	//字体默认颜色
	const unsigned char tc[3] = {255,255,255};
	const unsigned char* textColor = DcGp::Color::DEFAULTLABELCOLOR;

	//histogram?
	//const::ccScalarField::Histogram histogram = sf->getHistogram();
	//bool showHistogram = (ccGui::Parameters().colorScaleShowHistogram && !logScale && histogram.maxValue != 0 && histogram.size() > 1);

	//显示区域设置
	const int strHeight = strMetrics.height();
	const int scaleWidth = 50;
	const int scaleMaxHeight = (keyValues.size() > 1 ? max(context.glH-140,2*strHeight) : scaleWidth); //if 1 value --> we draw a cube

	//中心化平行投影
	int halfW = (context.glW>>1);  //右移位操作符相当于 /2
	int halfH = (context.glH>>1);

	//右上角偏移量
	const int xShift = 20 + (false ? scaleWidth/2 : 0);
	const int yShift = halfH - max(context.glH - 140,2 * strHeight) / 2 - 10;


	glPushAttrib(GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LINE_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	std::vector<double> sortedKeyValues(keyValues.begin(),keyValues.end());
	double maxRange = sortedKeyValues.back()-sortedKeyValues.front();

	const ColorType* lineColor = textColor;

	//绘制颜色条
	{
		//左下角起始点坐标
		int x = halfW-xShift-scaleWidth;
		int y = halfH-yShift-scaleMaxHeight;

		if (keyValues.size() > 1)
		{
			glLineWidth(1.0f);
			glBegin(GL_LINES);
			for (int j=0; j<scaleMaxHeight; ++j)
			{
				double value = sortedKeyValues.front() + ((double)j * maxRange) / (double)scaleMaxHeight;
				const RGBAColor col = scalarField->GetColor(static_cast<ScalarType>(value));
				ColorType color[4] = {col[0], col[1], col[2], col[3]};
				glColor3ubv(!col.empty() ? color : DcGp::Color::LIGHTGREY);

				glVertex2i(x,y+j);
				glVertex2i(x+scaleWidth,y+j);
			}
			glEnd();
		}

		//边框宽度线
		glLineWidth(2.0);
		glColor3ubv(lineColor);
		glBegin(GL_LINE_LOOP);
		glVertex2i(x,y);
		glVertex2i(x+scaleWidth,y);
		glVertex2i(x+scaleWidth,y+scaleMaxHeight);
		glVertex2i(x,y+scaleMaxHeight);
		glEnd();
	}

	//绘制刻度和文字，语句块
	{
		VlabelSet labelList;

		//起始刻度标签
		labelList.push_back(Vlabel(0,0,strHeight,sortedKeyValues.front()));

		//绘制终止刻度标签
		if (sortedKeyValues.size() > 1)
		{
			labelList.push_back(Vlabel(scaleMaxHeight,scaleMaxHeight-strHeight,scaleMaxHeight,sortedKeyValues.back()));
		}
		
		//递归创建标签
		if (sortedKeyValues.size() > 1)
		{
			//文本label间隔
			const int minGap = strHeight * 2;

			//临时变量
			unsigned drawnLabelsBefore = 0; //启动循环
			unsigned drawnLabelsAfter = labelList.size(); 

			//遍历直到没有新的标签生成
			while (drawnLabelsAfter > drawnLabelsBefore)
			{
				drawnLabelsBefore = drawnLabelsAfter;

				VlabelSet::iterator it1 = labelList.begin();
				VlabelSet::iterator it2 = it1; it2++;
				for (; it2 != labelList.end(); ++it2)
				{
					if (it1->yMax + 2*minGap < it2->yMin)
					{
						//插入标签
						double val = (it1->val + it2->val)/2.0;
						int yScale = static_cast<int>((val-sortedKeyValues[0]) * (double)scaleMaxHeight / maxRange);

						//插入到容器中的合适位置，按顺序排放
						labelList.insert(it2,Vlabel(yScale,yScale-strHeight/2,yScale+strHeight/2,val));
					}
					it1 = it2;
				}

				drawnLabelsAfter = labelList.size();
			}
		}

		//绘制标签
		glColor3f(0,1,0);

		//绘制并显示标量的名字
		//double index = scalarField->GetName().toDouble();
		//QString sfName = QObject::tr("Scalar Field #%1").arg(index);
		QString sfName = scalarField->GetName();
		if (!sfName.isEmpty())
		{
			//预留了一些间隙
			win->Display2DLabel(sfName, context.glW-xShift+10, context.glH-yShift+strMetrics.height(), 4 | 8);
		}

		//显示精度
		const unsigned precision = 3;
		//format
		const char format = ('f');
		//tick
		const int tickSize = 4;

		//for labels
		const int x = context.glW-xShift-scaleWidth-2*tickSize-1;
		const int y = context.glH-yShift-scaleMaxHeight;
		//for ticks
		const int xTick = halfW-xShift-scaleWidth-tickSize-1;
		const int yTick = halfH-yShift-scaleMaxHeight;

		for (VlabelSet::iterator it = labelList.begin(); it != labelList.end(); ++it)
		{
			VlabelSet::iterator itNext = it; itNext++;
			//position
			unsigned char align = 4;
			if (it == labelList.begin())
				align |= 8;
			else if (itNext == labelList.end())
				align |= 32;
			else
				align |= 16;

			double value = it->val;

			win->Display2DLabel(QString::number(value,format,precision), x, y+it->yPos, align);
			glBegin(GL_LINES);
			glVertex2f(xTick,yTick+it->yPos);
			glVertex2f(xTick+tickSize,yTick+it->yPos);
			glEnd();
		}
	}

	glPopAttrib();
}