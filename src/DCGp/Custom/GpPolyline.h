#ifndef DCGP_POLYLINE_H
#define DCGP_POLYLINE_H

#include "DCGp/DcGpDLL.h"

#include <vector>

#include "DCCore/DCDefs.h"
#include "DCGp/GpBasicTypes.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpPolyline)
	DECLARE_SMARTPTR(DcGpPolylineImpl)
	//简单的多段线类

	class DCGP_API DcGpPolyline
	{
		DECLARE_IMPLEMENT(DcGpPolyline)
	public:

		//构造函数
		DcGpPolyline();
		//析构函数
		virtual ~DcGpPolyline()
		{}

		//添加点
		inline void AddPoint(PointCoord point);

		//获取点
		inline PointCoord GetPoint(unsigned index) const;

		//获取点（向量版）
		inline void GetPoint(unsigned index, DCVector3D& point) const;

		//获取所有顶点
		inline std::vector<DCVector3D> GetPoints() const;
		void SetPoints(std::vector<DCVector3D> pts);

		//设置全局颜色
		inline void SetGlobalColor(RGBColor color);

		//获取全局颜色
		inline RGBColor GetGlobalColor() const;

		//获取闭合状态
		inline bool GetClosingState() const;

		//设置闭合状态
		inline void SetClosingState(bool state);
		
		void ChangePointByIndex(unsigned index, int x, int y);

		void ChangePointByIndex(unsigned index, DCVector3D point);

		void DeleteLastPoint();

		//清除所有顶点
		inline void Clear();

		//顶点数目
		inline unsigned GetVertexNumber() const;
	};

}

#endif //DC_POLYLINE_HEADER
