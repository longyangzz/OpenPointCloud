#ifndef DCGP_OBJECT_H
#define DCGP_OBJECT_H

#include "DCGp/DcGpDLL.h"

//Qt
#include "QString"
class QTableWidget;

#include "DCCore/DCDefs.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpObject)
	DECLARE_SMARTPTR(DcGpObjectImpl)

	class DCGP_API DcGpObject
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpObject)

		DECLARE_IMPLEMENT(DcGpObject)

		GET_CLASSNAME(DcGpObject)
		
	public:
		DcGpObject(QString name);
		virtual ~DcGpObject();

		//是否是指定类型
		inline bool IsA(QString strName);
		//是否属于指定类型
		inline bool IsKindOf(QString strName);

		//获取唯一ID标识
		unsigned long GetUniqueID() const;

		//显示属性
		virtual void ShowProperties(QTableWidget* pWidget);

		//是否可用，常用来标记选中状态
		bool IsEnabled() const;

		//设置可用性
		void SetEnabled(bool status);

		//获取名字
		virtual QString GetName() const;
		//设置名字
		virtual void SetName(QString name);

		//获取类型
		virtual QString GetType() const;
		
		//设置类型
		virtual void SetType(const QString& typeName/*!<_[in]_对象类型标识_*/ );

	protected:
		DcGpObject();
	};

}
#endif // DCOBJECT_H
