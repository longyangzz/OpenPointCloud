#ifndef REFERENCECLOUD_H
#define REFERENCECLOUD_H

#include <vector>

#include "GpPointCloud.h"

typedef std::vector<unsigned> ReferencesContainer;

namespace DcGp
{
	class DCGP_API ReferenceCloud : public DcGpPointCloud
	{
	public:
		ReferenceCloud(DcGpPointCloud* associatedCloud);
		~ReferenceCloud();

		bool AddPointIndex(unsigned globalIndex);

		unsigned CurrentSize();

		//!\brief 清空点索引
		void Clear();

		//获取m_theIndexes指定位置保存的索引号值
		unsigned GetValue(unsigned index);

		//!\brief 更改大小
		void Resize(unsigned num);

		unsigned GetPointGlobalIndex(unsigned localIndex) const;

		void SetPointIndex(unsigned localIndex, unsigned globalIndex);

		//!\brief获取指定索引位置的坐标值
		DCVector3D GetPoint(unsigned index);

		//!\brief 更新m_theAssociatedCloud关联的标量场的值
		void SetPointScalarValue(unsigned pointIndex, ScalarType value);

		DcGpPointCloud* GetAssociatedCloud() {return m_theAssociatedCloud;}

		void RemovePointGlobalIndex(unsigned localIndex);
	private:
		//! 相关点索引
		ReferencesContainer* m_theIndexes;

		//!\brief 关联的点云对象
		DcGpPointCloud* m_theAssociatedCloud;
	};
}

#endif // REFERENCECLOUD_H
