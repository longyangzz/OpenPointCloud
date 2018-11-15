#include "Time.h"

//ϵͳ
#include <Windows.h>

using namespace DCCore;

//-----------------------------------------------------------------------------
// Time
//-----------------------------------------------------------------------------
Time::Time()
{
  for(int i=0; i<16; ++i)
    mStart[i] = -1;

    SYSTEMTIME local_time;
    GetLocalTime(&local_time);
    mYear = local_time.wYear;
    mMonth = local_time.wMonth;
    mDayOfWeek = local_time.wDayOfWeek;
    mDayOfMonth = local_time.wDay;
    mHour = local_time.wHour;
    mMinute = local_time.wMinute;
    mSecond = local_time.wSecond;
    mMicrosecond = local_time.wMilliseconds * 1000;
}
//-----------------------------------------------------------------------------
namespace DCCore
{
  unsigned long long gStartTime = 0;

  void initStartTime()
  {
      LARGE_INTEGER Frequency;
      LARGE_INTEGER PerformanceCount;
      BOOL has_timer = QueryPerformanceFrequency( &Frequency );
      if (has_timer)
      {
        // DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 1);
        QueryPerformanceCounter( &PerformanceCount );
        // ::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
        gStartTime = PerformanceCount.QuadPart;
      }
      else
      {
        gStartTime = GetTickCount();
      }
  }
}
//-----------------------------------------------------------------------------
real Time::currentTime()
{
  if (gStartTime == 0)
    initStartTime();

    // Win32
    LARGE_INTEGER Frequency;
    LARGE_INTEGER PerformanceCount;
    BOOL has_timer = QueryPerformanceFrequency( &Frequency );
    if (has_timer)
    {
      // DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 1);
      QueryPerformanceCounter( &PerformanceCount );
      // ::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
      return (real)(PerformanceCount.QuadPart-gStartTime)/Frequency.QuadPart;
    }
    else
    {
      return (GetTickCount()-gStartTime) / 1000.0f;
    }
}
//-----------------------------------------------------------------------------
void Time::sleep(unsigned int milliseconds)
{
    Sleep(milliseconds);
}
//-----------------------------------------------------------------------------
