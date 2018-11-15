//----------------------------------------------------------------------
//author:wly
//time:
//dsc:Ê±¼ä
//
//----------------------------------------------------------------------

#ifndef DCCORE_TIME_H
#define DCCORE_TIME_H

#include "DCCoreDLL.h"

#include "BasicTypes.h"

namespace DCCore
{
  class DCCORE_EXPORT Time
  {
  public:
    Time();

    int year() const { return mYear; }
    
    int month() const { return mMonth; }
    
    int dayOfWeek() const { return mDayOfWeek; }
    
    int dayOfMonth() const { return mDayOfMonth; }
    
    int hour() const { return mHour; }
    
    int minute() const { return mMinute; }
    
    int second() const { return mSecond; }
    
    int microsecond() const { return mMicrosecond; }

    static real currentTime();
    
    static void sleep(unsigned int milliseconds);

    void start(int index=0) { mStart[index] = currentTime(); }
    
    void stop(int index=0)  { mStart[index] = -1.0; }
    
    bool isStarted(int index=0) const { return mStart[index] != -1; }
    
    real elapsed(int index=0) const { return mStart[index] >= 0 ? currentTime() - mStart[index] : -1; }

  protected:
    int mYear; // 1601 through 30827.
    int mMonth; // 1..12
    int mDayOfWeek; // 0 = Sunday, 6 = Saturday
    int mDayOfMonth; // 1..31
    int mHour; // 0..23
    int mMinute; // 0..59
    int mSecond; // 0..59
    int mMicrosecond; // 0 ... 999999

    real mStart[16];
  };
}

#endif
