#ifndef __PERFORMANCETIMER_H__
#define __PERFORMANCETIMER_H__
#include "WindowFunctions/WindowFunctions.pkg"
//##DEBUG##
#include <iostream>
#include <iomanip>

class PerformanceTimer
{
public:
	PerformanceTimer()
	{
		QueryPerformanceFrequency(&counterFrequency);
		Reset();
	}

	void Reset()
	{
		QueryPerformanceCounter(&executionTimeStart);
	}

	void Sync(double targetExecutionTime, bool enableSync = true)
	{
		LARGE_INTEGER executionTimeEnd;
		QueryPerformanceCounter(&executionTimeEnd);
		LARGE_INTEGER executionTimeRealEnd = executionTimeEnd;
		double executionTimeInNanoseconds = (double)(executionTimeEnd.QuadPart - executionTimeStart.QuadPart) * (1000000000.0/(double)counterFrequency.QuadPart);
		double remainingTimeInNanoseconds = targetExecutionTime - executionTimeInNanoseconds;
		LONGLONG accumulatedExecutionTimeInTicks;
		accumulatedExecutionTimeInTicks = (LONGLONG)((targetExecutionTime * ((double)counterFrequency.QuadPart/1000000000.0)) + 0.5);
		if(!enableSync)
		{
			QueryPerformanceCounter(&executionTimeEnd);
		}
		else
		{
			if(remainingTimeInNanoseconds > 2000000.0)
			{
				Sleep((DWORD)((remainingTimeInNanoseconds - 1000000.0) / 1000000.0));
			}
			while((executionTimeEnd.QuadPart - executionTimeStart.QuadPart) < accumulatedExecutionTimeInTicks)
			{
				QueryPerformanceCounter(&executionTimeEnd);
			}
		}

		//##DEBUG##
		std::wcout << std::setprecision(16) << targetExecutionTime << '\t' << accumulatedExecutionTimeInTicks << '\t' << executionTimeEnd.QuadPart - executionTimeStart.QuadPart << '\t' << executionTimeRealEnd.QuadPart - executionTimeStart.QuadPart << '\t' << std::setprecision(4) << ((double)accumulatedExecutionTimeInTicks / (double)(executionTimeEnd.QuadPart - executionTimeStart.QuadPart)) * 100.0 << '\t' << std::setprecision(4) << ((double)accumulatedExecutionTimeInTicks / (double)(executionTimeRealEnd.QuadPart - executionTimeStart.QuadPart)) * 100.0 << '\n';

		executionTimeStart = executionTimeEnd;
	}

private:
	LARGE_INTEGER counterFrequency;
	LARGE_INTEGER executionTimeStart;
};

#endif
