#include "Transform.h"

using namespace DCCore;

//-----------------------------------------------------------------------------
// Transform
//-----------------------------------------------------------------------------
Transform::~Transform()
{
  for(size_t i=0; i<mChildren.size(); ++i)
  {
    mChildren[i]->mParent = NULL;
    mChildren[i]->setLocalMatrix( mChildren[i]->worldMatrix() );
  }
}
//-----------------------------------------------------------------------------
void Transform::translate(float x, float y, float z)
{
  setLocalMatrix( mat4::getTranslation(x,y,z)*localMatrix() );
}
//-----------------------------------------------------------------------------
void Transform::translate(const DCVector3D& t)
{
  setLocalMatrix( mat4::getTranslation(t)*localMatrix() );
}
//-----------------------------------------------------------------------------
void Transform::scale(float x, float y, float z)
{
  setLocalMatrix( mat4::getScaling(x,y,z)*localMatrix() );
}
//-----------------------------------------------------------------------------
void Transform::rotate(float degrees, float x, float y, float z)
{
  setLocalMatrix( mat4::getRotation(degrees,x,y,z)*localMatrix() );
}
//-----------------------------------------------------------------------------
void Transform::rotate(const DCVector3D& from, const DCVector3D& to)
{
  setLocalMatrix( mat4::getRotation(from,to)*localMatrix() );
}
//-----------------------------------------------------------------------------
void Transform::preMultiply(const mat4& m)
{
  setLocalMatrix( m*localMatrix() );
}
//-----------------------------------------------------------------------------
void Transform::postMultiply(const mat4& m)
{
  setLocalMatrix( localMatrix()*m );
}
//-----------------------------------------------------------------------------
