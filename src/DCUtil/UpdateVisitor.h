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
*作者：dclw         时间：%time%
*文件名：%safeitemname%
*版本：V2.1.0
*$safeitemrootname$
*修改者：          时间：
*修改说明：
*===========================================================================
*/

#ifndef MPUTIL_UPDATEVISITOR_H
#define MPUTIL_UPDATEVISITOR_H

#include "mputilDLL.h"

#include "DCUtil/AbstractEntityVisitor.h"
#include "DCUtil/EntityQueueManager.h"

#include "DCUtil/DatabasePage.h"

namespace DcGp
{
	class DcGpEntity;
	class DcGpPointCloud;
	class PageLodEntity;
}

namespace DCUtil
{

	class MPUTIL_EXPORT UpdateVisitor : public DCUtil::AbstractEntityVisitor
	{
		Q_OBJECT

	public:
		UpdateVisitor(TraversalMode tm=eTRAVERSE_ALL_CHILDREN);
		virtual ~UpdateVisitor();

		//! 实体及点云
		virtual void Apply(DcGp::DcGpEntity& node);
		virtual void Apply(DcGp::DcGpPointCloud& node);
		virtual void Apply(DcGp::PageLodEntity& node);

		void Traverse(DcGp::DcGpEntity& node)
		{
			if (m_traversalMode == eTRAVERSE_PARENTS)
			{
			}
			else if (m_traversalMode != eTRAVERSE_NONE)
			{
				node.Traverse(*this);
			}
		}

		//! 分页数据库接口
		/** Set the handler for database requests.*/
		void SetDatabaseRequestHandler(DatabaseRequestHandler* handler) { m_databaseRequestHandler = handler; }

		/** Get the handler for database requests.*/
		DatabaseRequestHandler* GetDatabaseRequestHandler() { return m_databaseRequestHandler; }

		virtual float GetDistanceToViewPoint(const DCVector3D& /*pos*/, bool /*useLODScale*/) const;

		void SetViewPointLocal(DCVector3D vPoint);
	private:
		
		//! 动态加载过渡队列
		EntityQueueManager* m_pEntityQueueManager;

		DCVector3D m_viewPointLocal;

		float m_lodScale;
	};
}

#endif // MPUTIL_ENTITYVISITOR_H
