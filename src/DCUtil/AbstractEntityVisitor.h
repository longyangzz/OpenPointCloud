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

#ifndef DCGP_ABSTRACT_ENTITYVISITOR_H
#define DCGP_ABSTRACT_ENTITYVISITOR_H

#include <QObject>

#include "DCUtil/mputilDLL.h"

#include "DCUtil/DatabasePage.h"

namespace DcGp
{
	class DcGpEntity;
	class DcGpPointCloud;
	class PageLodEntity;
}

namespace DCUtil
{
	
	class MPUTIL_EXPORT AbstractEntityVisitor : public QObject
	{
		Q_OBJECT

	public:
		enum TraversalMode
		{
			eTRAVERSE_NONE,
			eTRAVERSE_PARENTS,
			eTRAVERSE_ALL_CHILDREN,
			eTRAVERSE_ACTIVE_CHILDREN
		};
	public:
		AbstractEntityVisitor(TraversalMode tm=eTRAVERSE_NONE)
			: QObject()
		{
			m_traversalMode = tm;
		}
		virtual ~AbstractEntityVisitor() 
		{
			if (m_databaseRequestHandler)
			{
				delete m_databaseRequestHandler;
				m_databaseRequestHandler = nullptr;
			}
		}

		virtual void Traverse(DcGp::DcGpEntity& node) {};
       

		virtual void Apply(DcGp::DcGpEntity& node) = 0;

		virtual void Apply(DcGp::DcGpPointCloud& node) = 0;

		virtual void Apply(DcGp::PageLodEntity& node) = 0;
		
		virtual DCUtil::DatabaseRequestHandler* GetDatabaseRequestHandler() { return nullptr; }

		virtual float GetDistanceToViewPoint(const DCVector3D& /*pos*/, bool /*useLODScale*/) const { return 0.0f; }
	protected:
		TraversalMode m_traversalMode;

		//!分页数据库管理器
		DatabaseRequestHandler* m_databaseRequestHandler;
	};
}

#endif // DCGP_ENTITYVISITOR_H
