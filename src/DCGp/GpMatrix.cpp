//System
#include <math.h>
#include <string.h>
#include <assert.h>

//Qt
#include "QString"

//Common
#include "DCCore/Const.h"

#include "DCGp/GpMatrix.h"

//Matrix element shortcuts in (line,column) order
#define R11 m_mat[0]
#define R21 m_mat[1]
#define R31 m_mat[2]
#define R41 m_mat[3]

#define R12 m_mat[4]
#define R22 m_mat[5]
#define R32 m_mat[6]
#define R42 m_mat[7]

#define R13 m_mat[8]
#define R23 m_mat[9]
#define R33 m_mat[10]
#define R43 m_mat[11]

#define R14 m_mat[12]
#define R24 m_mat[13]
#define R34 m_mat[14]
#define R44 m_mat[15]

DcGp::DcGpMatrix::DcGpMatrix()
{
	ToIdentity();
}

DcGp::DcGpMatrix::DcGpMatrix(const float* mat16)
{
    memcpy(m_mat, mat16, sizeof(float) * OPENGL_MATRIX_SIZE);
}

DcGp::DcGpMatrix::DcGpMatrix(const DcGpMatrix& mat)
{
	memcpy(m_mat, mat.m_mat, sizeof(float)*OPENGL_MATRIX_SIZE);
}

DcGp::DcGpMatrix::DcGpMatrix(const DCVector3D& X, const DCVector3D& Y, const DCVector3D& Z, const DCVector3D& T)
{
	R11 = X[0];
	R21 = X[1];
	R31 = X[2];
	R41 = 0.0f;

	R12 = Y[0];
	R22 = Y[1];
	R32 = Y[2];
	R42 = 0.0f;

	R13 = Z[0];
	R23 = Z[1];
	R33 = Z[2];
	R43 = 0.0f;

	R14 = T[0];
	R24 = T[1];
	R34 = T[2];
	R44 = 1.0f;
}

DcGp::DcGpMatrix::DcGpMatrix(const DCCore::SquareMatrix& R, const DCVector3D& T)
{
    ToIdentity();

	if (R.Size()==3)
	{
		//we copy each column
		float* mat = m_mat;
		for (unsigned j = 0; j < 3; ++j)
		{
			*mat++ = R.m_values[0][j];
			*mat++ = R.m_values[1][j];
			*mat++ = R.m_values[2][j];
			mat++;
		}
	}

    *this += T;
}

DcGp::DcGpMatrix::DcGpMatrix(const DCCore::SquareMatrix& R, const DCVector3D& T, double S)
{
    ToIdentity();

    if (R.Size()==3)
    {
        //we copy each column
        float* mat = m_mat;
        for (unsigned j = 0; j < 3; ++j)
        {
            *mat++ = R.m_values[0][j] * S;
            *mat++ = R.m_values[1][j] * S;
            *mat++ = R.m_values[2][j] * S;
            mat++;
        }
    }

    *this += T;
}

DcGp::DcGpMatrix::DcGpMatrix(const DCCore::SquareMatrix& R, const DCVector3D& T, const DCVector3D& rotCenter)
{
    *this = DcGpMatrix(R, T);
    ShiftRotationCenter(rotCenter);
}

void DcGp::DcGpMatrix::ToZero()
{
	memset(m_mat, 0, OPENGL_MATRIX_SIZE * sizeof(float));
}

void DcGp::DcGpMatrix::ToIdentity()
{
	memset(m_mat, 0, OPENGL_MATRIX_SIZE * sizeof(float));
	R11 = R22 = R33 = R44 = 1.0;
}

bool DcGp::DcGpMatrix::ToAsciiFile(const char* filename) const
{
	FILE* fp = fopen(filename,"wt");
	if (!fp)
		return false;

	const float* mat = m_mat;
	for (int i = 0; i < 4; ++i)
	{
		if (fprintf(fp, "%f %f %f %f\n", mat[0], mat[4], mat[8], mat[12])<0)
		{
			fclose(fp);
			return false;
		}
		++mat;
	}
	fclose(fp);

	return true;
}

bool DcGp::DcGpMatrix::FomAsciiFile(const char* filename)
{
	FILE* fp = fopen(filename, "rt");
	if (!fp)
		return false;

	float* mat = m_mat;
	for (int i=0;i<4;++i)
	{
		if (fscanf(fp, "%f %f %f %f\n", mat, mat + 4, mat + 8, mat + 12) < 4)
		{
			fclose(fp);
			return false;
		}
		++mat;
	}
	fclose(fp);

	return true;
}

void DcGp::DcGpMatrix::InitFromParameters(float alpha, const DCVector3D& axis3D, const DCVector3D& t3D)
{
	float cosw = cos(alpha);
	float sinw = sin(alpha);
	float inv_cosw = 1.0f - cosw;

	float l1 = axis3D.x;
	float l2 = axis3D.y;
	float l3 = axis3D.z;

	float l1_inv_cosw = l1 * inv_cosw;
	float l3_inv_cosw = l3 * inv_cosw;

	//1st column
	R11 = cosw + l1 * l1_inv_cosw;
	R21 = l2 * l1_inv_cosw + l3 * sinw;
	R31 = l3 * l1_inv_cosw - l2 * sinw;

	//2nd column
	R12 = l2 * l1_inv_cosw-l3 * sinw;
	R22 = cosw + l2 * l2 * inv_cosw;
	R32 = l2 * l3_inv_cosw + l1 * sinw;

	//3rd column
	R13 = l3 * l1_inv_cosw + l2 * sinw;
	R23 = l2 * l3_inv_cosw - l1 * sinw;
	R33 = cosw + l3 * l3_inv_cosw;

	//4th column
	R14 = t3D.x;
	R24 = t3D.y;
	R34 = t3D.z;
}

void DcGp::DcGpMatrix::GetParameters(float& alpha, DCVector3D& axis3D, DCVector3D& t3D) const
{
	float trace = R11 + R22 + R33;
	trace = 0.5f * (trace - 1.0f);
	if (fabs(trace) < 1.0f)
	{
		alpha = acos(trace);
		if (alpha > M_PI_2)
			alpha -= M_PI;
	}
	else
	{
		alpha = 0.0f;
	}

	axis3D.x = (R32 - R23);
	axis3D.y = (R13 - R31);
	axis3D.z = (R21 - R12);
	axis3D.Normalize();

	t3D.x = R14;
	t3D.y = R24;
	t3D.z = R34;
}

void DcGp::DcGpMatrix::InitFromParameters(float phi, float theta, float psi, const DCVector3D& t3D)
{
	float cos_phi = cos(phi);
	float cos_theta = cos(theta);
	float cos_psi = cos(psi);

	float sin_phi = sin(phi);
	float sin_theta = sin(theta);
	float sin_psi = sin(psi);

	//1st column
	R11 = cos_theta * cos_phi;
	R21 = cos_theta * sin_phi;
	R31 = -sin_theta;

	//2nd column
	R12 = sin_psi * sin_theta * cos_phi - cos_psi * sin_phi;
	R22 = sin_psi * sin_theta * sin_phi + cos_psi * cos_phi;
	R32 = sin_psi * cos_theta;

	//3rd column
	R13 = cos_psi * sin_theta * cos_phi + sin_psi * sin_phi;
	R23 = cos_psi * sin_theta * sin_phi - sin_psi * cos_phi;
	R33 = cos_psi * cos_theta;

	//4th column
	R14 = t3D.x;
	R24 = t3D.y;
	R34 = t3D.z;
}

void DcGp::DcGpMatrix::GetParameters(float &phi, float &theta, float &psi, DCVector3D& t3D) const
{
	if (fabs(R31)!=1.0)
	{
		theta = -asin(R31);
		float cos_theta = cos(theta);
		psi = atan2(R32 / cos_theta, R33 / cos_theta);
		phi = atan2(R21 / cos_theta, R11 / cos_theta);

		//Other solution
		/*theta = M_PI+asin(R31);
		UL_SCALAR cos_theta = cos(theta);
		psi = atan2(R32/cos_theta,R33/cos_theta);
		phi = atan2(R21/cos_theta,R11/cos_theta);
		//*/
	}
	else
	{
		phi = 0.0f;
		float sign = (R31 == -1.0f ? 1.0f : -1.0f);
		theta = sign * M_PI_2;
		psi = sign * atan2(R12, R13);
	}

	t3D.x = R14;
	t3D.y = R24;
	t3D.z = R34;
}

void DcGp::DcGpMatrix::ClearTranslation()
{
	R14 = R24 = R34 = 0;
}

void DcGp::DcGpMatrix::SetTranslation(const DCVector3D& T)
{
	R14 = T.x;
	R24 = T.y;
	R34 = T.z;
}

DcGp::DcGpMatrix DcGp::DcGpMatrix::operator * (const DcGpMatrix& M) const
{
	DcGpMatrix result;

	const float* A = m_mat;
	const float* B = M.m_mat;
	float* C = result.m_mat;

	for (int j = 0; j < 4; ++j)
	{
		for (int i = 0; i < 4; ++i)
			*C++ = A[i] * B[0] + A[i + 4] * B[1] + A[i + 8] * B[2] + A[i + 12] * B[3];

		B += 4;
	}

	return result;
}

DcGp::DcGpMatrix& DcGp::DcGpMatrix::operator *= (const DcGpMatrix& M)
{
	DcGpMatrix temp = (*this) * M;
	(*this) = temp;

	return (*this);
}

DcGp::DcGpMatrix& DcGp::DcGpMatrix::operator += (const DCVector3D& T)
{
	R14 += T.x;
	R24 += T.y;
	R34 += T.z;

	return (*this);
}

DcGp::DcGpMatrix& DcGp::DcGpMatrix::operator -= (const DCVector3D& T)
{
	R14 -= T.x;
	R24 -= T.y;
	R34 -= T.z;

	return (*this);
}

void DcGp::DcGpMatrix::ShiftRotationCenter(const DCVector3D& vec)
{
    //R(X-vec)+T+vec = R(X)+T + vec-R(vec)
    DCVector3D Rvec = vec;
    ApplyRotation(Rvec.u);
    *this += (vec - Rvec);
}

void DcGp::DcGpMatrix::Transpose()
{
	std::swap(R21, R12);
	std::swap(R31, R13);
	std::swap(R41, R14);
	std::swap(R32, R23);
	std::swap(R42, R24);
	std::swap(R43, R34);
}

DcGp::DcGpMatrix DcGp::DcGpMatrix::Transposed() const
{
	DcGpMatrix t(*this);
	t.Transpose();
	
	return t;
}

void DcGp::DcGpMatrix::Invert()
{
	//inverse scale as well!
	PointCoordinateType s2 = DCVector3D(m_mat).Normal2(); //we use the first column == X (its norm should be 1 for an 'unscaled' matrix ;)

	//we invert rotation
	std::swap(R21, R12);
	std::swap(R31, R13);
	std::swap(R32, R23);

	if (s2 != 0.0f && s2 != 1.0f)
	{
		R11 /= s2; R12 /= s2; R13 /= s2;
		R21 /= s2; R22 /= s2; R23 /= s2;
		R31 /= s2; R32 /= s2; R33 /= s2;
	}

	//eventually we invert translation
	ApplyRotation(m_mat + 12);
	R14 = -R14;
	R24 = -R24;
	R34 = -R34;

 }

DcGp::DcGpMatrix DcGp::DcGpMatrix::Inverse() const
{
	DcGpMatrix result = *this;
	result.Invert();

	return result;
}

DcGp::DcGpMatrix DcGp::DcGpMatrix::Interpolate(float coef, const DcGpMatrix& glMat1, const DcGpMatrix& glMat2)
{
	//we compute the transformation matrix between glMat1 and glMat2
	DcGpMatrix invTrans1 = glMat1.Inverse();
	DcGpMatrix m12 = invTrans1 * glMat2;

    DCVector3D axis,tr;
	float alpha;
	m12.GetParameters(alpha, axis, tr);

	//we only have to interpolate the angle value
	alpha *= coef;
	//and the translation
	tr *= coef;

	//we build-up the resulting matrix
	m12.InitFromParameters(alpha, axis, tr);

	//eventually we build-up resulting transformation
	return glMat1 * m12;
}

void DcGp::DcGpMatrix::Scale(float coef)
{
	for (unsigned i = 0;i < OPENGL_MATRIX_SIZE; ++i)
	{
		m_mat[i] *= coef;
	}
}

void DcGp::DcGpMatrix::ScaleLine(unsigned lineIndex, float coef)
{
	assert(lineIndex<4);

	m_mat[lineIndex] *= coef;
	m_mat[4 + lineIndex] *= coef;
	m_mat[8 + lineIndex] *= coef;
	m_mat[12 + lineIndex] *= coef;
}

void DcGp::DcGpMatrix::ScaleColumn(unsigned colIndex, float coef)
{
	assert(colIndex<4);

	float* col = GetColumn(colIndex);
	col[0] *= coef;
	col[1] *= coef;
	col[2] *= coef;
	col[3] *= coef;
}

DcGp::DcGpMatrix DcGp::DcGpMatrix::FromToRotation(const DCVector3D& from, const DCVector3D& to)
{
	float e = from.Dot(to);
	float f = (e < 0 ? -e : e);
	DcGpMatrix result;
	float* mat = result.Data();

	if (f > 1.0 - ZERO_TOLERANCE)     //"from" and "to"-vector almost parallel
	{
		DCVector3D x;       // vector most nearly orthogonal to "from"
		x.x = (from.x > 0 ? from.x : -from.x);
		x.y = (from.y > 0 ? from.y : -from.y);
		x.z = (from.z > 0 ? from.z : -from.z);

		if (x.x < x.y)
		{
			if (x.x < x.z)
			{
				x.x = 1.0f; x.y = x.z = 0;
			}
			else
			{
				x.z = 1.0f; x.x = x.y = 0;
			}
		}
		else
		{
			if (x.y < x.z)
			{
				x.y = 1.0f; x.x = x.z = 0;
			}
			else
			{
				x.z = 1.0f; x.x = x.y = 0;
			}
		}

		DCVector3D u(x.x - from.x, x.y - from.y, x.z - from.z);
		DCVector3D v(x.x - to.x, x.y - to.y, x.z - to.z);

		float c1 = 2.0f / u.Dot(u);
		float c2 = 2.0f / v.Dot(v);
		float c3 = c1 * c2  * u.Dot(v);

		for (unsigned i = 0; i < 3; i++)
		{
			for (unsigned j = 0; j < 3; j++)
			{
				mat[i * 4 + j]=  c3 * v.u[i] * u.u[j]
						   - c2 * v.u[i] * v.u[j]
						   - c1 * u.u[i] * u.u[j];
			}
			mat[i * 4 + i] += 1.0f;
		}
	}
	else  // the most common case, unless "from"="to", or "from"=-"to"
	{
		//hand optimized version (9 mults less)
		DCVector3D v = from.Cross(to);
		float h = 1.0f / (1.0f + e);
		float hvx = h * v.x;
		float hvz = h * v.z;
		float hvxy = hvx * v.y;
		float hvxz = hvx * v.z;
		float hvyz = hvz * v.y;

		mat[0] = e + hvx * v.x;
		mat[1] = hvxy - v.z;
		mat[2] = hvxz + v.y;

		mat[4] = hvxy + v.z;
		mat[5] = e + h * v.y * v.y;
		mat[6] = hvyz - v.x;

		mat[8] = hvxz - v.y;
		mat[9] = hvyz + v.x;
		mat[10] = e + hvz * v.z;
	}

	return result;
}

// bool DcGpMatrix::ToFile(QFile& out) const
// {
// 	assert(out.isOpen() && (out.openMode() & QIODevice::WriteOnly));
// 
// 	//data (dataVersion>=20)
// 	if (out.write((const char*)m_mat,Sizeof(double)*OPENGL_MATRIX_SIZE)<0)
// 		return WriteError();
// 
// 	return true;
// }

// bool DcGpMatrix::FromFile(QFile& in, short dataVersion)
// {
// 	assert(in.isOpen() && (in.openMode() & QIODevice::ReadOnly));
// 
// 	if (dataVersion<20)
// 		return CorruptError();
// 
// 	//data (dataVersion>=20)
// 	if (in.read((char*)m_mat,Sizeof(double)*OPENGL_MATRIX_SIZE)<0)
// 		return ReadError();
// 
// 	return true;
// }

DcGp::DcGpMatrix DcGp::DcGpMatrix::FromQuaternion(const float q[])
{
	assert(q);

	DcGpMatrix rotMat;
	float* mat = rotMat.Data();

	//diagonal
	{
		float q00 = q[0] * q[0];
		float q11 = q[1] * q[1];
		float q22 = q[2] * q[2];
		float q33 = q[3] * q[3];
		mat[0]	= q00 + q11 - q22 - q33;
		mat[5]	= q00 - q11 + q22 - q33;
		mat[10]	= q00 - q11 - q22 + q33;
		mat[15]	= 1.0f;
	}

	//non-diagonal elements
	{
		float q03 = q[0] * q[3];
		float q13 = q[1] * q[3];
		float q23 = q[2] * q[3];
		float q02 = q[0] * q[2];
		float q12 = q[1] * q[2];
		float q01 = q[0] * q[1];

		mat[1]	= 2.0f * (q12 + q03);
		mat[2]	= 2.0f * (q13 - q02);

		mat[4]	= 2.0f * (q12 - q03);
		mat[6]	= 2.0f * (q23 + q01);

		mat[8]	= 2.0f * (q13 + q02);
		mat[9]	= 2.0f * (q23 - q01);
	}

	return rotMat;
}

#define R11 m_mat[0]
#define R21 m_mat[1]
#define R31 m_mat[2]
#define R41 m_mat[3]

#define R12 m_mat[4]
#define R22 m_mat[5]
#define R32 m_mat[6]
#define R42 m_mat[7]

#define R13 m_mat[8]
#define R23 m_mat[9]
#define R33 m_mat[10]
#define R43 m_mat[11]


DcGp::DcGpMatrix DcGp::DcGpMatrix::XRotation() const
{
	DcGpMatrix newRotMat;
	newRotMat.ToIdentity();

	//we use a specific Euler angles convention here
	if (R13 >= 1.0f)
	{
		//simpler/faster to ignore this (very) specific case!
		return newRotMat;
	}
	float phi = -asin(R13);
	float cos_phi = cos(phi);
	float theta = atan2(R23 / cos_phi, R33 / cos_phi);

	newRotMat.R22 = newRotMat.R33 = cos(theta);
	newRotMat.R32 = newRotMat.R23 = sin(theta);
	newRotMat.R23 *= -1.0f;

	newRotMat.SetTranslation(GetTranslation());

	return newRotMat;
}

DcGp::DcGpMatrix DcGp::DcGpMatrix::YRotation() const
{
	DcGpMatrix newRotMat;
	newRotMat.ToIdentity();

	//we use a specific Euler angles convention here
	if (R32 >= 1.0f)
	{
		//simpler/faster to ignore this (very) specific case!
		return newRotMat;
	}
	float theta = asin(R32);
	float cos_theta = cos(theta);
	float phi = atan2(-R31 / cos_theta, R33 / cos_theta);

	newRotMat.R11 = newRotMat.R33 = cos(phi);
	newRotMat.R31 = newRotMat.R13 = sin(phi);
	newRotMat.R31 *= -1.0f;

	newRotMat.SetTranslation(GetTranslation());

	return newRotMat;
}

DcGp::DcGpMatrix DcGp::DcGpMatrix::ZRotation() const
{
	//we can use the standard Euler angles convention here
	float phi,theta,psi;
	DCVector3D T;
	GetParameters(phi,theta,psi,T);
	assert(T.Normal2() == 0);

	DcGpMatrix newRotMat;
	newRotMat.InitFromParameters(phi,0,0,T);

	return newRotMat;
}

QString DcGp::DcGpMatrix::ToString(int precision/*=12*/, QChar separator/*=' '*/) const
{
	QString str;
	for (unsigned l=0; l<4; ++l) //lines
	{
		for (unsigned c=0; c<4; ++c) //columns
		{
			str.append(QString::number(m_mat[c * 4 + l], 'f', precision));
			if (c != 3)
				str.append(separator);
		}
		if (l != 3)
			str.append("\n");
	}
	return str;
}
