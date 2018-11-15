#ifndef __DynamicBaseClass_H__
#define __DynamicBaseClass_H__

#include <string>
#include <functional>
#include <unordered_map>

#include "DCCoreDLL.h"

class DynamicBaseClass;
#define DYNAMIC_BASE_CLASS_NAME(classname) _##classname##Instance

#define DYNAMIC_BASE_CLASS_DECLARE(classname)\
protected:\
	static DynamicBaseClass* DYNAMIC_BASE_CLASS_NAME(classname); \
	static DynamicBaseClass* createObject()\
	{\
	return (new classname);\
	}

#define DYNAMIC_BASE_CLASS_REGISTER(classname) DynamicBaseClass* \
	classname::DYNAMIC_BASE_CLASS_NAME(classname) = new DynamicBaseClass \
		(#classname,std::bind(&classname::createObject))
	
	
	class DCCORE_EXPORT DynamicBaseClass
	{
		protected:    
			DynamicBaseClass(){}
			
		public:    
			DynamicBaseClass(const std::string& classname,std::function<DynamicBaseClass* (void) > function);    
			virtual ~DynamicBaseClass() {}
			
			static DynamicBaseClass* createObject(const std::string& classname);    
			static void destoryRegisterClasses();    
			static void destoryRegisterClassWithClassName(const std::string& classname);
		
		protected:    
			static void _register(DynamicBaseClass* instance);
	
		private:    
			std::string _classname;    
			std::function<DynamicBaseClass* (void) > _function;
	};
#endif