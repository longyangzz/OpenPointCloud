#ifndef MPUTIL_ENTITYINTERSECTIONVISITOR_H
#define MPUTIL_ENTITYINTERSECTIONVISITOR_H

#include "EntityVisitor.h"
#include "Common.h"
#include "Hit.h"

namespace MPUtil
{
	class EntityIntersectionVisitor : public EntityVisitor
	{
	public:
		EntityIntersectionVisitor(DCCore::Point_3 v1, DCCore::Point_3 v2);
		~EntityIntersectionVisitor();

		//! 实体及点云
		virtual void Apply(DcGp::DcGpEntity& node);
		virtual void Apply(DcGp::DcGpPointCloud& node);

		virtual void Apply(MPGp::DistanceLine& node);

		virtual void Apply(MPGp::IndividualPlane& node);

		virtual void Apply(MPGp::Outlines& node);

		virtual void Apply(MPGp::IntersectionLine& node);

		virtual void Apply(MPGp::OperationBox& node);

		//! 传入球半径--对应的是当前视窗4倍像素大小
		void SetPixelSize(float psize);

		void SetMask(PickMask mask);

		//返回结果
		HitList GetHits();

		//! 设置模型视图矩阵
		void SetModelViewMatrix(const double* modelViewMat);

		//double* GetModelViewMatrix() const;
	private:
		//过滤模式
		PickMask m_mask;

		//! 当点求交时候球半径
		float m_pixelSize;

		//! 射线坐标
		DCCore::Point_3 m_nearPoint;

		DCCore::Point_3 m_farPoint;

		//! nearPoint与边界盒交点的距离，用来执行深度监测的功能
		float m_intersectionDist;

		//! 返回值hits
		HitList m_hits;

		//! 模型视图矩阵
		double* m_modelViewMatrix;
	};

}

#endif // MPUTIL_ENTITYINTERSECTIONVISITOR_H
