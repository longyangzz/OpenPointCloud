#include "DCView/SymbolLibrary.h"
using namespace DCView;

//DCCore
#include "DCCore/DCGL.h"

//ªÊ÷∆‘≤√Ê
void DCView::DrawDisc(const Point_3& ptCenter, float raduis)
{
	const unsigned SEGMENTS = 360;
	const float RADIUS = raduis;
	GLfloat vertex[4] = {0.0f, 0.0f, 0.0f, 1.0f}; 

	const GLfloat delta_angle = 2.0 * 3.1415926 / SEGMENTS;  
	glBegin(GL_TRIANGLE_FAN);  

	glVertex4fv(vertex);  

	//draw the vertex on the contour of the circle   
	for(int i = 0; i < SEGMENTS ; i++)  
	{  
		vertex[0] = std::cos(delta_angle*i) * RADIUS + ptCenter.x;  
		vertex[1] = std::sin(delta_angle*i) * RADIUS + ptCenter.y;  
		vertex[2] = ptCenter.z;   
		glVertex4fv(vertex);  
	}  

	vertex[0] = 1.0 * RADIUS + ptCenter.x;  
	vertex[1] = 0.0 * RADIUS + ptCenter.y;  
	vertex[2] = ptCenter.z;  
	glVertex4fv(vertex); 

	glEnd(); 
	
}