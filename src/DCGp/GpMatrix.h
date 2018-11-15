#ifndef DCGP_MATRIX_H
#define DCGP_MATRIX_H

#include "DCGp/DcGpDLL.h"

//Qt
#include "QChar"
class QString;

#include "DCCore/DCDefs.h"

//DCcommon
#include "DCCore/BasicTypes.h"
#include "DCCore/Matrix.h"

namespace DcGp
{

//模型视图矩阵的大小(OpenGL)
static const unsigned OPENGL_MATRIX_SIZE = 16;

//矩阵的每个元素，按（行，列）排序
#define DC_MAT_R11 m_mat[0]
#define DC_MAT_R21 m_mat[1]
#define DC_MAT_R31 m_mat[2]
#define DC_MAT_R41 m_mat[3]

#define DC_MAT_R12 m_mat[4]
#define DC_MAT_R22 m_mat[5]
#define DC_MAT_R32 m_mat[6]
#define DC_MAT_R42 m_mat[7]

#define DC_MAT_R13 m_mat[8]
#define DC_MAT_R23 m_mat[9]
#define DC_MAT_R33 m_mat[10]
#define DC_MAT_R43 m_mat[11]

#define DC_MAT_R14 m_mat[12]
#define DC_MAT_R24 m_mat[13]
#define DC_MAT_R34 m_mat[14]
#define DC_MAT_R44 m_mat[15]

//矩阵进行变换的操作是M*X = M*R+T
	class DCGP_API DcGpMatrix
	{
	public:

		//!默认单位阵
		DcGpMatrix();

		//根据一个opengl格式的数组构造矩阵
		DcGpMatrix(const float* mat16);

		//! Constructor from 4 columns (X,Y,Z,T)
		/** \param X 3 first elements of the 1st column (last one is 0)
			\param Y 3 first elements of the 2nd column (last one is 0)
			\param Z 3 first elements of the 3rd column (last one is 0)
			\param T 3 first elements of the last column (last one is 1)
		**/
		//根据四列构造矩阵
		//xyz代表第1，2，3列，每列最后一个元素为0，t代表最后一列，最后一个元素1
		DcGpMatrix(const DCVector3D& X, const DCVector3D& Y, const DCVector3D& Z, const DCVector3D& T);

		//! 3*3的旋转矩阵和一个平移矩阵t
		DcGpMatrix(const DCCore::SquareMatrix& R, const DCVector3D& T);

		//! 3*3旋转矩阵、平移矩阵T、缩放因子S
		DcGpMatrix(const DCCore::SquareMatrix& R, const DCVector3D& T, double S);

		//! Constructor from a rotation center G, a 3x3 rotation matrix R and a vector T
		//选中中心G，3*3旋转矩阵R，平移矩阵T
		DcGpMatrix(const DCCore::SquareMatrix& R, const DCVector3D& T, const DCVector3D& rotCenter);

		//! 复制构造
		DcGpMatrix(const DcGpMatrix& mat);

		static DcGpMatrix Interpolate(PointCoordinateType coef, const DcGpMatrix& glMat1, const DcGpMatrix& glMat2);

		//从一个矩阵到另一个矩阵的 转换矩阵
		static DcGpMatrix FromToRotation(const DCVector3D& from, const DCVector3D& to);

		//转换四元数
		static DcGpMatrix FromQuaternion(const float q[]);

		//!返回旋转只在X
		DcGpMatrix XRotation() const;
		//!返回旋转只在Y
		DcGpMatrix YRotation() const;
		//!返回旋转只在Z
		DcGpMatrix ZRotation() const;

		//清空为0
		virtual void ToZero();

		//单位阵
		virtual void ToIdentity();

		//!平移值设置为0
		void ClearTranslation();

		//根据旋转轴，角度（弧度）和平移量初始化
		void InitFromParameters(float alpha, const DCVector3D& axis3D, const DCVector3D& t3D);

		//根据3个角度（弧度）和平移量初始化
		void InitFromParameters(float phi, float theta, float psi, const DCVector3D& t3D);

		void GetParameters(float& alpha, DCVector3D& axis3D, DCVector3D& t3D) const;

		void GetParameters(float &phi, float &theta, float &psi, DCVector3D& t3D) const;

		inline float* Data() { return m_mat; }

		inline const float* Data() const { return m_mat; }

		inline float* GetTranslation() { return m_mat+12; }

		inline const float* GetTranslation() const { return m_mat+12; }

		void SetTranslation(const DCVector3D& T);

		//! 返回某一列值
		inline float* GetColumn(int index) { return m_mat+(index<<2); }


		inline const float* GetColumn(int index) const { return m_mat+(index<<2); }

		//! 重载乘法运算
		DcGpMatrix operator * (const DcGpMatrix& mat) const;

		inline DCVector3D operator * (const DCVector3D& vec) const
		{
			return DCVector3D(ApplyX(vec),ApplyY(vec),ApplyZ(vec));
		}

		DcGpMatrix& operator *= (const DcGpMatrix& mat);

		DcGpMatrix& operator += (const DCVector3D& T);

		DcGpMatrix& operator -= (const DCVector3D& T);

		inline void Apply(DCVector3D& vec) const
		{
			vec = (*this)*vec;
		}

		inline float ApplyX(const DCVector3D& vec) const
		{
			return DC_MAT_R11*vec.x + DC_MAT_R12*vec.y + DC_MAT_R13*vec.z + DC_MAT_R14;
		}

		inline float ApplyY(const DCVector3D& vec) const
		{
			return DC_MAT_R21*vec.x + DC_MAT_R22*vec.y + DC_MAT_R23*vec.z + DC_MAT_R24;
		}

		inline float ApplyZ(const DCVector3D& vec) const
		{
			return DC_MAT_R31*vec.x + DC_MAT_R32*vec.y + DC_MAT_R33*vec.z + DC_MAT_R34;
		}

		inline void ApplyRotation(DCVector3D& vec) const
		{
			ApplyRotation(vec.u);
		}

		inline void ApplyRotation(float* vec) const
		{
			register double vx=vec[0];
			register double vy=vec[1];
			register double vz=vec[2];

			vec[0] = DC_MAT_R11*vx + DC_MAT_R12*vy + DC_MAT_R13*vz;
			vec[1] = DC_MAT_R21*vx + DC_MAT_R22*vy + DC_MAT_R23*vz;
			vec[2] = DC_MAT_R31*vx + DC_MAT_R32*vy + DC_MAT_R33*vz;
		}

		//! 移动旋转中心
		void ShiftRotationCenter(const DCVector3D& vec);

		//! 转置
		void Transpose();

		//! 返回转置矩阵
		DcGpMatrix Transposed() const;

		//! 求逆
		void Invert();

		DcGpMatrix Inverse() const;

		//! 保存到文本
		virtual bool ToAsciiFile(const char* filename) const;

		//! 从文本加载
		virtual bool FomAsciiFile(const char* filename);

		//缩放
		void Scale(float coef);

		//! 缩放某一行
		void ScaleLine(unsigned lineIndex, float coef);

		//! 缩放某一列
		void ScaleColumn(unsigned colIndex, float coef);

		//! 返回指定精度的字符串矩阵
		QString ToString(int precision = 12, QChar separator = ' ') const;

	protected:

		//!4*4opengl格式矩阵
		float m_mat[OPENGL_MATRIX_SIZE];
	};

}
#endif
