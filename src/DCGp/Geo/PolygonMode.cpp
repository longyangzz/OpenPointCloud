#include "PolygonMode.h"

using namespace DCGp;

PolygonMode::PolygonMode()
	: _modeFront(FILL)
	, _modeBack(FILL)
{
}

PolygonMode::PolygonMode(Face face,Mode mode):
_modeFront(FILL),
	_modeBack(FILL)
{
	setMode(face,mode);
}


PolygonMode::~PolygonMode()
{
}

void PolygonMode::setMode(Face face,Mode mode)
{
	switch(face)
	{
	case(FRONT):
		_modeFront = mode;
		break;
	case(BACK):
		_modeBack = mode;
		break;
	case(FRONT_AND_BACK):
		_modeFront = mode;
		_modeBack = mode;
		break;
	}    
}

PolygonMode::Mode PolygonMode::getMode(Face face) const
{
	switch(face)
	{
	case(FRONT):
		return _modeFront;
	case(BACK):
		return _modeBack;
	case(FRONT_AND_BACK):
		return _modeFront;
	}    

	return _modeFront;
}

void PolygonMode::Apply(State&) const
{
	if (_modeFront==_modeBack)
	{
		glPolygonMode(GL_FRONT_AND_BACK,(GLenum)_modeFront);
	}
	else
	{
		glPolygonMode(GL_FRONT,(GLenum)_modeFront);
		glPolygonMode(GL_BACK,(GLenum)_modeBack);
	}
}
