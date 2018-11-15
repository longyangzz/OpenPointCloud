#include "GpMesh.h"
#include <algorithm>

//Qt
#include "QtWidgets/QTableWidget"
#include "QtWidgets/QComboBox"
#include "QtWidgets/QPushButton"
#include "QFileInfo"
#include "QTextStream"

//IL
#include <IL/il.h>

//DcLib
#include "DCCore/Logger.h"

#include "DCCore/DCGL.h"
#include "Impl/NormalVectors.hxx"

#include "DCGp/Common.h"
#include "DCGp/GpDrawContext.h"
#include "DCGp/Custom/GpMaterial.h"
#include "Impl/GpMeshImpl.hxx"

DcGp::DcGpMesh::DcGpMesh()
	: DcGpEntity()
{
	m_pDcGpMeshImpl = DcGpMeshImpl::CreateObject();
}

DcGp::DcGpMesh::DcGpMesh(QString name)
	:DcGpEntity(name)
{
	m_pDcGpMeshImpl = DcGpMeshImpl::CreateObject();
}

DcGp::DcGpMesh::DcGpMesh(QString name, std::vector<DCVector3D> vertices)
	:DcGpEntity(name)
{
	m_pDcGpMeshImpl = DcGpMeshImpl::CreateObject();
	m_pDcGpMeshImpl->m_points = vertices;
}

DcGp::DcGpMesh::~DcGpMesh()
{

}

//添加三角形
void DcGp::DcGpMesh::AddTriangle(unsigned index1, unsigned index2, unsigned index3)
{
	Link polygon = {index1, index2, index3};

	AddTriangle(polygon);
}

//绘制mesh图形
void DcGp::DcGpMesh::DrawMyselfOnly(DcGpDrawContext& context)
{
	//handleColorRamp(context);

	//判断是否是绘制三维物体
	if (MACRO_Draw3D(context))
	{
		//是否存在多边形
		unsigned n, gonNum = m_pDcGpMeshImpl->m_links.size();
		if (gonNum == 0)
		{
			return;
		}

		//判断是否需要初始化创建纹理对象
		if (!HasTextures() && !m_pDcGpMeshImpl->m_teximgFileName.isEmpty())
		{
			//CreateTexture();
			m_pDcGpMeshImpl->LoadGLTextureByDevil();
		}

		//判断是否临时隐藏当前对象,只绘制点
		bool lodEnabled = (gonNum > DCCore::MAX_FACE_NUM && MACRO_LODActivated(context));

		//获取显示参数
		glDrawParameters glParams;
		GetDrawingParameters(glParams);
		glParams.showNormals &= bool(MACRO_LightIsEnabled(context));

		//是否显示法量
		glParams.showNormals = (HasNormals() && NormalsShown());

		//是否网格显示
		bool showWired = (m_pDcGpMeshImpl->m_showStyle == eShowDefault);

		//材质色还是颜色
		bool colorMaterialEnabled = false;
		if (glParams.showScalarField || glParams.showColors)
		{
			glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
			glEnable(GL_COLOR_MATERIAL);
			colorMaterialEnabled = true;
		}

		//GL name pushing
		bool pushName = MACRO_DrawEntityNames(context);
		//捕捉需要
		bool pushTriNames = MACRO_DrawTriangleNames(context);
		pushName |= pushTriNames;

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

		//选择颜色
		//绘制纯色
		if (glParams.showColors && IsColorOverriden())
		{
			glColor3ub(GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]);
			//glParams.showColors = false;
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
			/*if (m_pDcGpMeshImpl->m_material)
			{
				m_pDcGpMeshImpl->m_material->ApplyGL(true,colorMaterialEnabled);
			}*/
			context.defaultMat.ApplyGL(true,colorMaterialEnabled);
		}

		//压缩法向量
		NormalVectors* compressedNormals = 0;
		if (glParams.showNormals)
		{
			compressedNormals = NormalVectors::GetUniqueInstance();
		}

		unsigned vertCount = m_pDcGpMeshImpl->m_points.size();
		unsigned faceCount = m_pDcGpMeshImpl->m_links.size();
		if (glParams.showNormals && !m_pDcGpMeshImpl->m_comPressNormal.empty())
		{
			//更新法向量的值
			for (unsigned i = 0; i < vertCount; ++i)
			{
				//获取每个网格的三个顶点坐标
				//Link index = GetLink(i);
				m_pDcGpMeshImpl->m_Normals[i] = compressedNormals->GetNormal(m_pDcGpMeshImpl->m_comPressNormal[i]) * (-1);
			}
		}
		
		if (pushTriNames)
		{
			if (m_pDcGpMeshImpl->m_visibilityTable)
			{
				
			}
			else
			{
				//初始化当前参数
				//顶点色
				const ColorType *col1=0,*col2=0,*col3=0;
				//法向量值
				const PointCoordinateType *N1=0,*N2=0,*N3=0;
				//纹理坐标
				const float *Tx1=0,*Tx2=0,*Tx3=0;

				if (m_pDcGpMeshImpl->m_showStyle == eShowGrid)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}

				if (pushTriNames)
					glPushName(0);

				glBegin(GL_TRIANGLES);
				for (unsigned i = 0; i < gonNum; ++i)
				{
					//获取当前三角形索引号
					Link index = m_pDcGpMeshImpl->m_links[i];

					if (glParams.showScalarField)
					{
						col1 = &(m_pDcGpMeshImpl->m_scalarColors[index[0]][0]);
						col2 = &(m_pDcGpMeshImpl->m_scalarColors[index[1]][0]);
						col3 = &(m_pDcGpMeshImpl->m_scalarColors[index[2]][0]);
					}
					else if (glParams.showNormals)
					{
						N1 = &(m_pDcGpMeshImpl->m_Normals[index[0]][0]);
						N2 = &(m_pDcGpMeshImpl->m_Normals[index[1]][0]);
						N3 = &(m_pDcGpMeshImpl->m_Normals[index[2]][0]);
					}
					else if (glParams.showColors && IsColorOverriden())
					{
						col1 = &GetTempColor()[0];
						col2 = &GetTempColor()[1];
						col3 = &GetTempColor()[2];
					}
					else
					{
						glColor3fv(context.defaultMat.GetDiffuseFront());
					}
					if (pushTriNames)
					{
						glEnd();
						glLoadName(i);
						glBegin(GL_TRIANGLES);
					}
					//首先判断当前三角形是否被选中了(包含一点选中和三点选中)
					if (GetMeshSelected(i))
					{

						//顶点1
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[0]).x, GetPoint(index[0]).y, GetPoint(index[0]).z);

						//顶点2
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[1]).x, GetPoint(index[1]).y, GetPoint(index[1]).z);

						//顶点3
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[2]).x, GetPoint(index[2]).y, GetPoint(index[2]).z);
					}
					else //根据实际颜色绘制
					{
						//选择颜色
						//顶点1
						if (col1) glColor3ubv(col1);
						if (N1) glNormal3fv(N1);
						glVertex3f(GetPoint(index[0]).x, GetPoint(index[0]).y, GetPoint(index[0]).z);

						//顶点2
						if (col2) glColor3ubv(col2);
						if (N1)glNormal3fv(N2);
						glVertex3f(GetPoint(index[1]).x, GetPoint(index[1]).y, GetPoint(index[1]).z);

						//顶点3
						if (col3) glColor3ubv(col3);
						if (N1)glNormal3fv(N3);
						glVertex3f(GetPoint(index[2]).x, GetPoint(index[2]).y, GetPoint(index[2]).z);
					}
				}
				glEnd();

				if (pushTriNames)
					glPopName();
				if (m_pDcGpMeshImpl->m_showStyle == eShowGrid)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
		}
		else
		{
			if (m_pDcGpMeshImpl->m_visibilityTable || (IsSelected() && HasSelected()))
			{
				//初始化当前参数
				//顶点色
				const std::array<ColorType, 3> *col1=0,*col2=0,*col3=0;
				//法向量值
				std::array<PointCoordinateType, 3> *N1=0,*N2=0,*N3=0;
				//纹理坐标
				const float *Tx1=0,*Tx2=0,*Tx3=0;

				if (m_pDcGpMeshImpl->m_showStyle == eShowGrid)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}

				if (pushTriNames)
					glPushName(0);

				glBegin(GL_TRIANGLES);
				for (unsigned i = 0; i < gonNum; ++i)
				{
					//获取当前三角形索引号
					Link index = m_pDcGpMeshImpl->m_links[i];

					if (glParams.showNormals)
					{
						std::array<PointCoordinateType, 3> tempN1 = {m_pDcGpMeshImpl->m_Normals[index[0]][0],m_pDcGpMeshImpl->m_Normals[index[0]][1],m_pDcGpMeshImpl->m_Normals[index[0]][2]};
						N1 = &tempN1;
						std::array<PointCoordinateType, 3> tempN2 = {m_pDcGpMeshImpl->m_Normals[index[1]][0],m_pDcGpMeshImpl->m_Normals[index[1]][1],m_pDcGpMeshImpl->m_Normals[index[1]][2]};
						N2 = &tempN2;
						std::array<PointCoordinateType, 3> tempN3 = {m_pDcGpMeshImpl->m_Normals[index[2]][0],m_pDcGpMeshImpl->m_Normals[index[2]][1],m_pDcGpMeshImpl->m_Normals[index[2]][2]};
						N3 = &tempN3;
					}
					if (glParams.showColors && IsColorOverriden())
					{
						col1 = &GetTempColor();
						col2 = &GetTempColor();
						col3 = &GetTempColor();
					}
					else if (glParams.showScalarField)
					{
						col1 = &m_pDcGpMeshImpl->m_scalarColors[index[0]];
						col2 = &m_pDcGpMeshImpl->m_scalarColors[index[1]];
						col3 = &m_pDcGpMeshImpl->m_scalarColors[index[2]];
					}
					else
					{
						glColor3fv(context.defaultMat.GetDiffuseFront());
					}
					if (pushTriNames)
					{
						glEnd();
						glLoadName(i);
						glBegin(GL_TRIANGLES);
					}
					//首先判断当前三角形是否被选中了(包含一点选中和三点选中)
					if (GetMeshSelected(i))
					{

						if (N1) glNormal3f((*N1)[0],(*N1)[1], (*N1)[2]);
						//顶点1
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[0]).x, GetPoint(index[0]).y, GetPoint(index[0]).z);

						if (N2) glNormal3f((*N2)[0],(*N2)[1], (*N2)[2]);
						//顶点2
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[1]).x, GetPoint(index[1]).y, GetPoint(index[1]).z);

						if (N3) glNormal3f((*N3)[0],(*N3)[1], (*N3)[2]);
						//顶点3
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[2]).x, GetPoint(index[2]).y, GetPoint(index[2]).z);
					}
					else //根据实际颜色绘制
					{
						//选择颜色
						//顶点1
						if (col1) glColor3ub((*col1)[0], (*col1)[1], (*col1)[2]);
						if (N1) glNormal3f((*N1)[0],(*N1)[1], (*N1)[2]);
						glVertex3f(GetPoint(index[0]).x, GetPoint(index[0]).y, GetPoint(index[0]).z);

						//顶点2
						if (col2) glColor3ub((*col2)[0], (*col2)[1], (*col2)[2]);
						if (N2) glNormal3f((*N2)[0],(*N2)[1], (*N2)[2]);
						glVertex3f(GetPoint(index[1]).x, GetPoint(index[1]).y, GetPoint(index[1]).z);

						//顶点3
						if (col3) glColor3ub((*col3)[0], (*col3)[1], (*col3)[2]);
						if (N3) glNormal3f((*N3)[0],(*N3)[1], (*N3)[2]);
						glVertex3f(GetPoint(index[2]).x, GetPoint(index[2]).y, GetPoint(index[2]).z);
					}
				}
				glEnd();

				if (pushTriNames)
					glPopName();
				if (m_pDcGpMeshImpl->m_showStyle == eShowGrid)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
			else 
			{
				//启动顶点数组
				glEnableClientState(GL_VERTEX_ARRAY);
				//glColor3b(255, 0, 0);
				glVertexPointer(3, GL_FLOAT, 0, &(m_pDcGpMeshImpl->m_points[0][0]));
				//glVertexPointer(3, GL_FLOAT, 0, &(points[0][0]));

				//是否启动法量显示
				if (glParams.showNormals)
				{
					glEnableClientState(GL_NORMAL_ARRAY);
					glNormalPointer(GL_FLOAT, 0, &(m_pDcGpMeshImpl->m_Normals[0][0]));
				}
				//是否启动颜色数组
				if (glParams.showScalarField)
				{
					glEnableClientState(GL_COLOR_ARRAY);
					glColorPointer(3, GL_UNSIGNED_BYTE,
						3 * sizeof(ColorType), &(m_pDcGpMeshImpl->m_scalarColors[0][0]));
				}

				//是否启动纹理数组
				if (HasTextures())
				{
					if (!m_pDcGpMeshImpl->m_texCoords.empty())
					{
						//glEnable(GL_LIGHTING);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, m_pDcGpMeshImpl->m_targetTex);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glTexCoordPointer(3, GL_FLOAT,
							2 * sizeof(GL_FLOAT), &(m_pDcGpMeshImpl->m_texCoords[0][0]));
					}
					
				}

				//glFrontFace(GL_CW); //定义顺逆时针
				if (lodEnabled)
				{
					//只绘制点
					glDrawArrays(GL_POINTS, 0, PSize());
				}
				else
				{
					//判断显示模式
					if(m_pDcGpMeshImpl->m_showStyle == eShowGrid)
					{
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						//绘制网格,循环绘制速度更慢
						glDrawElements(GL_TRIANGLES, 3 * m_pDcGpMeshImpl->m_links.size(), 
							GL_UNSIGNED_INT, &(m_pDcGpMeshImpl->m_links[0][0]));
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					}
					else if (m_pDcGpMeshImpl->m_showStyle == eShowFill)
					{
						//绘制填充,循环绘制速度更慢
						glDrawElements(GL_TRIANGLES, 3 * m_pDcGpMeshImpl->m_links.size(), 
							GL_UNSIGNED_INT, &(m_pDcGpMeshImpl->m_links[0][0]));
						//glDrawArrays(GL_TRIANGLES, 0, faceCount * 3);
					}
					else if (m_pDcGpMeshImpl->m_showStyle == eShowDefault)
					{
						glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
						//绘制第一层填充
						glEnable(GL_POLYGON_OFFSET_FILL);
						glPolygonOffset(1.0, 1);
						glDrawElements(GL_TRIANGLES, 3 * m_pDcGpMeshImpl->m_links.size(), 
							GL_UNSIGNED_INT, &(m_pDcGpMeshImpl->m_links[0][0]));
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
						glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
						//glPolygonMode(GL_BACK, GL_LINE);
						glDrawElements(GL_TRIANGLES, 3 * m_pDcGpMeshImpl->m_links.size(), 
							GL_UNSIGNED_INT, &(m_pDcGpMeshImpl->m_links[0][0]));
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glPopAttrib();
					}

				}
			}
		}
		//关闭顶点数组,法量数组，颜色数组
		glDisableClientState(GL_VERTEX_ARRAY);
		if (glParams.showNormals)
			glDisableClientState(GL_NORMAL_ARRAY);
		if (glParams.showScalarField || glParams.showColors)
			glDisableClientState(GL_COLOR_ARRAY);

		if (HasTextures())
		{
			//glDisable(GL_LIGHTING);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		//关闭材质模式
		if (colorMaterialEnabled)
			glDisable(GL_COLOR_MATERIAL);
		//关闭灯光
		if (glParams.showNormals)
		{
			glDisable(GL_LIGHTING);
			glDisable((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2 ? GL_RESCALE_NORMAL : GL_NORMALIZE));
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

DcGp::DcGpBoundBox DcGp::DcGpMesh::GetDisplayBoundBox()
{
	return GetMyOwnBoundBox();
}

//
DcGp::DcGpBoundBox DcGp::DcGpMesh::GetMyOwnBoundBox()
{
	DcGpBoundBox emptyBox(GetMinPoint(), 
		                  GetMaxPoint());

	return emptyBox;
}

//显示属性信息
void DcGp::DcGpMesh::ShowProperties(QTableWidget* widget)
{
	QTableWidgetItem* tblmItem;
	//设置行数
	widget->setRowCount(5);
	//对象类型
	tblmItem = new QTableWidgetItem(tr("Name"));
	widget->setItem(0, 0, tblmItem);
	tblmItem = new QTableWidgetItem(GetName());
	widget->setItem(0, 1, tblmItem);
	//三角网数
	tblmItem = new QTableWidgetItem(tr("MeshNumber"));
	widget->setItem(1, 0, tblmItem);
	tblmItem = new QTableWidgetItem(QString::number(m_pDcGpMeshImpl->m_links.size()));
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

std::vector<PointCoord> DcGp::DcGpMesh::GetVertexs()
{
	return m_pDcGpMeshImpl->m_points;
}

void DcGp::DcGpMesh::AddPoint(DCVector3D point)
{
	m_pDcGpMeshImpl->m_points.push_back(point);
	//m_pDcGpMeshImpl->m_pointsSelected.push_back(false);
}

void DcGp::DcGpMesh::AddNormal(DCVector3D normal)
{
	m_pDcGpMeshImpl->m_Normals.push_back(normal);
}

void DcGp::DcGpMesh::AddVexColor(PointColor color)
{
	m_pDcGpMeshImpl->m_rgbColors.push_back(color);
}

//改变显示样式
void DcGp::DcGpMesh::ChangeShowStyle(int style)
{
	m_pDcGpMeshImpl->m_showStyle = static_cast<ShowStyle>(style);
	GetAssociatedWindow()->Redraw();
}

//是否拥有向量场
bool DcGp::DcGpMesh::HasNormals() const
{
	return !m_pDcGpMeshImpl->m_Normals.empty();
}

//是否拥有标量场
bool DcGp::DcGpMesh::HasScalarFields() const
{
	return (!m_pDcGpMeshImpl->m_scalarFields.empty());
}

void DcGp::DcGpMesh::ShowScalarField(bool state)
{
	DcGpEntity::ShowScalarField(state);
}

//更改当前标量场的颜色表
void DcGp::DcGpMesh::ChangeScalarFieldColorScale(QString name)
{
	if (!m_pDcGpMeshImpl->m_currentScalarField)
	{
		return;
	}

	m_pDcGpMeshImpl->m_currentScalarField->ChangeColorScale(name);
	m_pDcGpMeshImpl->m_currentScalarField->Prepare();
	m_pDcGpMeshImpl->m_scalarColors = m_pDcGpMeshImpl->m_currentScalarField->GetColors();
}

//设置当前标量场
void DcGp::DcGpMesh::SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type)
{
	if (index >= 0 && index < ScalarFieldCount())
	{
		m_pDcGpMeshImpl->m_currentScalarField = m_pDcGpMeshImpl->m_scalarFields[index];
	}
	else
	{
		return;
	}

	//更新高程和距离标量场的值
	m_pDcGpMeshImpl->UpdateScalarFieldData();
	
	m_pDcGpMeshImpl->m_currentScalarField->SetColorSteps(setps);
	m_pDcGpMeshImpl->m_currentScalarField->Prepare();
	m_pDcGpMeshImpl->m_scalarColors = m_pDcGpMeshImpl->m_currentScalarField->GetColors();
}

//获取指定点
//必须存在点时才可调用
DCVector3D DcGp::DcGpMesh::GetPoint(unsigned index) const
{
	assert(index < m_pDcGpMeshImpl->m_points.size() && index >= 0);

	if (index >= m_pDcGpMeshImpl->m_points.size())
	{
		index = m_pDcGpMeshImpl->m_points.size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpMeshImpl->m_points[index];
}

DcGp::LinkContainer DcGp::DcGpMesh::GetLinks()
{
	return m_pDcGpMeshImpl->m_links;
}

std::vector<Normal> DcGp::DcGpMesh::GetNormals()
{
	return m_pDcGpMeshImpl->m_Normals;
}

void DcGp::DcGpMesh::SetNormals(std::vector<DCVector3D> normals)
{
	m_pDcGpMeshImpl->m_Normals = normals;
}

void DcGp::DcGpMesh::SetPoints(std::vector<DCVector3D> points)
{
	m_pDcGpMeshImpl->m_points = points;

// 	m_pDcGpMeshImpl->m_pointsSelected.resize(points.size());
// 	std::fill(m_pDcGpMeshImpl->m_pointsSelected.begin(), m_pDcGpMeshImpl->m_pointsSelected.end(), false);
}

void DcGp::DcGpMesh::SetLinks(LinkContainer links)
{
	m_pDcGpMeshImpl->m_links = links;

	m_pDcGpMeshImpl->m_meshsSelected.resize(links.size());
	std::fill(m_pDcGpMeshImpl->m_meshsSelected.begin(), m_pDcGpMeshImpl->m_meshsSelected.end(), false);
}

//bool DcGp::DcGpMesh::BuildDelaunay(std::vector<DCVector2D> &point2ds)
//{
//	if (point2ds.empty())
//	{
//		return false;
//	}
//
//	//reset
//	m_pDcGpMeshImpl->m_numberOfTriangles = 0;
//	if (!m_pDcGpMeshImpl->m_links.empty())
//	{
//		m_pDcGpMeshImpl->m_links.clear();
//	}
//
//	//调用外部库triangle
//	triangulateio in;
//	memset(&in, 0, sizeof(triangulateio));
//	in.numberofpoints = point2ds.size();
//	in.pointlist = (float*)(&point2ds[0]);
//
//	try
//	{
//		triangulate("zQNn", &in, &in, 0);
//	}
//	catch(...)
//	{
//		return false;
//	}
//
//	//返回三角网个数，以及连接关系
//	m_pDcGpMeshImpl->m_numberOfTriangles = in.numberoftriangles;
//	if (m_pDcGpMeshImpl->m_numberOfTriangles > 0)
//	{
//		//计算最大最小索引
//		int minIndex = ((int*)in.trianglelist)[0];
//		int maxIndex = ((int*)in.trianglelist)[0];
//		for (unsigned i = 0;i<m_pDcGpMeshImpl->m_numberOfTriangles;++i)
//		{
//			if (minIndex > ((int*)in.trianglelist)[i])
//				minIndex = ((int*)in.trianglelist)[i];
//			else if (maxIndex < ((int*)in.trianglelist)[i])
//				maxIndex = ((int*)in.trianglelist)[i];
//			//添加links
//			AddTriangle(((int*)in.trianglelist)[3*i], ((int*)in.trianglelist)[3*i+1], ((int*)in.trianglelist)[3*i+2]);
//			//添加拓扑关系
//			AddTopology(((int*)in.neighborlist)[3*i], ((int*)in.neighborlist)[3*i+1], ((int*)in.neighborlist)[3*i+2]);
//		}
//	}
//	
//	return true;
//}

void DcGp::DcGpMesh::AddTopology(long neighbor1, long neighbor2, long neighbor3)
{
	std::array<long, 3> topo = {neighbor1, neighbor2, neighbor3};
	m_pDcGpMeshImpl->m_topologys.push_back(topo);
}

std::vector<std::array<long, 3>> DcGp::DcGpMesh::GetTopologys()
{
	return m_pDcGpMeshImpl->m_topologys;
}

void DcGp::DcGpMesh::AddColorBarInfo(DcGpDrawContext& context)
{
	if (!m_pDcGpMeshImpl->m_currentScalarField)
	{
		return;
	}

	context.sfColorScaleToDisplay = static_cast<DcGp::DcGpScalarField*>(m_pDcGpMeshImpl->m_currentScalarField.get());
}

bool DcGp::DcGpMesh::LoadFromFile(const QString& fileName, bool coordinatesShiftEnabled, double* coordinatesShift /* = 0 */)
{
	//查找扩展名
	QString extension = QFileInfo(fileName).suffix();

	bool status;
	//根据扩展名判断调用何种读取方式
	if (extension.toUpper() == "WRL") //解析WRL格式文件
	{
		status = LoadWrl(fileName, coordinatesShiftEnabled, coordinatesShift);
	}
	return status;
}

//解析WRL格式文件
bool DcGp::DcGpMesh::LoadWrl(const QString& fileName , bool coordinatesShiftEnabled /* = false  */, double* coordinatesShift /* = 0 */ )
{
	QFile file(fileName);
	if (!file.exists())
	{
		DCCore::Logger::Error(QObject::tr("File [%1] doesn\'t exist.").arg(fileName));
		return false;
	}

	//打开文件读
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	QTextStream stream(&file);
	//逐行读取
	PointCoord point(0,0,0);
	PointCoord shift(0,0,0);
	bool finish = false;

	QString firstLine = stream.readLine();
	if (!firstLine.toUpper().startsWith("#VRML"))
	{
		DCCore::Logger::Warning(QObject::tr("WRL File Header is Valid?"));
		return false;
	}

	//进度条
	while(!stream.atEnd())
	{
		QString line = stream.readLine();

		if (line.toUpper() == "COORD COORDINATE {")
		{
			auto m = 4;
		}
		//解析存储点坐标
		if (line.toUpper().startsWith("COORD") && line.toUpper() == "COORD COORDINATE {")
		{
		}
	}

	file.close();

	return true;
}

//获取最大点
PointCoord DcGp::DcGpMesh::GetMaxPoint() const
{
	m_pDcGpMeshImpl->CalculateMyOwnBoundBox();

	PointCoord maxPoint(m_pDcGpMeshImpl->m_maxPoint.x, 
		m_pDcGpMeshImpl->m_maxPoint.y, 
		m_pDcGpMeshImpl->m_maxPoint.z);
	return maxPoint;
}

//获取最小点
PointCoord DcGp::DcGpMesh::GetMinPoint() const
{
	m_pDcGpMeshImpl->CalculateMyOwnBoundBox();

	PointCoord minPoint(m_pDcGpMeshImpl->m_minPoint.x, 
		m_pDcGpMeshImpl->m_minPoint.y, 
		m_pDcGpMeshImpl->m_minPoint.z);
	return minPoint;
}

void DcGp::DcGpMesh::SetMinPoint(PointCoord pt)
{
	//取 pt和m_pDcGpMeshImpl->m_minPoint中的最小点
	m_pDcGpMeshImpl->m_minPoint[0] = std::min(m_pDcGpMeshImpl->m_minPoint[0], pt[0]);
	m_pDcGpMeshImpl->m_minPoint[1] = std::min(m_pDcGpMeshImpl->m_minPoint[1], pt[1]);
	m_pDcGpMeshImpl->m_minPoint[2] = std::min(m_pDcGpMeshImpl->m_minPoint[2], pt[2]);
}

void DcGp::DcGpMesh::SetMaxPoint(PointCoord pt)
{
	//取 pt和m_pDcGpMeshImpl->m_maxPoint中的最大点
	m_pDcGpMeshImpl->m_maxPoint[0] = std::max(m_pDcGpMeshImpl->m_maxPoint[0], pt[0]);
	m_pDcGpMeshImpl->m_maxPoint[1] = std::max(m_pDcGpMeshImpl->m_maxPoint[1], pt[1]);
	m_pDcGpMeshImpl->m_maxPoint[2] = std::max(m_pDcGpMeshImpl->m_maxPoint[2], pt[2]);
}

DcGp::Link DcGp::DcGpMesh::GetLink(unsigned index)
{
	return m_pDcGpMeshImpl->m_links.at(index);
}

void DcGp::DcGpMesh::SetPointNormalIndex(unsigned pointIndex, NormalType norm)
{
	//assert(m_normals && pointIndex < m_normals->currentSize());
	if (m_pDcGpMeshImpl->m_comPressNormal.empty() && m_pDcGpMeshImpl->m_points.size() > 0)
	{
		m_pDcGpMeshImpl->m_comPressNormal.resize(m_pDcGpMeshImpl->m_points.size());
	}

	m_pDcGpMeshImpl->m_comPressNormal[pointIndex] = norm;
}
void DcGp::DcGpMesh::SetPointNormal(unsigned pointIndex, const DCVector3D& N)
{
	SetPointNormalIndex(pointIndex, NormalVectors::GetNormIndex(N));
}

void DcGp::DcGpMesh::SetTopologys(std::vector<std::array<long, 3>> topologys)
{
	m_pDcGpMeshImpl->m_topologys = topologys;
}

DcGp::LinkContainer& DcGp::DcGpMesh::GetLinks() const
{
	return m_pDcGpMeshImpl->m_links;
}

void DcGp::DcGpMesh::GetVertexs(std::vector<DCVector3D>& points)
{
	points = m_pDcGpMeshImpl->m_points;
}

//设置指定点的选中状态
// void DcGp::DcGpMesh::SetPointSelected(unsigned index, bool status)
// {
// 	m_pDcGpMeshImpl->m_pointsSelected[index] = status;
// 
// 	//设置选取点的颜色
// 	if (status)
// 	{
// 		m_pDcGpMeshImpl->m_selectedColors[index][0] = 255;
// 		m_pDcGpMeshImpl->m_selectedColors[index][1] = 0;
// 		m_pDcGpMeshImpl->m_selectedColors[index][2] = 0;
// 	}
// 	else
// 	{
// 		if (GetColorMode() == eFalseRangeColor ||
// 			GetColorMode() == eFalseHeightColor ||
// 			GetColorMode() == eFalseScalarColor)
// 		{
// 			m_pDcGpMeshImpl->m_selectedColors[index] = m_pDcGpMeshImpl->m_scalarColors[index];
// 		}
// 		else
// 		{
// 			m_pDcGpMeshImpl->m_selectedColors[index][0] = GetTempColor()[0];
// 			m_pDcGpMeshImpl->m_selectedColors[index][1] = GetTempColor()[1];
// 			m_pDcGpMeshImpl->m_selectedColors[index][2] = GetTempColor()[2];
// 		}
// 	}
// }

//获取指定点的选中状态
// bool DcGp::DcGpMesh::GetPointSelected(unsigned index) const
// {
// 	return m_pDcGpMeshImpl->m_pointsSelected[index];
// }

//启用选取颜色
void DcGp::DcGpMesh::EnableSelectedColors()
{
	if (!m_pDcGpMeshImpl->m_selectedColors.empty())
	{
		return;
	}
	else
	{
		PointColor color = {GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]};
		m_pDcGpMeshImpl->m_selectedColors.resize(GetVertexs().size());
		std::fill_n(m_pDcGpMeshImpl->m_selectedColors.begin(), GetVertexs().size(), color);
	}
}

//停用选取颜色
void DcGp::DcGpMesh::DisableSelectedColors()
{
	m_pDcGpMeshImpl->m_selectedColors.clear();
}

unsigned DcGp::DcGpMesh::PSize()
{
	return m_pDcGpMeshImpl->m_points.size();
}

//删除所选的部分
void DcGp::DcGpMesh::DeleteSelected()
{
	//更新链接关系，但并不删除顶点
	unsigned triNum = m_pDcGpMeshImpl->m_links.size();

	//判断是否存在选中的
	bool hasSelect = HasSelected();
	if (!hasSelect)
	{
		return;
	}

	//创建一个链接关系
	LinkContainer links;

	for (unsigned i = 0; i < triNum; ++i)
	{
		//获取当前三角形索引号
		Link index = m_pDcGpMeshImpl->m_links[i];
		//首先判断当前三角形是否被选中了(包含一点选中和三点选中)
		if (!GetMeshSelected(i))
		{
			links.push_back(index);
		}
	}

	//清空链接关系
	m_pDcGpMeshImpl->m_links.clear();
	m_pDcGpMeshImpl->m_links = links;

	//清空点的可见性
	CancelSelected();
}

//取消选取
void DcGp::DcGpMesh::CancelSelected()
{
	//取消每个点的选取状态
	std::fill_n(m_pDcGpMeshImpl->m_meshsSelected.begin(), LinkSize(), false);

	//清空每个点的选取颜色
	if (GetColorMode() == eFalseRangeColor ||
		GetColorMode() == eFalseHeightColor ||
		GetColorMode() == eFalseScalarColor)
	{
		m_pDcGpMeshImpl->m_selectedColors = m_pDcGpMeshImpl->m_scalarColors;
	}
	else
	{
		PointColor color = {GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]};
		std::fill_n(m_pDcGpMeshImpl->m_selectedColors.begin(), PSize(), color);
	}

	//禁用显示表
	m_pDcGpMeshImpl->m_visibilityTable = false;
}

double GetAngleDeg(DCVector3D& AB, DCVector3D& AC)
{
	AB.Normalize();
	AC.Normalize();
	double dotprod = AB.Dot(AC);
	if (dotprod<=-1.0)
		return 180.0;
	else if (dotprod>1.0)
		return 0.0;
	return 180.0*acos(dotprod)/M_PI;
}

void DcGp::DcGpMesh::Filter(FilterPara para)
{
	unsigned triNum = m_pDcGpMeshImpl->m_links.size();

	//创建一个链接关系表
	std::map<unsigned, Link> linkTable;

	for (unsigned i = 0; i < triNum; ++i)
	{
		linkTable[i] = GetLink(i);
	}

	//清空链接关系
	m_pDcGpMeshImpl->m_links.clear();

	//对链接关系表进行处理
	for (unsigned i = 0; i < triNum; ++i)
	{
		//最长边过滤
		//获取三个顶点坐标
		DCVector3D point1 = GetPoint(linkTable[i].at(0));
		DCVector3D point2 = GetPoint(linkTable[i].at(1));
		DCVector3D point3 = GetPoint(linkTable[i].at(2));

		//计算两点间距离12,13,23
		float dis1 = DCVector3D::VDistance(point1, point2);
		float dis2 = DCVector3D::VDistance(point1, point3);
		float dis3 = DCVector3D::VDistance(point2, point3);

		//计算三个内角值
		DCVector3D point12 = point2 - point1;
		DCVector3D point13 = point3 - point1;
		DCVector3D point23 = point3 - point2;

		//取负值
		DCVector3D nPoint12 = -point12;
		DCVector3D nPoint13 = -point13;
		DCVector3D nPoint23 = -point23;

		double angle1 = GetAngleDeg(point12,point13);
		double angle2 = GetAngleDeg(point23,nPoint12);
		double angle3 = GetAngleDeg(nPoint13,nPoint23); 

		if (dis1 > para.maxEdge || dis2 > para.maxEdge || dis3 > para.maxEdge || 
			angle1 > para.maxAngle || angle2 > para.maxAngle || angle3 > para.maxAngle ||
			angle1 < para.minAngel || angle2 < para.minAngel || angle3 < para.minAngel)  //边长大于最大边阈值,角度阈值
		{
			//删除该条链接记录
			linkTable.erase(i);
		}
	}
	
	//根据处理后的linkTable表，更新m_links内容,同时更新拓扑关系内容
	//创建临时拓扑关系数组

	std::vector<std::array<long, 3>> topoTable;
	std::array<long, 3> topo;

	for (auto it = linkTable.begin(); it != linkTable.end(); ++it)
	{
		AddTriangle(it->second);

		//判断是否存在拓扑关系表，存在则更新拓扑关系表1.根据key值取出有效的，2.再判断有效的拓扑关系中是否有不存在的索引号则赋值为-1
		if (!m_pDcGpMeshImpl->m_topologys.empty())
		{
			//取出当前有效的拓扑关系存入到topo中
			topo = GetTopology(it->first);

			//判断topo中是否含有某个索引号在linkTable的key值中没有该元素
			for (int tIndexNum = 0; tIndexNum < topo.size(); ++tIndexNum)
			{
				bool isExist = linkTable.find(topo[tIndexNum]) != linkTable.end();   //存在
				if (!isExist)
				{
					topo[tIndexNum] = -1;
				}
			}

			topoTable.push_back(topo);
		}
	}
	if (!topoTable.empty())
	{
		m_pDcGpMeshImpl->m_topologys = topoTable;
	}
}

DCVector3D& DcGp::DcGpMesh::GetPoint(unsigned index)
{
	return m_pDcGpMeshImpl->m_points.at(index);
}

void DcGp::DcGpMesh::AddTriangle(Link tri)
{	
	m_pDcGpMeshImpl->m_links.push_back(tri);

	m_pDcGpMeshImpl->m_meshsSelected.push_back(false);
}

std::array<long, 3> DcGp::DcGpMesh::GetTopology(unsigned index)
{
	if (!m_pDcGpMeshImpl->m_topologys.empty())
	{
		return m_pDcGpMeshImpl->m_topologys.at(index);
	}
}

unsigned DcGp::DcGpMesh::LinkSize()
{
	if (m_pDcGpMeshImpl->m_links.empty())
	{
		return 0;
	}
	else
	{
		return m_pDcGpMeshImpl->m_links.size();
	}
}

bool DcGp::DcGpMesh::HasSelected()
{
	std::vector<bool >::iterator it = std::find(m_pDcGpMeshImpl->m_meshsSelected.begin(), m_pDcGpMeshImpl->m_meshsSelected.end(), true);
	if (it == m_pDcGpMeshImpl->m_meshsSelected.end())
	{
		return false;
	}
	return true;
}

bool DcGp::DcGpMesh::GetMeshSelected(unsigned index) const
{
	return m_pDcGpMeshImpl->m_meshsSelected[index];
}

void DcGp::DcGpMesh::SetMeshSelected(unsigned index, bool status)
{
	m_pDcGpMeshImpl->m_meshsSelected[index] = status;
}

std::vector<unsigned> DcGp::DcGpMesh::GetSelectMeshs()
{
	unsigned triNum = LinkSize();

	std::vector<unsigned> meshNumber;

	for (unsigned i = 0; i < triNum; ++i)
	{
		//首先判断当前三角形是否被选中了(包含一点选中和三点选中)
		if (GetMeshSelected(i))
		{
			meshNumber.push_back(i);
		}
	}
	return meshNumber;
}

bool DcGp::DcGpMesh::SaveToFile(QFile& file) const
{
	//导出mesh对象为ply格式
	QString fileName = file.fileName();
	
	return true;
}

//添加标量场
void DcGp::DcGpMesh::AddScalarField(DcGpScalarFieldPtr scalarField)
{
	m_pDcGpMeshImpl->m_scalarFields.push_back(scalarField);
}

DcGp::DcGpScalarFieldPtr DcGp::DcGpMesh::GetScalarFieldByIndex(int index)
{
	assert(index >= 0);
	return m_pDcGpMeshImpl->m_scalarFields[index];
}

//获取指定顶点的颜色
PointColor DcGp::DcGpMesh::GetPointColor(unsigned index) const
{
	if (index >= m_pDcGpMeshImpl->m_points.size())
	{
		index = m_pDcGpMeshImpl->m_points.size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpMeshImpl->m_rgbColors[index];

}

DCVector3D DcGp::DcGpMesh::GetPointNormal(unsigned index) const
{
	if (index >= m_pDcGpMeshImpl->m_Normals.size())
	{
		index = m_pDcGpMeshImpl->m_Normals.size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpMeshImpl->m_Normals[index];

}

void DcGp::DcGpMesh::MapTexBySetTextImgFile(QString& fileName, const QString& tfwFile, const QString& suffix)
{
	if (fileName.isEmpty())
		return;
	
	//存在tfw文件，则读取参数
	bool hasTfw = false;
	TFWParas paras;
	if (!tfwFile.isEmpty())
	{
		paras = GetParasFromTfw(tfwFile);
		hasTfw = true;
	}

	if(!glIsTexture(m_pDcGpMeshImpl->m_targetTex))
	{
		//生成纹理
		//BindTargetTexture(image, suffix);

		//! 方法一 devil
		m_pDcGpMeshImpl->m_teximgFileName = fileName;
		int* width = new int[1];
		int* height = new int[1];
		//m_pDcGpMeshImpl->LoadGLTextureByDevil(width, height);

		//方法二 qimage
		QImage image(fileName);
		LoadGLTextureByQImage(image, suffix);
		*width = image.width();
		*height = image.height();

		//为每个顶点生成纹理坐标
		Point_3D shift = GetShift();
		for (unsigned i = 0; i < PSize(); ++i)
		{
			Point_3D point = Point_3D::FromArray(GetPoint(i).u);
			point -= shift;

			//查询当前point对应在image中的像素行列号
			int row = 0, column = 0;
			if (!hasTfw)
			{
				GetPixelPosition(Point_3D::FromArray(GetMinPoint().u)-shift, Point_3D::FromArray(GetMaxPoint().u)-shift, *height, *width, point, row, column);
			}
			else
			{
				GetPixelPositionByTFW(paras, point, row, column);
			}
			

			////根据image的size归一化row，column，得到纹理坐标
			DCVector2D texCoord;
			NormalizePixelPos(*height, *width, row, column, texCoord);

			m_pDcGpMeshImpl->m_texCoords.push_back(texCoord);
		}

		delete width;
		delete height;

		width = 0;
		height = 0;
	}
}

void DcGp::DcGpMesh::LoadGLTextureByQImage(QImage& image, const QString& suffix)
{
	if (image.isNull())
		return;
	if (m_pDcGpMeshImpl->m_targetTex) 
	{
		glDeleteTextures(1, &m_pDcGpMeshImpl->m_targetTex);
		m_pDcGpMeshImpl->m_targetTex = 0;
	}

	// create texture
	glGenTextures(1, &m_pDcGpMeshImpl->m_targetTex);
	QImage tximg = QGLWidget::convertToGLFormat(image);
	glBindTexture(GL_TEXTURE_2D, m_pDcGpMeshImpl->m_targetTex);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	if (suffix.toUpper() == "BMP")
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, tximg.width(), tximg.height(), GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, tximg.width(), tximg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());
	}
} 

void DcGp::DcGpMesh::AddTexCoord(DCVector2D texCoord)
{
	m_pDcGpMeshImpl->m_texCoords.push_back(texCoord);
}

bool DcGp::DcGpMesh::HasTextures()
{
	return m_pDcGpMeshImpl->m_targetTex;
}

void DcGp::DcGpMesh::CancelTexture()
{
	if (m_pDcGpMeshImpl->m_targetTex)
	{
		glDeleteTextures(1, &m_pDcGpMeshImpl->m_targetTex);
	}
	m_pDcGpMeshImpl->m_targetTex = 0;
	m_pDcGpMeshImpl->m_teximgFileName = "";
	m_pDcGpMeshImpl->m_texCoords.clear();
}

DcGp::ShowStyle DcGp::DcGpMesh::GetShowStyle()
{
	return m_pDcGpMeshImpl->m_showStyle;
}

//设置显示样式
void DcGp::DcGpMesh::SetShowStyle(ShowStyle showStyle)
{
	m_pDcGpMeshImpl->m_showStyle = showStyle;
	GetAssociatedWindow()->Redraw();
}

unsigned DcGp::DcGpMesh::ScalarFieldCount() const
{
	return m_pDcGpMeshImpl->m_scalarFields.size();
}

QStringList DcGp::DcGpMesh::ScalarFieldNames() const
{
	QStringList names;
	unsigned number = m_pDcGpMeshImpl->m_scalarFields.size();

	for (unsigned i = 0; i < number; ++i)
	{
		names.push_back(m_pDcGpMeshImpl->m_scalarFields.at(i)->GetName());
	}
	return names;
}

//坐标转换
void DcGp::DcGpMesh::ApplyTransform(EigenMatrix4d& transMatrix)
{
	//初始化最大值最小值
	m_pDcGpMeshImpl->m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
	m_pDcGpMeshImpl->m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);

	Eigen::Matrix<double, 1, 3> temp;
	for (unsigned i = 0; i != PSize(); ++i)
	{
		/*temp << m_points[3 * i], m_points[3 * i + 1], m_points[3 * i + 2];*/
		temp << m_pDcGpMeshImpl->m_points[i][0], 
			m_pDcGpMeshImpl->m_points[i][1], 
			m_pDcGpMeshImpl->m_points[i][2];

		temp *= transMatrix.block(0, 0, 3, 3).transpose();
		temp += transMatrix.block(0, 3, 3, 1).transpose();
		// 		m_points[3 * i] = temp[0];
		// 		m_points[3 * i + 1] = temp[1];
		// 		m_points[3 * i + 2] = temp[2];
		m_pDcGpMeshImpl->m_points[i][0] = temp[0];
		m_pDcGpMeshImpl->m_points[i][1] = temp[1];
		m_pDcGpMeshImpl->m_points[i][2] = temp[2];


		//重新计算最大值
		m_pDcGpMeshImpl->m_minPoint[0] = std::min(m_pDcGpMeshImpl->m_minPoint[0], m_pDcGpMeshImpl->m_points[i][0]);
		m_pDcGpMeshImpl->m_minPoint[1] = std::min(m_pDcGpMeshImpl->m_minPoint[1], m_pDcGpMeshImpl->m_points[i][1]);
		m_pDcGpMeshImpl->m_minPoint[2] = std::min(m_pDcGpMeshImpl->m_minPoint[2], m_pDcGpMeshImpl->m_points[i][2]);

		m_pDcGpMeshImpl->m_maxPoint[0] = std::max(m_pDcGpMeshImpl->m_maxPoint[0], m_pDcGpMeshImpl->m_points[i][0]);
		m_pDcGpMeshImpl->m_maxPoint[1] = std::max(m_pDcGpMeshImpl->m_maxPoint[1], m_pDcGpMeshImpl->m_points[i][1]);
		m_pDcGpMeshImpl->m_maxPoint[2] = std::max(m_pDcGpMeshImpl->m_maxPoint[2], m_pDcGpMeshImpl->m_points[i][2]);
	}

	//将Eigen::Matrix4d矩阵值，传给QMatrix4x4类型的矩阵
	double* matValue = transMatrix.data();

	//m_pDcGpMeshImpl->m_pop = QMatrix4x4(matValue);

	//重新计算边界盒
	m_pDcGpMeshImpl->m_borderValid = false;

}

void DcGp::DcGpMesh::SetTexImgFile(const QString& texFileName)
{
	if (!texFileName.isEmpty())
	{
		m_pDcGpMeshImpl->m_teximgFileName = texFileName;
	}
}

void DcGp::DcGpMesh::SetMaterial(DcGp::DcGpMaterial* mat)
{
	if (m_pDcGpMeshImpl->m_material)
	{
		delete m_pDcGpMeshImpl->m_material;
		m_pDcGpMeshImpl->m_material = nullptr;
	}
	m_pDcGpMeshImpl->m_material = mat;
}
