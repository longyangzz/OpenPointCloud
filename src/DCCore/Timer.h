#ifndef DCCORE_TIMER_H
#define DCCORE_TIMER_H

#include "DCCoreDLL.h"

//Qt
#include <QElapsedTimer>

namespace DCCore
{

//抽象的计时器
//必须在程序运行时进行初始化
	class DCCORE_EXPORT Timer
{
public:

    //初始化静态实体
	//必须用在其他函数之前
    static void Init();

    //返回自初始化开始的时间(秒)
    static int Second();
    //返回自初始化开始的时间(毫秒)
    static int MilliSecond();

protected:
	Timer();
};

}
#endif //_DCC_TIMER_H_
