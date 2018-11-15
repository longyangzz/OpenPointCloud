
//Common
#include "DCCore/Const.h"

#include "DCCore/DCDefs.h"

#include "DCGp/GpBasicDevice.h"
#include "../GpBoundBox.h"
#include "DCGp/GpDrawContext.h"
#include "../GpScalarField.h"

#include "DCCore/DCGL.h"

#include "DCGp/GpBasicTypes.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpMeshImpl)
	//! DcGpMesh 的数据实现
	/**
		* 主要管理GpMesh中的数据
	*/
	class DcGpMeshImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpMeshImpl)

		//计算当前实体对象最大最小值
		void CalculateMyOwnBoundBox();

		bool LoadGLTextureByDevil(int* width = 0, int* height = 0);

		void UpdateScalarFieldData();
			 
	protected:
		DcGpMeshImpl();

	private:
		friend class DcGpMesh;

		std::vector<DCVector3D> m_points;
		unsigned m_globalIterator;

		//XYZ范围
		DCVector3D m_minPoint;
		DCVector3D m_maxPoint;

		//连接关系
		DcGp::LinkContainer m_links;

		//拓扑关系
		std::vector<std::array<long, 3>> m_topologys;

		//显示模式
		DcGp::ShowStyle m_showStyle;
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

		//!\brief 德罗尼三角网个数
		unsigned m_numberOfTriangles;

		//!\brief 是否已经计算过最大最小值
		bool m_borderValid;

		//顶点压缩法量
		std::vector<NormalType> m_comPressNormal;

		//选取状态下的颜色
		std::vector<PointColor> m_selectedColors;

		//是否只显示部分内容
		bool m_visibilityTable;

		//mesh的选中状态
		std::vector<bool> m_meshsSelected;

		//!\brief 目标纹理对象
		GLuint m_targetTex;

		//!\brief 纹理坐标
		std::vector<DCVector2D> m_texCoords;

		//!\brief 绑定的影像文件名
		QString m_teximgFileName;

		//! 默认材质
		DcGpMaterial* m_material; 
	};
}