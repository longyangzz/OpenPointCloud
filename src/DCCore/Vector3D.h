#ifndef DCCORE_VECTOR3D_H
#define DCCORE_VECTOR3D_H

// namespace DCCore
// {
	template <class Type> class Vector3D
	{
	public:
		typedef Type value_type;
		union
		{
			struct
			{
				Type x,y,z;
			};
			Type u[3];
		};

		//默认构造函数
		//将向量初始化为(0,0,0)
		inline Vector3D(Type s = 0)
			: x(s)
			, y(s)
			, z(s) 
		{}

		//通过三个坐标值构造向量
		inline Vector3D(Type _x, Type _y, Type _z)
			: x(_x)
			, y(_y)
			, z(_z)
		{}

		//通过三个元素的一维数组进行构造向量
		inline Vector3D(const Type p[])
			: x(p[0]),y(p[1]),z(p[2]) {}

		//拷贝初始化
		inline Vector3D(const Vector3D& v)
			: x(v.x)
			, y(v.y)
			, z(v.z) 
		{}

		inline void Set( value_type x, value_type y, value_type z)
		{
			u[0]=x; u[1]=y; u[2]=z;
		}

		inline void Set( const Vector3D& rhs)
		{
			u[0]=rhs.u[0]; u[1]=rhs.u[1]; u[2]=rhs.u[2];
		}

		//点乘
		inline Type Dot(const Vector3D& v) const 
		{ 
			return (x * v.x) + (y * v.y) + (z * v.z); 
		}

		//叉乘
		inline Vector3D Cross(const Vector3D &v) const 
		{
			return Vector3D((y * v.z) - (z * v.y),
				(z * v.x) - (x * v.z), 
				(x * v.y) - (y * v.x)); 
		}

		Type Length() const { return ::sqrt((x * x) + (y * y) + (z * z)); }
		Type LengthSquared() const { return (x * x) + (y * y) + (z * z); }

		//向量模的平方
		inline Type Normal2() const 
		{ 
			return (x * x) + (y * y) + (z * z);
		}

		//向量的模
		inline Type Normal() const 
		{ 
			return sqrt(Normal2()); 
		}

		//向量转为单位向量
		inline void Normalize() 
		{ 
			Type n = Normal2(); 
			//如果向量的模不为0
			if (n>0) 
			{
				*this /= sqrt(n); 
			}
		}



		//求向量的正交向量
		inline Vector3D Orthogonal() const 
		{ 
			Vector3D ort; 
			VOrthogonal(u, ort.u); 
			return ort; 
		}

		//负号运算符
		inline Vector3D operator - () const 
		{ 
			//每个元素求负
			Vector3D V(-x, -y, -z); 
			return V; 
		}

		//加法赋值
		inline Vector3D& operator += (const Vector3D& v)
		{ 
			x += v.x;
			y += v.y; 
			z += v.z;
			return *this; 
		}
		//判断相等  // 修改---20140510
		inline bool operator == (const Vector3D& v) const
		{
			if (abs(x - v.x) < 0.0000001
				&& abs(y - v.y) < 0.0000001
				&& abs(z - v.z) < 0.0000001)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		//减法赋值
		inline Vector3D& operator -= (const Vector3D& v)
		{
			x -= v.x; 
			y -= v.y; 
			z -= v.z; 
			return *this; 
		}

		//乘法赋值
		inline Vector3D& operator *= (Type v) 
		{ 
			x *= v; 
			y *= v; 
			z *= v; 
			return *this; }

		//除法赋值
		inline Vector3D& operator /= (Type v) 
		{ 
			x /= v;
			y /= v; 
			z /= v; 
			return *this;
		}

		//加法
		inline Vector3D operator + (const Vector3D& v) const 
		{ 
			return Vector3D(x + v.x, y + v.y, z + v.z);
		}

		//减法
		inline Vector3D operator - (const Vector3D& v) const 
		{ 
			return Vector3D(x - v.x, y - v.y, z - v.z); 
		}

		//数乘
		inline Vector3D operator * (Type s) const 
		{
			return Vector3D(x * s, y * s, z * s); 
		}

		//数除
		inline Vector3D operator / (Type s) const 
		{
			return Vector3D(x / s, y / s, z / s);
		}

		//叉乘
		inline Vector3D operator * (const Vector3D& v) const 
		{
			return Cross(v); 
		}

		//赋值
		inline Vector3D& operator = (const Vector3D &v) 
		{
			x = v.x; 
			y = v.y; 
			z = v.z; 
			return *this; 
		}

		//点乘
		inline Type operator && (const Vector3D &v) const 
		{ 
			return Dot(v);
		}

		//下标运算
		inline Type& operator [] (unsigned i) 
		{ 
			return u[i]; 
		}

		//下标运算
		inline const Type& operator [] (unsigned i) const 
		{ 
			return u[i];
		}

		bool IsNull() const { return !x && !y && !z; }

		//根据数组生成 int
		static inline Vector3D FromArray(const int array[3]) 
		{ 
			return Vector3D(static_cast<Type>(array[0]),
				static_cast<Type>(array[1]),
				static_cast<Type>(array[2])); 
		}
		//根据数组生成 float
		static inline Vector3D FromArray(const float array[3]) 
		{ 
			return Vector3D(static_cast<Type>(array[0]),
				static_cast<Type>(array[1]),
				static_cast<Type>(array[2])); 
		}
		//根据数组生成 double
		static inline Vector3D FromArray(const double array[3]) 
		{ 
			return Vector3D(static_cast<Type>(array[0]),
				static_cast<Type>(array[1]),
				static_cast<Type>(array[2])); 
		}


		//向量除法
		static inline void VDivide(const Type p[], Type s, Type r[])		//第一个参数为要运算的向量
		{															//第二个参数为除数
			r[0] = p[0] / s;										//第三个参数为运算结果
			r[1] = p[1] / s;
			r[2] = p[2] / s;
		}

		static inline void VDivide(Type p[], Type s)						//将第一个参数除以第二个参数并返回
		{
			p[0] /= s;
			p[1] /= s; 
			p[2] /= s;
		}

		//向量乘法
		static inline void VMultiply(const Type p[], Type s, Type r[]) 
		{
			r[0] = p[0] * s;
			r[1] = p[1] * s;
			r[2] = p[2] * s;
		}

		static inline void VMultiply(Type p[], Type s) 
		{
			p[0] *= s;
			p[1] *= s; 
			p[2] *= s;
		}

		static inline Type VDot(const Type p[], const Type q[]) 
		{
			return (p[0] * q[0]) + 
				(p[1] * q[1]) + 
				(p[2] * q[2]);
		}

		static inline void VCross(const Type p[], const Type q[], Type r[]) 
		{
			r[0] = (p[1] * q[2]) - (p[2] * q[1]); 
			r[1] = (p[2] * q[0]) - (p[0] * q[2]); 
			r[2] = (p[0] * q[1]) - (p[1] * q[0]);
		}

		static inline void VCopy(const Type p[], Type q[]) 
		{
			q[0] = p[0];
			q[1] = p[1]; 
			q[2] = p[2];
		}

		static inline void VSet(Type p[], Type s)
		{
			p[0] = p[1] = p[2] = s;
		}

		static inline void VSet(Type p[], Type x, Type y, Type z) 
		{
			p[0] = x;
			p[1] = y; 
			p[2] = z;
		}

		static inline void VAdd(const Type p[], const Type q[], Type r[]) 
		{
			r[0] = p[0] + q[0];
			r[1] = p[1] + q[1]; 
			r[2] = p[2] + q[2];
		}

		static inline void VSubstract(const Type p[], const Type q[], Type r[]) 
		{
			r[0] = p[0] - q[0]; 
			r[1] = p[1] - q[1];
			r[2] = p[2] - q[2];
		}

		static inline void VCombination(Type a, const Type p[], Type b, const Type q[], Type r[])
		{
			r[0] = (a * p[0]) + (b * q[0]); 
			r[1] = (a * p[1]) + (b * q[1]);
			r[2] = (a * p[2]) + (b * q[2]);
		}

		static inline void VCombination(const Type p[], Type b, const Type q[], Type r[])
		{
			r[0] = p[0] + (b * q[0]);
			r[1] = p[1] + (b * q[1]); 
			r[2] = p[2] + (b * q[2]);
		}

		static inline void VNormalize(Type p[]) 
		{
			Type n = VNormal2(p);
			if (n>0) 
			{
				VDivide(p, sqrt(n));
			}
		}

		static inline Type VNormal2(const Type p[])
		{
			return (p[0] * p[0]) + 
				(p[1] * p[1]) +
				(p[2] * p[2]);
		}

		static inline Type VDistance2(const Type p[], const Type q[]) 
		{
			return ((p[0] - q[0]) * (p[0] - q[0])) + 
				((p[1] - q[1]) * (p[1] - q[1])) +
				((p[2] - q[2]) * (p[2] - q[2]));
		}

		static inline Type VDistance2(const Vector3D& p1, const Vector3D& p2) 
		{
			return ((p1.x - p2.x) * (p1.x - p2.x)) + 
				((p1.y - p2.y) * (p1.y - p2.y)) +
				((p1.z - p2.z) * (p1.z - p2.z));
		}

		static inline Type VNormal(const Type p[]) 
		{
			return sqrt(VNormal2(p));
		}

		static inline Type VDistance(const Type p[], const Type q[]) 
		{
			return sqrt(VDistance2(p, q));
		}

		static inline Type VDistance(const Vector3D& p1, const Vector3D& p2) 
		{
			return sqrt(VDistance2(p1, p2));
		}

		//计算向量一的正交向量(参数二)
		static inline void VOrthogonal(const Type p[], Type q[])
		{
			if (fabs(p[0]) <= fabs(p[1]) && fabs(p[0]) <= fabs(p[2]))
			{
				q[0] = 0; 
				q[1] = p[2]; 
				q[2] = -p[1];
			}
			else if (fabs(p[1]) <= fabs(p[0]) && fabs(p[1]) <= fabs(p[2]))
			{
				q[0] = -p[2];
				q[1] = 0; 
				q[2] = p[0];
			}
			else
			{
				q[0] = p[1];
				q[1] = -p[0];
				q[2] = 0;
			}
			VNormalize(q);
		}

		const Vector3D& normalize(Type *len=NULL)
		{
			Type l = Length();
			if (len)
				*len = l;
			if (l)
				*this *= (Type)(1.0/l); 
			return *this; 
		}
	};

	//! A 3 components vector with \p int precision.
	typedef Vector3D<int> ivec3;
	//! A 3 components vector with \p unsigned int precision.
	typedef Vector3D<unsigned int> uvec3;
	//! A 3 components vector with \p float precision.
	typedef Vector3D<float> fvec3;
	//! A 3 components vector with \p double precision.
	typedef Vector3D<double> dvec3;
	//! A 3 components vector with \p char precision.
	typedef Vector3D<char> bvec3;
	//! A 3 components vector with \p unsigned char precision.
	typedef Vector3D<unsigned char> ubvec3;
	//! A 3 components vector with \p short precision.
	typedef Vector3D<short> svec3;
	//! A 3 components vector with \p unsigned short precision.
	typedef Vector3D<unsigned short> usvec3;

	inline float dot(const fvec3& v1, const fvec3& v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
	inline double dot(const dvec3& v1, const dvec3& v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
	inline float dot(const ivec3& v1, const ivec3& v2) { return (float)(v1.x*v2.x + v1.y*v2.y + v1.z*v2.z); }
	inline float dot(const uvec3& v1, const uvec3& v2) { return (float)(v1.x*v2.x + v1.y*v2.y + v1.z*v2.z); }

	inline fvec3 cross(const fvec3& v1, const fvec3& v2)
	{
		fvec3 t;
		t.x = v1.y*v2.z - v1.z*v2.y;
		t.y = v1.z*v2.x - v1.x*v2.z;
		t.z = v1.x*v2.y - v1.y*v2.x;
		return t;
	}

	inline dvec3 cross(const dvec3& v1, const dvec3& v2)
	{
		dvec3 t;
		t.x = v1.y*v2.z - v1.z*v2.y;
		t.y = v1.z*v2.x - v1.x*v2.z;
		t.z = v1.x*v2.y - v1.y*v2.x;
		return t;
	}

	inline fvec3 Min(const fvec3& a, const fvec3& b)
	{
		return fvec3( a.x < b.x ? a.x : b.x,
			a.y < b.y ? a.y : b.y,
			a.z < b.z ? a.z : b.z );
	}
	inline fvec3 Min(const fvec3& a, float b)
	{
		return fvec3( a.x < b ? a.x : b,
			a.y < b ? a.y : b,
			a.z < b ? a.z : b );
	}
	inline dvec3 Min(const dvec3& a, const dvec3& b)
	{
		return dvec3( a.x < b.x ? a.x : b.x,
			a.y < b.y ? a.y : b.y,
			a.z < b.z ? a.z : b.z );
	}
	inline dvec3 Min(const dvec3& a, double b)
	{
		return dvec3( a.x < b ? a.x : b,
			a.y < b ? a.y : b,
			a.z < b ? a.z : b );
	}
	inline ivec3 Min(const ivec3& a, const ivec3& b)
	{
		return ivec3( a.x < b.x ? a.x : b.x,
			a.y < b.y ? a.y : b.y,
			a.z < b.z ? a.z : b.z );
	}
	inline ivec3 Min(const ivec3& a, int b)
	{
		return ivec3( a.x < b ? a.x : b,
			a.y < b ? a.y : b,
			a.z < b ? a.z : b );
	}
	inline uvec3 Min(const uvec3& a, const uvec3& b)
	{
		return uvec3( a.x < b.x ? a.x : b.x,
			a.y < b.y ? a.y : b.y,
			a.z < b.z ? a.z : b.z );
	}
	inline uvec3 Min(const uvec3& a, unsigned int b)
	{
		return uvec3( a.x < b ? a.x : b,
			a.y < b ? a.y : b,
			a.z < b ? a.z : b );
	}
	inline fvec3 Max(const fvec3& a, const fvec3& b)
	{
		return fvec3( a.x > b.x ? a.x : b.x,
			a.y > b.y ? a.y : b.y,
			a.z > b.z ? a.z : b.z );
	}
	inline fvec3 Max(const fvec3& a, float b)
	{
		return fvec3( a.x > b ? a.x : b,
			a.y > b ? a.y : b,
			a.z > b ? a.z : b );
	}
	inline dvec3 Max(const dvec3& a, const dvec3& b)
	{
		return dvec3( a.x > b.x ? a.x : b.x,
			a.y > b.y ? a.y : b.y,
			a.z > b.z ? a.z : b.z );
	}
	inline dvec3 Max(const dvec3& a, double b)
	{
		return dvec3( a.x > b ? a.x : b,
			a.y > b ? a.y : b,
			a.z > b ? a.z : b );
	}
	inline ivec3 Max(const ivec3& a, const ivec3& b)
	{
		return ivec3( a.x > b.x ? a.x : b.x,
			a.y > b.y ? a.y : b.y,
			a.z > b.z ? a.z : b.z );
	}
	inline ivec3 Max(const ivec3& a, int b)
	{
		return ivec3( a.x > b ? a.x : b,
			a.y > b ? a.y : b,
			a.z > b ? a.z : b );
	}
	inline uvec3 Max(const uvec3& a, const uvec3& b)
	{
		return uvec3( a.x > b.x ? a.x : b.x,
			a.y > b.y ? a.y : b.y,
			a.z > b.z ? a.z : b.z );
	}
	inline uvec3 Max(const uvec3& a, unsigned int b)
	{
		return uvec3( a.x > b ? a.x : b,
			a.y > b ? a.y : b,
			a.z > b ? a.z : b );
	}

	inline fvec3 clamp(const fvec3& x, float minval, float maxval) { return Min(Max(x,minval),maxval); }
	inline fvec3 clamp(const fvec3& x, const fvec3& minval, const fvec3& maxval) { return Min(Max(x,minval),maxval); }
	inline ivec3 clamp(const ivec3& x, const ivec3& minval, const ivec3& maxval) { return Min(Max(x,minval),maxval); }
	inline dvec3 clamp(const dvec3& x, double minval, double maxval) { return Min(Max(x,minval),maxval); }
	inline dvec3 clamp(const dvec3& x, const dvec3& minval, const dvec3& maxval) { return Min(Max(x,minval),maxval); }
	inline ivec3 clamp(const ivec3& x, int minval, int maxval) { return Min(Max(x,minval),maxval); }
	inline uvec3 clamp(const uvec3& x, unsigned int minval, unsigned int maxval) { return Min(Max(x,minval),maxval); }
	inline uvec3 clamp(const uvec3& x, const uvec3& minval, const uvec3& maxval) { return Min(Max(x,minval),maxval); }

#endif