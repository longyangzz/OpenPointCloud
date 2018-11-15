#include "GpObject.h"

//Qt
#include "QString"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpObjectImpl)

	class DcGpObjectImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpObjectImpl)

	protected:
		DcGpObjectImpl()
			: m_name("")
			, m_type("Unknow")
			, m_enabled(true)
		{
			//统计创建的数量
			++s_count;

			m_uniqueID = s_count;
		}

	private:
		friend class DcGpObject;

		//创建的DcGpObject的数量
		static unsigned long s_count;

		//可使用性
		bool m_enabled;

		//唯一标识ID，用于OpenGL选择使用
		unsigned long m_uniqueID;

		//名字
		QString m_name;

		//! 对象类型标识
		QString m_type;
	};
}

unsigned long DcGp::DcGpObjectImpl::s_count = 0;

DcGp::DcGpObject::DcGpObject()
{
	m_pDcGpObjectImpl = DcGpObjectImpl::CreateObject();
}

DcGp::DcGpObject::DcGpObject(QString name)
{
	m_pDcGpObjectImpl = DcGpObjectImpl::CreateObject();

	m_pDcGpObjectImpl->m_name = name;
}

DcGp::DcGpObject::~DcGpObject()
{
	//delete m_pDcGpObjectImpl.get();
}

bool DcGp::DcGpObject::IsA(QString strName)
{
	return GetClassname() == strName;
}

bool DcGp::DcGpObject::IsKindOf(QString strName)
{
	return DcGpObject::IsA(strName);
}

void DcGp::DcGpObject::ShowProperties(QTableWidget* pWidget)
{

}

unsigned long DcGp::DcGpObject::GetUniqueID() const 
{
	return m_pDcGpObjectImpl->m_uniqueID;
}

//是否可用
bool DcGp::DcGpObject::IsEnabled() const 
{
	return m_pDcGpObjectImpl->m_enabled;
}

//设置可用性
void DcGp::DcGpObject::SetEnabled(bool status)
{
	m_pDcGpObjectImpl->m_enabled = status;
}

//获取名字
QString DcGp::DcGpObject::GetName() const
{
	return m_pDcGpObjectImpl->m_name;
}
//设置名字
void DcGp::DcGpObject::SetName(QString name)
{
	m_pDcGpObjectImpl->m_name = name;
}

QString DcGp::DcGpObject::GetType() const
{
	return m_pDcGpObjectImpl->m_type;
}

void DcGp::DcGpObject::SetType(const QString& typeName)
{
	m_pDcGpObjectImpl->m_type = typeName;
}