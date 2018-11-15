#include "Plane.h"
#include "AABB.h"

using namespace DCCore;

//-----------------------------------------------------------------------------
// Plane
//-----------------------------------------------------------------------------
float Plane::distance(const DCVector3D &v) const 
{
  return dot(v, mNormal) - mOrigin;
}
//-----------------------------------------------------------------------------
bool Plane::isOutside(const AABB& aabb) const
{
  DCVector3D pt;
  pt.x = mNormal.x >= 0 ? aabb.minCorner().x : aabb.maxCorner().x;
  pt.y = mNormal.y >= 0 ? aabb.minCorner().y : aabb.maxCorner().y;
  pt.z = mNormal.z >= 0 ? aabb.minCorner().z : aabb.maxCorner().z;
  return distance(pt) >= 0;
}
//-----------------------------------------------------------------------------
int Plane::classify(const AABB& aabb) const
{
  DCVector3D corner[] = 
  {
    DCVector3D(aabb.minCorner().x, aabb.minCorner().y, aabb.minCorner().z),
    DCVector3D(aabb.minCorner().x, aabb.minCorner().y, aabb.maxCorner().z),
    DCVector3D(aabb.minCorner().x, aabb.maxCorner().y, aabb.minCorner().z),
    DCVector3D(aabb.minCorner().x, aabb.maxCorner().y, aabb.maxCorner().z),
    DCVector3D(aabb.maxCorner().x, aabb.minCorner().y, aabb.minCorner().z),
    DCVector3D(aabb.maxCorner().x, aabb.minCorner().y, aabb.maxCorner().z),
    DCVector3D(aabb.maxCorner().x, aabb.maxCorner().y, aabb.minCorner().z),
    DCVector3D(aabb.maxCorner().x, aabb.maxCorner().y, aabb.maxCorner().z)
  };

  int left  = 0;
  int right = 0;
  float const NEPS = -0.0001f;
  float const PEPS = +0.0001f;

  for(int i=0;i<8; ++i)
  {
    if ( distance(corner[i]) < NEPS ) 
      left++;
    else 
    if ( distance(corner[i]) > PEPS ) 
      right++;
    // else -> we don't count the points on the plane

    if(left && right) // its clearly intersecting the plane
      return 0;
  }

  if (left)
    return -1;
  else 
  if (right)
    return +1;
  else // all the points were on the plane
    return 0;
}
//-----------------------------------------------------------------------------
