#ifndef __CLOCKSOURCE_H__
#define __CLOCKSOURCE_H__
#include <list>

//##TODO## Implement the ClockSource class
class ClockSource
{
public:
	//Enumerations

private:
	std::list<ClockSource*> dependentClockSources;
};

#include "ClockSource.inl"
#endif
