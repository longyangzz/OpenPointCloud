//----------------------------------------------------------------------
//author:wly
//time:
//dsc:÷·œÚ±ﬂΩÁ∫–
//
//----------------------------------------------------------------------

#ifndef DCCORE_SPHERE_H
#define DCCORE_SPHERE_H

#include "AABB.h"

namespace DCCore
{
//-----------------------------------------------------------------------------
// Sphere
//-----------------------------------------------------------------------------
  /** The Sphere class defines a sphere using a center and a radius using vl::float precision. */
  class DCCORE_EXPORT Sphere
  {
  public:
    /** Constructor: creates a null sphere. */
    Sphere(): mRadius(-1) { }

    /** Constructor: creates a sphere with the given center and radius. */
    Sphere(const DCVector3D& center, float radius): mCenter(center), mRadius(radius) {}

    /** Copy-constructor. */
    Sphere(const AABB& aabb) { *this = aabb; }

    /** Sets the sphere as null.*/
    void setNull()  { mRadius =-1.0f; mCenter = DCVector3D(0,0,0); }

    /** Returns true if the sphere is null, ie, if radius is < 0. */    
    bool isNull()  const { return mRadius <  0.0f; }
    
    /** Returns true if the sphere as radius == 0 */
    bool isPoint() const { return mRadius == 0.0f; }
    
    /** Sets the center of the sphere. */
    void setCenter(const DCVector3D& center) { mCenter = center; }
    
    /** Returns the center of the sphere. */
    const DCVector3D& center() const { return mCenter; }
    
    /** Sets the radius of the sphere. */
    void setRadius( float radius ) { mRadius = radius; }
    
    /** Returns the radius of the sphere. */
    float radius() const { return mRadius; }

    /** Returns true if a sphere contains the specified sphere. */
    bool includes(const Sphere& other) const
    {
      if (isNull())
        return false;
      else
      if (other.isNull())
        return true;
      else
      {
        float distance = (center() - other.center()).Length();
        return radius() >= distance + other.radius();
      }
    }

    /** Returns true if two spheres are identical. */
    bool operator==(const Sphere& other) const 
    {
      return mCenter == other.mCenter && mRadius == other.mRadius;
    }

    /** Returns true if two spheres are not identical. */
    bool operator!=(const Sphere& other) const
    {
      return !operator==(other);
    }
    
    /** Constructs a sphere that contains the specified AABB. */
    Sphere& operator=(const AABB& aabb)
    {
      if (aabb.isNull())
        setNull();
      else
      {
        mCenter = aabb.center();
        mRadius = (aabb.minCorner() - aabb.maxCorner()).Length() / (float)2.0;
      }
      return *this;
    }

    /** Returns a sphere that contains the two specified spheres. */
    Sphere operator+(const Sphere& other)
    {
      Sphere t = *this;
      return t += other;
    }

    /** Enlarges the sphere to contain the specified sphere. */
    const Sphere& operator+=(const Sphere& other)
    {
      if (this->isNull())
        *this = other;
      else
      if (other.includes(*this))
      {
        *this = other;
      }
      else
      if (!other.isNull() && !this->includes(other))
      {
        DCVector3D v = other.center() - this->center();
        if (v.IsNull())
        {
          // the center remains the same
          // sets the maximum radius
          setRadius( radius() > other.radius() ? radius() : other.radius() );
        }
        else
        {
          v.normalize();
          DCVector3D p0 = this->center() - v * this->radius();
          DCVector3D p1 = other.center() + v * other.radius();
          setCenter( (p0 + p1)*(float)0.5 );
          setRadius( (p0 - p1).Length()*(float)0.5 );
        }
      }

      return *this;
    }

    /** Returns a sphere that contains the original sphere transformed by the given matrix. */
    void transformed(Sphere& out, const mat4& mat) const 
    {
      out.setNull();
      if ( !isNull() )
      {
        out.mCenter = mat * center();
        // DCVector3D p = center() + DCVector3D( (float)0.577350269189625840, (float)0.577350269189625840, (float)0.577350269189625840 ) * radius();
        // p = mat * p;
        // p = p - out.center();
        // out.setRadius(p.Length());
        DCVector3D p0 = center() + DCVector3D(radius(),0,0);
        DCVector3D p1 = center() + DCVector3D(0,radius(),0);
        DCVector3D p2 = center() + DCVector3D(0,0,radius());
        p0 = mat * p0;
        p1 = mat * p1;
        p2 = mat * p2;
        float d0 = (p0 - out.mCenter).LengthSquared();
        float d1 = (p1 - out.mCenter).LengthSquared();
        float d2 = (p2 - out.mCenter).LengthSquared();
        out.mRadius = ::sqrt( d0>d1 ? (d0>d2?d0:d2) : (d1>d2?d1:d2) );
      }
    }

    /** Returns a sphere that contains the original sphere transformed by the given matrix. */
    Sphere transformed(const mat4& mat) const 
    {
      Sphere sphere;
      transformed(sphere, mat);
      return sphere;
    }

  protected:
    DCVector3D mCenter;
    float mRadius;
  };
}

#endif
