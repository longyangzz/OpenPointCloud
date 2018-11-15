//----------------------------------------------------------------------
//author:wly
//time:
//dsc:ÖáÏò±ß½çºÐ
//
//----------------------------------------------------------------------

#ifndef DCCORE_AABB_H
#define DCCORE_AABB_H

#include "BasicTypes.h"

#include "DCCoreDLL.h"

namespace DCCore
{
  //-----------------------------------------------------------------------------
  // AABB
  //-----------------------------------------------------------------------------
  /** The AABB class implements an axis-aligned bounding box using vl::float precision. */
  class DCCORE_EXPORT AABB 
  {
  public:
    /** Constructs a null AABB. */
    AABB();

    /** Constructs an AABB large enough to contain a sphere with the specified \p radius and \p center. */
    AABB( const DCVector3D& center, float radius );

    /** Constructs an AABB large enough to contain the two specified points and enlarged by the amount specified by \p displace. */
    AABB( const DCVector3D& pt1, const DCVector3D& pt2, float displace=0);

    /** Sets ths AABB as null, that is, empty. */
    void setNull() { mMin = 1; mMax = -1; }

    /** Returns true if the AABB is null. */
    bool isNull()  const { return mMin.x > mMax.x || mMin.y > mMax.y || mMin.z > mMax.z; }

    /** Returns true if the AABB contains a single point, that is, if the min and max corners of the AABB are equal. */
    bool isPoint() const { return mMin == mMax; }

    /** Enlarges the AABB in all directions by \p displace amount. 
        As a result every edge of the AABB will be \p displace*2 longer. */
    void enlarge(float displace);

    /** Returns true if an AABB intersects with the given AABB. */
    bool intersects(const AABB & bb) const;

    /** Clips the position of the given \p p point to be inside an AABB. */
    DCVector3D clip(const DCVector3D& p, bool clipx=true, bool clipy=true, bool clipz=true) const;

    /** Returns true if the given point is inside the AABB. 
        This method allows you to restrict the test to any of the x, y, z axes. */
    bool isInside(const DCVector3D& p, bool clipx, bool clipy, bool clipz) const;

    /** Returns true if the given point is inside the AABB. */
    bool isInside(const DCVector3D& p) const;

    /** Returns the width of the AABB computed as max.x - min.x */
    float width() const;

    /** Returns the height of the AABB computed as max.y - min.y */
    float height() const;

    /** Returns the depth of the AABB computed as max.z - min.z */
    float depth() const;

    /** Returns true if two AABB are identical. */
    bool operator==(const AABB& aabb) const
    {
      return mMin == aabb.mMin && mMax == aabb.mMax;
    }

    /** Returns true if two AABB are not identical. */
    bool operator!=(const AABB& aabb) const
    {
      return !operator==(aabb);
    }

    /** Returns an AABB which contains the two source AABB. */
    AABB operator+(const AABB& aabb) const;

    /** Enlarges (if necessary) an AABB so that it contains the given AABB. */
    AABB& operator+=(const AABB& other)
    {
      *this = *this + other;
      return *this;
    }

    /** Returns an AABB which contains the source AABB and the given point. */
    AABB operator+(const DCVector3D& p)
    {
      AABB aabb = *this;
      aabb += p;
      return aabb;
    }

    /** Enlarges (if necessary) an AABB to contain the given point. */
    const AABB& operator+=(const DCVector3D& p)
    {
      addPoint(p);
      return *this;
    }

    /** Returns the center of the AABB. */
    DCVector3D center() const;

    /** Returns the longest dimension of the AABB. */
    float longestSideLength() const
    {
      float side = width();
      if (height() > side)
        side = height();
      if (depth() > side)
        side = depth();
      return side;
    }

    /** Updates the AABB to contain the given point. 
        The point can represent a sphere if \p radius > 0. */
    void addPoint(const DCVector3D& p, float radius);

    /** Updates the AABB to contain the given point. */
    void addPoint(const DCVector3D& p) 
    {
      if (isNull())
      {
        mMax = p;
        mMin = p;
        return;
      }

      if ( mMax.x < p.x ) mMax.x = p.x;
      if ( mMax.y < p.y ) mMax.y = p.y;
      if ( mMax.z < p.z ) mMax.z = p.z;
      if ( mMin.x > p.x ) mMin.x = p.x;
      if ( mMin.y > p.y ) mMin.y = p.y;
      if ( mMin.z > p.z ) mMin.z = p.z;
    }

    /** Transforms an AABB by the given matrix and returns it into the \p out parameter. */
    void transformed(AABB& out, const mat4& mat) const 
    {
      out.setNull();
      if ( !isNull() )
      {
        out.addPoint( mat * DCVector3D(minCorner().x, minCorner().y, minCorner().z) );
        out.addPoint( mat * DCVector3D(minCorner().x, maxCorner().y, minCorner().z) );
        out.addPoint( mat * DCVector3D(maxCorner().x, maxCorner().y, minCorner().z) );
        out.addPoint( mat * DCVector3D(maxCorner().x, minCorner().y, minCorner().z) );
        out.addPoint( mat * DCVector3D(minCorner().x, minCorner().y, maxCorner().z) );
        out.addPoint( mat * DCVector3D(minCorner().x, maxCorner().y, maxCorner().z) );
        out.addPoint( mat * DCVector3D(maxCorner().x, maxCorner().y, maxCorner().z) );
        out.addPoint( mat * DCVector3D(maxCorner().x, minCorner().y, maxCorner().z) );
      }
    }

    /** Returns the AABB transformed by the given matrix. */
    AABB transformed(const mat4& mat) const 
    {
      AABB aabb;
      transformed(aabb, mat);
      return aabb;
    }
    
    /** Returns the corner of the AABB with the minimum x y z coordinates. */
    const DCVector3D& minCorner() const { return mMin; }

    /** Returns the corner of the AABB with the maximum x y z coordinates. */
    const DCVector3D& maxCorner() const { return mMax; }

    /** Sets the corner of the AABB with the minimum x y z coordinates. */
    void setMinCorner(float x, float y, float z) { mMin.x = x; mMin.y = y; mMin.z = z; }

    /** Sets the corner of the AABB with the minimum x y z coordinates. */
    void setMinCorner(const DCVector3D& v) { mMin = v; }

    /** Sets the corner of the AABB with the maximum x y z coordinates. */
    void setMaxCorner(float x, float y, float z) { mMax.x = x; mMax.y = y; mMax.z = z; }

    /** Sets the corner of the AABB with the maximum x y z coordinates. */
    void setMaxCorner(const DCVector3D& v) { mMax = v; }

    /** Returns the volume of the AABB. */
    float volume() const { return width() * height() * depth(); }

  protected:
    DCVector3D mMin;
    DCVector3D mMax;
  };
}

#endif
