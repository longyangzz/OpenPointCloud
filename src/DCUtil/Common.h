//----------------------------------------------------------------------
//author:wly
//time:
//dsc:
//
//----------------------------------------------------------------------

#ifndef MPUTIL_COMMON_TYPES
#define MPUTIL_COMMON_TYPES

#include "DCCore/BasicTypes.h"
#include "Hit.h"

namespace DcGp
{
	class DcGpEntity;
}

namespace MPUtil
{
	typedef std::vector<Hit > HitList;

	enum MaskValues
	{
		eVisible							= 0x0001,
		ePointCloud				            = 0x0002,
		eOutlinePoint						= 0x0004,
		eIndivdulaPlanePoint				= 0x0008,

		eEntity					            = 0x0010,
		eOutline							= 0x0020,
		eDistanceLine						= 0x0040,
		eIndividulPlane						= 0x0080,
		eIntersectionLine					= 0x0100,
		eEndpoint							= 0x0200,

		eOperationBox                       = 0x0400,

		eAll					            = 0x0800,
	};

	typedef unsigned int PickMask;

	bool RaySphereCollision(DCCore::Point_3 vSphereCenter, float fSphereRadius, DCCore::Point_3 vA, DCCore::Point_3 vB);

	bool RayOBBIntersection(
		DCCore::Point_3  vA,        // ÆðµãRay origin, in world space
		DCCore::Point_3  vB,		//ÖÕµã
		DCCore::Point_3  aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
		DCCore::Point_3  aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
		const double* ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
		float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
		);

	bool RayAABBIntersection(DCCore::Point_3  vA,        // Ray origin, in world space
		DCCore::Point_3  vB,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
		DCCore::Point_3  aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
		DCCore::Point_3  aabb_max,
		float& intersection_distance);
}

#endif