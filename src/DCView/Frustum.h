//----------------------------------------------------------------------
//author:wly
//time:
//dsc:封装glviewport
//
//----------------------------------------------------------------------

#ifndef DCVIEW_FRUSTUM_H
#define DCVIEW_FRUSTUM_H

#include "DCCore/AABB.h"
#include "DCCore/Plane.h"
#include "DCCore/Sphere.h"

namespace DCView
{
  //-----------------------------------------------------------------------------
  // 一组平面组成的视景体
  //-----------------------------------------------------------------------------
  class Frustum
  {
  public:
    std::vector<DCCore::Plane>& planes() { return mPlanes; }
    const std::vector<DCCore::Plane>& planes() const { return mPlanes; }

    void setPlane(unsigned i, const DCCore::Plane& plane) { mPlanes[i] = plane; }
    const DCCore::Plane& plane(unsigned i) const { return mPlanes[i]; }

    bool cull(const DCCore::Sphere& sphere) const
    {
      // null spheres are always visible
      if (sphere.isNull())
        return false;
      for(unsigned i=0; i<planes().size(); ++i)
      {
        if ( plane(i).distance(sphere.center()) > sphere.radius() )
          return true;
      }
      return false;
    }

    bool cull(const DCCore::AABB& aabb) const
    {
      if (aabb.isNull())
        return false;
      for(unsigned i=0; i<planes().size(); ++i)
      {
        if ( plane(i).isOutside(aabb) )
          return true;
      }
      return false;
    }

    bool cull(const std::vector<fvec3>& points) const
    {
      for(unsigned i=0; i<planes().size(); ++i)
      {
        unsigned j=0;
        for(; j<points.size(); ++j)
          if ( plane(i).distance((DCVector3D)points[j]) <= 0 )
            break;
        if(j == points.size())
          return true;
      }
      return false;
    }

  protected:
    std::vector<DCCore::Plane> mPlanes;
  };
}

#endif
