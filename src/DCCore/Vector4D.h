#ifndef DC_Vector4D
#define DC_Vector4D

/** General purpose double quad. Uses include representation
  * of color coordinates.
  * No support yet added for double * Vector4D - is it necessary?
  * Need to define a non-member non-friend operator*  etc.
  * Vector4D * double is okay
*/
template <class T_Scalar> class Vector4D
{
    public:
        /** Number of vector components. */
        enum { num_components = 4 };
        
		union
		{
			struct
			{
				T_Scalar x,y,z,w;
			};
			T_Scalar _v[4];
		};

        /** Constructor that sets all components of the vector to zero */
        Vector4D() { _v[0]=0.0; _v[1]=0.0; _v[2]=0.0; _v[3]=0.0; }

        Vector4D(T_Scalar x, T_Scalar y, T_Scalar z, T_Scalar w)
        {
            _v[0]=x;
            _v[1]=y;
            _v[2]=z;
            _v[3]=w;
        }

        Vector4D(const Vector3D<T_Scalar >& v3,T_Scalar w)
        {
            _v[0]=v3[0];
            _v[1]=v3[1];
            _v[2]=v3[2];
            _v[3]=w;
        }
            
        inline Vector4D(const Vector4D<T_Scalar >& vec) { _v[0]=vec._v[0]; _v[1]=vec._v[1]; _v[2]=vec._v[2]; _v[3]=vec._v[3];}
        
        inline operator Vector4D<T_Scalar >() const { return Vector4D<T_Scalar >(static_cast<float>(_v[0]),static_cast<float>(_v[1]),static_cast<float>(_v[2]),static_cast<float>(_v[3]));}


        inline bool operator == (const Vector4D& v) const { return _v[0]==v._v[0] && _v[1]==v._v[1] && _v[2]==v._v[2] && _v[3]==v._v[3]; }

        inline bool operator != (const Vector4D& v) const { return _v[0]!=v._v[0] || _v[1]!=v._v[1] || _v[2]!=v._v[2] || _v[3]!=v._v[3]; }

        inline bool operator <  (const Vector4D& v) const
        {
            if (_v[0]<v._v[0]) return true;
            else if (_v[0]>v._v[0]) return false;
            else if (_v[1]<v._v[1]) return true;
            else if (_v[1]>v._v[1]) return false;
            else if (_v[2]<v._v[2]) return true;
            else if (_v[2]>v._v[2]) return false;
            else return (_v[3]<v._v[3]);
        }

        inline T_Scalar* ptr() { return _v; }
        inline const T_Scalar* ptr() const { return _v; }

        inline void set( T_Scalar x, T_Scalar y, T_Scalar z, T_Scalar w)
        {
            _v[0]=x; _v[1]=y; _v[2]=z; _v[3]=w;
        }

        T_Scalar& operator[](unsigned int i) { return _v[i]; }
        const T_Scalar&  operator[](unsigned int i) const { return _v[i]; }

        T_Scalar& Getx() { return _v[0]; }
        T_Scalar& Gety() { return _v[1]; }
        T_Scalar& Getz() { return _v[2]; }
        T_Scalar& Getw() { return _v[3]; }

        inline T_Scalar Getx() const { return _v[0]; }
        inline T_Scalar Gety() const { return _v[1]; }
        inline T_Scalar Getz() const { return _v[2]; }
        inline T_Scalar Getw() const { return _v[3]; }

        inline T_Scalar& r() { return _v[0]; }
        inline T_Scalar& g() { return _v[1]; }
        inline T_Scalar& b() { return _v[2]; }
        inline T_Scalar& a() { return _v[3]; }

        inline T_Scalar r() const { return _v[0]; }
        inline T_Scalar g() const { return _v[1]; }
        inline T_Scalar b() const { return _v[2]; }
        inline T_Scalar a() const { return _v[3]; }

		Vector3D<T_Scalar> Getxyz() const { return Vector3D<T_Scalar>(Getx(),Gety(),Getz()); }
		Vector3D<T_Scalar> Getrgb() const { return Vector3D<T_Scalar>(Getx(),Gety(),Getz()); }
		Vector3D<T_Scalar> Getstp() const { return Vector3D<T_Scalar>(Getx(),Gety(),Getz()); }

		Vector2D<T_Scalar> Getxy() const { return Vector2D<T_Scalar>(Getx(),Gety()); }
		Vector2D<T_Scalar> Getrg() const { return Vector2D<T_Scalar>(Getx(),Gety()); }
		Vector2D<T_Scalar> Getst() const { return Vector2D<T_Scalar>(Getx(),Gety()); }

		bool IsNull() const { return !x && !y && !z && !w; }

        inline unsigned int asABGR() const
        {
            return (unsigned int)clampTo((_v[0]*255.0),0.0,255.0)<<24 |
                   (unsigned int)clampTo((_v[1]*255.0),0.0,255.0)<<16 |
                   (unsigned int)clampTo((_v[2]*255.0),0.0,255.0)<<8  |
                   (unsigned int)clampTo((_v[3]*255.0),0.0,255.0);
        }

        inline unsigned int asRGBA() const
        {
            return (unsigned int)clampTo((_v[3]*255.0),0.0,255.0)<<24 |
                   (unsigned int)clampTo((_v[2]*255.0),0.0,255.0)<<16 |
                   (unsigned int)clampTo((_v[1]*255.0),0.0,255.0)<<8  |
                   (unsigned int)clampTo((_v[0]*255.0),0.0,255.0);
        }

        /** Returns true if all components have values that are not NaN. */
        inline bool valid() const { return !isNaN(); }
        /** Returns true if at least one component has value NaN. */
        inline bool isNaN() const { return DC::isNaN(_v[0]) || DC::isNaN(_v[1]) || DC::isNaN(_v[2]) || DC::isNaN(_v[3]); }

        /** Dot product. */
        inline T_Scalar operator * (const Vector4D& rhs) const
        {
            return _v[0]*rhs._v[0]+
                   _v[1]*rhs._v[1]+
                   _v[2]*rhs._v[2]+
                   _v[3]*rhs._v[3] ;
        }

        /** Multiply by scalar. */
        inline Vector4D operator * (T_Scalar rhs) const
        {
            return Vector4D(_v[0]*rhs, _v[1]*rhs, _v[2]*rhs, _v[3]*rhs);
        }

        /** Unary multiply by scalar. */
        inline Vector4D& operator *= (T_Scalar rhs)
        {
            _v[0]*=rhs;
            _v[1]*=rhs;
            _v[2]*=rhs;
            _v[3]*=rhs;
            return *this;
        }

        /** Divide by scalar. */
        inline Vector4D operator / (T_Scalar rhs) const
        {
            return Vector4D(_v[0]/rhs, _v[1]/rhs, _v[2]/rhs, _v[3]/rhs);
        }

        /** Unary divide by scalar. */
        inline Vector4D& operator /= (T_Scalar rhs)
        {
            _v[0]/=rhs;
            _v[1]/=rhs;
            _v[2]/=rhs;
            _v[3]/=rhs;
            return *this;
        }

        /** Binary vector add. */
        inline Vector4D operator + (const Vector4D& rhs) const
        {
            return Vector4D(_v[0]+rhs._v[0], _v[1]+rhs._v[1],
                         _v[2]+rhs._v[2], _v[3]+rhs._v[3]);
        }

        /** Unary vector add. Slightly more efficient because no temporary
          * intermediate object.
        */
        inline Vector4D& operator += (const Vector4D& rhs)
        {
            _v[0] += rhs._v[0];
            _v[1] += rhs._v[1];
            _v[2] += rhs._v[2];
            _v[3] += rhs._v[3];
            return *this;
        }

        /** Binary vector subtract. */
        inline Vector4D operator - (const Vector4D& rhs) const
        {
            return Vector4D(_v[0]-rhs._v[0], _v[1]-rhs._v[1],
                         _v[2]-rhs._v[2], _v[3]-rhs._v[3] );
        }

        /** Unary vector subtract. */
        inline Vector4D& operator -= (const Vector4D& rhs)
        {
            _v[0]-=rhs._v[0];
            _v[1]-=rhs._v[1];
            _v[2]-=rhs._v[2];
            _v[3]-=rhs._v[3];
            return *this;
        }

        /** Negation operator. Returns the negative of the Vector4D. */
        inline const Vector4D operator - () const
        {
            return Vector4D (-_v[0], -_v[1], -_v[2], -_v[3]);
        }

        /** Length of the vector = sqrt( vec . vec ) */
        inline T_Scalar length() const
        {
            return sqrt( _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] + _v[3]*_v[3]);
        }

        /** Length squared of the vector = vec . vec */
        inline T_Scalar length2() const
        {
            return _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] + _v[3]*_v[3];
        }

        /** Normalize the vector so that it has length unity.
          * Returns the previous length of the vector.
        */
        inline T_Scalar normalize()
        {
            T_Scalar norm = Vector4D::length();
            if (norm>0.0f)
            {
                T_Scalar inv = 1.0/norm;
                _v[0] *= inv;
                _v[1] *= inv;
                _v[2] *= inv;
                _v[3] *= inv;
            }
            return( norm );
        }

		/** Compute the dot product of a (Vec3,1.0) and a Vector4D. */
		inline T_Scalar Multi3_4(const Vector3D<T_Scalar >& lhs,const Vector4D& rhs)
		{
			return lhs[0]*rhs[0]+lhs[1]*rhs[1]+lhs[2]*rhs[2]+rhs[3];
		}


		/** Compute the dot product of a Vector4D and a (Vec3,1.0). */
		inline T_Scalar Multi4_3(const Vector4D& lhs,const Vector3D<T_Scalar >& rhs)
		{
			return lhs[0]*rhs[0]+lhs[1]*rhs[1]+lhs[2]*rhs[2]+lhs[3];
		}

		/** multiply by vector components. */
		inline Vector4D componentMultiply(const Vector4D& lhs, const Vector4D& rhs)
		{
			return Vector4D(lhs[0]*rhs[0], lhs[1]*rhs[1], lhs[2]*rhs[2], lhs[3]*rhs[3]);
		}

		/** divide rhs components by rhs vector components. */
		inline Vector4D componentDivide(const Vector4D& lhs, const Vector4D& rhs)
		{
			return Vector4D(lhs[0]/rhs[0], lhs[1]/rhs[1], lhs[2]/rhs[2], lhs[3]/rhs[3]);
		}

};

//! A 4 components vector with \p int precision.
typedef Vector4D<int> ivec4;
//! A 4 components vector with \p unsigned int precision.
typedef Vector4D<unsigned int> uvec4;
//! A 4 components vector with \p float precision.
typedef Vector4D<float> fvec4;
//! A 4 components vector with \p double precision.
typedef Vector4D<double> dvec4;
//! A 4 components vector with \p char precision.
typedef Vector4D<char> bvec4;
//! A 4 components vector with \p unsigned char precision.
typedef Vector4D<unsigned char> ubvec4;
//! A 4 components vector with \p short precision.
typedef Vector4D<short> svec4;
//! A 4 components vector with \p unsigned short precision.
typedef Vector4D<unsigned short> usvec4;

typedef fvec4 vec4;

#endif
