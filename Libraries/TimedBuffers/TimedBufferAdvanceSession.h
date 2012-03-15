#ifndef __TIMEDBUFFERADVANCESESSION_H__
#define __TIMEDBUFFERADVANCESESSION_H__

//##TODO## Add an optional parameter to retrieve the new value and write address for the
//next write, as well as the next write time. This will allow us to efficiently implement
//CRAM write flicker.
template<class DataType, class TimesliceType> struct TimedBufferAdvanceSession
{
public:
	//Constructors
	inline TimedBufferAdvanceSession();

	//Session functions
	inline void Reset();

public:
	//Data members
	bool initialized;
	TimesliceType timeRemovedDuringSession;
	TimesliceType initialTimeOffset;
	TimesliceType nextWriteTime; 
};

#include "TimedBufferAdvanceSession.inl"
#endif
