#include "NodeDebug.h"
#include "DCCore/DCGL.h"

using namespace DC::Pyra;

NodeDebug::NodeDebug()
{

}

NodeDebug::~NodeDebug()
{

}

void NodeDebug::RenderDebugLines()				// This renders all of the lines
{
	//glDisable(GL_LIGHTING);					// Turn OFF lighting so the debug lines are bright yellow

	glBegin(GL_LINES);						// Start rendering lines

	glColor3ub(255, 255, 0);			// Turn the lines yellow

	// Go through the whole list of lines stored in the vector m_vLines.
	for(unsigned int i = 0; i < m_vLines.size(); i++)
	{
		// Pass in the current point to be rendered as part of a line
		glVertex3f(m_vLines[i].x, m_vLines[i].y, m_vLines[i].z);
	}	

	glEnd();								// Stop rendering lines
	//glEnable(GL_LIGHTING);	
}

///////////////////////////////// ADD DEBUG LINE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This adds a debug LINE to the stack of lines
/////
///////////////////////////////// ADD DEBUG LINE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void NodeDebug::AddDebugLine(Point3D vPoint1, Point3D vPoint2)
{
	// Add the 2 points that make up the line into our line list.
	m_vLines.push_back(vPoint1);
	m_vLines.push_back(vPoint2);
}


///////////////////////////////// ADD DEBUG RECTANGLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This adds a debug RECTANGLE to the stack of lines
/////
///////////////////////////////// ADD DEBUG RECTANGLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void NodeDebug::AddDebugRectangle(Point3D minCorner, Point3D maxCorner)
{
	// Below we create all the 8 points so it will be easier to input the lines
	// of the cube.  With the dimensions we calculate the points.
	Point3D vTopLeftFront( minCorner.x, maxCorner.y, maxCorner.z );
	Point3D vTopLeftBack(  minCorner.x, maxCorner.y, minCorner.z);
	Point3D vTopRightBack( maxCorner.x, maxCorner.y, minCorner.z);
	Point3D vTopRightFront(maxCorner.x, maxCorner.y, maxCorner.z);

	Point3D vBottom_LeftFront( minCorner.x, minCorner.y, maxCorner.z);
	Point3D vBottom_LeftBack(  minCorner.x, minCorner.y, minCorner.z);
	Point3D vBottomRightBack( maxCorner.x, minCorner.y, minCorner.z);
	Point3D vBottomRightFront(maxCorner.x, minCorner.y, maxCorner.z);

	////////// TOP LINES ////////// 

	// Store the top front line of the box
	m_vLines.push_back(vTopLeftFront);		m_vLines.push_back(vTopRightFront);

	// Store the top back line of the box
	m_vLines.push_back(vTopLeftBack);  		m_vLines.push_back(vTopRightBack);

	// Store the top left line of the box
	m_vLines.push_back(vTopLeftFront);		m_vLines.push_back(vTopLeftBack);

	// Store the top right line of the box
	m_vLines.push_back(vTopRightFront);		m_vLines.push_back(vTopRightBack);

	////////// BOTTOM LINES ////////// 

	// Store the bottom front line of the box
	m_vLines.push_back(vBottom_LeftFront);	m_vLines.push_back(vBottomRightFront);

	// Store the bottom back line of the box
	m_vLines.push_back(vBottom_LeftBack);	m_vLines.push_back(vBottomRightBack);

	// Store the bottom left line of the box
	m_vLines.push_back(vBottom_LeftFront);	m_vLines.push_back(vBottom_LeftBack);

	// Store the bottom right line of the box
	m_vLines.push_back(vBottomRightFront);	m_vLines.push_back(vBottomRightBack);

	////////// SIDE LINES ////////// 

	// Store the bottom front line of the box
	m_vLines.push_back(vTopLeftFront);		m_vLines.push_back(vBottom_LeftFront);

	// Store the back left line of the box
	m_vLines.push_back(vTopLeftBack);		m_vLines.push_back(vBottom_LeftBack);

	// Store the front right line of the box
	m_vLines.push_back(vTopRightBack);		m_vLines.push_back(vBottomRightBack);

	// Store the front left line of the box
	m_vLines.push_back(vTopRightFront);		m_vLines.push_back(vBottomRightFront);
}


///////////////////////////////// CLEAR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This clears all of the debug lines
/////
///////////////////////////////// CLEAR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void NodeDebug::Clear()						
{
	// Destroy the list using the standard vector clear() function
	m_vLines.clear();
}
