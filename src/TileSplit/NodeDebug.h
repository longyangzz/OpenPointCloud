#ifndef NODEDEBUG_H
#define NODEDEBUG_H

#include "Common.h"
#include <vector>

namespace DC
{
	namespace Pyra
	{

		class NodeDebug
		{
		public:
			NodeDebug();
			~NodeDebug();

			// This adds a line to our list of debug lines
			void AddDebugLine(Point3D vPoint1, Point3D vPoint2);

			// This adds a rectangle with a given center, width, height and depth to our list
			void AddDebugRectangle(Point3D minCorner, Point3D maxCorner);

			// This renders all of the lines
			void RenderDebugLines();		

			// This clears all of the debug lines
			void Clear();					

		private:

			// This is the vector list of all of our lines
			std::vector<Point3D> m_vLines;	
		};

	}
}

#endif // NODEDEBUG_H
