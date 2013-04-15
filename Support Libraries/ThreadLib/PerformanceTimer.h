#ifndef __PERFORMANCETIMER_H__
#define __PERFORMANCETIMER_H__
#include "WindowFunctions/WindowFunctions.pkg"

class PerformanceTimer
{
public:
	//Constructors
	inline PerformanceTimer();

	//Synchronization functions
	inline void Reset();
	inline void Sync(double targetExecutionTime, bool enableSync = true, double executeAheadTolerance = 0.001);

private:
	LARGE_INTEGER counterFrequency;
	LARGE_INTEGER executionTimeStart;
	LONGLONG executionTimeAhead;
};

#include "PerformanceTimer.inl"
#endif
