#ifndef POINTSGEOMETRY_H
#define POINTSGEOMETRY_H

#include "Geometry.h"

namespace DC
{
	namespace Pyra
	{
		class PointsGeometry : public Geometry
		{
		public:
			PointsGeometry(Quadnode* node);
			~PointsGeometry();

			void Draw();
		private:

		};
	}
}



#endif // POINTSGEOMETRY_H
