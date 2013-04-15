//##DEBUG##
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PerformanceTimer::PerformanceTimer()
{
	QueryPerformanceFrequency(&counterFrequency);
	Reset();
}

//----------------------------------------------------------------------------------------
//Synchronization functions
//----------------------------------------------------------------------------------------
void PerformanceTimer::Reset()
{
	executionTimeAhead = 0;
	QueryPerformanceCounter(&executionTimeStart);
}

//----------------------------------------------------------------------------------------
void PerformanceTimer::Sync(double targetExecutionTime, bool enableSync, double executeAheadTolerance)
{
	//Obtain the current time in ticks, making allowance for counter overflow if it
	//occurs. Note that the official documentation doesn't give any assurance as to the
	//point at which wrapping occurs, and the counter value itself is defined as signed,
	//so we don't try and guess the wrapping point here for safety reasons, rather, we
	//just assume the counter wraps back around to 0, and we rebase our start time from 0.
	//This will probably result in a too short execution timespan being calculated at the
	//time the counter wraps, but it's the safest option pending further testing, or some
	//assurance from the documentation as to the wrap point.
	LARGE_INTEGER executionTimeEnd;
	QueryPerformanceCounter(&executionTimeEnd);
	if(executionTimeEnd.QuadPart < executionTimeStart.QuadPart)
	{
		executionTimeStart.QuadPart = 0;
	}

	//Record the captured current time as the real end time for this execution block. We
	//only do this so we can report on it later.
	LARGE_INTEGER executionTimeRealEnd = executionTimeEnd;

	//Calculate how long we would have expected the target execution time to take in ticks
	LONGLONG executionTimeInTicks = (LONGLONG)((targetExecutionTime * ((double)counterFrequency.QuadPart/1000000000.0)) + 0.5);

	//If synchronization is enabled, block until we reach the correct time.
	if(enableSync)
	{
		//Calculate the actual time we want to advance to. As part of this calculation, we
		//incorporate a tolerance, allowing the system to actually execute a bit too fast.
		//We then reclaim extra time on the next synchronization point. We do this as our
		//synchronization block is not perfect, and while we may stop exactly at the
		//target time, it's far more likely that we will slightly overshoot it each time.
		//Adding this tolerance factor allows us to float near the target, just a bit
		//before or after, which will over time average out to the correct execution time.
		LONGLONG executeAheadToleranceInTicks = (LONGLONG)((double)executionTimeInTicks * executeAheadTolerance);
		LONGLONG targetExecutionSpanInTicks = executionTimeInTicks - executeAheadToleranceInTicks;
		targetExecutionSpanInTicks += executionTimeAhead;

		//If we're significantly ahead of the target time, yield this thread until we get
		//closer to the target time. Note that according to research, it appears that all
		//versions of Windows based on the Windows NT line actually have a minimum
		//resolution 10 milliseconds for the sleep timer. To account for this, we only
		//allow the Sleep function to be called if there is at least 15 milliseconds of
		//free time, and we subtract 15 milliseconds from the sleep time, to ensure we
		//don't delay too long.
		static const double sleepErrorAdjustmentInNanoseconds = 15000000.0;
		double executionTimeInNanoseconds = (double)(executionTimeEnd.QuadPart - executionTimeStart.QuadPart) * (1000000000.0/(double)counterFrequency.QuadPart);
		double remainingTimeInNanoseconds = targetExecutionTime - executionTimeInNanoseconds;
		if(remainingTimeInNanoseconds >= sleepErrorAdjustmentInNanoseconds)
		{
			DWORD sleepTimeInMilliseconds = (DWORD)((remainingTimeInNanoseconds - sleepErrorAdjustmentInNanoseconds) / 1000000.0);
			Sleep(sleepTimeInMilliseconds);
		}

		//Spin around sampling the current time until we reach the target time, taking
		//counter overflow into account.
		while((executionTimeEnd.QuadPart - executionTimeStart.QuadPart) < targetExecutionSpanInTicks)
		{
			QueryPerformanceCounter(&executionTimeEnd);
			if(executionTimeEnd.QuadPart < executionTimeStart.QuadPart)
			{
				executionTimeStart.QuadPart = 0;
			}
		}

		//If we stopped slightly before the target time, record the number of ticks which
		//we are ahead of where we should be.
		LONGLONG actualExecutionSpanInTicks = executionTimeEnd.QuadPart - executionTimeStart.QuadPart;
		executionTimeAhead = (actualExecutionSpanInTicks >= executionTimeInTicks)? 0: executionTimeInTicks - actualExecutionSpanInTicks;
	}

	//##DEBUG##
	std::wcout << std::setprecision(16) << targetExecutionTime << '\t' << executionTimeInTicks << '\t' << executionTimeEnd.QuadPart - executionTimeStart.QuadPart << '\t' << executionTimeRealEnd.QuadPart - executionTimeStart.QuadPart << '\t' << std::setprecision(4) << ((double)executionTimeInTicks / (double)(executionTimeEnd.QuadPart - executionTimeStart.QuadPart)) * 100.0 << '\t' << std::setprecision(4) << ((double)executionTimeInTicks / (double)(executionTimeRealEnd.QuadPart - executionTimeStart.QuadPart)) * 100.0 << '\t' << executionTimeAhead << '\n';

	//Save the end time for this synchronization point as the start time for the next
	//synchronization point
	executionTimeStart = executionTimeEnd;
}
