#ifndef DCGP_BOUND_BOX_H
#define DCGP_BOUND_BOX_H

#include "DCGp/DcGpDLL.h"

#include "DCCore/DCDefs.h"

#include "QString"

//DCCore
#include "DCCore/BasicTypes.h"
#include "DCCore/Matrix.h"

#include "DCGp/GpMatrix.h"
#include "DCGp/GpBasicTypes.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpBoundBoxImpl)

class DCGP_API DcGpBoundBox
{
	DECLARE_IMPLEMENT(DcGpBoundBox)

	GET_CLASSNAME(DcGpBoundBox)

public:

	DcGpBoundBox();

	DcGpBoundBox(const DcGpBoundBox& aBBox);
	DcGpBoundBox(const DCVector3D &bbMinCorner, const DCVector3D &bbMaxCorner);

	//! 求和
	DcGpBoundBox operator + (const DcGpBoundBox& aBBox) const;

	const DcGpBoundBox& operator += (const DcGpBoundBox& aBBox);
	//! 偏移
	const DcGpBoundBox& operator += (const DCVector3D& aVector);
	//!偏移
	const DcGpBoundBox& operator -= (const DCVector3D& aVector);
	//!缩放
	const DcGpBoundBox& operator *= (const PointCoordinateType& scaleFactor);
	//! 旋转
	const DcGpBoundBox& operator *= (const DCCore::SquareMatrix& aMatrix);
	//!加载矩阵
	const DcGpBoundBox& operator *= (const DcGpMatrix& mat);

	void Clear();

	//等于operator += (const DCVector3D& aVector);
	void Add(const DCVector3D& aVector);

	//! 左下角坐标
	const DCVector3D& MinCorner() const;
	//! 右上角坐标
	const DCVector3D& MaxCorner() const;

	//! 左下角坐标
	DCVector3D& MinCorner();
	//! 右上角坐标
	DCVector3D& MaxCorner();

	//! 中心
	DCVector3D GetCenter() const;
	//! 对角线向量
	DCVector3D GetDiagVector() const;
	//! 对角线长度
	PointCoordinateType GetDiagNormal() const;

	//绘制边界盒
	void Draw(const ColorType col[]) const;

	//! 设置边界盒可见状态
	void SetValidity(bool state);

	//! 返回是否可见
	bool IsValid() const;

	//! 返回两个边界盒的绝对距离
	// 返回-1至少一个边界盒无效
	PointCoordinateType MinDistTo(const DcGpBoundBox& box) const;

	//返回点是否在边界盒中
	inline bool Contains(const DCVector3D& P) const;
};
}
#endif
