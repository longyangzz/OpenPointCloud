//C++标准库
#include <assert.h>

//Qt标准库
#include <QTime>

#include "DCCore/Timer.h"

//unique instance
static QTime* s_eTimer = 0;

void DCCore::Timer::Init()
{
	if (!s_eTimer)
	{
		s_eTimer = new QTime();
		s_eTimer->start();
	}
}

int DCCore::Timer::Second()
{
	assert(s_eTimer && s_eTimer->isValid());
	return (s_eTimer ? s_eTimer->elapsed()/1000 : 0);
}

int DCCore::Timer::MilliSecond()
{
	assert(s_eTimer && s_eTimer->isValid());
	return (s_eTimer ? s_eTimer->elapsed() : 0);
}
