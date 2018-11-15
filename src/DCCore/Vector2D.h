#ifndef DCCORE_VECTOR2D_H
#define DCCORE_VECTOR2D_H

// namespace DCCore
// {
	//二维向量
	template <class Type> class Vector2D
	{
	public:

		union
		{
			struct
			{
				Type x,y;
			};
			Type u[2];
		};

		//默认构造函数
		Vector2D(Type s = 0)
			: x(s)
			, y(s) 
		{}

		//通过两个元素构造
		Vector2D(Type _x, Type _y)
			: x(_x)
			, y(_y)
		{}

		//点乘
		inline Type Dot(const Vector2D& v) const
		{
			return (x * v.x + y * v.y);
		}

		//叉乘
		inline Vector2D Cross(const Vector2D &v) const 
		{
			return Vector2D((x * v.y) - (y * v.x)); 
		}

		//向量模的平方
		inline Type Normal2() const
		{
			return (x * x) + (y * y); 
		}

		//向量模
		inline Type Normal() const
		{ 
			return sqrt(Normal2()); 
		}

		//单位化
		inline void Normalize()
		{ 
			Type n = Normal2();
			if (n>0)
			{
				*this /= sqrt(n);
			}
		}

		bool IsNull() const { return !x && !y; }

		//负号运算符
		inline Vector2D& operator - () 
		{
			x = -x; 
			y = -y; 
			return *this;
		}

		//加法赋值
		inline Vector2D& operator += (const Vector2D& v) 
		{
			x += v.x;
			y += v.y; 
			return *this;
		}

		//减法赋值
		inline Vector2D& operator -= (const Vector2D& v) 
		{
			x -= v.x;
			y -= v.y; 
			return *this;
		}

		//乘法赋值
		inline Vector2D& operator *= (Type v) 
		{
			x *= v; 
			y *= v; 
			return *this;
		}

		//除法赋值
		inline Vector2D& operator /= (Type v) 
		{
			x /= v;
			y /= v; 
			return *this;
		}

		inline Vector2D& operator /= (Vector2D v) 
		{
			x /= v.x;
			y /= v.y; 
			return *this;
		}
		//加法
		inline Vector2D operator + (const Vector2D& v) const 
		{
			return Vector2D(x + v.x, y + v.y);
		}

		//减法
		inline Vector2D operator - (const Vector2D& v) const 
		{
			return Vector2D(x - v.x, y - v.y);
		}

		//乘法
		inline Vector2D operator * (Type s) const
		{
			return Vector2D(x * s, y * s);
		}

		//除法
		inline Vector2D operator / (Type s) const 
		{
			return Vector2D(x / s, y / s);
		}

		inline Vector2D operator / (Vector2D v)
		{
			return Vector2D(x / v.x, y / v.y);
		}

		//赋值
		inline Vector2D& operator = (const Vector2D &v) 
		{
			x = v.x;
			y = v.y; 
			return *this;
		}
		//下标运算符
		inline Type& operator [] (unsigned i) 
		{
			return u[i];
		}
		//下标运算
		inline const Type& operator [] (unsigned i) const 
		{
			return u[i];
		}

		//根据数组生成 int
		static inline Vector2D FromArray(const int array[2]) 
		{ 
			return Vector2D(static_cast<Type>(array[0]),
				static_cast<Type>(array[1])); 
		}
		//根据数组生成 float
		static inline Vector2D FromArray(const float array[2]) 
		{ 
			return Vector2D(static_cast<Type>(array[0]),
				static_cast<Type>(array[1])); 
		}
		//根据数组生成 double
		static inline Vector2D FromArray(const double array[2]) 
		{ 
			return Vector2D(static_cast<Type>(array[0]),
				static_cast<Type>(array[1])); 
		}

		static inline Type VDistance2(const Type p[], const Type q[]) 
		{
			return ((p[0] - q[0]) * (p[0] - q[0])) + 
				((p[1] - q[1]) * (p[1] - q[1]));
		}

		static inline Type VDistance2(const Vector2D& p1, const Vector2D& p2) 
		{
			return ((p1.x - p2.x) * (p1.x - p2.x)) + 
				((p1.y - p2.y) * (p1.y - p2.y));
		}

		static inline Type VDistance(const Type p[], const Type q[]) 
		{
			return sqrt(VDistance2(p, q));
		}

		static inline Type VDistance(const Vector2D& p1, const Vector2D& p2) 
		{
			return sqrt(VDistance2(p1, p2));
		}
	};

	//! A 2 components vector with \p int precision.
	typedef Vector2D<int> ivec2;
	//! A 2 components vector with \p unsigned int precision.
	typedef Vector2D<unsigned int> uvec2;
	//! A 2 components vector with \p float precision.
	typedef Vector2D<float> fvec2;
	//! A 2 components vector with \p double precision.
	typedef Vector2D<double> dvec2;
	//! A 2 components vector with \p char precision.
	typedef Vector2D<char> bvec2;
	//! A 2 components vector with \p unsigned char precision.
	typedef Vector2D<unsigned char> ubvec2;
	//! A 2 components vector with \p short precision.
	typedef Vector2D<short> svec2;
	//! A 2 components vector with \p unsigned short precision.
	typedef Vector2D<unsigned short> usvec2;

	typedef fvec2 vec2;


	inline float Min(float a, float b) { return a < b ? a : b; }
	inline double Min(double a, double b) { return a < b ? a : b; }
	inline int Min(int a, int b) { return a < b ? a : b; }
	inline unsigned int Min(unsigned int a, unsigned int b) { return a < b ? a : b; }
	inline float Max(float a, float b) { return a > b ? a : b; }
	inline double Max(double a, double b) { return a > b ? a : b; }
	inline int Max(int a, int b) { return a > b ? a : b; }
	inline unsigned int Max(unsigned int a, unsigned int b) { return a > b ? a : b; }
	inline float clamp(float x, float minval, float maxval) { return Min(Max(x,minval),maxval); }
	inline double clamp(double x, double minval, double maxval) { return Min(Max(x,minval),maxval); }
	inline int clamp(int x, int minval, int maxval) { return Min(Max(x,minval),maxval); }
	inline unsigned int clamp(unsigned int x, unsigned int minval, unsigned int maxval) { return Min(Max(x,minval),maxval); }

	inline fvec2 Min(const fvec2& a, const fvec2& b)
	{
		return fvec2( a.x < b.x ? a.x : b.x,
			a.y < b.y ? a.y : b.y);
	}
	inline fvec2 Min(const fvec2& a, float b)
	{
		return fvec2( a.x < b ? a.x : b,
			a.y < b ? a.y : b);
	}
	inline dvec2 Min(const dvec2& a, const dvec2& b)
	{
		return dvec2( a.x < b.x ? a.x : b.x,
			a.y < b.y ? a.y : b.y);
	}
	inline dvec2 Min(const dvec2& a, double b)
	{
		return dvec2( a.x < b ? a.x : b,
			a.y < b ? a.y : b);
	}
	inline ivec2 Min(const ivec2& a, const ivec2& b)
	{
		return ivec2( a.x < b.x ? a.x : b.x,
			a.y < b.y ? a.y : b.y);
	}
	inline ivec2 Min(const ivec2& a, int b)
	{
		return ivec2( a.x < b ? a.x : b,
			a.y < b ? a.y : b);
	}
	inline uvec2 Min(const uvec2& a, const uvec2& b)
	{
		return uvec2( a.x < b.x ? a.x : b.x,
			a.y < b.y ? a.y : b.y);
	}
	inline uvec2 Min(const uvec2& a, unsigned int b)
	{
		return uvec2( a.x < b ? a.x : b,
			a.y < b ? a.y : b);
	}
	inline fvec2 Max(const fvec2& a, const fvec2& b)
	{
		return fvec2( a.x > b.x ? a.x : b.x,
			a.y > b.y ? a.y : b.y);
	}
	inline fvec2 Max(const fvec2& a, float b)
	{
		return fvec2( a.x > b ? a.x : b,
			a.y > b ? a.y : b);
	}
	inline dvec2 Max(const dvec2& a, const dvec2& b)
	{
		return dvec2( a.x > b.x ? a.x : b.x,
			a.y > b.y ? a.y : b.y);
	}
	inline dvec2 Max(const dvec2& a, double b)
	{
		return dvec2( a.x > b ? a.x : b,
			a.y > b ? a.y : b);
	}
	inline ivec2 Max(const ivec2& a, const ivec2& b)
	{
		return ivec2( a.x > b.x ? a.x : b.x,
			a.y > b.y ? a.y : b.y);
	}
	inline ivec2 Max(const ivec2& a, int b)
	{
		return ivec2( a.x > b ? a.x : b,
			a.y > b ? a.y : b);
	}
	inline uvec2 Max(const uvec2& a, const uvec2& b)
	{
		return uvec2( a.x > b.x ? a.x : b.x,
			a.y > b.y ? a.y : b.y);
	}
	inline uvec2 Max(const uvec2& a, unsigned int b)
	{
		return uvec2( a.x > b ? a.x : b,
			a.y > b ? a.y : b);
	}
	inline fvec2 clamp(const fvec2& x, float minval, float maxval) { return Min(Max(x,minval),maxval); }
	inline fvec2 clamp(const fvec2& x, const fvec2& minval, const fvec2& maxval) { return Min(Max(x,minval),maxval); }
	inline dvec2 clamp(const dvec2& x, double minval, double maxval) { return Min(Max(x,minval),maxval); }
	inline dvec2 clamp(const dvec2& x, const dvec2& minval, const dvec2& maxval) { return Min(Max(x,minval),maxval); }
	inline ivec2 clamp(const ivec2& x, int minval, int maxval) { return Min(Max(x,minval),maxval); }
	inline ivec2 clamp(const ivec2& x, const ivec2& minval, const ivec2& maxval) { return Min(Max(x,minval),maxval); }
	inline uvec2 clamp(const uvec2& x, unsigned int minval, unsigned int maxval) { return Min(Max(x,minval),maxval); }
	inline uvec2 clamp(const uvec2& x, const uvec2& minval, const uvec2& maxval) { return Min(Max(x,minval),maxval); }
//}

#endif