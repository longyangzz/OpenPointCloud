#include "DCCore/DynamicBaseClass.h"

#include <algorithm>
static std::unordered_map<std::string,DynamicBaseClass*>& getClassInfoMap()
{    
	static std::unordered_map<std::string,DynamicBaseClass*> classInfoMap;    
	return classInfoMap;
}

DynamicBaseClass::DynamicBaseClass(const std::string& classname,std::function<DynamicBaseClass* (void) > function)
{    
	_classname = classname;    
	_function = function;    
	_register(this);
}

DynamicBaseClass* DynamicBaseClass::createObject(const std::string& classname)
{    
	auto iter = getClassInfoMap().find(classname);    
	if (iter != getClassInfoMap().end())        
		return iter->second->_function();    
	return nullptr;
}

void DynamicBaseClass::_register(DynamicBaseClass *instance)
{    
	getClassInfoMap().insert(std::make_pair(instance->_classname, instance));
}

void DynamicBaseClass::destoryRegisterClasses()
{    
	auto iter=getClassInfoMap().begin();    
	for (;iter!=getClassInfoMap().end();) 
	{       
		delete iter->second;        
		getClassInfoMap().erase(iter++);    
	}
}

void DynamicBaseClass::destoryRegisterClassWithClassName(const std::string& classname)
{    
	if (classname.empty()) 
	{        return;    }    
	auto iter = getClassInfoMap().find(classname);   
	if (iter != getClassInfoMap().end()) 
	{        
		delete iter->second;        
		getClassInfoMap().erase(iter);    
	}
}