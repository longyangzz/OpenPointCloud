#ifndef DCCORE_MATRIX2_INCLUDE_ONCE
#define DCCORE_MATRIX2_INCLUDE_ONCE


#include <cstring> // memcpy

#include "DCCore/Vector2D.h"

namespace DCCore
{
  //-----------------------------------------------------------------------------
  // Matrix2
  //-----------------------------------------------------------------------------
  /**
   * The Matrix2 class is a template class that implements a generic 2x2 matrix, see also vl::dmat2, vl::fmat2, vl::umat2, vl::imat2.
   * \sa Vector4, Vector3, Vector2D, Matrix4, Matrix3
   */
  template<typename Type>
  class Matrix2
  {
  public:
    typedef Type scalar_type;
    //-----------------------------------------------------------------------------
    template<typename T>
    explicit Matrix2(const Matrix2<T>& m)
    {
      e(0,0) = (Type)m.e(0,0); e(1,0) = (Type)m.e(1,0);
      e(0,1) = (Type)m.e(0,1); e(1,1) = (Type)m.e(1,1);
    }
    //-----------------------------------------------------------------------------
    Matrix2()
    {
      setIdentity();
    }
    //-----------------------------------------------------------------------------
    explicit Matrix2(Type n)
    {
      setIdentity();
      e(0,0) = e(1,1) = n;
    }
    //-----------------------------------------------------------------------------
    explicit Matrix2(Type e00, Type e01,
                      Type e10, Type e11 )
    {
      e(0,0) = e00; e(0,1) = e01;
      e(1,0) = e10; e(1,1) = e11;
    }
    //-----------------------------------------------------------------------------
    Matrix2& fill(Type val)
    {
      e(0,0) = e(1,0) = 
      e(0,1) = e(1,1) = val;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Type diff(const Matrix2& other) const
    {
      Type err = 0;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          if (e(j,i) > other.e(j,i)) // avoid fabs/abs
            err += e(j,i) - other.e(j,i);
          else
            err += other.e(j,i) - e(j,i);
      return err;
    }
    //-----------------------------------------------------------------------------
    bool operator==(const Matrix2& m) const 
    {
      return memcmp(m.mVec, mVec, sizeof(Type)*4) == 0;
    }
    //-----------------------------------------------------------------------------
    bool operator!=(const Matrix2& m) const 
    {
      return !operator==(m);
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator=(const Matrix2& m) 
    {
      memcpy(mVec, m.mVec, sizeof(Type)*4);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator+(const Matrix2& m) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) + m.e(j,i);
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator+=(const Matrix2& m)
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) += m.e(j,i);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator-(const Matrix2& m) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) - m.e(j,i);
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator-=(const Matrix2& m)
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) -= m.e(j,i);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator*=(const Matrix2& m)
    {
      return postMultiply(m);
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator-() const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = -e(j,i);
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator+(Type d) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) + d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator+=(Type d)
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) += d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator-(Type d) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) - d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator-=(Type d)
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) -= d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator*(Type d) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) * d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator*=(Type d)
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) *= d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator/(Type d) const
    {
      d = (Type)1 / d;
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) * d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator/=(Type d)
    {
      d = (Type)1 / d;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) *= d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    bool isIdentity() const
    {
      Matrix2 i;
      return memcmp(ptr(), i.ptr(), sizeof(Type)*4) == 0;
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
    Matrix2& transpose()
    {
      Type tmp;
      for(int i=0; i<2; ++i)
        for(int j=i; j<2; ++j)
        {
          tmp = e(j,i);
          e(j,i) = e(i,j);
          e(i,j) = tmp;
        }
        return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 getTransposed() const
    {
      Matrix2 m;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          m.e(j,i) = e(i,j);
      return m;
    }
    //-----------------------------------------------------------------------------
    Matrix2& getTransposed(Matrix2& dest) const
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          dest.e(j,i) = e(i,j);
      return dest;
    }
    //-----------------------------------------------------------------------------
    bool isNull() const
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          if(mVec[j][i] != 0)
            return false;
      return true;
    }
    //-----------------------------------------------------------------------------
    Matrix2& setNull() 
    {
      fill(0);
      return *this;
    }
    //-----------------------------------------------------------------------------
    static Matrix2& getNull(Matrix2& out)
    {
      out.fill(0);
      return out;
    }
    //-----------------------------------------------------------------------------
    static Matrix2 getNull()
    {
      return Matrix2().fill(0);
    }
    //-----------------------------------------------------------------------------
    Matrix2& setIdentity()
    {
      static const Type I2d[] = 
      { 
        (Type)1, (Type)0,
        (Type)0, (Type)1
      };
      memcpy(mVec, I2d, sizeof(Type)*4);
      return *this;
    }
    //-----------------------------------------------------------------------------
    static Matrix2 getIdentity()
    {
      return Matrix2();
    }
    //-----------------------------------------------------------------------------
    static Matrix2& getIdentity(Matrix2& out)
    {
      out.setIdentity();
      return out;
    }
    //-----------------------------------------------------------------------------
    Type getInverse(Matrix2& dest) const
    {
      if (&dest == this)
      {
        Matrix2 tmp;
        Type det = getInverse(tmp);
        dest = tmp;
        return det;
      }
      else
      {
        const Type& a11 = e(0,0); 
        const Type& a12 = e(1,0); 
        const Type& a21 = e(0,1); 
        const Type& a22 = e(1,1); 

        dest.fill(0);

        Type det = a11*a22-a12*a21;

        if (det != 0)
          dest = Matrix2(+a22, -a12, -a21, +a11) / det;

        return det;
      }
    }
    //-----------------------------------------------------------------------------
    Matrix2 getInverse(Type *determinant=NULL) const
    {
      Matrix2 tmp;
      Type det = getInverse(tmp);
      if (determinant)
        *determinant = det;
      return tmp;
    }
    //-----------------------------------------------------------------------------
    Matrix2& invert(Type *determinant=NULL)
    {
      Type det = getInverse(*this);
      if (determinant)
        *determinant = det;
      return *this;
    }
    //-----------------------------------------------------------------------------
    static Matrix2& multiply(Matrix2& out, const Matrix2& p, const Matrix2& q)
    {
      out.e(0,0) = q.e(0,0)*p.e(0,0) + q.e(1,0)*p.e(0,1);
      out.e(0,1) = q.e(0,1)*p.e(0,0) + q.e(1,1)*p.e(0,1);

      out.e(1,0) = q.e(0,0)*p.e(1,0) + q.e(1,0)*p.e(1,1);
      out.e(1,1) = q.e(0,1)*p.e(1,0) + q.e(1,1)*p.e(1,1);

      return out;
    }
    //-----------------------------------------------------------------------------
    Matrix2& postMultiply(const Matrix2& m)
    {
      Matrix2<Type> t;
      return *this = multiply(t, *this, m);
    }
    //-----------------------------------------------------------------------------
    Matrix2& preMultiply(const Matrix2& m)
    {
      Matrix2<Type> t;
      return *this = multiply(t, m, *this);
    }
    //-----------------------------------------------------------------------------

    const Type& e(int i, int j) const { return mVec[j][i]; }
    Type& e(int i, int j) { return mVec[j][i]; }

  private:
    const Vector2D<Type>& operator[](unsigned int i) const { return mVec[i]; }
    Vector2D<Type>& operator[](unsigned int i) { return mVec[i]; }

  protected:
    Vector2D<Type> mVec[2];
  };

  //-----------------------------------------------------------------------------
  // OPERATORS
  //-----------------------------------------------------------------------------
  template<typename Type>
  inline Matrix2<Type> operator*(const Matrix2<Type>& p, const Matrix2<Type>& q)
  {
    Matrix2<Type> t;
    Matrix2<Type>::multiply(t, p, q);
    return t;
  }
  //-----------------------------------------------------------------------------
  template<typename Type>
  inline Matrix2<Type> operator+(Type d, const Matrix2<Type>& m)
  {
    return m + d;
  }
  //-----------------------------------------------------------------------------
  template<typename Type>
  inline Matrix2<Type> operator*(Type d, const Matrix2<Type>& m)
  {
    return m * d;
  }
  //-----------------------------------------------------------------------------
  // post multiplication: matrix * column vector
  template<typename Type>
  inline Vector2D<Type> operator*(const Matrix2<Type>& m, const Vector2D<Type>& v)
  {
    Vector2D<Type> t;
    t.x = v.x*m.e(0,0) + v.y*m.e(0,1);
    t.y = v.x*m.e(1,0) + v.y*m.e(1,1);
    return t;
  }
  //-----------------------------------------------------------------------------
  // pre-multiplication: row vector * matrix
  template<typename Type>
  inline Vector2D<Type> operator*(const Vector2D<Type>& v, const Matrix2<Type>& m)
  {
    Vector2D<Type> t;
    t.x = v.x*m.e(0,0) + v.y*m.e(1,0);
    t.y = v.x*m.e(0,1) + v.y*m.e(1,1);
    return t;
  }
  //-----------------------------------------------------------------------------

  //! A 2x2 matrix using \p double precision.
  typedef Matrix2<double> dmat2;
  //! A 2x2 matrix using \p float precision.
  typedef Matrix2<float>  fmat2;
  //! A 2x2 matrix using \p int precision.
  typedef Matrix2<int>    imat2;
  //! A 2x2 matrix using \p unsigned int precision.
  typedef Matrix2<unsigned int>  umat2;

  typedef fmat2 mat2;
}

#endif
