#include "Viewport.h"

using namespace DCView;

//-----------------------------------------------------------------------------
// Viewport
//-----------------------------------------------------------------------------
Viewport::Viewport()
{
	mX = 0;
	mY = 0;
	mWidth = 0;
	mHeight = 0;
	mClearColor = fvec4(0,0,0,1);
}
//-----------------------------------------------------------------------------
Viewport::Viewport(int x, int y, int w, int h)
{
	mX = x;
	mY = y;
	mWidth  = w;
	mHeight = h;
	mClearColor = fvec4(0.8f,0,0.1f,1);
}
//-----------------------------------------------------------------------------
void Viewport::activate() const
{
	// viewport
	int x = mX;
	int y = mY;
	int w = mWidth;
	int h = mHeight;

	if (w < 1) w = 1;
	if (h < 1) h = 1;

	//glViewport(x, y, w, h);
}
//-----------------------------------------------------------------------------
bool Viewport::isPointInside(int x, int y, int framebuffer_height) const
{
  // set x/y relative to the viewport
  x -= this->x();
  y -= framebuffer_height - 1 - (this->y() + height() -1);

  // check that the click is in the viewport

  if (x<0 || y<0 || x>=this->width() || y>=this->height())
    return false;
  else
    return true;
}
//-----------------------------------------------------------------------------
