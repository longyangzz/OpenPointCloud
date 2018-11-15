//----------------------------------------------------------------------
//author:wly
//time:
//dsc:·â×°glviewport
//
//----------------------------------------------------------------------

#ifndef DCVIEW_VIEWPORT
#define DCVIEW_VIEWPORT

#include "ViewDLL.h"

#include "QRect"

#include "DCCore/BasicTypes.h"

namespace DCView
{
  //-----------------------------------------------------------------------------
  // Viewport
  //-----------------------------------------------------------------------------
  class DC_VIEW_API Viewport
  {
  public:
    Viewport();
    Viewport(int x, int y, int w, int h);

    void activate() const;

    bool null() { return mWidth == 0 || mHeight == 0; }

    void set(int x, int y, int w, int h) { mX = x; mY = y; mWidth = w; mHeight = h; }
    void setX(int x) { mX = x; }
    int x() const { return mX; }
    void setY(int y) { mY = y; }
    int y() const { return mY; }
    void setWidth(int width) { mWidth = width; }
    int width() const { return mWidth; }
    void setHeight(int height) { mHeight = height; }
    int height() const { return mHeight; }
    fvec2 center() const { return fvec2(mX + mWidth / 2.0f, mY + mHeight / 2.0f); }

    /**
     * Returns the rectangular area that defines the viewport computed as RectI(x(), y(), x()+width()-1, y()+height()-1).
     */
    QRect rect() const { return QRect(x(),y(),width(),height()); }

    void setClearColor(float r, float g, float b, float a) { mClearColor = fvec4(r,g,b,a); }
    void setClearColor(const fvec4& color) { mClearColor = color; }
    const fvec4& clearColor() const { return mClearColor; }

    /**
     * Returns true if the given point is inside the Viewport
    */
    bool isPointInside(int x, int y, int framebuffer_height) const;

  protected:
    fvec4 mClearColor;

    int mX;
    int mY;
    int mWidth;
    int mHeight;
  };
}

#endif
