#ifndef DCCORE_MATRIX3_INCLUDE_ONCE
#define DCCORE_MATRIX3_INCLUDE_ONCE

#include "DCCore/Vector3D.h"
#include "Matrix2.h"

namespace DCCore
{
  //-----------------------------------------------------------------------------
  // Matrix3
  //-----------------------------------------------------------------------------
  /**
   * The Matrix3 class is a template class that implements a generic 3x3 matrix, see also vl::dmat3, vl::fmat3, vl::umat3, vl::imat3.
   * \sa Vector4, Vector3D, Vector2D, Matrix4, Matrix2
   */
  template<typename Type>
  class Matrix3
  {
  public:
    typedef Type scalar_type;
    //-----------------------------------------------------------------------------
    template<typename T>
    explicit Matrix3(const Matrix3<T>& m)
    {
      e(0,0) = (Type)m.e(0,0); e(1,0) = (Type)m.e(1,0); e(2,0) = (Type)m.e(2,0);
      e(0,1) = (Type)m.e(0,1); e(1,1) = (Type)m.e(1,1); e(2,1) = (Type)m.e(2,1);
      e(0,2) = (Type)m.e(0,2); e(1,2) = (Type)m.e(1,2); e(2,2) = (Type)m.e(2,2);
    }
    //-----------------------------------------------------------------------------
    Matrix3()
    {
      setIdentity();
    }
    //-----------------------------------------------------------------------------
    explicit Matrix3(Type n)
    {
      setIdentity();
      e(0,0) = e(1,1) = e(2,2) = n;
    }
    //-----------------------------------------------------------------------------
    explicit Matrix3(Type e00, Type e01, Type e02,
                      Type e10, Type e11, Type e12,
                      Type e20, Type e21, Type e22)
    {
      e(0,0) = e00; e(0,1) = e01; e(0,2) = e02; 
      e(1,0) = e10; e(1,1) = e11; e(1,2) = e12; 
      e(2,0) = e20; e(2,1) = e21; e(2,2) = e22;
    }
    //-----------------------------------------------------------------------------
    Matrix3& fill(Type val)
    {
      e(0,0) = e(1,0) = e(2,0) = 
      e(0,1) = e(1,1) = e(2,1) = 
      e(0,2) = e(1,2) = e(2,2) = val;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Type diff(const Matrix3& other) const
    {
      Type err = 0;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          if (e(j,i) > other.e(j,i)) // avoid fabs/abs
            err += e(j,i) - other.e(j,i);
          else
            err += other.e(j,i) - e(j,i);
      return err;
    }
    //-----------------------------------------------------------------------------
    Vector2D<Type> getX() const
    {
      Vector2D<Type> v;
      v.x = e(0,0);
      v.y = e(1,0);
      return v;
    }
    //-----------------------------------------------------------------------------
    Vector2D<Type> getY() const
    {
      Vector2D<Type> v;
      v.x = e(0,1);
      v.y = e(1,1);
      return v;
    }
    //-----------------------------------------------------------------------------
    Vector2D<Type> getT() const
    {
      Vector2D<Type> v;
      v.x = e(0,2);
      v.y = e(1,2);
      return v;
    }
    //-----------------------------------------------------------------------------
    Matrix3& setX(const Vector2D<Type>& v) 
    {
      e(0,0) = v.x;
      e(1,0) = v.y;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3& setY(const Vector2D<Type>& v) 
    {
      e(0,1) = v.x;
      e(1,1) = v.y;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3& setT(const Vector2D<Type>& v) 
    {
      e(0,2) = v.x;
      e(1,2) = v.y;
      return *this;
    }
    //-----------------------------------------------------------------------------
    bool operator==(const Matrix3& m) const 
    {
      return memcmp(m.mVec, mVec, sizeof(Type)*9) == 0;
    }
    //-----------------------------------------------------------------------------
    bool operator!=(const Matrix3& m) const 
    {
      return !operator==(m);
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator=(const Matrix3& m) 
    {
      memcpy(mVec, m.mVec, sizeof(Type)*9);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator+(const Matrix3& m) const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t.e(j,i) = e(j,i) + m.e(j,i);
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator+=(const Matrix3& m)
    {
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          e(j,i) += m.e(j,i);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator-(const Matrix3& m) const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t.e(j,i) = e(j,i) - m.e(j,i);
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator-=(const Matrix3& m)
    {
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          e(j,i) -= m.e(j,i);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator*=(const Matrix3& m)
    {
      return postMultiply(m);
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator-() const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t.e(j,i) = -e(j,i);
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator+(Type d) const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t.e(j,i) = e(j,i) + d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator+=(Type d)
    {
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          e(j,i) += d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator-(Type d) const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t.e(j,i) = e(j,i) - d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator-=(Type d)
    {
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          e(j,i) -= d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator*(Type d) const
    {
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t.e(j,i) = e(j,i) * d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator*=(Type d)
    {
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          e(j,i) *= d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3 operator/(Type d) const
    {
      d = (Type)1 / d;
      Matrix3 t;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          t.e(j,i) = e(j,i) * d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix3& operator/=(Type d)
    {
      d = (Type)1 / d;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          e(j,i) *= d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    bool isIdentity() const
    {
      Matrix3 i;
      return memcmp(ptr(), i.ptr(), sizeof(Type)*9) == 0;
    }
    //-----------------------------------------------------------------------------
    Matrix2<Type> get2x2() const
    {
      Matrix2<Type> t;
      t.e(0,0) = e(0,0); t.e(1,0) = e(1,0);
      t.e(0,1) = e(0,1); t.e(1,1) = e(1,1);
      return t;
    }
    //-----------------------------------------------------------------------------
    //! This writes only on the upper 2x2 part of the matrix without touching the last row and column. 
    void set2x2(const Matrix2<Type>& m)
    {
      e(0,0) = m.e(0,0); e(1,0) = m.e(1,0);
      e(0,1) = m.e(0,1); e(1,1) = m.e(1,1);
    }
    //-----------------------------------------------------------------------------
    Type* ptr()
    {
      return &e(0,0);
    }
    //-----------------------------------------------------------------------------
    const Type* ptr() const
    {
      return &e(0,0);
    }
    //-----------------------------------------------------------------------------
    Matrix3& transpose()
    {
      Type tmp;
      for(int i=0; i<3; ++i)
        for(int j=i; j<3; ++j)
        {
          tmp = e(j,i);
          e(j,i) = e(i,j);
          e(i,j) = tmp;
        }
        return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix3 getTransposed() const
    {
      Matrix3 m;
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          m.e(j,i) = e(i,j);
      return m;
    }
    //-----------------------------------------------------------------------------
    Matrix3& getTransposed(Matrix3& dest) const
    {
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          dest.e(j,i) = e(i,j);
      return dest;
    }
    //-----------------------------------------------------------------------------
    bool isNull() const
    {
      for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
          if(mVec[j][i] != 0)
            return false;
      return true;
    }
    //-----------------------------------------------------------------------------
    Matrix3& setNull() 
    {
      fill(0);
      return *this;
    }
    //-----------------------------------------------------------------------------
    static Matrix3& getNull(Matrix3& out)
    {
      out.fill(0);
      return out;
    }
    //-----------------------------------------------------------------------------
    static Matrix3 getNull()
    {
      return Matrix3().fill(0);
    }
    //-----------------------------------------------------------------------------
    Matrix3& setIdentity()
    {
      static const Type I3d[] = 
      { 
        (Type)1, (Type)0, (Type)0,  
        (Type)0, (Type)1, (Type)0, 
        (Type)0, (Type)0, (Type)1, 
      };
      memcpy(mVec, I3d, sizeof(Type)*9);
      return *this;
    }
    //-----------------------------------------------------------------------------
    static Matrix3 getIdentity()
    {
      return Matrix3();
    }
    //-----------------------------------------------------------------------------
    static Matrix3& getIdentity(Matrix3& out)
    {
      out.setIdentity();
      return out;
    }
    //-----------------------------------------------------------------------------
    Type getInverse(Matrix3& dest) const;
    //-----------------------------------------------------------------------------
    Matrix3 getInverse(Type *determinant=NULL) const
    {
      Matrix3 tmp;
      Type det = getInverse(tmp);
      if (determinant)
        *determinant = det;
      return tmp;
    }
    //-----------------------------------------------------------------------------
    Matrix3& invert(Type *determinant=NULL)
    {
      Type det = getInverse(*this);
      if (determinant)
        *determinant = det;
      return *this;
    }
    //-----------------------------------------------------------------------------
    static Matrix3 getRotation(Type degrees);
    //-----------------------------------------------------------------------------
    Matrix3& rotate(Type degrees)
    {
      return preMultiply(getRotation(degrees));
    }
    //-----------------------------------------------------------------------------
    static Matrix3& getTranslation(Matrix3& out, const Vector2D<Type>& v)
    {
      return getTranslation(out, v.x, v.y);
    }
    //-----------------------------------------------------------------------------
    static Matrix3 getTranslation(const Vector2D<Type>& v)
    {
      return getTranslation(v.x, v.y);
    }
    //-----------------------------------------------------------------------------
    static Matrix3 getTranslation(Type x, Type y)
    {
      Matrix3 m;
      return getTranslation(m, x, y);
    }
    //-----------------------------------------------------------------------------
    static Matrix3& getTranslation(Matrix3& out, Type x, Type y)
    {
      out.setIdentity();
      out.e(0,2) = x;
      out.e(1,2) = y;
      return out;
    }
    //-----------------------------------------------------------------------------
    Matrix3& translate(Type x, Type y)
    {
      return preMultiply(getTranslation(x,y));
    }
    //-----------------------------------------------------------------------------
    Matrix3& translate(const Vector2D<Type>& v)
    {
      return preMultiply(getTranslation(v));
    }
    //-----------------------------------------------------------------------------
    static Matrix3& getScaling(Matrix3& out, const Vector2D<Type>& v)
    {
      return getScaling(out, v.x, v.y);
    }
    //-----------------------------------------------------------------------------
    static Matrix3 getScaling(const Vector2D<Type>& v)
    {
      Matrix3 m;
      return getScaling(m, v.x, v.y);
    }
    //-----------------------------------------------------------------------------
    static Matrix3 getScaling(Type x, Type y)
    {
      Matrix3 m;
      return getScaling(m, x, y);
    }
    //-----------------------------------------------------------------------------
    static Matrix3& getScaling(Matrix3& out, Type x, Type y)
    {
      out.setIdentity();
      out.e(0,0) = x;
      out.e(1,1) = y;
      return out;
    }
    //-----------------------------------------------------------------------------
    Matrix3& scale(Type x, Type y)
    {
      return preMultiply(getScaling(x,y));
    }
    //-----------------------------------------------------------------------------
    Matrix3& scale(const Vector2D<Type>& v)
    {
      return preMultiply(getScaling(v.x,v.y));
    }
    //-----------------------------------------------------------------------------
    static Matrix3& multiply(Matrix3& out, const Matrix3& p, const Matrix3& q)
    {
      out.e(0,0) = q.e(0,0)*p.e(0,0) + q.e(1,0)*p.e(0,1) + q.e(2,0)*p.e(0,2);
      out.e(0,1) = q.e(0,1)*p.e(0,0) + q.e(1,1)*p.e(0,1) + q.e(2,1)*p.e(0,2);
      out.e(0,2) = q.e(0,2)*p.e(0,0) + q.e(1,2)*p.e(0,1) + q.e(2,2)*p.e(0,2);

      out.e(1,0) = q.e(0,0)*p.e(1,0) + q.e(1,0)*p.e(1,1) + q.e(2,0)*p.e(1,2);
      out.e(1,1) = q.e(0,1)*p.e(1,0) + q.e(1,1)*p.e(1,1) + q.e(2,1)*p.e(1,2);
      out.e(1,2) = q.e(0,2)*p.e(1,0) + q.e(1,2)*p.e(1,1) + q.e(2,2)*p.e(1,2);

      out.e(2,0) = q.e(0,0)*p.e(2,0) + q.e(1,0)*p.e(2,1) + q.e(2,0)*p.e(2,2);
      out.e(2,1) = q.e(0,1)*p.e(2,0) + q.e(1,1)*p.e(2,1) + q.e(2,1)*p.e(2,2);
      out.e(2,2) = q.e(0,2)*p.e(2,0) + q.e(1,2)*p.e(2,1) + q.e(2,2)*p.e(2,2);

      return out;
    }
    //-----------------------------------------------------------------------------
    Matrix3& postMultiply(const Matrix3& m)
    {
      Matrix3<Type> t;
      return *this = multiply(t, *this, m);
    }
    //-----------------------------------------------------------------------------
    Matrix3& preMultiply(const Matrix3& m)
    {
      Matrix3<Type> t;
      return *this = multiply(t, m, *this);
    }
    //-----------------------------------------------------------------------------

    const Type& e(int i, int j) const { return mVec[j][i]; }
    Type& e(int i, int j) { return mVec[j][i]; }

  private:
    const Vector3D<Type>& operator[](unsigned int i) const {return mVec[i]; }
    Vector3D<Type>& operator[](unsigned int i) { return mVec[i]; }

  protected:
    Vector3D<Type> mVec[3];
  };

  //-----------------------------------------------------------------------------
  // OPERATORS
  //-----------------------------------------------------------------------------
  template<typename Type>
  inline Matrix3<Type> operator*(const Matrix3<Type>& p, const Matrix3<Type>& q)
  {
    Matrix3<Type> t;
    Matrix3<Type>::multiply(t, p, q);
    return t;
  }
  //-----------------------------------------------------------------------------
  template<typename Type>
  inline Matrix3<Type> operator+(Type d, const Matrix3<Type>& m)
  {
    return m + d;
  }
  //-----------------------------------------------------------------------------
  template<typename Type>
  inline Matrix3<Type> operator*(Type d, const Matrix3<Type>& m)
  {
    return m * d;
  }
  //-----------------------------------------------------------------------------
  //! Post multiplication: matrix * column vector
  template<typename Type>
  inline Vector3D<Type> operator*(const Matrix3<Type>& m, const Vector3D<Type>& v)
  {
    Vector3D<Type> t;
    t.x = v.x*m.e(0,0) + v.y*m.e(0,1) + v.z*m.e(0,2);
    t.y = v.x*m.e(1,0) + v.y*m.e(1,1) + v.z*m.e(1,2);
    t.z = v.x*m.e(2,0) + v.y*m.e(2,1) + v.z*m.e(2,2);
    return t;
  }
  //-----------------------------------------------------------------------------
  //! Post multiplication: matrix * column vector
  //! The incoming vector is considered a Vector3D<Type> with the component z = 0
  template<typename Type>
  inline Vector2D<Type> operator*(const Matrix3<Type>& m, const Vector2D<Type>& v)
  {
    Vector2D<Type> t;
    t.x = v.x*m.e(0,0) + v.y*m.e(0,1) /*+ 0*m.e(0,2)*/;
    t.y = v.x*m.e(1,0) + v.y*m.e(1,1) /*+ 0*m.e(1,2)*/;
    return t;
  }
  //-----------------------------------------------------------------------------
  //! pre-multiplication: row vector * matrix
  template<typename Type>
  inline Vector3D<Type> operator*(const Vector3D<Type>& v, const Matrix3<Type>& m)
  {
    Vector3D<Type> t;
    t.x = v.x*m.e(0,0) + v.y*m.e(1,0) + v.z*m.e(2,0);
    t.y = v.x*m.e(0,1) + v.y*m.e(1,1) + v.z*m.e(2,1);
    t.z = v.x*m.e(0,2) + v.y*m.e(1,2) + v.z*m.e(2,2);
    return t;
  }
  //-----------------------------------------------------------------------------
  //! pre-multiplication: row vector * matrix
  //! The incoming vector is considered a Vector3D<Type> with the component z = 0
  template<typename Type>
  inline Vector2D<Type> operator*(const Vector2D<Type>& v, const Matrix3<Type>& m)
  {
    Vector2D<Type> t;
    t.x = v.x*m.e(0,0) + v.y*m.e(1,0) /*+ 0*m.e(2,0)*/;
    t.y = v.x*m.e(0,1) + v.y*m.e(1,1) /*+ 0*m.e(2,1)*/;
    return t;
  }
  //-----------------------------------------------------------------------------
  template<typename Type>
  Matrix3<Type> Matrix3<Type>::getRotation(Type degrees)
  {
    Matrix3<Type> rot;
    degrees = degrees * (Type)DC_DEG_TO_RAD;
    Type s = (Type) sin(degrees);
    Type c = (Type) cos(degrees);
    rot.e(0,0) = (Type)c;
    rot.e(1,1) = (Type)c;
    rot.e(1,0) = (Type)s;
    rot.e(0,1) = -(Type)s;
    return rot;
  }
  //-----------------------------------------------------------------------------
  template<typename Type>
  Type Matrix3<Type>::getInverse(Matrix3<Type>& dest) const
  {
    if (&dest == this)
    {
      Matrix3<Type> tmp;
      Type det = getInverse(tmp);
      dest = tmp;
      return det;
    }
    else
    {
      const Type& a11 = e(0,0);
      const Type& a21 = e(1,0);
      const Type& a31 = e(2,0);
      const Type& a12 = e(0,1);
      const Type& a22 = e(1,1);
      const Type& a32 = e(2,1);
      const Type& a13 = e(0,2);
      const Type& a23 = e(1,2);
      const Type& a33 = e(2,2);

      Type A = a22*a33 - a32*a23;
      Type B = a23*a31 - a33*a21;
      Type C = a21*a32 - a31*a22;

      Type det = a11*A + a12*B + a13*C;

      if (det == 0)
        dest.fill(0);
      else
        dest = Matrix3<Type>(A, a13*a32 - a33*a12, a12*a23 - a22*a13, 
                                 B, a11*a33 - a31*a13, a13*a21 - a23*a11,
                                 C, a12*a31 - a32*a11, a11*a22 - a21*a12) / det;
      return det;
    }
  }
  //-----------------------------------------------------------------------------

  //! A 3x3 matrix using \p double precision.
  typedef Matrix3<double> dmat3;
  //! A 3x3 matrix using \p float precision.
  typedef Matrix3<float>  fmat3;
  //! A 3x3 matrix using \p int precision.
  typedef Matrix3<int>    imat3;
  //! A 3x3 matrix using \p unsigned int precision.
  typedef Matrix3<unsigned int>  umat3;

  
  typedef fmat3 mat3;
}

#endif
