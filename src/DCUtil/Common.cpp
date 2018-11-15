//----------------------------------------------------------------------
//author:wly
//time:
//dsc:
//
//----------------------------------------------------------------------

#include "Common.h"

bool MPUtil::RaySphereCollision(DCCore::Point_3 vSphereCenter, float fSphereRadius, DCCore::Point_3 vA, DCCore::Point_3 vB)
{
	// Create the vector from end point vA to center of sphere
	DCCore::Point_3 vDirToSphere = vSphereCenter - vA;

	// Create a normalized direction vector from end point vA to end point vB
	DCCore::Point_3 temp = (vB-vA);
	temp.Normalize();
	DCCore::Point_3 vLineDir = temp;

	// Find length of line segment
	float fLineLength = DCCore::Point_3::VDistance(vA, vB);

	// Using the dot product, we project the vDirToSphere onto the vector vLineDir
	//float t = glm::dot(vDirToSphere, vLineDir);
	float t = vDirToSphere.Dot(vLineDir);

	DCCore::Point_3 vClosestPoint;
	// If our projected distance from vA is less than or equal to 0, the closest point is vA
	if (t <= 0.0f)
		vClosestPoint = vA;
	// If our projected distance from vA is greater thatn line length, closest point is vB
	else if (t >= fLineLength)
		vClosestPoint = vB;
	// Otherwise calculate the point on the line using t and return it
	else
		vClosestPoint = vA+vLineDir*t;

	// Now just check if closest point is within radius of sphere
	float dist = DCCore::Point_3::VDistance(vSphereCenter, vClosestPoint);
	return dist <= fSphereRadius;
}


bool MPUtil::RayOBBIntersection(
	DCCore::Point_3  vA,        // Ray origin, in world space
	DCCore::Point_3  vB,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
	DCCore::Point_3  aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
	DCCore::Point_3  aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
	const double* ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
	float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
	)
{
	DCCore::Point_3  ray_origin = vA;      // Ray origin, in world space
	DCCore::Point_3  ray_direction ; 
	DCCore::Point_3 temp = (vB-vA);

	temp.Normalize();
	ray_direction = temp;
	// Intersection method from Real-Time Rendering and Essential Mathematics for Games

	float tMin = 0.0f;
	float tMax = 100000.0f;

	DCCore::Point_3  OBBposition_worldspace(0, 0, 0);

	DCCore::Point_3 delta = OBBposition_worldspace - ray_origin;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		DCCore::Point_3 xaxis(1, 0, 0);
		float e = xaxis.Dot(delta);
		float f = ray_direction.Dot(xaxis);

		if ( fabs(f) > 0.001f ){ // Standard case

			float t1 = (e+aabb_min.x)/f; // Intersection with the "left" plane
			float t2 = (e+aabb_max.x)/f; // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2
			if (t1>t2){
				float w=t1;t1=t2;t2=w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if ( t2 < tMax )
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if ( t1 > tMin )
				tMin = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (tMax < tMin )
				return false;

		}else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if(-e+aabb_min.x > 0.0f || -e+aabb_max.x < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		DCCore::Point_3 yaxis(0, 1, 0);
		float e = yaxis.Dot(delta);
		float f = ray_direction.Dot(yaxis);

		if ( fabs(f) > 0.001f ){

			float t1 = (e+aabb_min.y)/f;
			float t2 = (e+aabb_max.y)/f;

			if (t1>t2){float w=t1;t1=t2;t2=w;}

			if ( t2 < tMax )
				tMax = t2;
			if ( t1 > tMin )
				tMin = t1;
			if (tMin > tMax)
				return false;

		}else{
			if(-e+aabb_min.y > 0.0f || -e+aabb_max.y < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		DCCore::Point_3 zaxis(0, 0, 1);
		float e = zaxis.Dot(delta);
		float f = ray_direction.Dot(zaxis);

		if ( fabs(f) > 0.001f ){

			float t1 = (e+aabb_min.z)/f;
			float t2 = (e+aabb_max.z)/f;

			if (t1>t2){float w=t1;t1=t2;t2=w;}

			if ( t2 < tMax )
				tMax = t2;
			if ( t1 > tMin )
				tMin = t1;
			if (tMin > tMax)
				return false;

		}else{
			if(-e+aabb_min.z > 0.0f || -e+aabb_max.z < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;

}

bool MPUtil::RayAABBIntersection(DCCore::Point_3  vA,        // Ray origin, in world space
	DCCore::Point_3  vB,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
	DCCore::Point_3  aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
	DCCore::Point_3  aabb_max,
	float& intersection_distance)
{
	float tmin = 0.0f ;
	float tmax = FLT_MAX ;

	DCCore::Point_3 dir = vB - vA;
	DCCore::Point_3 origin = vA;

	//The plane perpendicular to x-axie
	if(abs(dir.x) < 0.000001f) //If the ray parallel to the plane
	{
		//If the ray is not within AABB box, then not intersecting
		if(origin.x < aabb_min.x || origin.x > aabb_max.x)
			return false ;
	}
	else
	{
		//Compute the distance of ray to the near plane and far plane
		float ood = 1.0f / dir.x ;
		float t1 = (aabb_min.x - origin.x) * ood ;
		float t2 = (aabb_max.x - origin.x) * ood ;

		//Make t1 be intersecting with the near plane, t2 with the far plane
		if(t1 > t2)
		{
			float temp = t1 ;
			t1 = t2 ;
			t2 = temp ;
		}

		//Compute the intersection of slab intersection intervals
		if(t1 > tmin) tmin = t1 ;
		if(t2 < tmax) tmax = t2 ;

		//Exit with no collision as soon as slab intersection becomes empty
		if(tmin > tmax) return false ;
	}// end for perpendicular to x-axie

	//The plane perpendicular to y-axie
	if(abs(dir.y) < 0.000001f) //If the ray parallel to the plane
	{
		//If the ray is not within AABB box, then not intersecting
		if(origin.y < aabb_min.y || origin.y > aabb_max.y)
			return false ;
	}
	else
	{
		//Compute the distance of ray to the near plane and far plane
		float ood = 1.0f / dir.y ;
		float t1 = (aabb_min.y - origin.y) * ood ;
		float t2 = (aabb_max.y - origin.y) * ood ;

		//Make t1 be intersecting with the near plane, t2 with the far plane
		if(t1 > t2)
		{
			float temp = t1 ;
			t1 = t2 ;
			t2 = temp ;
		}

		//Compute the intersection of slab intersection intervals
		if(t1 > tmin) tmin = t1 ;
		if(t2 < tmax) tmax = t2 ;

		//Exit with no collision as soon as slab intersection becomes empty
		if(tmin > tmax) return false ;
	}// end for perpendicular to y-axie

	//The plane perpendicular to z-axie
	if(abs(dir.z) < 0.000001f) //If the ray parallel to the plane
	{
		//If the ray is not within AABB box, then not intersecting
		if(origin.z < aabb_min.z || origin.z > aabb_max.z)
			return false ;
	}
	else
	{
		//Compute the distance of ray to the near plane and far plane
		float ood = 1.0f / dir.z ;
		float t1 = (aabb_min.z - origin.z) * ood ;
		float t2 = (aabb_max.z - origin.z) * ood ;

		//Make t1 be intersecting with the near plane, t2 with the far plane
		if(t1 > t2)
		{
			float temp = t1 ;
			t1 = t2 ;
			t2 = temp ;
		}

		//Compute the intersection of slab intersection intervals
		if(t1 > tmin) tmin = t1 ;
		if(t2 < tmax) tmax = t2 ;

		//Exit with no collision as soon as slab intersection becomes empty
		if(tmin > tmax) return false ;
	}// end for perpendicular to z-axie

	intersection_distance = tmin;
	return true ;
}