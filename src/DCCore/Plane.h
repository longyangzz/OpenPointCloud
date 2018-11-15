//----------------------------------------------------------------------
//author:wly
//time:
//dsc:视景体平面
//
//----------------------------------------------------------------------

#ifndef DCCORE_PLANE_H
#define DCCORE_PLANE_H

#include "BasicTypes.h"
#include "DCCoreDLL.h"

namespace DCCore
{
  class AABB;

  //-----------------------------------------------------------------------------
  // Plane
  //-----------------------------------------------------------------------------
  /**
   * The Plane class defines a plane using a normal and an origin.
  */
  class DCCORE_EXPORT Plane
  {
  public:
    Plane( float o=0.0f, DCVector3D n=DCVector3D(0,0,0) ): mNormal(n), mOrigin(o) 
    {
      
    }

    Plane( const DCVector3D& o, const DCVector3D& n ) 
    {
      mNormal = n;
      mOrigin = dot(o, n);
    }

    float distance(const DCVector3D &v) const;

    //! returns 0 if the AABB intersects the plane, 1 if it's in the positive side, 
    //! -1 if it's in the negative side.
    int classify(const AABB&) const;

    bool isOutside(const AABB&) const;

    const DCVector3D& normal() const { return mNormal; }

    float origin() const { return mOrigin; }

    void setNormal(const DCVector3D& normal) { mNormal = normal; }

    void setOrigin(float origin) { mOrigin = origin; }

  protected:
    DCVector3D mNormal;
    float mOrigin;
  };
}

#endif
