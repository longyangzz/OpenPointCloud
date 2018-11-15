#ifndef MPUTIL_HITS_H
#define MPUTIL_HITS_H

#include "DCCore/BasicTypes.h"

#include "mputilDLL.h"

namespace DcGp
{
	class DcGpEntity;
}

namespace MPUtil
{
	class MPUTIL_EXPORT Hit
	{
	public:
		Hit();
		~Hit();

		Hit(DcGp::DcGpEntity* obj, DCCore::Point_3 wPoint, unsigned long ix);

		Hit(DcGp::DcGpEntity* obj);

		DCCore::Point_3 GetWordPoint() const;

		DcGp::DcGpEntity* GetEntity() const;
	private:

		//! 实体对象
		DcGp::DcGpEntity* entity;

		//! 点索引号
		unsigned long index;

		//! 世界坐标
		DCCore::Point_3 wordPoint;

		//! 屏幕坐标
		DCCore::Point_3 screenPoint;
	};

}

#endif // MPUTIL_HITS_H
