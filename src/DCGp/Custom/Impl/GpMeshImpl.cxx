#include "GpMeshImpl.hxx"

//IL
#include <IL/il.h>

#include "QtGui/QImage"

#include "DCCore/Logger.h"

DcGp::DcGpMeshImpl::DcGpMeshImpl()
	: m_borderValid(false)
	, m_visibilityTable(false)
	, m_targetTex(0)
	, m_material(nullptr)
{
	m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
	m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);
	m_showStyle = DcGp::eShowFill;

	m_currentScalarField = DcGpScalarFieldPtr();

	//初始化高程标量场
	DcGpScalarFieldPtr heightScalarField = DcGpScalarFieldPtr(new DcGpScalarField("Height"));
	m_scalarFields.push_back(heightScalarField);

	//初始化到原点距离标量场
	DcGpScalarFieldPtr distanceScalarField = DcGpScalarFieldPtr(new DcGpScalarField("Distacne"));
	m_scalarFields.push_back(distanceScalarField);
}

void DcGp::DcGpMeshImpl::CalculateMyOwnBoundBox()
{
	if (!m_borderValid)
	{
		//根据m_points，计算最小最大值m_minPoint，m_maxPoint
		for (auto i = 0; i < m_points.size(); ++i)
		{
			m_minPoint[0] = std::min(m_minPoint[0], m_points[i][0]);
			m_minPoint[1] = std::min(m_minPoint[1], m_points[i][1]);
			m_minPoint[2] = std::min(m_minPoint[2], m_points[i][2]);

			m_maxPoint[0] = std::max(m_maxPoint[0], m_points[i][0]);
			m_maxPoint[1] = std::max(m_maxPoint[1], m_points[i][1]);
			m_maxPoint[2] = std::max(m_maxPoint[2], m_points[i][2]);
		}
		//更新状态值
		m_borderValid = true;
	}
}

//更新标量场
void DcGp::DcGpMeshImpl::UpdateScalarFieldData()
{
	int num = m_scalarFields.size();
	unsigned long pNum = m_points.size();

	for (int i = 0; i != num; ++i)
	{
		//该函数更新的是数据，当点坐标和标量场data个数相等的时候就不需要更新
		if (pNum == m_scalarFields[i]->GetDatas().size())
		{
			continue;
		}

		QString scalarName = m_scalarFields[i]->GetName();
		if (scalarName == QObject::tr("Height"))
		{
			m_scalarFields[i]->GetDatas().clear();
			for (unsigned j = 0; j != pNum; ++j)
			{
				m_scalarFields[i]->AddData(m_points[j].z);
			}
		}
		else if (scalarName == QObject::tr("Distacne"))
		{
			m_scalarFields[i]->GetDatas().clear();

			for (unsigned j = 0; j != pNum; ++j)
			{
				m_scalarFields[i]->AddData(sqrt(m_points[j].x * m_points[j].x 
					+ m_points[j].y * m_points[j].y 
					+ m_points[j].z * m_points[j].z));
			}
		}
	}
}

//添加纹理
bool DcGp::DcGpMeshImpl::LoadGLTextureByDevil(int* width /*= 0*/, int* height/* = 0*/)
{
	if (m_targetTex) 
	{
		glDeleteTextures(1, &m_targetTex);
		m_targetTex = 0;
	}

	ILboolean success;

	//初始化前,必须检查IL的版本
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		// 版本不对应
		std::string err_msg = "Wrong DevIL version. Old devil.dll in system32/SysWow64?";
		char* cErr_msg = (char *) err_msg.c_str();
		return false;
	}

	// 初始化DevIL
	ilInit();

	int numTextures = 1;

	// DevIL image IDs 数组
	ILuint imageIds = NULL;

	// 创建 DevIL Image IDs 
	ilGenImages(numTextures, &imageIds);

	// 绑定名字
	glGenTextures(numTextures, &m_targetTex);

	//for (int i=0; i<numTextures; i++) 目前处理一个对象 
	{
		//绑定DevIL 图像名
		ilBindImage(imageIds);
		std::string fileloc = m_teximgFileName.toStdString();
		
		//加载图像
		success = ilLoadImage(fileloc.c_str());

		if (success)
		{
			//类型转换,将 颜色值转换为RGB值 如果包含Alpha值可以使用IL_RGBA
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); 
			if (!success)
			{
				return false;
			}

			//绑定纹理
			glBindTexture(GL_TEXTURE_2D, m_targetTex); 
			//放大筛选器 使用线性插值
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
			//修正滤波器 使用线性插值
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//指定纹理
			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
				ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
				ilGetData()); 

			if (width && height)
			{
				*width = ilGetInteger(IL_IMAGE_WIDTH);
				*height = ilGetInteger(IL_IMAGE_HEIGHT);
			}
		}
		else
		{
			QImage image(m_teximgFileName);
			if (image.isNull())
			{
				DCCore::Logger::Error(QString("纹理图片%1读取失败").arg(m_teximgFileName));
				return false;
			} 
			
			QImage tximg = QGLWidget::convertToGLFormat(image);
			glBindTexture(GL_TEXTURE_2D, m_targetTex);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tximg.width(), tximg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());
			
			if (width && height)
			{
				*width = tximg.width();
				*height = tximg.height();
			}
		}
	}

	//释放内存
	ilDeleteImages(numTextures, &imageIds); 
	imageIds = NULL;

	return true;
}
