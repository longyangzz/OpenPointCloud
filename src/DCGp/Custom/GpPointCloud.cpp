#include "DCGp/Custom/GpPointCloud.h"

//C++标准库
#include <algorithm>
#include <string>
#include <memory>
#include <cmath>
#include <fstream>
#include <iostream>	
#include <array>

//Qt
#include "QtWidgets/QTableWidget"
#include "QtWidgets/QSpinBox"
#include "QTextStream"
#include "QtWidgets/QPushButton"
#include "QFileInfo"
#include "QCoreApplication"

//common
#include "DCCore/Logger.h"
#include "DCCore/Const.h"
#include "DCCore/CoordinatesShiftManager.h"

#include "DCCore/vsShaderLib.h"

#include "DCCore/DCGL.h"
#include "DCGp/GpBasicDevice.h"
#include "DCGp/Custom/GpBoundBox.h"
#include "DCGp/GpDrawContext.h"

//#include "DCFilters/BlockedGridToolkit.h"

//#include "DCUtil/AbstractEntityVisitor.h"

#include "impl/GpPointCloudImpl.hxx"

//default material for clouds (with normals)
#define DC_DEFAULT_CLOUD_AMBIENT_COLOR		DcGp::Color::BRIGHT
#define DC_DEFAULT_CLOUD_SPECULAR_COLOR		DcGp::Color::BRIGHT
#define DC_DEFAULT_CLOUD_DIFFUSE_COLOR		DcGp::Color::BRIGHT
#define DC_DEFAULT_CLOUD_EMISSION_COLOR		DcGp::Color::NIGHT
#define DC_DEFAULT_CLOUD_SHININESS			50.0f
//定义每块数据大小(点个数)
#define DC_CHUNK_COUNT 100000.0f

const unsigned MAX_LOD_NUM = 200000;

DcGp::DcGpPointCloud::DcGpPointCloud()
	: DcGpEntity()
{
	m_pDcGpPointCloudImpl = DcGpPointCloudImpl::CreateObject();
	ShowName(true);
}

DcGp::DcGpPointCloud::DcGpPointCloud(QString name)
	: DcGpEntity(name)
{
	m_pDcGpPointCloudImpl = DcGpPointCloudImpl::CreateObject();
	m_pDcGpPointCloudImpl->m_qIntface = this;
	ShowName(true);
}

DcGp::DcGpPointCloud::DcGpPointCloud(QString name, long num)
	: DcGpEntity(name)
{
	m_pDcGpPointCloudImpl = DcGpPointCloudImpl::CreateObject();
	m_pDcGpPointCloudImpl->m_qIntface = this;

	m_pDcGpPointCloudImpl->m_points.resize(num);
	m_pDcGpPointCloudImpl->m_pointsVisibility.resize(num);
	m_pDcGpPointCloudImpl->m_pointsChoosedState.resize(num);
	std::fill(m_pDcGpPointCloudImpl->m_pointsVisibility.begin(), 
		m_pDcGpPointCloudImpl->m_pointsVisibility.end(), DCCore::POINT_VISIBLE);
	std::fill(m_pDcGpPointCloudImpl->m_pointsChoosedState.begin(), 
		m_pDcGpPointCloudImpl->m_pointsChoosedState.end(), false);

	ShowName(true);
}

DcGp::DcGpPointCloud::DcGpPointCloud(DcGpPointCloud* associatedCloud)
	: DcGpEntity("")
{
	m_pDcGpPointCloudImpl = DcGpPointCloudImpl::CreateObject();
	m_pDcGpPointCloudImpl->m_qIntface = this;
    m_pDcGpPointCloudImpl->m_indexs.clear();

	ShowName(true);
}

DcGp::DcGpPointCloud::~DcGpPointCloud()
{
	/*if (m_pDcGpPointCloudImpl->m_blockedGridToolkit)
	{
		delete m_pDcGpPointCloudImpl->m_blockedGridToolkit;
		m_pDcGpPointCloudImpl->m_blockedGridToolkit = nullptr;
	}*/
}

void DcGp::DcGpPointCloud::SetNormals(std::vector<DCVector3D> normals)
{
	m_pDcGpPointCloudImpl->m_Normals = normals;
}

std::vector<Normal> DcGp::DcGpPointCloud::GetNormals()
{
	return m_pDcGpPointCloudImpl->m_Normals;
}

DcGp::DcGpScalarFieldPtr DcGp::DcGpPointCloud::GetScalarFieldByIndex(int index)
{
	assert(index >= 0);
	return m_pDcGpPointCloudImpl->m_scalarFields[index];
}

int DcGp::DcGpPointCloud::GetScalarFieldIndexByName(QString name)
{
	size_t sfCount = ScalarFieldCount();
	for (size_t i=0; i < sfCount; ++i)
	{
		//确保标量场名称唯一
		if (ScalarFieldNames()[i] == name)
		{
			return static_cast<int>(i);
		}
	}

	return -1;
}

unsigned DcGp::DcGpPointCloud::ScalarFieldCount() const
{
	return m_pDcGpPointCloudImpl->m_scalarFields.size();
}

QStringList DcGp::DcGpPointCloud::ScalarFieldNames() const
{
	QStringList names;
	unsigned number = m_pDcGpPointCloudImpl->m_scalarFields.size();

	for (unsigned i = 0; i < number; ++i)
	{
		names.push_back(m_pDcGpPointCloudImpl->m_scalarFields.at(i)->GetName());
	}
	return names;
}


std::vector<DCVector3D> DcGp::DcGpPointCloud::GetPoints()
{
	return m_pDcGpPointCloudImpl->m_points;
}

std::vector<DCVector3D>& DcGp::DcGpPointCloud::GetPointsRef()
{
	return m_pDcGpPointCloudImpl->m_points;
}

void DcGp::DcGpPointCloud::SetPoints(std::vector<DCVector3D>& points)
{
	m_pDcGpPointCloudImpl->m_points = points;
	
	//初始化状态值
	m_pDcGpPointCloudImpl->m_pointsVisibility.resize(points.size());
	std::fill(m_pDcGpPointCloudImpl->m_pointsVisibility.begin(), m_pDcGpPointCloudImpl->m_pointsVisibility.end(), DCCore::POINT_VISIBLE);

	m_pDcGpPointCloudImpl->m_pointsChoosedState.resize(points.size());
	std::fill(m_pDcGpPointCloudImpl->m_pointsChoosedState.begin(), m_pDcGpPointCloudImpl->m_pointsChoosedState.end(), false);
}

unsigned long DcGp::DcGpPointCloud::Size() const
{
	return m_pDcGpPointCloudImpl->m_points.size();
}

unsigned long DcGp::DcGpPointCloud::GetChunk()
{
	return ceil(Size() / DC_CHUNK_COUNT);
}

void DcGp::DcGpPointCloud::SetColorSize(long num)
{
	m_pDcGpPointCloudImpl->m_rgbColors.reserve(num);
}

//
DCVector3D DcGp::DcGpPointCloud::GetDataByIndex(unsigned index)
{
	if (index >= Size())
	{
		index = Size() - 1;
	}

	return m_pDcGpPointCloudImpl->m_points[index];
}

//添加点(向量版)
void DcGp::DcGpPointCloud::AddPoint(DCVector3D vector)
{
	m_pDcGpPointCloudImpl->m_points.push_back(vector);
	m_pDcGpPointCloudImpl->m_pointsVisibility.push_back(DCCore::POINT_VISIBLE);
	m_pDcGpPointCloudImpl->m_pointsChoosedState.push_back(false);
}

//获取指定点
//必须存在点时才可调用
DCVector3D DcGp::DcGpPointCloud::GetPoint(unsigned index) const
{
	assert(index < Size() && index >= 0);

	if (index >= Size())
	{
		index = Size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpPointCloudImpl->m_points[index];
}

void DcGp::DcGpPointCloud::FastDrawMyselfOnly(DcGpDrawContext& context)
{

	//如果数据隐藏，则返回上层数据
	if (!IsVisible())
	{
		return;
	}

	//是否存在点数据，否则返回上层函数
	if (m_pDcGpPointCloudImpl->m_points.empty())
		return;

	//判断是否是绘制三维物体
	if (MACRO_Draw3D(context))
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (!m_pDcGpPointCloudImpl->m_fastDrawShader.getProgramIndex())
		{
			//! 初始化一个shader program，用来处理快速渲染时候点隐藏的问题
			char* byteGlVersion  = (char*)glGetString(GL_VERSION);
			if (byteGlVersion[0] < '3' )
			{
				return;
			}
			else
			{
				m_pDcGpPointCloudImpl->SetupFastDrawShaders();
			}

		}

		if (m_pDcGpPointCloudImpl->m_fastDrawShader.getProgramIndex())
		{
			glUseProgram(m_pDcGpPointCloudImpl->m_fastDrawShader.getProgramIndex());

			//! 为block量赋值
			double matrixProjection[16];
			double matrixView[16];
			glGetDoublev(GL_PROJECTION_MATRIX, matrixProjection);
			glGetDoublev(GL_MODELVIEW_MATRIX, matrixView);

			DCCore::mat4 matPro;
			DCCore::mat4 matView;
			for (int i = 0; i != 16; ++i)
			{
				matPro.ptr()[i] = matrixProjection[i];
				matView.ptr()[i] = matrixView[i];
			}

			DCCore::mat4 pvm = matPro * matView;

			m_pDcGpPointCloudImpl->m_fastDrawShader.setUniform("pvm", pvm.ptr());
			float isSingleColor = 0;
			m_pDcGpPointCloudImpl->m_fastDrawShader.setUniform("isSingleColor", &isSingleColor);
		}


		//获取显示参数
		glDrawParameters glParams;
		GetDrawingParameters(glParams);
		glParams.showNormals &= bool(MACRO_LightIsEnabled(context));	//是否使用光照


		//! 计算纯色值
		std::vector<float > singleColor;
		if (glParams.showColors && IsColorOverriden())
		{
			singleColor.push_back(GetTempColor()[0]);
			singleColor.push_back(GetTempColor()[1]);
			singleColor.push_back(GetTempColor()[2]);
			glParams.showColors = false;
		}
		else
		{
			singleColor.push_back(context.pointsDefaultCol[0]);
			singleColor.push_back(context.pointsDefaultCol[1]);
			singleColor.push_back(context.pointsDefaultCol[2]);
		}


		// L.O.D.
		unsigned numberOfPoints = Size();
		unsigned sample = 1;//标准的显示全部点
		unsigned totalSamplePoints = numberOfPoints;
		if (numberOfPoints >= DCCore::MAX_FACE_NUM)
		{
			sample = static_cast<int>(ceil(static_cast<float>(numberOfPoints) / DCCore::MAX_FACE_NUM));
		}

		if (m_pDcGpPointCloudImpl->m_pointSize != 0)
			glPointSize((GLfloat)m_pDcGpPointCloudImpl->m_pointSize);


		//! 渲染绘制，因为为shader传入了可见性变量，所以不管什么情况只要控制传入
		//! 顶点、颜色、法向量即可。顶点是不变的，颜色是变化的（纯色、标量色、选取色），法向量是不变的（存在或者不存在）
		//!开启alpha测试
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);
		

		//! 1、每帧中顶点不变
		glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
		glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 3, GL_FLOAT, 0, sample * 3 * sizeof(PointCoordinateType), 
			&(m_pDcGpPointCloudImpl->m_points[0][0]));

		//! 2、每帧中顶点可见性不变，传递顶点的可见性变量到shader中
		glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB1);
		glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB1, 1, GL_INT, 0, sample * 1 * sizeof(int), 
			&(m_pDcGpPointCloudImpl->m_pointsVisibility[0]));

		//! 3、确定颜色值怎么选择（只存在纯色、真彩色、标量色三种）
		if (glParams.showColors)
		{
			glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
			
			if (IsChoosed())
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_rgbColors[0][0]));
			}
		}
		else if (glParams.showScalarField) 
		{
			glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
			if (IsChoosed())
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_scalarColors[0][0]));
			}
		}
		else
		{
			if (IsChoosed())
			{
				float isSingleColor = 0;
				m_pDcGpPointCloudImpl->m_fastDrawShader.setUniform("isSingleColor", &isSingleColor);
				
				glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				//! 纯色处理，由于纯色不是颜色数组，所以采用uniform实现
				float isSingleColor = 1;
				m_pDcGpPointCloudImpl->m_fastDrawShader.setUniform("isSingleColor", &isSingleColor);

				//!传入纯色值
				m_pDcGpPointCloudImpl->m_fastDrawShader.setUniform("singleColor", &singleColor[0]);
			}
			
		}

		
		//! 渲染
		long lodNum = Size();
		if (sample > 1)
		{
			float numFloat = static_cast<float >(lodNum) / sample;
			lodNum = static_cast<unsigned>(floor(numFloat));
		}
		glDrawArrays(GL_POINTS, 0, lodNum);


		glDisableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
		glDisableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB1);
		glDisableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);

		
		glDisable(GL_ALPHA_TEST);

		
		glPopAttrib();
		glUseProgram(0);
	}
}

void DcGp::DcGpPointCloud::DrawWithNamesMyselfOnly(DcGpDrawContext& context, bool drawSample /* = false */)
{
	
}

//绘制点云
void DcGp::DcGpPointCloud::DrawMyselfOnly(DcGpDrawContext& context)
{	
	//如果数据隐藏，则返回上层数据
	if (!IsVisible())
	{
		return;
	}

	//是否存在点数据，否则返回上层函数
	if (m_pDcGpPointCloudImpl->m_points.empty())
		return;

	//判断是否是绘制三维物体
	if (MACRO_Draw3D(context))
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (!m_pDcGpPointCloudImpl->m_generalDrawShader.getProgramIndex())
		{
			//! 初始化一个shader program，用来处理快速渲染时候点隐藏的问题
			char* byteGlVersion  = (char*)glGetString(GL_VERSION);
			if (byteGlVersion[0] < '3' )
			{
				return;
			}
			else
			{
				m_pDcGpPointCloudImpl->SetupGeneralDrawShaders();
			}

		}

		if (m_pDcGpPointCloudImpl->m_generalDrawShader.getProgramIndex())
		{
			glUseProgram(m_pDcGpPointCloudImpl->m_generalDrawShader.getProgramIndex());

			//! 为block量赋值
			double matrixProjection[16];
			double matrixView[16];
			glGetDoublev(GL_PROJECTION_MATRIX, matrixProjection);
			glGetDoublev(GL_MODELVIEW_MATRIX, matrixView);

			DCCore::mat4 matPro;
			DCCore::mat4 matView;
			for (int i = 0; i != 16; ++i)
			{
				matPro.ptr()[i] = matrixProjection[i];
				matView.ptr()[i] = matrixView[i];
			}

			DCCore::mat4 pvm = matPro * matView;

			m_pDcGpPointCloudImpl->m_generalDrawShader.setUniform("pvm", pvm.ptr());
			float isSingleColor = 0;
			m_pDcGpPointCloudImpl->m_generalDrawShader.setUniform("isSingleColor", &isSingleColor);
		}


		//获取显示参数
		glDrawParameters glParams;
		GetDrawingParameters(glParams);
		glParams.showNormals &= bool(MACRO_LightIsEnabled(context));	//是否使用光照


		//! 计算纯色值
		std::vector<float > singleColor;
		if (glParams.showColors && IsColorOverriden())
		{
			singleColor.push_back(GetTempColor()[0]);
			singleColor.push_back(GetTempColor()[1]);
			singleColor.push_back(GetTempColor()[2]);
			glParams.showColors = false;
		}
		else
		{
			singleColor.push_back(context.pointsDefaultCol[0]);
			singleColor.push_back(context.pointsDefaultCol[1]);
			singleColor.push_back(context.pointsDefaultCol[2]);
		}


		// L.O.D.
		unsigned numberOfPoints = Size();
		unsigned decimStep = 0;

		if (m_pDcGpPointCloudImpl->m_pointSize != 0)
			glPointSize((GLfloat)m_pDcGpPointCloudImpl->m_pointSize);


		//! 渲染绘制，因为为shader传入了可见性变量，所以不管什么情况只要控制传入
		//! 顶点、颜色、法向量即可。顶点是不变的，颜色是变化的（纯色、标量色、选取色），法向量是不变的（存在或者不存在）
		//!开启alpha测试
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);

		int sample = 0;

		

		//! 1、每帧中顶点不变
		glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
		glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 3, GL_FLOAT, 0, sample * 3 * sizeof(PointCoordinateType), 
			&(m_pDcGpPointCloudImpl->m_points[0][0]));

		//! 2、每帧中顶点可见性不变，传递顶点的可见性变量到shader中
		glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB1);
		glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB1, 1, GL_INT, 0, sample * 1 * sizeof(int), 
			&(m_pDcGpPointCloudImpl->m_pointsVisibility[0]));

		//! 3、确定颜色值怎么选择（只存在纯色、真彩色、标量色三种）
		if (glParams.showColors)
		{
			glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
			
			if (IsChoosed())
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_rgbColors[0][0]));
			}
		}
		else if (glParams.showScalarField) 
		{
			glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
			if (IsChoosed())
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_scalarColors[0][0]));
			}
		}
		else
		{
			if (IsChoosed())
			{
				float isSingleColor = 0;
				m_pDcGpPointCloudImpl->m_generalDrawShader.setUniform("isSingleColor", &isSingleColor);
				
				glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				//! 纯色处理，由于纯色不是颜色数组，所以采用uniform实现
				float isSingleColor = 1;
				m_pDcGpPointCloudImpl->m_generalDrawShader.setUniform("isSingleColor", &isSingleColor);

				//!传入纯色值
				m_pDcGpPointCloudImpl->m_generalDrawShader.setUniform("singleColor", &singleColor[0]);
			}
			
		}

		
		//! 渲染
		//glDrawArrays(GL_POINTS, 0, numberOfPoints);

		//绘制顶点数组时候均采用循环处理，采用数据定长处理，每次处理100000数据
		if (GetChunk() > 1)
		{
			for (long i = 0; i != GetChunk(); ++i)
			{
				if (i == GetChunk()-1)
				{
					glDrawArrays(GL_POINTS, DC_CHUNK_COUNT * i, Size() - DC_CHUNK_COUNT * i);
				}
				else
				{
					glDrawArrays(GL_POINTS, DC_CHUNK_COUNT * i, DC_CHUNK_COUNT);
				}
			}
		} 
		else
		{
			glDrawArrays(GL_POINTS, 0, Size());
		}

		glDisableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
		glDisableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB1);
		glDisableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);

		
		glDisable(GL_ALPHA_TEST);

		
		glPopAttrib();
		glUseProgram(0);
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
	//绘制文本
	//context._win->Display3DLabel( "Qt", GetPoint(0) + DCVector3D(context.pickedPointsTextShift), DCColor::MAGENTA, QFont());
}

//设置点大小
void DcGp::DcGpPointCloud::SetPointSize(int size)
{
	m_pDcGpPointCloudImpl->m_pointSize = size;
}

//坐标转换
void DcGp::DcGpPointCloud::ApplyTransform(EigenMatrix4d& transMatrix)
{
	//初始化最大值最小值
	m_pDcGpPointCloudImpl->m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
	m_pDcGpPointCloudImpl->m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);

	Eigen::Matrix<double, 1, 3> eshift;
	Point_3D shift = GetShift();

	eshift << shift[0], shift[1], shift[2];

	Eigen::Matrix<double, 1, 3> temp;
	for (unsigned i = 0; i != Size(); ++i)
	{
		temp << m_pDcGpPointCloudImpl->m_points[i][0], 
				m_pDcGpPointCloudImpl->m_points[i][1], 
				m_pDcGpPointCloudImpl->m_points[i][2];

		temp = (temp - eshift) * transMatrix.block(0, 0, 3, 3).transpose() + eshift;
		temp += transMatrix.block(0, 3, 3, 1).transpose();
		m_pDcGpPointCloudImpl->m_points[i][0] = temp[0];
		m_pDcGpPointCloudImpl->m_points[i][1] = temp[1];
		m_pDcGpPointCloudImpl->m_points[i][2] = temp[2];

		//重新计算最大值
		m_pDcGpPointCloudImpl->m_minPoint[0] = std::min(m_pDcGpPointCloudImpl->m_minPoint[0], m_pDcGpPointCloudImpl->m_points[i][0]);
		m_pDcGpPointCloudImpl->m_minPoint[1] = std::min(m_pDcGpPointCloudImpl->m_minPoint[1], m_pDcGpPointCloudImpl->m_points[i][1]);
		m_pDcGpPointCloudImpl->m_minPoint[2] = std::min(m_pDcGpPointCloudImpl->m_minPoint[2], m_pDcGpPointCloudImpl->m_points[i][2]);

		m_pDcGpPointCloudImpl->m_maxPoint[0] = std::max(m_pDcGpPointCloudImpl->m_maxPoint[0], m_pDcGpPointCloudImpl->m_points[i][0]);
		m_pDcGpPointCloudImpl->m_maxPoint[1] = std::max(m_pDcGpPointCloudImpl->m_maxPoint[1], m_pDcGpPointCloudImpl->m_points[i][1]);
		m_pDcGpPointCloudImpl->m_maxPoint[2] = std::max(m_pDcGpPointCloudImpl->m_maxPoint[2], m_pDcGpPointCloudImpl->m_points[i][2]);
	}

	//将Eigen::Matrix4d矩阵值，传给QMatrix4x4类型的矩阵
	//double* matValue = transMatrix.data();

	//m_pDcGpPointCloudImpl->m_pop = QMatrix4x4(matValue);

	//重新计算边界盒
	m_pDcGpPointCloudImpl->SetBorderInValid();
}

//获取显示参数
void DcGp::DcGpPointCloud::GetDrawingParameters(glDrawParameters& params) const
{
	//颜色重写
	if (IsColorOverriden())
	{
		params.showColors = true;
		params.showNormals = HasNormals() &&  NormalsShown();
		params.showScalarField = false;
	}
	else
	{
		params.showScalarField = HasScalarFields() && ScalarFieldShown();
		params.showNormals = HasNormals() &&  NormalsShown();

		params.showColors = !params.showScalarField && HasColors() && ColorsShown();
	}
}

DcGp::DcGpBoundBox DcGp::DcGpPointCloud::GetDisplayBoundBox()
{
	return GetMyOwnBoundBox();
}

//
DcGp::DcGpBoundBox DcGp::DcGpPointCloud::GetMyOwnBoundBox()
{
	DcGpBoundBox emptyBox(GetMinPoint(), 
							GetMaxPoint());

	return emptyBox;
}

//获取指定点的可见性
uchar DcGp::DcGpPointCloud::GetPointVisibility(unsigned index) const
{
	if (index < Size() && index >= 0)
	{
		return m_pDcGpPointCloudImpl->m_pointsVisibility[index];
	}
	return DCCore::POINT_HIDDEN;
}

//获取指定顶点的颜色
PointColor DcGp::DcGpPointCloud::GetPointColor(unsigned index) const
{
	if (index >= Size())
	{
		index = Size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpPointCloudImpl->m_rgbColors[index];

}

//添加颜色
void DcGp::DcGpPointCloud::AddColor(PointColor color)
{
	if (!HasColors())
	{
		ShowColors(true);
	}

	m_pDcGpPointCloudImpl->m_rgbColors.push_back(color);

}

//获取指定点的选中状态
bool DcGp::DcGpPointCloud::GetPointChoosed(unsigned index) const
{
	return m_pDcGpPointCloudImpl->m_pointsChoosedState[index];
}

//设置指定点的选中状态
void DcGp::DcGpPointCloud::SetPointChoosed(unsigned index, bool status)
{

	if (GetPointVisibility(index) == DCCore::POINT_VISIBLE)
	{
		m_pDcGpPointCloudImpl->m_pointsChoosedState[index] = status;

		//设置选取点的颜色
		if (status)
		{
			m_pDcGpPointCloudImpl->m_choosedColors[index][0] = 255;
			m_pDcGpPointCloudImpl->m_choosedColors[index][1] = 0;
			m_pDcGpPointCloudImpl->m_choosedColors[index][2] = 0;
		}
		else
		{
			if (HasColors() && GetColorMode() == eTrueLinearColor)
			{
				m_pDcGpPointCloudImpl->m_choosedColors[index] = m_pDcGpPointCloudImpl->m_rgbColors[index];
			}
			else if (GetColorMode() == eFalseRangeColor ||
				GetColorMode() == eFalseHeightColor ||
				GetColorMode() == eFalseScalarColor)
			{
				m_pDcGpPointCloudImpl->m_choosedColors[index] = m_pDcGpPointCloudImpl->m_scalarColors[index];
			}
			else
			{
				m_pDcGpPointCloudImpl->m_choosedColors[index][0] = GetTempColor()[0];
				m_pDcGpPointCloudImpl->m_choosedColors[index][1] = GetTempColor()[1];
				m_pDcGpPointCloudImpl->m_choosedColors[index][2] = GetTempColor()[2];
			}
		}
	}
}

//启用选取颜色
void DcGp::DcGpPointCloud::EnableChoosedColors()
{
	if (!m_pDcGpPointCloudImpl->m_choosedColors.empty())
	{
		return;
	}

	if (!m_pDcGpPointCloudImpl->m_rgbColors.empty())
	{
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_rgbColors;
	}
	else
	{
		PointColor color = {GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]};
		m_pDcGpPointCloudImpl->m_choosedColors.resize(Size());
		std::fill_n(m_pDcGpPointCloudImpl->m_choosedColors.begin(), Size(), color);
	}
}

//停用选取颜色
void DcGp::DcGpPointCloud::DisableChoosedColors()
{
	m_pDcGpPointCloudImpl->m_choosedColors.clear();
}

//
void DcGp::DcGpPointCloud::EnableVisibiltyTable(bool status, bool flag /* = false */)
{
	m_pDcGpPointCloudImpl->m_visibilityTable = status;
	m_pDcGpPointCloudImpl->m_visibilityType = flag;

	if (m_pDcGpPointCloudImpl->m_visibilityTable)
	{
		if (!m_pDcGpPointCloudImpl->m_visibilityType)   //隐藏选择的点
		{
			//更新点的可见性，然后取消点的选中状态
			unsigned num = Size();
			for (unsigned i = 0; i != num; ++i)
			{
				if (GetPointChoosed(i))  //当前被选中
				{
					SetPointVisibility(i, DCCore::POINT_HIDDEN);
				}
			}
		}
		else  //隐藏未被选择的点
		{
			//更新点的可见性，然后取消点的选中状态
			unsigned num = Size();
			for (unsigned i = 0; i != num; ++i)
			{
				if (!GetPointChoosed(i))  //当前未被选中
				{
					SetPointVisibility(i, DCCore::POINT_HIDDEN);
				}
			}
		}
		CancelChoosed();
	}
	else  //显示所有，禁用可见性表
	{
		m_pDcGpPointCloudImpl->m_pointsVisibility.resize(Size());
		std::fill(m_pDcGpPointCloudImpl->m_pointsVisibility.begin(), m_pDcGpPointCloudImpl->m_pointsVisibility.end(), DCCore::POINT_VISIBLE);
		m_pDcGpPointCloudImpl->m_visibilityTable = false;
		CancelChoosed();
	}
}

bool DcGp::DcGpPointCloud::IsChoosed()
{
	//不仅被选择，而且启用了选取颜色，才算是选取状态
	//return (IsSelected() && !m_pDcGpPointCloudImpl->m_choosedColors.empty());
	return (!m_pDcGpPointCloudImpl->m_choosedColors.empty());
}

//删除所选的部分
void DcGp::DcGpPointCloud::DeleteChoosed()
{
	std::vector<DCVector3D> points;
	std::vector<PointColor> rgbColors;
	std::vector<PointColor> selectColors;
	std::vector<int> pointsVisibility;  //显示表

	for (unsigned i = 0; i < Size(); ++i)
	{
		if (m_pDcGpPointCloudImpl->m_pointsChoosedState[i] == false)
		{
			points.push_back(m_pDcGpPointCloudImpl->m_points[i]);
			selectColors.push_back(m_pDcGpPointCloudImpl->m_choosedColors[i]);
			if (HasColors())
			{
				rgbColors.push_back(m_pDcGpPointCloudImpl->m_rgbColors[i]);
			}

			//取出当前显示状态保存起来
			pointsVisibility.push_back(GetPointVisibility(i));

			//更新标量场值
			
		}
	}
	m_pDcGpPointCloudImpl->m_points = points;
	m_pDcGpPointCloudImpl->m_choosedColors = selectColors;
	m_pDcGpPointCloudImpl->m_rgbColors = rgbColors;
	m_pDcGpPointCloudImpl->m_pointsVisibility = pointsVisibility;
	m_pDcGpPointCloudImpl->m_pointsChoosedState.resize(Size());
	std::fill_n(m_pDcGpPointCloudImpl->m_pointsChoosedState.begin(), Size(), false);
	
	//m_pDcGpPointCloudImpl->m_visibilityTable = false;
	m_pDcGpPointCloudImpl->SetBorderInValid();
}

//取消选取
void DcGp::DcGpPointCloud::CancelChoosed()
{
	if (!IsChoosed())
	{
		return;
	}
	//取消每个点的选取状态
	std::fill_n(m_pDcGpPointCloudImpl->m_pointsChoosedState.begin(), Size(), false);

	//清空每个点的选取颜色
	if (HasColors() && GetColorMode() == eTrueLinearColor)
	{
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_rgbColors;
	}
	else if (GetColorMode() == eFalseRangeColor ||
			GetColorMode() == eFalseHeightColor ||
			GetColorMode() == eFalseScalarColor)
	{
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_scalarColors;
	}
	else
	{
		PointColor color = {GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]};
		std::fill_n(m_pDcGpPointCloudImpl->m_choosedColors.begin(), Size(), color);
	}
}

//刷新选取颜色
void DcGp::DcGpPointCloud::RefreshChoosedColor()
{
	if (m_pDcGpPointCloudImpl->m_choosedColors.size() == 0)
	{
		return;
	}

	switch (GetColorMode())
	{
	case eAmplitudeLinearColor:
		break;
	case eFalseRangeColor:
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_scalarColors;
		break;
	case eFalseHeightColor:
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_scalarColors;
		break;
	case eFalseScalarColor:
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_scalarColors;
		break;
	case eTrueLinearColor:
		if (HasColors())
		{
			//如果数据有真实颜色
			for (unsigned i = 0; i < Size(); ++i)
			{
				if (false == m_pDcGpPointCloudImpl->m_pointsChoosedState[i])
				{
					m_pDcGpPointCloudImpl->m_choosedColors[i] = m_pDcGpPointCloudImpl->m_rgbColors[i];
				}
			}
		}
		else
		{
			//数据没有真实颜色
			for (unsigned i = 0; i < Size(); ++i)
			{
				if (false == m_pDcGpPointCloudImpl->m_pointsChoosedState[i])
				{
					m_pDcGpPointCloudImpl->m_choosedColors[i] = GetTempColor();
				}
			}
		}
		break;
	case eTrueLogarithmicColor:
		break;
	case eSingleColor:
		for (unsigned i = 0; i < Size(); ++i)
		{
			if (false == m_pDcGpPointCloudImpl->m_pointsChoosedState[i])
			{
				m_pDcGpPointCloudImpl->m_choosedColors[i] = GetTempColor();
			}
		}
		break;
	case eDeviationColor:
		break;
	default:
		break;
	}
}

//是否拥有颜色
bool DcGp::DcGpPointCloud::HasColors() const
{
	return !m_pDcGpPointCloudImpl->m_rgbColors.empty();
}

//是否拥有向量场
bool DcGp::DcGpPointCloud::HasNormals() const
{
	return !m_pDcGpPointCloudImpl->m_Normals.empty();
}

//是否拥有标量场
bool DcGp::DcGpPointCloud::HasScalarFields() const
{
	return (!m_pDcGpPointCloudImpl->m_scalarFields.empty());
}

//添加标量场
void DcGp::DcGpPointCloud::AddScalarField(DcGpScalarFieldPtr scalarField)
{
	m_pDcGpPointCloudImpl->m_scalarFields.push_back(scalarField);
}

void DcGp::DcGpPointCloud::ShowScalarField(bool state)
{
	DcGpEntity::ShowScalarField(state);
}

//更改当前标量场的颜色表
void DcGp::DcGpPointCloud::ChangeScalarFieldColorScale(QString name)
{
	if (!m_pDcGpPointCloudImpl->m_currentScalarField)
	{
		return;
	}

	m_pDcGpPointCloudImpl->m_currentScalarField->ChangeColorScale(name);
	m_pDcGpPointCloudImpl->m_currentScalarField->Prepare();
	m_pDcGpPointCloudImpl->m_scalarColors = m_pDcGpPointCloudImpl->m_currentScalarField->GetColors();
}

//设置当前标量场
void DcGp::DcGpPointCloud::SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type)
{
	if (index >= 0 && index < ScalarFieldCount())
	{
		m_pDcGpPointCloudImpl->m_currentScalarField = m_pDcGpPointCloudImpl->m_scalarFields[index];
	}
	else
	{
		return;
	}
	
	//更新高程和距离标量场的值
	m_pDcGpPointCloudImpl->UpdateScalarFieldData();

	m_pDcGpPointCloudImpl->m_currentScalarField->SetColorSteps(setps);
	m_pDcGpPointCloudImpl->m_currentScalarField->Prepare();
	m_pDcGpPointCloudImpl->m_scalarColors = m_pDcGpPointCloudImpl->m_currentScalarField->GetColors();
}

//保存到文件
bool DcGp::DcGpPointCloud::SaveToFile(QFile& file) const
{
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream out(&file);
		//设置格式控制
		out.setRealNumberNotation(QTextStream::FixedNotation);
		out.setRealNumberPrecision(3);
		//获取偏移量坐标值
		Point_3D shift = GetShift();
		for (unsigned i = 0; i < Size(); ++i)
		{
			out << m_pDcGpPointCloudImpl->m_points[i][0] - shift[0] << ' ' 
				<< m_pDcGpPointCloudImpl->m_points[i][1] - shift[1] << ' ' 
				<< m_pDcGpPointCloudImpl->m_points[i][2] - shift[2] << endl;
		}
		file.close();
		return true;
	}
	return false;
}

//自动调整各成员变量中闲置的内存
void DcGp::DcGpPointCloud::AutoAdjust()
{
	//调整点坐标的内存
	m_pDcGpPointCloudImpl->m_points.shrink_to_fit();

	//调整点的显示状态
	m_pDcGpPointCloudImpl->m_pointsVisibility.shrink_to_fit();

	//调整点的选中状态的内存
	m_pDcGpPointCloudImpl->m_pointsChoosedState.shrink_to_fit();

	//调整颜色的内存

	//真彩色
	m_pDcGpPointCloudImpl->m_rgbColors.shrink_to_fit();

	//选取颜色
	m_pDcGpPointCloudImpl->m_choosedColors.shrink_to_fit();

	//标量颜色
	m_pDcGpPointCloudImpl->m_scalarColors.shrink_to_fit();

	//调整标量的内存
	m_pDcGpPointCloudImpl->m_scalarFields.shrink_to_fit();
}

//获取最大点
PointCoord DcGp::DcGpPointCloud::GetMaxPoint() const
{
	m_pDcGpPointCloudImpl->CalculateMyOwnBoundBox();

	PointCoord maxPoint(m_pDcGpPointCloudImpl->m_maxPoint.x, 
						m_pDcGpPointCloudImpl->m_maxPoint.y, 
						m_pDcGpPointCloudImpl->m_maxPoint.z);
	return maxPoint;
}

//获取最小点
PointCoord DcGp::DcGpPointCloud::GetMinPoint() const
{
	m_pDcGpPointCloudImpl->CalculateMyOwnBoundBox();

	PointCoord minPoint(m_pDcGpPointCloudImpl->m_minPoint.x, 
						m_pDcGpPointCloudImpl->m_minPoint.y, 
						m_pDcGpPointCloudImpl->m_minPoint.z);
	return minPoint;
}

int DcGp::DcGpPointCloud::GetPointSize()
{
	return m_pDcGpPointCloudImpl->m_pointSize;
}

void DcGp::DcGpPointCloud::SetRGBColor(std::vector<PointColor> rgbColor)
{
	m_pDcGpPointCloudImpl->m_rgbColors = rgbColor;
}

std::vector<PointColor> DcGp::DcGpPointCloud::GetRGBColor()
{
	return m_pDcGpPointCloudImpl->m_rgbColors;
}

std::vector<PointColor>& DcGp::DcGpPointCloud::GetRGBColorRef()
{
	return m_pDcGpPointCloudImpl->m_rgbColors;
}

void DcGp::DcGpPointCloud::SetSfColorBarVisibility(bool status)
{
	DcGpEntity::SetSfColorBarVisibility(status);
}

void DcGp::DcGpPointCloud::AddColorBarInfo(DcGpDrawContext& context)
{
	if (!m_pDcGpPointCloudImpl->m_currentScalarField)
	{
		return;
	}
	
	context.sfColorScaleToDisplay = static_cast<DcGp::DcGpScalarField*>(m_pDcGpPointCloudImpl->m_currentScalarField.get());
}

void DcGp::DcGpPointCloud::SetPop(QMatrix4x4 matrix)
{
	m_pDcGpPointCloudImpl->m_pop = matrix;
}

QMatrix4x4 DcGp::DcGpPointCloud::GetPop()
{
	return m_pDcGpPointCloudImpl->m_pop;
}


void DcGp::DcGpPointCloud::GetChoosedPoints(std::vector<DCVector3D>& points)
{
	for (unsigned i = 0; i < Size(); ++i)
	{
		if (m_pDcGpPointCloudImpl->m_pointsChoosedState[i] == true)
		{
			points.push_back(m_pDcGpPointCloudImpl->m_points[i]);
		}
	}
}

void DcGp::DcGpPointCloud::SetPointVisibility(unsigned index, uchar visibility)
{
	m_pDcGpPointCloudImpl->m_visibilityTable = true;
	m_pDcGpPointCloudImpl->m_pointsVisibility[index] = visibility;
}

bool DcGp::DcGpPointCloud::HasChoosed()
{
	std::vector<bool >::iterator it = std::find(m_pDcGpPointCloudImpl->m_pointsChoosedState.begin(), m_pDcGpPointCloudImpl->m_pointsChoosedState.end(), true);
	if (it == m_pDcGpPointCloudImpl->m_pointsChoosedState.end())
	{
		return false;
	}
	return true;
}

DCVector3D DcGp::DcGpPointCloud::GetPointNormal(unsigned index) const
{
	if (index >= m_pDcGpPointCloudImpl->m_Normals.size())
	{
		index = m_pDcGpPointCloudImpl->m_Normals.size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpPointCloudImpl->m_Normals[index];
}

//添加点(向量版)
void DcGp::DcGpPointCloud::AddNormal(DCVector3D normal)
{
	m_pDcGpPointCloudImpl->m_Normals.push_back(normal);
}

void DcGp::DcGpPointCloud::SetNormalSize(long num)
{
	m_pDcGpPointCloudImpl->m_Normals.reserve(num);
}

DcGp::DcGpScalarFieldPtr DcGp::DcGpPointCloud::GetCurrentScalarField()
{
	return m_pDcGpPointCloudImpl->m_currentScalarField;
}

int DcGp::DcGpPointCloud::GetCurrentScalarFieldIndex()
{
	if (!m_pDcGpPointCloudImpl->m_currentScalarField)
	{
		return -1;
	}

	return ScalarFieldNames().indexOf(m_pDcGpPointCloudImpl->m_currentScalarField->GetName());
}

void DcGp::DcGpPointCloud::Accept(DCUtil::AbstractEntityVisitor& nv)
{
	nv.Apply(*this);
}

//DcFilter::BlockedGridToolkit* DcGp::DcGpPointCloud::GetBlocked()
//{
//	if (!m_pDcGpPointCloudImpl->m_blockedGridToolkit)
//	{
//		m_pDcGpPointCloudImpl->m_blockedGridToolkit = new DcFilter::BlockedGridToolkit();
//		m_pDcGpPointCloudImpl->m_blockedGridToolkit->SetInputCloud(GetPointsRef());
//		m_pDcGpPointCloudImpl->m_blockedGridToolkit->SetResolution(2, 2);
//		m_pDcGpPointCloudImpl->m_blockedGridToolkit->BlockingGridFromInputCloud();
//	}
//
//	return m_pDcGpPointCloudImpl->m_blockedGridToolkit;
//}