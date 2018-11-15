#ifndef GP_POINT_CLOUD_IMPL_H
#define GP_POINT_CLOUD_IMPL_H

//Qt
#include "QtGui/QMatrix4x4"

//DcCore
#include "DCCore/DCDefs.h"
#include "DCCore/DCGL.h"
#include "DCCore/vsShaderLib.h"

#include "DCGp/GpBasicDevice.h"
#include "DCGp/Custom/GpBoundBox.h"
#include "DCGp/GpDrawContext.h"
#include "DCGp/Custom/GpScalarField.h"

//#include "DCFilters/BlockedGridToolkit.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpPointCloudImpl)
	class DcGpPointCloudImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpPointCloudImpl)
	public:
		~DcGpPointCloudImpl();

		GLuint SetupFastDrawShaders();

		GLuint SetupGeneralDrawShaders();

		void SetBorderInValid();

		//计算当前实体对象最大最小值
		void CalculateMyOwnBoundBox();

		void UpdateScalarFieldData();

	protected:
		DcGpPointCloudImpl();

	private:
		friend class DcGpPointCloud;

		//点数据
		std::vector<DCVector3D> m_points;
		//点的显示状态
		std::vector<int> m_pointsVisibility;
		//点的选中状态
		std::vector<bool> m_pointsChoosedState;
		//是否只显示部分内容
		bool m_visibilityTable;
		//部分显示的方式
		bool m_visibilityType;
		std::vector<DCVector3D> m_Normals;
		//各顶点颜色
		std::vector<PointColor> m_rgbColors;
		//标量场颜色
		std::vector<PointColor> m_scalarColors;
		//选取状态下的颜色
		std::vector<PointColor> m_choosedColors;
		//标量场
		std::vector<DcGpScalarFieldPtr> m_scalarFields;

		//当前使用的标量场
		DcGpScalarFieldPtr m_currentScalarField;
		//点的索引
		std::vector<unsigned long> m_indexs;
		//点大小
		unsigned m_pointSize;
		//XYZ范围
		DCVector3D m_minPoint;
		DCVector3D m_maxPoint;

		unsigned int m_nVBOVertices;

		//是否已经计算过最大最小值
		bool m_borderValid;

		//点云当前转换矩阵
		QMatrix4x4 m_pop;

		//分区结构
		//DcFilter::BlockedGridToolkit* m_blockedGridToolkit;

		DcGpPointCloud* m_qIntface;

		//! 旋转平移缩放时候抽稀采样使用的shader
		VSShaderLib m_fastDrawShader;

		//! 普通情况渲染绘制
		VSShaderLib m_generalDrawShader;
	};
}

#endif