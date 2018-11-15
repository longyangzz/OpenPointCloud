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
*【功能概述】场景对象，存放图元实体等
*记录存放在view类中，实现场景的渲染
*作者：dclw         时间：%time%
*文件名：%Scene%
*版本：V2.1.0
*$Scene$
*修改者：          时间：
*修改说明：
*===========================================================================
*/

#ifndef SCENE_H
#define SCENE_H

//boost
#include <boost/shared_ptr.hpp>

namespace DcGp
{
	class DcGpEntity;
};
class Scene
{
public:
	Scene();
	~Scene();

	//! 接口
	void SetRootEntity(DcGp::DcGpEntity* entity);
	DcGp::DcGpEntity* GetRootEntity();

	//! 添加一个实体对象
	void AddEntity(DcGp::DcGpEntity* subEntity);
private:
	//! 实体根节点
	DcGp::DcGpEntity* m_rootEntity;
};

typedef boost::shared_ptr<Scene> ScenePtr;

#endif // SCENE_H
