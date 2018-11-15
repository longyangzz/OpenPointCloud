#include <assert.h>
#include <algorithm>

#include "GpDem.h"

//Qt
#include "QtWidgets/QTableWidget"
#include "QtWidgets/QComboBox"
#include "QtWidgets/QPushButton"

#include "Eigen/Dense"

#include "DCCore/DCGL.h"
#include "GpScalarField.h"
#include "DCGp/GpDrawContext.h"

#include "DCCore/Const.h"
#include "DCGp/Common.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpDEMImpl)

	class DcGpDEMImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpDEMImpl)

		//计算当前实体对象最大最小值
		void CalculateMyOwnBoundBox()
		{
			if (!m_borderValid)
			{
				//根据m_points，计算最小最大值m_minPoint，m_maxPoint
				for (auto i = 0; i < m_vertex.size(); ++i)
				{
					m_minPoint[0] = std::min(m_minPoint[0], m_vertex[i][0]);
					m_minPoint[1] = std::min(m_minPoint[1], m_vertex[i][1]);
					m_minPoint[2] = std::min(m_minPoint[2], m_vertex[i][2]);

					m_maxPoint[0] = std::max(m_maxPoint[0], m_vertex[i][0]);
					m_maxPoint[1] = std::max(m_maxPoint[1], m_vertex[i][1]);
					m_maxPoint[2] = std::max(m_maxPoint[2], m_vertex[i][2]);
				}
				//更新状态值
				m_borderValid = true;
			}
		}

	protected:
		DcGpDEMImpl()
			: m_xNumber(100)									//默认X方向的格网数量
			, m_yNumber(100)									//默认Y方向的格网数量
			, m_xInterval(1.0)								//默认X方向的格网间距
			, m_yInterval(1.0)								//默认Y方向的格网间距
			, m_showStyle(DcGpDEM::eShowPoint)
			, m_borderValid(false)
			, m_targetTex(0)
		{
			m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
			m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);
			
			m_currentScalarField = DcGpScalarFieldPtr();
			//初始化高程标量场
			m_heightScalarField = DcGpScalarFieldPtr(new DcGpScalarField("Height"));

			//初始化到原点距离标量场
			m_distanceScalarField = DcGpScalarFieldPtr(new DcGpScalarField("Distacne"));
		}

	private:
		friend class DcGpDEM;
		//格网的大小
		GridSize_Type m_xInterval;
		GridSize_Type m_yInterval;
		//格网的数量
		GridCount_Type m_xNumber;
		GridCount_Type m_yNumber;
		//格网的顶点数据
		std::vector<PointCoord> m_vertex;
		//高程信息
		DcGpDEM::VertexHeight m_heights;
		//左下角坐标
		Point_3D m_startPoint;
		//连接关系
		std::vector<std::array<unsigned, 4>> m_links;
		//显示模式
		DcGpDEM::ShowStyle m_showStyle;
		//顶点法量
		std::vector<DCVector3D> m_Normals;

		//各顶点颜色
		std::vector<PointColor> m_rgbColors;

		//标量场颜色
		std::vector<PointColor> m_scalarColors;
		//标量场
		std::vector<DcGpScalarFieldPtr> m_scalarFields;
		//当前使用的标量场
		DcGpScalarFieldPtr m_currentScalarField;
		//距离原点的标量场
		DcGpScalarFieldPtr m_distanceScalarField;
		//高程标量场
		DcGpScalarFieldPtr m_heightScalarField;

		//!\brief 是否已经计算过最大最小值
		bool m_borderValid;

		//XYZ范围
		DCVector3D m_minPoint;
		DCVector3D m_maxPoint;

		//!\brief 目标纹理对象
		GLuint m_targetTex;

		//!\brief 纹理坐标
		std::vector<DCVector2D> m_texCoords;
	};
}

DcGp::DcGpDEM::DcGpDEM()
	: DcGpEntity()
{
	m_pDcGpDEMImpl = DcGpDEMImpl::CreateObject();
}

DcGp::DcGpDEM::DcGpDEM(QString name)
	: DcGpEntity(name)
{
	m_pDcGpDEMImpl = DcGpDEMImpl::CreateObject();
}

DcGp::DcGpDEM::~DcGpDEM()
{

}

//增加顶点
// void DcGpDEM::AddVertex(DEMVertex vertex)
// {
// 	m_vertex = vertex;
// }

//设置左下角的坐标和格网数量
void DcGp::DcGpDEM::SetParameter(Point_3D startPoint, 
	GridCount_Type xNumber, 
	GridCount_Type yNumber, 
	GridSize_Type xInterval,
	GridSize_Type yInterval,
	VertexHeight heights,
	bool holdBoder)
{
	m_pDcGpDEMImpl->m_startPoint = startPoint;
	m_pDcGpDEMImpl->m_xNumber = xNumber;
	m_pDcGpDEMImpl->m_yNumber = yNumber;
	m_pDcGpDEMImpl->m_xInterval = xInterval;
	m_pDcGpDEMImpl->m_yInterval = yInterval;

	//先根据heights添加链接关系，if (heights.find(index) == heights.end())，说明该分区没有有效点，不添加对应的链接关系
	for (unsigned i = 0; i < (xNumber + 1) * yNumber; ++i)
	{
		if ((i + 1) % (xNumber + 1) != 0)
		{
			//根据i值确定index值，
			Index index = {i % (xNumber + 1), i / (xNumber + 1)};
			if (heights.find(index) == heights.end() && !holdBoder)
			{
				continue;
			}
			std::array<unsigned, 4> link = {i, i + 1, i + xNumber + 2, i + xNumber + 1};
			m_pDcGpDEMImpl->m_links.push_back(link);
		}
	}

	for (unsigned i = 0; i < yNumber + 1; ++i)
	{
		for (unsigned j = 0; j < xNumber + 1; ++j)
		{
			Index index = {j, i};
			PointCoord point;
			if (heights.find(index) == heights.end())
			{
				point[0] = startPoint[0] + xInterval * j;
				point[1] = startPoint[1] + yInterval * i;
				point[2] = startPoint[2];
				heights[index].push_back(point[2]);
			}
			else
			{
				point[0] = startPoint[0] + xInterval * j;
				point[1] = startPoint[1] + yInterval * i;
				point[2] = heights[index][0];
			}
			m_pDcGpDEMImpl->m_vertex.push_back(point);
		}
	}
	m_pDcGpDEMImpl->m_heights = heights;
}

//坐标转换
void DcGp::DcGpDEM::ApplyTransform(EigenMatrix4d& transMatrix)
{
	//初始化最大值最小值
	m_pDcGpDEMImpl->m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
	m_pDcGpDEMImpl->m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);

	Eigen::Matrix<double, 1, 3> temp;
	for (unsigned i = 0; i != PSize(); ++i)
	{
		/*temp << m_points[3 * i], m_points[3 * i + 1], m_points[3 * i + 2];*/
		temp << m_pDcGpDEMImpl->m_vertex[i][0], 
			m_pDcGpDEMImpl->m_vertex[i][1], 
			m_pDcGpDEMImpl->m_vertex[i][2];

		temp *= transMatrix.block(0, 0, 3, 3).transpose();
		temp += transMatrix.block(0, 3, 3, 1).transpose();
		// 		m_points[3 * i] = temp[0];
		// 		m_points[3 * i + 1] = temp[1];
		// 		m_points[3 * i + 2] = temp[2];
		m_pDcGpDEMImpl->m_vertex[i][0] = temp[0];
		m_pDcGpDEMImpl->m_vertex[i][1] = temp[1];
		m_pDcGpDEMImpl->m_vertex[i][2] = temp[2];


		//重新计算最大值
		m_pDcGpDEMImpl->m_vertex[0] = std::min(m_pDcGpDEMImpl->m_minPoint[0], m_pDcGpDEMImpl->m_vertex[i][0]);
		m_pDcGpDEMImpl->m_vertex[1] = std::min(m_pDcGpDEMImpl->m_minPoint[1], m_pDcGpDEMImpl->m_vertex[i][1]);
		m_pDcGpDEMImpl->m_vertex[2] = std::min(m_pDcGpDEMImpl->m_minPoint[2], m_pDcGpDEMImpl->m_vertex[i][2]);

		m_pDcGpDEMImpl->m_vertex[0] = std::max(m_pDcGpDEMImpl->m_maxPoint[0], m_pDcGpDEMImpl->m_vertex[i][0]);
		m_pDcGpDEMImpl->m_vertex[1] = std::max(m_pDcGpDEMImpl->m_maxPoint[1], m_pDcGpDEMImpl->m_vertex[i][1]);
		m_pDcGpDEMImpl->m_vertex[2] = std::max(m_pDcGpDEMImpl->m_maxPoint[2], m_pDcGpDEMImpl->m_vertex[i][2]);
	}

	//将Eigen::Matrix4d矩阵值，传给QMatrix4x4类型的矩阵
	double* matValue = transMatrix.data();

	//m_pDcGpDEMImpl->m_pop = QMatrix4x4(matValue);

	//重新计算边界盒
	m_pDcGpDEMImpl->m_borderValid = false;

}

//绘制
void DcGp::DcGpDEM::DrawMyselfOnly(DcGpDrawContext& context)
{
	if (MACRO_Draw3D(context))
	{
		//是否存在多边形
		unsigned n, gonNum = m_pDcGpDEMImpl->m_links.size();
		unsigned pNum = PSize();
		if (pNum == 0)
		{
			return;
		}
		//获取显示参数
		glDrawParameters glParams;
		GetDrawingParameters(glParams);
		glParams.showNormals &= bool(MACRO_LightIsEnabled(context));

		//是否网格显示
		bool showWired = (m_pDcGpDEMImpl->m_showStyle == eShowGrid);

		//材质色还是颜色
		bool colorMaterialEnabled = false;
		if (glParams.showScalarField || glParams.showColors)
		{
			glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
			glEnable(GL_COLOR_MATERIAL);
			colorMaterialEnabled = true;
		}

		//是否捕捉
		bool pushName = MACRO_DrawEntityNames(context);
		bool pushDemNames = MACRO_DrawTriangleNames(context);

		pushName |= pushDemNames;

		if (pushName)
		{
			//not fast at all!
			if (MACRO_DrawFastNamesOnly(context))
				return;

			glPushName(GetUniqueID());
			//最小显示选择模式
			glParams.showNormals = false;
			glParams.showColors = false;
			if (glParams.showScalarField);// && m_currentDisplayedScalarField->areNaNValuesShownInGrey())
			glParams.showScalarField = false; //--> we keep it only if SF 'NaN' values are potentially hidden
		}

		//绘制纯色
		if (glParams.showColors && IsColorOverriden())
		{
			glColor3ub(GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]);
			glParams.showColors = false;
		}
		else
		{
			glColor3fv(context.defaultMat.GetDiffuseFront());
		}

		//开启灯光
		if (glParams.showNormals)
		{
			//DGM: Strangely, when Qt::renderPixmap is called, the OpenGL version can fall to 1.0!
			glEnable((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2 ? GL_RESCALE_NORMAL : GL_NORMALIZE));
			glEnable(GL_LIGHTING);
			context.defaultMat.ApplyGL(true,colorMaterialEnabled);
		}
		//启动顶点数组
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &(m_pDcGpDEMImpl->m_vertex[0][0]));

		//是否启动法量显示
		if (glParams.showNormals)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, &(m_pDcGpDEMImpl->m_Normals[0][0]));
		}
		if (glParams.showScalarField)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3,GL_UNSIGNED_BYTE,3 * sizeof(ColorType), &(m_pDcGpDEMImpl->m_scalarColors[0][0]));
		}

		if (glParams.showColors)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3,GL_UNSIGNED_BYTE,3 * sizeof(ColorType), &(m_pDcGpDEMImpl->m_rgbColors[0][0]));
		}

		//是否启动纹理数组
		if (HasTextures())
		{
			//glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, m_pDcGpDEMImpl->m_targetTex);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(3, GL_FLOAT,
				2 * sizeof(GL_FLOAT), &(m_pDcGpDEMImpl->m_texCoords[0][0]));
		}

		if (pushDemNames)
		{
			if (pushDemNames)
				glPushName(0);

			//循环绘制n个dem网格
			
			for (unsigned i = 0; i < gonNum; ++i)
			{
				//取出当前索引号对应的链接关系，绘制四个点
				std::array<unsigned, 4> link = GetLink(i);
				if (link.size() != 4)
				{
					return;
				}
				glLoadName(i);
				glBegin(GL_QUADS);
				//根据链接关系取出四个点坐标
				DCVector3D point1 = GetVertex(link[0]);
				DCVector3D point2 = GetVertex(link[1]);
				DCVector3D point3 = GetVertex(link[2]);
				DCVector3D point4 = GetVertex(link[3]);

				
				glVertex3f(point1.x, point1.y, point1.z);
				glVertex3f(point2.x, point2.y, point2.z);
				glVertex3f(point3.x, point3.y, point3.z);
				glVertex3f(point4.x, point4.y, point4.z);
				glEnd();
			}

			if (pushDemNames)
				glPopName();
		}
		else
		{
			//判断显示模式
			if(m_pDcGpDEMImpl->m_showStyle == eShowPoint)
			{
				glDrawArrays(GL_POINTS, 0, PSize());
			}
			else if(m_pDcGpDEMImpl->m_showStyle == eShowGrid && gonNum)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				//绘制网格,循环绘制速度更慢
				glDrawElements(GL_QUADS, 4 * m_pDcGpDEMImpl->m_links.size(), 
					GL_UNSIGNED_INT, &(m_pDcGpDEMImpl->m_links[0][0]));
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else if (m_pDcGpDEMImpl->m_showStyle == eShowFill && gonNum)
			{
				//绘制网格,循环绘制速度更慢
				glDrawElements(GL_QUADS, 4 * m_pDcGpDEMImpl->m_links.size(), 
					GL_UNSIGNED_INT, &(m_pDcGpDEMImpl->m_links[0][0]));
			}
			else if (m_pDcGpDEMImpl->m_showStyle == eShowDefault && gonNum)
			{
				glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
				//绘制第一层填充
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0, 1);
				glDrawElements(GL_QUADS, 4 * m_pDcGpDEMImpl->m_links.size(), 
					GL_UNSIGNED_INT, &(m_pDcGpDEMImpl->m_links[0][0]));
				glDisable(GL_POLYGON_OFFSET_FILL);

				//绘制第二层网格，为了显示出网格效果，需要先关闭掉颜色数组
				if (glParams.showScalarField || glParams.showColors)
				{
					glDisableClientState(GL_COLOR_ARRAY);
				}
				if (glParams.showNormals)
				{
					glDisableClientState(GL_NORMAL_ARRAY);
				}
				//网格颜色值
				glColor3f(1.0f,1.0f,1.0f);

				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawElements(GL_QUADS, 4 * m_pDcGpDEMImpl->m_links.size(), 
					GL_UNSIGNED_INT, &(m_pDcGpDEMImpl->m_links[0][0]));
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glPopAttrib();
			}
		}

		//关闭顶点数组,法量数组，颜色数组
		glDisableClientState(GL_VERTEX_ARRAY);
		if (glParams.showNormals)
			glDisableClientState(GL_NORMAL_ARRAY);
		if (glParams.showScalarField || glParams.showColors)
			glDisableClientState(GL_COLOR_ARRAY);

		//关闭材质模式
		if (colorMaterialEnabled)
			glDisable(GL_COLOR_MATERIAL);
		//关闭灯光
		if (glParams.showNormals)
		{
			glDisable(GL_LIGHTING);
			glDisable((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2 ? GL_RESCALE_NORMAL : GL_NORMALIZE));
		}

		if (HasTextures())
		{
			//glDisable(GL_LIGHTING);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		if (pushName)
			glPopName();
	}
	else if (MACRO_Draw2D(context))
	{
		if (MACRO_Foreground(context) && !context.sfColorScaleToDisplay)
		{
			if (IsSfColorbarVisible()  && ScalarFieldShown())
			{
				//drawScale(context);
				AddColorBarInfo(context);
			}
		}
	}
}

//设置显示模式
void DcGp::DcGpDEM::SetShowStyle(ShowStyle style)
{
	m_pDcGpDEMImpl->m_showStyle = style;
}

//显示属性
void DcGp::DcGpDEM::ShowProperties(QTableWidget* widget)
{
	QTableWidgetItem* tblmItem;
	//设置行数
	widget->setRowCount(5);
	//对象类型
	tblmItem = new QTableWidgetItem(tr("Name"));
	widget->setItem(0, 0, tblmItem);
	tblmItem = new QTableWidgetItem(GetName());
	widget->setItem(0, 1, tblmItem);
	//网格的数量
	tblmItem = new QTableWidgetItem(tr("GridNumber"));
	widget->setItem(1, 0, tblmItem);
	tblmItem = new QTableWidgetItem(QString::number(m_pDcGpDEMImpl->m_links.size()));
	widget->setItem(1, 1, tblmItem);

	//顶点颜色
	tblmItem = new QTableWidgetItem(tr("Color"));
	widget->setItem(2, 0, tblmItem);

	//设置颜色按钮
	QPushButton* pbtnItem = new QPushButton(tr("Set Color"));
	//建立信号与槽的连接
	connect(pbtnItem, SIGNAL(clicked()), this, SLOT(SetColor()));
	widget->setCellWidget(2, 1, pbtnItem);


	//填充模式
	tblmItem = new QTableWidgetItem(tr("ShowStyle"));
	widget->setItem(3, 0, tblmItem);

	QComboBox* cmbxItem = new QComboBox();
	cmbxItem->insertItem(eShowGrid, tr("Grid"));
	cmbxItem->insertItem(eShowFill, tr("Fill"));
	cmbxItem->insertItem(eShowDefault, tr("Grid and Fill"));
	//建立信号与槽的连接
	connect(cmbxItem, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeShowStyle(int)));
	widget->setCellWidget(3, 1, cmbxItem);

	//显示的窗口
	tblmItem = new QTableWidgetItem(tr("View"));
	widget->setItem(4, 0, tblmItem);
	//获取当前对象所属的窗口
	QString title;
	if (GetAssociatedWindow())
	{
		title = GetAssociatedWindow()->GetWindowTitle();
	} 
	else
	{
		title = "None";
	}
	tblmItem = new QTableWidgetItem(title);
	widget->setItem(4, 1, tblmItem);
}

//改变显示样式
void DcGp::DcGpDEM::ChangeShowStyle(int style)
{
	m_pDcGpDEMImpl->m_showStyle = static_cast<ShowStyle>(style);
	GetAssociatedWindow()->Redraw();
}


//根据网格顶点的索引生成序号
unsigned DcGp::DcGpDEM::GetNumberByIndex(Index index)
{
	return index[0] + index[1] * (m_pDcGpDEMImpl->m_xNumber + 1);
}

//根据网格的序号生成索引
DcGp::DcGpDEM::Index DcGp::DcGpDEM::GetIndexByNumber(unsigned number)
{
	Index index;
	index[0] = number / (m_pDcGpDEMImpl->m_xNumber + 1);
	index[1] = number % (m_pDcGpDEMImpl->m_xNumber + 1);
	return index;
}

//根据提供的整平高程，计算挖填方量
void DcGp::DcGpDEM::CalculateEarthVolumn(double height, double& excavation, double& fill)
{
	double area = m_pDcGpDEMImpl->m_xInterval * m_pDcGpDEMImpl->m_yInterval;

	for (auto i = m_pDcGpDEMImpl->m_links.cbegin(); i != m_pDcGpDEMImpl->m_links.cend(); ++i)
	{
		double gridH = (m_pDcGpDEMImpl->m_vertex[(*i)[0]][2] +
			m_pDcGpDEMImpl->m_vertex[(*i)[1]][2] +
			m_pDcGpDEMImpl->m_vertex[(*i)[2]][2] +
			m_pDcGpDEMImpl->m_vertex[(*i)[3]][2]) / 4 - GetShift()[2];

		//如果网格高程高于整平高程，则计算挖方量
		if (gridH >= height)
		{
			excavation += (gridH - height) * area;
		}
		else
		{
			fill += (height - gridH) * area;
		}
	}
}

//是否拥有向量场
bool DcGp::DcGpDEM::HasNormals() const
{
	return !m_pDcGpDEMImpl->m_Normals.empty();
}

//是否拥有标量场
bool DcGp::DcGpDEM::HasDisplayedScalarField() const
{
	return (m_pDcGpDEMImpl->m_currentScalarField && !m_pDcGpDEMImpl->m_scalarFields.empty()) || m_pDcGpDEMImpl->m_heightScalarField;
}

void DcGp::DcGpDEM::ShowScalarField(bool state)
{
	DcGpEntity::ShowScalarField(state);
}

//更改当前标量场的颜色表
void DcGp::DcGpDEM::ChangeScalarFieldColorScale(QString name)
{
	if (!m_pDcGpDEMImpl->m_currentScalarField)
	{
		return;
	}

	m_pDcGpDEMImpl->m_currentScalarField->ChangeColorScale(name);
	m_pDcGpDEMImpl->m_currentScalarField->Prepare();
	m_pDcGpDEMImpl->m_scalarColors = m_pDcGpDEMImpl->m_currentScalarField->GetColors();
}

//设置当前标量场
void DcGp::DcGpDEM::SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type)
{
	switch (type)
	{
	case eScalarField:
		if (index >= 0 && index < ScalarFieldCount())
		{
			m_pDcGpDEMImpl->m_currentScalarField = m_pDcGpDEMImpl->m_scalarFields[index];
		}
		break;
	case eHeightField:
		if (!m_pDcGpDEMImpl->m_heightScalarField)
		{
			return;
		}
		//生成高程标量场
		for (unsigned i = 0; i != m_pDcGpDEMImpl->m_vertex.size(); ++i)
		{
			m_pDcGpDEMImpl->m_heightScalarField->AddData(m_pDcGpDEMImpl->m_vertex[i].z - GetShift().z);
		}
		m_pDcGpDEMImpl->m_currentScalarField = m_pDcGpDEMImpl->m_heightScalarField;
		break;
	case eDistanceField:
		if (!m_pDcGpDEMImpl->m_distanceScalarField)
		{
			return;
		}
		//生成距离标量场
		for (unsigned i = 0; i != m_pDcGpDEMImpl->m_vertex.size(); ++i)
		{
			m_pDcGpDEMImpl->m_distanceScalarField->
				AddData(sqrt(m_pDcGpDEMImpl->m_vertex[i].x * m_pDcGpDEMImpl->m_vertex[i].x +
				m_pDcGpDEMImpl->m_vertex[i].y * m_pDcGpDEMImpl->m_vertex[i].y + 
				m_pDcGpDEMImpl->m_vertex[i].z * m_pDcGpDEMImpl->m_vertex[i].z));
		}
		m_pDcGpDEMImpl->m_currentScalarField = m_pDcGpDEMImpl->m_distanceScalarField;
		break;
	default:
		break;
	}
	m_pDcGpDEMImpl->m_currentScalarField->SetColorSteps(setps);
	m_pDcGpDEMImpl->m_currentScalarField->Prepare();
	m_pDcGpDEMImpl->m_scalarColors = m_pDcGpDEMImpl->m_currentScalarField->GetColors();
}

void DcGp::DcGpDEM::SetNormals(std::vector<DCVector3D> normals)
{
	m_pDcGpDEMImpl->m_Normals = normals;
}

std::vector<Normal> DcGp::DcGpDEM::GetNormals()
{
	return m_pDcGpDEMImpl->m_Normals;
}

std::vector<PointCoord> DcGp::DcGpDEM::GetVertexs()
{
	return m_pDcGpDEMImpl->m_vertex;
}

std::vector<std::array<unsigned, 4>> DcGp::DcGpDEM::GetLinks()
{
	return m_pDcGpDEMImpl->m_links;
}

DcGp::DcGpBoundBox DcGp::DcGpDEM::GetMyOwnBoundBox()
{
	DcGpBoundBox emptyBox(GetMinPoint(), 
		GetMaxPoint());

	return emptyBox;
}

void DcGp::DcGpDEM::AddColorBarInfo(DcGpDrawContext& context)
{
	if (!m_pDcGpDEMImpl->m_currentScalarField)
	{
		return;
	}

	context.sfColorScaleToDisplay = static_cast<DcGp::DcGpScalarField*>(m_pDcGpDEMImpl->m_currentScalarField.get());
}

//获取最大点
PointCoord DcGp::DcGpDEM::GetMaxPoint() const
{
	m_pDcGpDEMImpl->CalculateMyOwnBoundBox();

	PointCoord maxPoint(m_pDcGpDEMImpl->m_maxPoint.x, 
		m_pDcGpDEMImpl->m_maxPoint.y, 
		m_pDcGpDEMImpl->m_maxPoint.z);
	return maxPoint;
}

//获取最小点
PointCoord DcGp::DcGpDEM::GetMinPoint() const
{
	m_pDcGpDEMImpl->CalculateMyOwnBoundBox();

	PointCoord minPoint(m_pDcGpDEMImpl->m_minPoint.x, 
		m_pDcGpDEMImpl->m_minPoint.y, 
		m_pDcGpDEMImpl->m_minPoint.z);
	return minPoint;
}

void DcGp::DcGpDEM::AddPoint(DCVector3D vector)
{
	m_pDcGpDEMImpl->m_vertex.push_back(vector);
}

void DcGp::DcGpDEM::AddPoint(DCVector3D vector, unsigned index)
{
	if (index < PSize() && index >= 0)
	{
		m_pDcGpDEMImpl->m_vertex[index] = vector;
	}
}

void DcGp::DcGpDEM::Resize(unsigned size)
{
	m_pDcGpDEMImpl->m_vertex.resize(size);
}

void DcGp::DcGpDEM::AddLink(std::array<unsigned, 4> link)
{
	m_pDcGpDEMImpl->m_links.push_back(link);
}

std::array<unsigned, 4> DcGp::DcGpDEM::GetLink(unsigned index)
{
	return m_pDcGpDEMImpl->m_links.at(index);
}

unsigned DcGp::DcGpDEM::PSize() const
{
	return m_pDcGpDEMImpl->m_vertex.size();
}

unsigned DcGp::DcGpDEM::LinkSize()
{
	return m_pDcGpDEMImpl->m_links.size();
}

PointCoord* DcGp::DcGpDEM::GetVertex(unsigned index) const
{
	if (index >= PSize())
	{
		index = PSize() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return &(m_pDcGpDEMImpl->m_vertex.at(index));
}

PointCoord DcGp::DcGpDEM::GetVertex(unsigned index)
{
	if (index >= PSize())
	{
		index = PSize() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpDEMImpl->m_vertex.at(index);
}

void DcGp::DcGpDEM::InvalidBoundBox()
{
	m_pDcGpDEMImpl->m_borderValid = false;
}

//获取指定顶点的颜色
PointColor DcGp::DcGpDEM::GetPointColor(unsigned index) const
{
	if (index >= PSize())
	{
		index = PSize() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpDEMImpl->m_rgbColors[index];
}

void DcGp::DcGpDEM::AddColor(PointColor color, unsigned index)
{
	if (m_pDcGpDEMImpl->m_rgbColors.empty() && !m_pDcGpDEMImpl->m_vertex.empty())
	{
		m_pDcGpDEMImpl->m_rgbColors.resize(m_pDcGpDEMImpl->m_vertex.size());
	}

	if (index >= 0 && index < m_pDcGpDEMImpl->m_vertex.size())
	{
		m_pDcGpDEMImpl->m_rgbColors[index] = color;
	}
}

//添加标量场
void DcGp::DcGpDEM::AddScalarField(DcGpScalarFieldPtr scalarField)
{
	m_pDcGpDEMImpl->m_scalarFields.push_back(scalarField);
}

DcGp::DcGpScalarFieldPtr DcGp::DcGpDEM::GetScalarFieldByIndex(int index)
{
	assert(index >= 0);
	return m_pDcGpDEMImpl->m_scalarFields[index];
}

int DcGp::DcGpDEM::GetScalarFieldIndexByName(QString name)
{
	size_t sfCount = ScalarFieldCount();
	for (size_t i=0; i<sfCount; ++i)
	{
		//we don't accept two SF with the same name!
		if (ScalarFieldNames()[i] == name)
			return static_cast<int>(i);
	}

	return -1;
}

unsigned DcGp::DcGpDEM::ScalarFieldCount() const
{
	return m_pDcGpDEMImpl->m_scalarFields.size();
}

QStringList DcGp::DcGpDEM::ScalarFieldNames() const
{
	QStringList names;
	unsigned number = m_pDcGpDEMImpl->m_scalarFields.size();

	for (unsigned i = 0; i < number; ++i)
	{
		names.push_back(m_pDcGpDEMImpl->m_scalarFields.at(i)->GetName());
	}
	return names;
}

DcGp::DcGpScalarFieldPtr DcGp::DcGpDEM::GetCurrentScalarField()
{
	return m_pDcGpDEMImpl->m_currentScalarField;
}

QMap<QString, double> DcGp::DcGpDEM::GetParameter()
{
	QMap<QString, double> header;
	header["x"] = m_pDcGpDEMImpl->m_startPoint.x;
	header["y"] = m_pDcGpDEMImpl->m_startPoint.y;
	header["z"] = m_pDcGpDEMImpl->m_startPoint.z;
	header["xInterval"] = m_pDcGpDEMImpl->m_xInterval;
	header["yInterval"] = m_pDcGpDEMImpl->m_yInterval;
	header["xNumber"] = m_pDcGpDEMImpl->m_xNumber;
	header["yNumber"] = m_pDcGpDEMImpl->m_yNumber;

	return header;
}

void DcGp::DcGpDEM::UpdateParameter(Point_3D startPoint, GridCount_Type xNumber, GridCount_Type yNumber, GridSize_Type xInterval, GridSize_Type yInterval)
{
	m_pDcGpDEMImpl->m_startPoint = startPoint;
	m_pDcGpDEMImpl->m_xNumber = xNumber;
	m_pDcGpDEMImpl->m_yNumber = yNumber;
	m_pDcGpDEMImpl->m_xInterval = xInterval;
	m_pDcGpDEMImpl->m_yInterval = yInterval;
}

bool DcGp::DcGpDEM::ReserveTheRGBTable()
{
	if(m_pDcGpDEMImpl->m_vertex.empty())
	{
		return false;
	}

	if (m_pDcGpDEMImpl->m_rgbColors.empty())
	{
		m_pDcGpDEMImpl->m_rgbColors.resize(m_pDcGpDEMImpl->m_vertex.size());

		//初始化值
		PointColor default = {255,255,255};
		std::fill(m_pDcGpDEMImpl->m_rgbColors.begin(), m_pDcGpDEMImpl->m_rgbColors.end(), default);
	}

	return true;
}

PointColor& DcGp::DcGpDEM::GetColor(const unsigned index)
{
	assert(index < m_pDcGpDEMImpl->m_vertex.size());
	assert(index >= 0);
	assert(m_pDcGpDEMImpl->m_rgbColors.size() == m_pDcGpDEMImpl->m_vertex.size());
	

	return m_pDcGpDEMImpl->m_rgbColors[index];
}

void DcGp::DcGpDEM::SetImage(QImage& image, const QString& tfwFile)
{
	if (image.isNull())
		return;

	//存在tfw文件，则读取参数
	bool hasTfw = false;
	TFWParas paras;
	if (!tfwFile.isEmpty())
	{
		paras = GetParasFromTfw(tfwFile);
		hasTfw = true;
	}

	if(!glIsTexture(m_pDcGpDEMImpl->m_targetTex))
	{
		//生成纹理
		BindTargetTexture(image);

		//为每个顶点生成纹理坐标
		Point_3D shift = GetShift();
		for (unsigned i = 0; i < PSize(); ++i)
		{
			Point_3D point = Point_3D::FromArray(GetVertex(i).u);
			point -= shift;

			//查询当前point对应在image中的像素行列号
			int row = 0, column = 0;
			
			if (!hasTfw)
			{
				GetPixelPosition(Point_3D::FromArray(GetMinPoint().u)-shift, Point_3D::FromArray(GetMaxPoint().u)-shift, image.height(), image.width(), point, row, column);
			}
			else
			{
				GetPixelPositionByTFW(paras, point, row, column);
			}

			////根据image的size归一化row，column，得到纹理坐标
			DCVector2D texCoord;
			NormalizePixelPos(image.height(), image.width(), row, column, texCoord);

			m_pDcGpDEMImpl->m_texCoords.push_back(texCoord);
		}
	}
}

void DcGp::DcGpDEM::BindTargetTexture(QImage& image)
{
	if (image.isNull())
		return;
	if (m_pDcGpDEMImpl->m_targetTex) 
	{
		glDeleteTextures(1, &m_pDcGpDEMImpl->m_targetTex);
		m_pDcGpDEMImpl->m_targetTex = 0;
	}

	// create texture
	glGenTextures(1, &m_pDcGpDEMImpl->m_targetTex);
	QImage tximg = QGLWidget::convertToGLFormat(image);
	glBindTexture(GL_TEXTURE_2D, m_pDcGpDEMImpl->m_targetTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, tximg.width(), tximg.height(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());

	//glBindTexture(GL_TEXTURE_2D, 0);
} 

void DcGp::DcGpDEM::AddTexCoord(DCVector2D texCoord)
{
	m_pDcGpDEMImpl->m_texCoords.push_back(texCoord);
}

bool DcGp::DcGpDEM::HasTextures()
{
	return m_pDcGpDEMImpl->m_targetTex && !m_pDcGpDEMImpl->m_texCoords.empty();
}

void DcGp::DcGpDEM::CancelTexture()
{
	if (m_pDcGpDEMImpl->m_targetTex)
	{
		glDeleteTextures(1, &m_pDcGpDEMImpl->m_targetTex);
	}
	m_pDcGpDEMImpl->m_targetTex = 0;
	m_pDcGpDEMImpl->m_texCoords.clear();
}