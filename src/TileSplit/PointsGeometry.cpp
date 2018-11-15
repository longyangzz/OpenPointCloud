//gp
#include "DCCore/DCGL.h"
#include "DCGp/GpBasicTypes.h"

#include "Quadnode.h"
#include "PointsGeometry.h"

using namespace DC::Pyra;

extern double loadnum;

PointsGeometry::PointsGeometry(Quadnode* node)
	: Geometry(node)
{

}

PointsGeometry::~PointsGeometry()
{

}

void PointsGeometry::Draw()
{
	//»æÖÆ
	if (GetNode()->GetPointsSize())
	{
		loadnum++;
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 
			1 * 3 * sizeof(PointCoordinateType), 
			&(GetNode()->GetPoints()[0][0]));

		glColorPointer(3, GL_UNSIGNED_BYTE, 3*sizeof(ColorType), &(GetNode()->GetRGBColors()[0][0]));

		glDrawArrays(GL_POINTS, 0, GetNode()->GetPointsSize());
	}
}
