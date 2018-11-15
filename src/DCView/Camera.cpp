#include "Camera.h"

#undef near
#undef far

using namespace DCView;

//-----------------------------------------------------------------------------
// Camera
//-----------------------------------------------------------------------------
Camera::Camera()
{
  mFOV = 60.0;
  mNearPlane = (float)0.05;
  mFarPlane  = (float)10000.0;
  mLeft = mRight = mTop = mBottom = -1;
  mViewport = new Viewport();

  mProjectionMatrix  = DCCore::mat4::getPerspective(fov(), 640.0f/480.0f, nearPlane(), farPlane());
  mProjectionType = PMT_PerspectiveProjection;
}

Camera::~Camera()
{
	delete mViewport;
	mViewport = nullptr;
}
//-----------------------------------------------------------------------------
void Camera::applyModelViewMatrix(const DCCore::mat4& model_matrix) const
{
  /* some OpenGL drivers (ATI) require this instead of the more general (and mathematically correct) viewMatrix() */
  DCCore::mat4 viewm = viewMatrix();
  viewm.e(3,0) = 0.0;
  viewm.e(3,1) = 0.0;
  viewm.e(3,2) = 0.0;
  viewm.e(3,3) = 1.0;

  //glMatrixMode(GL_MODELVIEW);
#if 0
  VL_glLoadMatrix( viewm.ptr() );
  VL_glMultMatrix( matrix.ptr() );
#elif 0
  viewm = viewm * matrix;
  VL_glLoadMatrix( viewm.ptr() );
#else
  //VL_glLoadMatrix( (viewm * model_matrix).ptr() );
#endif
}
//-----------------------------------------------------------------------------
void Camera::applyProjMatrix() const
{
  // projection matrix
  //glMatrixMode( GL_PROJECTION );
  //VL_glLoadMatrix( projectionMatrix().ptr() );
}
//-----------------------------------------------------------------------------
void Camera::applyViewMatrix() const
{
  /* some OpenGL drivers (ATI) require this instead of the more general (and mathematically correct) viewMatrix() */
  DCCore::mat4 viewm = viewMatrix();
  viewm.e(3,0) = 0.0;
  viewm.e(3,1) = 0.0;
  viewm.e(3,2) = 0.0;
  viewm.e(3,3) = 1.0;
  //glMatrixMode(GL_MODELVIEW);
  //VL_glLoadMatrix( viewm.ptr() );
}

//-----------------------------------------------------------------------------
void Camera::computeFrustumPlanes()
{
  // build modelview matrix
  DCCore::mat4 viewproj = projectionMatrix() * viewMatrix();
  // frustum plane extraction
  //mFrustum.planes().resize(6);
  //extractPlanes( &mFrustum.planes()[0], viewproj );
}
//-----------------------------------------------------------------------------
void Camera::setProjectionFrustum(float left, float right, float bottom, float top, float near, float far)
{
  // see http://www.opengl.org/resources/faq/technical/transformations.htm
  setFOV( 2.0f*atan((top-bottom)*0.5f/near) );
  setNearPlane(near);
  setFarPlane(far);
  setProjectionMatrix(DCCore::mat4::getFrustum(left, right, bottom, top, near, far), PMT_PerspectiveProjectionFrustum);
}
//-----------------------------------------------------------------------------
void Camera::setProjectionPerspective(float fov, float near, float far)
{
  setFOV(fov);
  setNearPlane(near);
  setFarPlane(far);
  setProjectionMatrix(DCCore::mat4::getPerspective(fov, aspectRatio(), near, far), PMT_PerspectiveProjection);
}
//-----------------------------------------------------------------------------
void Camera::setProjectionPerspective()
{
  setProjectionMatrix(DCCore::mat4::getPerspective(fov(), aspectRatio(), nearPlane(), farPlane()), PMT_PerspectiveProjection);
}
//-----------------------------------------------------------------------------
void Camera::setProjectionOrtho()
{
  mLeft   = 0;
  mRight  = (float)mViewport->width();
  mBottom = 0;
  mTop    = (float)mViewport->height();
  mFOV = -1;
  setProjectionMatrix( DCCore::mat4::getOrtho( mLeft, mRight, mBottom, mTop, mNearPlane, mFarPlane), PMT_OrthographicProjection );
}
//-----------------------------------------------------------------------------
void Camera::setProjectionOrtho(float left, float right, float bottom, float top, float znear, float zfar)
{
  mLeft   = left;
  mRight  = right;
  mBottom = bottom;
  mTop    = top;
  mFOV = -1;
  mNearPlane = znear;
  mFarPlane  = zfar;
  setProjectionMatrix( DCCore::mat4::getOrtho( mLeft, mRight, mBottom, mTop, mNearPlane, mFarPlane), PMT_OrthographicProjection );
}
//-----------------------------------------------------------------------------
void Camera::setProjectionOrtho(float offset)
{
  mLeft   = offset;
  mRight  = viewport()->width() + offset;
  mBottom = offset;
  mTop    = viewport()->height() + offset;
  mFOV = -1;
  mNearPlane = -1;
  mFarPlane  = +1;
  setProjectionMatrix( DCCore::mat4::getOrtho( mLeft, mRight, mBottom, mTop, mNearPlane, mFarPlane), PMT_OrthographicProjection );
}
//-----------------------------------------------------------------------------
void Camera::setViewMatrixLookAt( const DCVector3D& eye, const DCVector3D& at, const DCVector3D& up)
{
  // note: this sets both the local matrix and the view matrix
  setViewMatrix( DCCore::mat4::getLookAt(eye, at, up) );
}
//-----------------------------------------------------------------------------
void Camera::getViewMatrixAsLookAt( DCVector3D& eye, DCVector3D& at, DCVector3D& up, DCVector3D& right) const
{
  mModelingMatrix.getAsLookAtModeling(eye, at, up, right);
}
//-----------------------------------------------------------------------------
bool Camera::project(const vec4& in, vec4& out) const
{
  out = mProjectionMatrix * mViewMatrix * in;

  if (out.Getw() == 0.0f)
    return false;

  out.Getx() /= out.Getw();
  out.Gety() /= out.Getw();
  out.Getz() /= out.Getw();

  // map to range 0-1
  out.Getx() = out.Getx() * 0.5f + 0.5f;
  out.Gety() = out.Gety() * 0.5f + 0.5f;
  out.Getz() = out.Getz() * 0.5f + 0.5f;

  // map to viewport
  out.Getx() = out.Getx() * mViewport->width()  + mViewport->x();
  out.Gety() = out.Gety() * mViewport->height() + mViewport->y();
  return true;
}
//-----------------------------------------------------------------------------
bool Camera::unproject(const DCVector3D& win, vec4& out) const
{
    vec4 v;
    v.Getx() = win.x;
    v.Gety() = win.y;
    v.Getz() = win.z;
    v.Getw() = 1.0;

    // map from viewport to 0-1
    v.Getx() = (v.Getx() - mViewport->x()) / mViewport->width();
    v.Gety() = (v.Gety() - mViewport->y()) / mViewport->height();

    // map to range -1 to 1
    v.Getx() = v.Getx() * 2.0f - 1.0f;
    v.Gety() = v.Gety() * 2.0f - 1.0f;
    v.Getz() = v.Getz() * 2.0f - 1.0f;

    float det=0;
    DCCore::mat4 inverse = (mProjectionMatrix * mViewMatrix).getInverse(&det);
    if (!det)
      return false;

    v = inverse * v;
    if (v.Getw() == 0.0)
      return false;

    out = v / v.Getw();
    return true;
}
//-----------------------------------------------------------------------------
bool Camera::unproject(std::vector<DCVector3D>& win) const
{
  float det=0;
  DCCore::mat4 inverse = (mProjectionMatrix * mViewMatrix).getInverse(&det);
  if (!det)
    return false;

  bool ok = true;
  for(unsigned i=0; i<win.size(); ++i)
  {
    vec4 v;
    v = vec4( win[i], 1.0 );

    // map from viewport to 0-1
    v.Getx() = (v.Getx() - mViewport->x()) / mViewport->width();
    v.Gety() = (v.Gety() - mViewport->y()) / mViewport->height();

    // map to range -1 to 1
    v.Getx() = v.Getx() * 2.0f - 1.0f;
    v.Gety() = v.Gety() * 2.0f - 1.0f;
    v.Getz() = v.Getz() * 2.0f - 1.0f;

    v = inverse * v;
    if (v.Getw() == 0.0)
    {
      ok = false;
      continue;
    }

    v = v / v.Getw();
    win[i] = v.Getxyz();
  }
  return ok;
}