#ifndef NORMALVECTORS_H
#define NORMALVECTORS_H

#include "DCCore/BasicTypes.h"
#include <vector>
#include <limits>
#include "DcGp/GpBasicTypes.h"

//! '1' as a PointCoordinateType value
/** To avoid compiler warnings about 'possible loss of data' **/
const PointCoordinateType PC_ONE = static_cast<PointCoordinateType>(1.0);

//! 'NaN' as a PointCoordinateType value
/** \warning: handle with care! **/
const PointCoordinateType PC_NAN = std::numeric_limits<PointCoordinateType>::quiet_NaN();

class NormalVectors
{
public:
	NormalVectors();
	~NormalVectors();

	static NormalVectors* GetUniqueInstance();

	static const unsigned NORMALS_QUANTIZE_LEVEL =	6;

	//! Returns the precomputed normal corresponding to a given compressed index
	inline const DCVector3D& GetNormal(unsigned normIndex) const { return m_theNormalVectors[normIndex]; }

	//! Returns the compressed index corresponding to a normal vector
	static inline NormalType GetNormIndex(const PointCoordinateType N[]) { return static_cast<NormalType>( Quant_quantize_normal(N,NORMALS_QUANTIZE_LEVEL) ); }
	//! Returns the compressed index corresponding to a normal vector (shortcut)
	static inline NormalType GetNormIndex(const DCVector3D& N) { return GetNormIndex(N.u); }
private:
	//! Inits internal structures
	bool Init(unsigned quantizeLevel);

	//! Decompression algorithm
	static void Quant_dequantize_normal(unsigned q, unsigned level, PointCoordinateType* res);

	//! Compression algorithm
	static unsigned Quant_quantize_normal(const PointCoordinateType* n, unsigned level);

	static NormalVectors* s_instance;

	std::vector<DCVector3D> m_theNormalVectors;
};

#endif // NORMALVECTORS_H
