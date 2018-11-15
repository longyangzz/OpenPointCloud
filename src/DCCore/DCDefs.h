#ifndef DC_DEFS_H
#define DC_DEFS_H

//Boost
#include "boost/shared_ptr.hpp"

//声明智能指针
#ifndef DECLARE_SMARTPTR
#define DECLARE_SMARTPTR(ClassName)\
class ClassName;\
	typedef boost::shared_ptr<ClassName> ClassName##Ptr;
#endif

//类型装换
 #ifndef SMARTPTR_DYNAMIC_CAST
 #define SMARTPTR_DYNAMIC_CAST(ClassName)\
 	boost::dynamic_pointer_cast<ClassName>
 #endif

//定义伪构造函数
#ifndef DECLARE_PHONY_CONSTRUCTOR
#define DECLARE_PHONY_CONSTRUCTOR(ClassName)\
public:\
	static ClassName##Ptr CreateObject()\
	{\
	return ClassName##Ptr(new ClassName);\
	}
#endif

//定义实现
#ifndef DECLARE_IMPLEMENT
#define DECLARE_IMPLEMENT(ClassName)\
private:\
	ClassName##ImplPtr m_p##ClassName##Impl;
#endif

//定义类型名函数
#ifndef GET_CLASSNAME
#define GET_CLASSNAME(ClassName)\
public:\
	virtual QString GetClassname()\
	{\
	return #ClassName;\
	}
#endif

#endif