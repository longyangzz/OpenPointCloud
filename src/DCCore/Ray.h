
//----------------------------------------------------------------------
//author:wly
//time:
//dsc:…‰œﬂ
//
//----------------------------------------------------------------------
#ifndef DCCORE_RAY_H
#define DCCORE_RAY_H

#include "BasicTypes.h"

namespace DCCore
{
  class Ray
  {
  public:
    const DCVector3D& direction() const { return mDirection; }
    const DCVector3D& origin() const { return mOrigin; }
    void setDirection(const DCVector3D& dir) { mDirection = dir; }
    void setOrigin(const DCVector3D& orig) { mOrigin = orig; }

  protected:
    DCVector3D mDirection;
    DCVector3D mOrigin;
  };
}

#endif
