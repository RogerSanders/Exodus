#ifndef __ICLOCKSOURCE_H__
#define __ICLOCKSOURCE_H__
#include "IDeviceContext.h"

class IClockSource
{
public:
	//Enumerations
	enum ClockType;

public:
	//Constructors
	virtual ~IClockSource() = 0 {}

	//Clock type functions
	virtual ClockType GetClockType() const = 0;

	//Clock frequency functions
	virtual bool SetClockFrequency(double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual bool SetClockDivider(double divider, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual bool SetClockMultiplier(double multiplier, IDeviceContext* caller, double accessTime, unsigned int accessContext) = 0;
	virtual bool TransparentSetClockFrequency(double clockRate) = 0;
	virtual bool TransparentSetClockDivider(double divider) = 0;
	virtual bool TransparentSetClockMultiplier(double multiplier) = 0;
};

#include "IClockSource.inl"
#endif
