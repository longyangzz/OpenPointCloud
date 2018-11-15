//----------------------------------------------------------------------
//author:wly
//time:
//dsc:创建一个几何体对象，目的是保存几何体顶点属性信息，并关联一个
//封装的opegnGL图元类型，实现图元的渲染
//----------------------------------------------------------------------
#ifndef MPGP_GEOMETRY_H
#define MPGP_GEOMETRY_H

#include "DCCore/BasicTypes.h"

#include "DCGp/GpEntity.h"

#include "State.h"

#include "DCGp/DcGpDLL.h"
namespace DcGp
{
	class DcGpBoundBox;
};
namespace DCGp
{
	class PrimitiveSet;
	class DCGP_API Geometry : public DcGp::DcGpEntity
	{
		Q_OBJECT
		GET_CLASSNAME(Geometry)
	public:
		Geometry(QString name);
		Geometry(QString name, std::vector<DCVector3D> vertexes);

		virtual ~Geometry();

		//!---------------------------------渲染相关--------------------------------------------------------
		virtual void DrawMyselfOnly(DcGp::DcGpDrawContext& context);
		virtual void FastDrawMyselfOnly(DcGp::DcGpDrawContext& context);
		virtual void DrawWithNames(DcGp::DcGpDrawContext& context, bool drawSample = false);

		//! 获取边界盒
		virtual DcGp::DcGpBoundBox GetMyOwnBoundBox();

		//! 设置和获取图元渲染状态属性
		void SetState(State state);
		State& GetState();
        
        //! 设置颜色
		void SetColor(RGBColor colors);

		//设置当前的临时颜色
		//参数一：颜色值
		//参数二：是否自动激活临时颜色
		virtual void SetTempColor(const RGBColor color, bool autoActivate = true);

		//! 唯一色
		virtual const RGBColor GetTempColor() const;

		//!--------------------------------Set/Get property-----------------------------------------
		//! 添加一个图元
		bool AddPrimitiveSet(PrimitiveSet* primitiveset);

		//! 添加一个点位置坐标
		void AddVertex(DCVector3D vertex);

		//! 添加一个颜色坐标
		void AddColorf(DCVector3D colorf);

		void AddTexCoord(DCVector2D texCoord);

		//! 返回顶点
		std::vector<DCVector3D>& GetVertexes();

		//! 纹理坐标
		std::vector<DCVector2D >& GetTexCoords();

		//! 返回顶点数目
		int GetVertexesSize();

		//! 取最小值点
		DCVector3D GetMinPoint();

		//! 取最大值点
		DCVector3D GetMaxPoint();

		DCVector3D& GetVertex(const long index);

		std::vector<PrimitiveSet* > GetPrimitive();
		//! 重置边界盒
		void SetBorderInValid();
	private:
		//计算自身的边界盒
		void CalcMyOwnBoundBox();
		
	private:
		//!------------------------------顶点的属性信息，包括位置、纹理、颜色等----------------------------------------
		std::vector<DCVector3D > m_vertexs;

		std::vector<DCVector3D > m_normals;

		RGBColor m_colors;

		std::vector<DCVector3D > m_secondaryColors;

		std::vector<DCVector2D > m_texCoords;

		std::vector<unsigned int > m_indexs;

		//!------------------------------图元类型----------------------------------------
		std::vector<PrimitiveSet* > m_primitveSets;

		//!-----------------------------渲染支持--------------------------------------------------
		//! 图元渲染state
		State m_state;

		bool m_useVertexBufferObjects;

		//! 边界盒
		DCVector3D m_minPoint;
		DCVector3D m_maxPoint;

		bool m_borderValid;
	};
}

#endif // GEOMETRY_H
