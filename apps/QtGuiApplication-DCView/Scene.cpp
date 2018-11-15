//##########################################################################
//#                                                                        #
//#                            DCLW                                        #
//#                                                                        #
//#  This library is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU Lesser General Public License(LGPL) as  #
//#  published by the Free Software Foundation; version 2 of the License.  #
//#                                                                        #
//#  This library is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU Lesser General Public License for more details.                   #
//#                                                                        #
//#          COPYRIGHT: DCLW             																	 #
//#                                                                        #
//##########################################################################

/*==========================================================================
*【功能概述】
*
*作者：dclw         时间：$time$
*文件名：$safeitemname$
*版本：V2.1.0
*
*修改者：          时间：
*修改说明：
*===========================================================================
*/

#include "Scene.h"

//DCGp
#include "DCGp/GpEntity.h"

Scene::Scene()
	:m_rootEntity(nullptr)
{
	//创建一个root实体对象
	m_rootEntity = new DcGp::DcGpEntity("root");
}

Scene::~Scene()
{
	delete m_rootEntity;
	m_rootEntity = nullptr;
}

void Scene::SetRootEntity(DcGp::DcGpEntity* entity)
{
	m_rootEntity = entity;
	
}

DcGp::DcGpEntity* Scene::GetRootEntity()
{
	return m_rootEntity;
}

void Scene::AddEntity(DcGp::DcGpEntity* subEntity)
{
	m_rootEntity->AddChild(subEntity);
}