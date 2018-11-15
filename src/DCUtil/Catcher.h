//----------------------------------------------------------------------
//author:wly
//time:
//dsc:通过传入当前实体根节点以射线点，通过射线求交获取捕捉点信息
//
//----------------------------------------------------------------------

#ifndef MPUTIL_POINTPICKING_H
#define MPUTIL_POINTPICKING_H

#include "DCCore/BasicTypes.h"

#include "mputilDLL.h"

#include "Common.h"

#include "Hit.h"

namespace DcGp
{
	class DcGpEntity;
}

namespace MPUtil
{
	class MPUTIL_EXPORT Catcher
	{
	public:
		Catcher(PickMask mask = eVisible | ePointCloud);
		~Catcher();

		//! 传入球半径--对应的是当前视窗4倍像素大小
		void SetPixelSize(float psize);

		void ComputeIntersections(DcGp::DcGpEntity* sceneRoot, const DCCore::Point_3 v1, const DCCore::Point_3 v2, HitList& hitList);
	
		//! 设置捕捉器的捕捉mask
		void SetMask(PickMask mask);

		//! 设置模型视图矩阵
		 void SetModelViewMatrix(const double* modelViewMat);
	private:
		PickMask m_pickMask;

		//! 像素大小
		float m_pixelSize;

		//模型视图矩阵
		double* m_modelViewMatrix;
	
	};
}

#endif // MPUTIL_POINTPICKING_H
