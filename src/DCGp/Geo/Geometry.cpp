#include "Geometry.h"

//C++
#include <math.h>

#include "PrimitiveSet.h"

//DCCore
#include "DCCore/Const.h"

//dcgp
#include "DCGp/Custom/GpBoundBox.h"
#include "DCGp/GpDrawContext.h"
using namespace DCGp;

Geometry::Geometry(QString name)
	: DcGp::DcGpEntity(name)
	, m_borderValid(false)
	, m_state(State())
{
	m_colors[0] = 0;
	m_colors[1] = 255;
	m_colors[2] = 0;
	SetBorderInValid();
}

Geometry::Geometry(QString name, std::vector<DCVector3D> vertexes)
	: DcGp::DcGpEntity(name)
	, m_vertexs(vertexes)
	, m_borderValid(false)
	, m_state(State())
{
	m_colors[0] = 0;
	m_colors[1] = 255;
	m_colors[2] = 0;
	SetBorderInValid();
	SetSelectionBehavior(DcGpEntity::eSelectionIgnored);

	SetToolTip(GetName());
}

Geometry::~Geometry()
{

}

void Geometry::SetState(State state)
{
	m_state = state;
}

void Geometry::SetColor(RGBColor colors)
{
	m_colors[0] = colors[0];
	m_colors[1] = colors[1];
	m_colors[2] = colors[2];
}

State& Geometry::GetState()
{
	return m_state;
}

std::vector<DCVector3D>& Geometry::GetVertexes()
{
	return m_vertexs;
}

int Geometry::GetVertexesSize()
{
	return m_vertexs.size();
}

DCVector3D Geometry::GetMinPoint()
{
	if (!m_borderValid)
	{
		CalcMyOwnBoundBox();
	}
	return m_minPoint;
}

DCVector3D Geometry::GetMaxPoint()
{
	if (!m_borderValid)
	{
		CalcMyOwnBoundBox();
	}
	return m_maxPoint;
}

std::vector<PrimitiveSet* > Geometry::GetPrimitive()

{
	return m_primitveSets;
}

//设置当前的临时颜色
//参数一：颜色值
//参数二：是否自动激活临时颜色
void Geometry::SetTempColor(const RGBColor color, bool autoActivate /*= true*/)
{
	m_colors = color;
}


//获取临时颜色
const RGBColor Geometry::GetTempColor() const
{
	return m_colors;
}

void Geometry::FastDrawMyselfOnly(DcGp::DcGpDrawContext& context)
{
	DrawMyselfOnly(context);
}

void Geometry::DrawWithNames(DcGp::DcGpDrawContext& context, bool drawSample /* = false */)
{
	if (MACRO_Draw3D(context))
	{
		if (!IsEnabled() || !IsVisible())
		{
			return;
		}
		//! 名字堆栈，用来实现实体捕捉
		bool pushName = MACRO_DrawEntityNames(context);

		if (pushName)
		{
			glPushName(GetUniqueID());
		}


		//! 是否支持使用VBO
		bool usingVBOs = m_useVertexBufferObjects;

		//! 根据是否使用vbo，来确定怎么使用setVertexPointer，即set up arrays
		//! osg是通过state封装了state.setVertexPointer(_vertexData.array.get());
		{
			//使用顶点数组
			//启用顶点数组
			if (!m_vertexs.empty())
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, 0, &m_vertexs[0][0]);
				//m_state.SetVertexPointer(m_vertexs);
			}

			if (!m_texCoords.empty())
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, 0, &m_texCoords[0][0]);
				//m_state.SetTexCoordPointer(2, GL_FLOAT, 0, &m_texCoords[0][0]);
			}
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		/*if (IsSelected())
		{
		glColor3ub(255, 0,0);
		}
		else*/
		{
			//! 纯色显示
			if (!m_colors.empty())
			{
				glColor3ub(m_colors[0],m_colors[1],m_colors[2]);
			}
		}

		//!  上一步执行完了数据设置后，根据当前图元类型，遍历传入的图元类型绘制
		for(unsigned int primitiveSetNum=0; primitiveSetNum!= m_primitveSets.size(); ++primitiveSetNum)
		{
			//解引用数组,传给显卡.
			//auto aa = m_primitveSets[0]->GetMode();
			//glDrawArrays(m_primitveSets[0]->GetMode(), 0, m_vertexs.size());

			//! 图元绘制之前要开启对应的model和设置对应的属性，然后才是绘制，所以在这个步骤前
			//! 应该有两个类来管理opengl的 glEnable（osgMode），属性（Attrib）（osgPolygonMode）
			//! 在osg中这两个东西都存放于state类中

			//! 应用属性设置
			//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE | GL_POLYGON | GL_POINT);
			m_state.Apply();
			m_primitveSets[primitiveSetNum]->Draw(false);
		}

		glPopAttrib();

		if (pushName)
		{
			glPopName();
		}
	}

	//m_state.ApplyDisablingOfVertexAttributes();
	if (!m_vertexs.empty())
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	if (!m_texCoords.empty())
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void Geometry::DrawMyselfOnly(DcGp::DcGpDrawContext& context)
{
	if (MACRO_Draw3D(context))
	{
		//! 是否支持使用VBO
		bool usingVBOs = m_useVertexBufferObjects;

		//! 根据是否使用vbo，来确定怎么使用setVertexPointer，即set up arrays
		//! osg是通过state封装了state.setVertexPointer(_vertexData.array.get());
		{
			//使用顶点数组
			//启用顶点数组
			if (!m_vertexs.empty())
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, 0, &m_vertexs[0][0]);
				//m_state.SetVertexPointer(m_vertexs);
			}
			
			if (!m_texCoords.empty())
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, 0, &m_texCoords[0][0]);
				//m_state.SetTexCoordPointer(2, GL_FLOAT, 0, &m_texCoords[0][0]);
			}
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		if (IsSelected())
		{
			glColor3ub(255, 0,0);
		}
		else
		{
			//! 纯色显示
			if (!m_colors.empty())
			{
				glColor3ub(m_colors[0],m_colors[1],m_colors[2]);
			}
		}

		//!  上一步执行完了数据设置后，根据当前图元类型，遍历传入的图元类型绘制
		for(unsigned int primitiveSetNum=0; primitiveSetNum!= m_primitveSets.size(); ++primitiveSetNum)
		{
			//解引用数组,传给显卡.
			//auto aa = m_primitveSets[0]->GetMode();
			//glDrawArrays(m_primitveSets[0]->GetMode(), 0, m_vertexs.size());

			//! 图元绘制之前要开启对应的model和设置对应的属性，然后才是绘制，所以在这个步骤前
			//! 应该有两个类来管理opengl的 glEnable（osgMode），属性（Attrib）（osgPolygonMode）
			//! 在osg中这两个东西都存放于state类中

			//! 应用属性设置
			//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE | GL_POLYGON | GL_POINT);
			m_state.Apply();
			m_primitveSets[primitiveSetNum]->Draw(false);
		}

		glPopAttrib();
	}

	//m_state.ApplyDisablingOfVertexAttributes();
	if (!m_vertexs.empty())
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	if (!m_texCoords.empty())
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

bool Geometry::AddPrimitiveSet(PrimitiveSet* primitiveset)
{
	m_primitveSets.push_back(primitiveset);

	return true;
}

void Geometry::AddVertex(DCVector3D vertex)
{
	m_vertexs.push_back(vertex);
}

void Geometry::AddTexCoord(DCVector2D texCoord)
{
	m_texCoords.push_back(texCoord);
}

void Geometry::AddColorf(DCVector3D colorf)
{
	//m_colors.push_back(colorf);
}

std::vector<DCVector2D >& Geometry::GetTexCoords()
{
	return m_texCoords;
}


void Geometry::CalcMyOwnBoundBox()
{
	if (m_borderValid)
	{
		return ;
	}
	std::vector<DCVector3D> bndaryPoint = m_vertexs;

	//根据m_points，计算最小最大值m_minPoint，m_maxPoint
	for (auto i = 0; i < bndaryPoint.size(); ++i)
	{
		m_minPoint[0] = std::min(m_minPoint[0], bndaryPoint[i][0]);
		m_minPoint[1] = std::min(m_minPoint[1], bndaryPoint[i][1]);
		m_minPoint[2] = std::min(m_minPoint[2], bndaryPoint[i][2]);

		m_maxPoint[0] = std::max(m_maxPoint[0], bndaryPoint[i][0]);
		m_maxPoint[1] = std::max(m_maxPoint[1], bndaryPoint[i][1]);
		m_maxPoint[2] = std::max(m_maxPoint[2], bndaryPoint[i][2]);
	}

	m_borderValid =  true;
}

void Geometry::SetBorderInValid()
{
	m_borderValid = false;

	m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
	m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);

}

DcGp::DcGpBoundBox Geometry::GetMyOwnBoundBox()
{
	DcGp::DcGpBoundBox emptyBox(GetMinPoint(), GetMaxPoint());
	return emptyBox;
}

DCVector3D& Geometry::GetVertex(const long index)
{
	return m_vertexs[index];
}