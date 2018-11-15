#include "Hit.h"

using namespace MPUtil;

Hit::Hit()
	: entity(nullptr)
	, index(-1)
	, wordPoint(DCCore::Point_3())
	, screenPoint(DCCore::Point_3())
{

}

Hit::Hit(DcGp::DcGpEntity* obj, DCCore::Point_3 wPoint, unsigned long ix)
	: entity(obj)
	, wordPoint(wPoint)
	, index(ix)
	, screenPoint(DCCore::Point_3())
{

}

Hit::Hit(DcGp::DcGpEntity* obj)
	: entity(obj)
	, index(-1)
	, wordPoint(DCCore::Point_3())
	, screenPoint(DCCore::Point_3())
{

}

Hit::~Hit()
{
	entity = nullptr;
	index = -1;
	wordPoint = DCCore::Point_3();
	screenPoint = DCCore::Point_3();
}

DCCore::Point_3 Hit::GetWordPoint() const
{
	return wordPoint;
}

DcGp::DcGpEntity* Hit::GetEntity() const
{
	return entity;
}