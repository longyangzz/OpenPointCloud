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

#ifndef MPUTIL_ENTITYVISITOR_H
#define MPUTIL_ENTITYVISITOR_H

#include "mputilDLL.h"

#include "DCUtil/AbstractEntityVisitor.h"

namespace DcGp
{
	class DcGpEntity;
	class DcGpPointCloud;
}

namespace MPGp
{
	class DistanceLine;
	class IndividualPlane;
	class Outlines;
	class IntersectionLine;
	class BoundaryLine;
	class Round;
	class OperationBox;
}

namespace MPUtil
{
	class MPUTIL_EXPORT EntityVisitor : public DCUtil::AbstractEntityVisitor
	{
		Q_OBJECT

	public:
		EntityVisitor(TraversalMode tm=eTRAVERSE_ALL_CHILDREN);
		virtual ~EntityVisitor();

		//! 实体及点云
		virtual void Apply(DcGp::DcGpEntity& node);
		virtual void Apply(DcGp::DcGpPointCloud& node);


	protected:
		
	};
}

#endif // MPUTIL_ENTITYVISITOR_H
