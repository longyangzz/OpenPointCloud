#ifndef DCCORE_MATRIX_H
#define DCCORE_MATRIX_H

#ifdef _MSC_VER

#pragma warning( disable: 4251 )
#pragma warning( disable: 4530 )
#endif

//C++标准库
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

#include "BasicTypes.h"


#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);a[k][l]=h+s*(g-h*tau);

namespace DCCore
{

	//方阵
	template <typename Scalar> class Matrix
	{
	public:

		//默认构造，矩阵不可用
		Matrix()
		{
			Init(0);
		}

		//指定大小的方阵
		Matrix(unsigned size)
		{
			Init(size);
		}

		//拷贝提供的方阵
		Matrix(const Matrix& mat)
		{
			Init(mat.m_matrixSize);

			*this = mat;
		}

		Matrix(const float M16f[], bool rotationOnly = false)
		{
			unsigned l,c,size=(rotationOnly ? 3 : 4);

			Init(size);

			for (l=0;l<size;l++)
				for (c=0;c<size;c++)
					m_values[l][c] = (Scalar)M16f[c*4+l];
		}

		Matrix(const double M16d[], bool rotationOnly = false)
		{
			unsigned l,c,size=(rotationOnly ? 3 : 4);

			Init(size);

			for (l=0;l<size;l++)
				for (c=0;c<size;c++)
					m_values[l][c] = (Scalar)M16d[c*4+l];
		}

		virtual ~Matrix()
		{
			Invalidate();
		}

		//矩阵是否可用，如果矩阵大小为0，则返回false
		bool IsValid() const
		{
			return (m_matrixSize != 0);
		}

		//使矩阵作废，将大小设置为0
		void Invalidate()
		{
			if (eigenValues)
			{
				delete[] eigenValues;
				eigenValues = 0;
			}

			for (unsigned i = 0; i < m_matrixSize; ++i)
			{
				delete[] m_values[i];
			}

			delete[] m_values;
			m_values = 0;

			m_matrixSize = matrixSquareSize = 0;
		}

		//矩阵的大小
		unsigned Size() const 
		{ 
			return m_matrixSize; 
		}

		//矩阵的数据，按行存储
		Scalar** m_values;

		//返回指定行的指针
		Scalar* Line(unsigned index) 
		{ 
			return m_values[index];
		}

		//设置指定位置的值，（提供行列号）
		void SetValue(unsigned line, unsigned column, Scalar value)
		{
			m_values[line][column] = value;
		}

		//获取指定位置的值
		Scalar GetValue(unsigned line, unsigned column) const
		{
			return m_values[line][column];
		}

		//赋值
		Matrix& operator = (const Matrix& B)
		{
			//如果两个矩阵的大小不等，则进行大小变换
			if (m_matrixSize != B.Size())
			{
				Invalidate();
				Init(B.Size());
			}

			//进行赋值
			for (unsigned i = 0; i < m_matrixSize; ++i)
			{
				for (unsigned j = 0; j < m_matrixSize; ++j)
				{
					m_values[i][j] = B.m_values[i][j];
				}
			}

			if (B.eigenValues)
			{
				EnableEigenValues();
				memcpy(eigenValues, B.eigenValues, sizeof(Scalar) * m_matrixSize);
			}

			return *this;
		}

		//加法
		Matrix operator + (const Matrix& B) const
		{
			Matrix C = *this;
			C += B;

			return C;
		}

		//加法赋值
		const Matrix& operator += (const Matrix& B)
		{
			assert(B.Size() == m_matrixSize);

			unsigned i,j;
			for (i = 0; i < m_matrixSize; ++i)
			{
				for (j = 0; j < m_matrixSize; ++j)
				{
					m_values[i][j] += B.m_values[i][j];
				}
			}

			return *this;
		}

		//减法
		Matrix operator - (const Matrix& B) const
		{
			Matrix C = *this;
			C -= B;

			return C;
		}

		//减法赋值
		const Matrix& operator -= (const Matrix& B)
		{
			assert(B.Size() == m_matrixSize);

			unsigned i,j;

			for (i = 0; i < m_matrixSize; ++i)
			{
				for (j = 0; j < m_matrixSize; ++j)
				{
					m_values[i][j] += B.m_values[i][j];
				}
			}

			return *this;
		}

		//乘法
		Matrix operator * (const Matrix& B) const
		{
			assert(B.Size() == m_matrixSize);

			Matrix C(m_matrixSize);

			for (unsigned i = 0; i < m_matrixSize; ++i)
			{
				for (unsigned j = 0; j < m_matrixSize; ++j)
				{
					Scalar sum=0.0;

					for (unsigned k = 0; k < m_matrixSize; ++k)
					{
						sum += m_values[i][k]*B.m_values[k][j];
					}

					C.m_values[i][j]=sum;
				}
			}

			return C;
		}

		//与向量相乘
		DCVector3D operator * (const DCVector3D& V) const
		{
			assert(m_matrixSize == 3);

			DCVector3D result;

			Apply(V.u, result.u);

			return result;
		}

		//乘法赋值
		const Matrix& operator *= (const Matrix& B)
		{
			*this = (*this) * B;

			return *this;
		}

		void Apply(Scalar Vec[]) const
		{

			Scalar* V = new Scalar[m_matrixSize];
			Apply(Vec, V);
			memcpy(Vec, V, sizeof(Scalar) * m_matrixSize);

			delete[] V;
		}

		
		void Apply(const Scalar Vec[], Scalar result[]) const
		{
			for (unsigned l=0;l<m_matrixSize;l++)
			{
				Scalar sum=0.0;
				for (unsigned k=0;k<m_matrixSize;k++)
					sum += m_values[l][k] * (Scalar)Vec[k];
				result[l]=sum;
			}
		}

		//转置
		void Transpose()
		{
			for (unsigned i = 0; i < m_matrixSize - 1; ++i)
			{
				for (unsigned j = i + 1; j < m_matrixSize; ++j)
				{
					std::swap(m_values[i][j], m_values[j][i]);
				}
			}
		}

		//返回转置后的矩阵
		Matrix ToTranspose()
		{
			Matrix T(*this);
			T.Transpose();

			return T;
		}

		//清空矩阵，将每个元素设置为0
		void Clear()
		{
			for (unsigned i = 0; i < m_matrixSize; ++i)
			{
				memset(m_values[i], 0, sizeof(Scalar) * m_matrixSize);
			}

			if (eigenValues)
			{
				memset(eigenValues , 0, sizeof(Scalar) * m_matrixSize);
			}
		}

		//返回矩阵的逆
		Matrix Inv() const
		{
			unsigned i,j,k;
			Scalar tmpVal;

			//we create the n by 2n matrix, composed of this matrix and the identity
			Scalar** tempM = new Scalar*[m_matrixSize];
			for (i = 0; i < m_matrixSize; ++i)
			{
				tempM[i] = new Scalar[2 * m_matrixSize];
			}

			for (i=0;i<m_matrixSize;i++)
			{
				for (j=0;j<m_matrixSize;j++)
				{
					tempM[i][j] = m_values[i][j];
					if (i == j)
						tempM[i][j+m_matrixSize] = 1.0;
					else
						tempM[i][j+m_matrixSize] = 0.0;
				}
			}

			//Gauss pivot
			for (i = 0;i < m_matrixSize; ++i)
			{
				//we look for pivot (first non zero element)
				j = i;

				while (tempM[j][i] == 0)
				{
					if (++j >= m_matrixSize)
					{
						//non inversible matrix!
						return Matrix();
					}
				}

				//swap the 2 lines if they are different
				//(we only start by the ith element (as all the others are zero!)
				if (i != j)
				{
					for (k = i; k < 2 * m_matrixSize; ++k)
					{
						std::swap(tempM[i][k], tempM[j][k]);
					}
				}

				//we scale the matrix to make the pivot equal to 1
				if (tempM[i][i] != 1.0)
				{
					tmpVal = tempM[i][i];
					for (j = i; j < 2 * m_matrixSize; ++j)
						tempM[i][j] /= tmpVal;
				}

				//after the pivot value, all elements are set to zero
				for (j = i + 1 ; j < m_matrixSize; ++j)
					if (tempM[j][i] != 0.0)
					{
						tmpVal = tempM[j][i];
						for (k = i; k < 2 * m_matrixSize; ++k)
						{
							tempM[j][k] -= tempM[i][k] * tmpVal;
						}
					}
			}

			//reduction
			for (i = m_matrixSize-1; i > 0; --i)
			{
				for (j = 0; j < i; ++j)
					if (tempM[j][i] != 0.0)
					{
						tmpVal = tempM[j][i];
						for (k = i; k < 2  *m_matrixSize; ++k)
						{
							tempM[j][k] -= tempM[i][k] * tmpVal;
						}
					}
			}

			//result: second part or tempM
			Matrix result(m_matrixSize);
			for (i = 0; i < m_matrixSize; ++i)
			{
				for (j = 0; j < m_matrixSize; ++j)
				{
					result.m_values[i][j] = tempM[i][j + m_matrixSize];
				}
			}

			//we release temp matrix from memory
			for (i = 0;i < m_matrixSize; ++i)
			{
				delete[] tempM[i];
			}
			delete[] tempM;

			return result;
		}

		//将矩阵输出到控制台或文件
		void Print(FILE* out = 0) const
		{
			for (unsigned i = 0; i < m_matrixSize; ++i)
			{
				for (unsigned j = 0; j < m_matrixSize; ++j)
				{
					if (out)
					{
						fprintf(out, "%6.4f ", m_values[i][j]);
					}
					else
					{
						printf("%6.4f ",m_values[i][j]);
					}
				}

				if (out)
				{
					fprintf(out,"\n");
				}
				else
				{
					printf("\n");
				}
			}
		}

		//设置为单位阵
		void ToIdentity()
		{
			//清空数据
			Clear();

			for (unsigned i = 0; i < m_matrixSize; ++i)
			{
				m_values[i][i] = 1.0;
			}
		}

		//缩放矩阵，每个元素等比例缩放
		void Scale(Scalar coef)
		{
			for (unsigned i = 0; i < m_matrixSize; ++i)
			{
				for (unsigned j = 0; j < m_matrixSize; ++j)
				{
					m_values[i][j] *= coef;
				}
			}
		}

		//返回矩阵的秩
		Scalar Trace() const
		{
			Scalar trace = 0;

			for (unsigned i = 0; i < m_matrixSize; ++i)
				trace += m_values[i][i];

			return trace;
		}

		//返回矩阵的行列式
		double ComputeDet() const
		{
			return ComputeSubDet(m_values, m_matrixSize);
		}

		void InitFromQuaternion(const float q[])
		{
			double qd[4]={  (double)q[0],
							(double)q[1],
							(double)q[2],
							(double)q[3]};

			InitFromQuaternion(qd);
		}

		void InitFromQuaternion(const double q[])
		{
			if (m_matrixSize==0)
				Init(3);
			assert(m_matrixSize==3);

			double q00 = q[0]*q[0];
			double q11 = q[1]*q[1];
			double q22 = q[2]*q[2];
			double q33 = q[3]*q[3];
			double q03 = q[0]*q[3];
			double q13 = q[1]*q[3];
			double q23 = q[2]*q[3];
			double q02 = q[0]*q[2];
			double q12 = q[1]*q[2];
			double q01 = q[0]*q[1];

			m_values[0][0] = (Scalar)(q00 + q11 - q22 - q33);
			m_values[1][1] = (Scalar)(q00 - q11 + q22 - q33);
			m_values[2][2] = (Scalar)(q00 - q11 - q22 + q33);
			m_values[0][1] = (Scalar)(2.0*(q12-q03));
			m_values[1][0] = (Scalar)(2.0*(q12+q03));
			m_values[0][2] = (Scalar)(2.0*(q13+q02));
			m_values[2][0] = (Scalar)(2.0*(q13-q02));
			m_values[1][2] = (Scalar)(2.0*(q23-q01));
			m_values[2][1] = (Scalar)(2.0*(q23+q01));
		}

		bool ToQuaternion(double q[/*4*/])
		{
			if (m_matrixSize!=3)
				return false;

			double w,x,y,z;		//quaternion

			double dTrace = (double)m_values[0][0] + (double)m_values[1][1] + (double)m_values[2][2] + 1.0;
			if(dTrace > 1.0e-6)
			{
				double S = 2.0 * sqrt(dTrace);
				x = (m_values[2][1] - m_values[1][2]) / S;
				y = (m_values[0][2] - m_values[2][0]) / S;
				z = (m_values[1][0] - m_values[0][1]) / S;
				w = 0.25 * S;	
			}
			else if((m_values[0][0] > m_values[1][1]) && (m_values[0][0] > m_values[2][2]))
			{
				double S = sqrt(1.0 + m_values[0][0] - m_values[1][1] - m_values[2][2]) * 2.0;
				x = 0.25 * S;
				y = (m_values[1][0] + m_values[0][1]) / S;
				z = (m_values[0][2] + m_values[2][0]) / S;
				w = (m_values[2][1] - m_values[1][2]) / S;
			}
			else if(m_values[1][1] > m_values[2][2])
			{
				double S = sqrt(1.0 + m_values[1][1] - m_values[0][0] - m_values[2][2]) * 2.0;
				x = (m_values[1][0] + m_values[0][1]) / S;
				y = 0.25 * S;
				z = (m_values[2][1] + m_values[1][2]) / S;
				w = (m_values[0][2] - m_values[2][0]) / S;
			}
			else
			{
				double S = sqrt(1.0 + m_values[2][2] - m_values[0][0] - m_values[1][1]) * 2.0;
				x = (m_values[0][2] + m_values[2][0]) / S;
				y = (m_values[2][1] + m_values[1][2]) / S;
				z = 0.25 * S;
				w = (m_values[1][0] - m_values[0][1]) / S;
			}

			
			double len = sqrt( w*w + x*x + y*y + z*z);
			if (len != 0.)
			{	
				q[0] = w/len;
				q[1] = x/len;
				q[2] = y/len;
				q[3] = z/len;

				return true;
			}

			return false;
		}


		Scalar DeltaDeterminant(unsigned column, Scalar* Vec) const
		{
			Matrix mat(m_matrixSize);

			unsigned i,j;
			for (i=0;i<m_matrixSize;i++)
			{
				if (column==i)
				{
					for (j=0;j<m_matrixSize;j++)
					{
						mat.m_values[j][i] = (Scalar)(*Vec);
						Vec++;
					}
				}
				else
				{
					for (j=0;j<m_matrixSize;j++)
						mat.m_values[j][i] = m_values[j][i];
				}
			}

			return mat.ComputeDet();
		}

		
		Matrix ComputeJacobianEigenValuesAndVectors() const
		{
			if (!IsValid())
				return Matrix();

			int j,iq,ip,i,nrot;
			Scalar tresh,theta,tau,t,sm,s,h,g,c,*b,*z,*d;

			Matrix eigenVectors(m_matrixSize);
			eigenVectors.ToIdentity();
			eigenVectors.EnableEigenValues();

			b = new Scalar[m_matrixSize];
			z = new Scalar[m_matrixSize];
			d = eigenVectors.eigenValues;

			for (ip=0;ip<(int)m_matrixSize;ip++)
			{
				b[ip]=d[ip]=m_values[ip][ip]; 
				z[ip]=0.0;
			}

			nrot=0;
			for (i=1;i<=50;i++)
			{
				sm=0.0;
				for (ip=0;ip<(int)m_matrixSize-1;ip++) 
				{
					for (iq=ip+1;iq<(int)m_matrixSize;iq++)
						sm += fabs(m_values[ip][iq]);
				}

				if (sm == 0.0) 
				{
					delete[] z;
					delete[] b;

					//we only need the absolute values of eigenvalues
					for (ip=0;ip<(int)m_matrixSize;ip++)
						d[ip]=fabs(d[ip]);

					return eigenVectors;
				}

				if (i < 4)
					tresh=(Scalar)0.2 * sm/(Scalar)matrixSquareSize; 
				else
					tresh=(Scalar)0.0; //...thereafter.

				for (ip=0;ip<(int)m_matrixSize-1;ip++)
				{
					for (iq=ip+1;iq<(int)m_matrixSize;iq++)
					{
						g=(Scalar)100.0 * fabs(m_values[ip][iq]);
						//After four sweeps, skip the rotation if the off-diagonal element is small.
						if (i > 4 && (float)(fabs(d[ip])+g) == (float)fabs(d[ip])
							&& (float)(fabs(d[iq])+g) == (float)fabs(d[iq]))
						{
							m_values[ip][iq]=0.0;
						}
						else if (fabs(m_values[ip][iq]) > tresh)
						{
							h=d[iq]-d[ip];
							if ((float)(fabs(h)+g) == (float)fabs(h))
								t = m_values[ip][iq]/h; //t = 1/(2heta)
							else
							{
								theta=(Scalar)0.5 * h/m_values[ip][iq]; //Equation (11.1.10).
								t=(Scalar)1.0/(fabs(theta)+sqrt((Scalar)1.0+theta*theta));
								if (theta < 0.0)
									t = -t;
							}

							c=(Scalar)1.0/sqrt((Scalar)1.0+t*t);
							s=t*c;
							tau=s/((Scalar)1.0+c);
							h=t*m_values[ip][iq];
							z[ip] -= h;
							z[iq] += h;
							d[ip] -= h;
							d[iq] += h;
							m_values[ip][iq]=0.0;

							for (j=0;j<=ip-1;j++) //Case of rotations 1 <= j < p.
							{
								ROTATE(m_values,j,ip,j,iq)
							}
							for (j=ip+1;j<=iq-1;j++) //Case of rotations p < j < q.
							{
								ROTATE(m_values,ip,j,j,iq)
							}
							for (j=iq+1;j<(int)m_matrixSize;j++) //Case of rotations q < j <= n.
							{
								ROTATE(m_values,ip,j,iq,j)
							}
							for (j=0;j<(int)m_matrixSize;j++)
							{
								ROTATE(eigenVectors.m_values,j,ip,j,iq)
							}

							++nrot;
						}

					}

				}

				for (ip=0;ip<(int)m_matrixSize;ip++)
				{
					b[ip]+=z[ip];
					d[ip]=b[ip]; //Update d with the sum of tapq,
					z[ip]=0.0; //and reInitialize z.
				}

			}

			delete[] b;
			delete[] z;

			//Too many iterations in routine jacobi!
			return Matrix();
		}

		//! Converts a 3*3 or 4*4 matrix to an OpenGL-style float matrix (float[16])
		void ToGlMatrix(float M16f[]) const
		{
			assert(m_matrixSize == 3 || m_matrixSize == 4);
			memset(M16f,0,sizeof(float)*16);

			unsigned char l,c;

			for (l=0;l<3;l++)
				for (c=0;c<3;c++)
					M16f[l+c*4] = (float)m_values[l][c];

			if (m_matrixSize == 4)
				for (l=0;l<3;l++)
				{
					M16f[12+l] = (float)m_values[3][l];
					M16f[3+l*4] = (float)m_values[l][3];
				}

			M16f[15]=1.0f;
		}

		//! Converts a 3*3 or 4*4 matrix to an OpenGL-style double matrix (double[16])
		void ToGlMatrix(double M16d[]) const
		{
			assert(m_matrixSize == 3 || m_matrixSize == 4);
			memset(M16d,0,sizeof(double)*16);

			unsigned char l,c;

			for (l=0;l<3;l++)
				for (c=0;c<3;c++)
					M16d[l+c*4] = (double)m_values[l][c];

			if (m_matrixSize == 4)
				for (l=0;l<3;l++)
				{
					M16d[12+l] = (double)m_values[3][l];
					M16d[3+l*4] = (double)m_values[l][3];
				}

			M16d[15]=1.0;
		}

		void SortEigenValuesAndVectors()
		{
			if (!eigenValues)
				return;

			unsigned k,j,i;
			for (i=0;i<m_matrixSize-1;i++)
			{
				k=i;
				for (j=i+1;j<m_matrixSize;j++)
					if (eigenValues[j] > eigenValues[k])
						k=j;

				if (k!=i)
				{
					std::swap(eigenValues[i],eigenValues[k]);
					for (j=0;j<m_matrixSize;j++)
						std::swap(m_values[j][i],m_values[j][k]);
				}
			}
		}


	
		Scalar GetMaxEigenValueAndVector(Scalar maxEigenVector[]) const
		{
			assert(eigenValues);

			unsigned i,maxIndex=0;
			for (i=1;i<m_matrixSize;++i)
				if (eigenValues[i]>eigenValues[maxIndex])
					maxIndex=i;

			return GetEigenValueAndVector(maxIndex,maxEigenVector);
		}

		Scalar GetMinEigenValueAndVector(Scalar minEigenVector[]) const
		{
			assert(eigenValues);

			unsigned i,minIndex=0;
			for (i=1;i<m_matrixSize;++i)
				if (eigenValues[i]<eigenValues[minIndex])
					minIndex=i;

			return GetEigenValueAndVector(minIndex,minEigenVector);
		}

		
		Scalar GetEigenValueAndVector(unsigned index, Scalar* eigenVector = 0) const
		{
			assert(eigenValues);
			assert(index < m_matrixSize);

			if (eigenVector)
				for (unsigned i=0;i<m_matrixSize;++i)
					eigenVector[i]=m_values[i][index];

			return eigenValues[index];
		}

	protected:

		
		void Init(unsigned size)
		{
			m_matrixSize = size;
			matrixSquareSize = m_matrixSize*m_matrixSize;

			m_values = 0;
			eigenValues=0;

			if (size>0)
			{
				m_values = new Scalar*[m_matrixSize];
				for (unsigned i=0;i<m_matrixSize;i++)
				{
					m_values[i] = new Scalar[m_matrixSize];
					memset(m_values[i],0,sizeof(Scalar)*m_matrixSize);
				}
			}
		}

		void EnableEigenValues()
		{
			if (!eigenValues && m_matrixSize>0)
				eigenValues = new Scalar[m_matrixSize];
		}

		double ComputeSubDet(Scalar** mat, unsigned matSize) const
		{
			double subDet = 0;

			if (matSize == 2)
			{
				return (double)(mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]);
			}
			else
			{
				unsigned i,k,line;
				Scalar** subMat = new Scalar*[matSize-1];
				double sign = 1.0;

				for (line=0;line<matSize;line++)
				{
					k=0;
					for (i=0;i<matSize;i++)
						if (i!=line)
							subMat[k++] = mat[i]+1;

					subDet += (double)mat[line][0] * ComputeSubDet(subMat,matSize-1) * sign;
					sign = -sign;
				}

				delete[] subMat;
			}

			return subDet;
		}

		unsigned m_matrixSize;

		unsigned matrixSquareSize;

		Scalar* eigenValues;
	};

	typedef Matrix<PointCoordinateType> SquareMatrix;

	typedef Matrix<float> SquareMatrixf;

	typedef Matrix<double> SquareMatrixd;

}

#endif
