#ifndef __Z80_EXECUTETIME_H__
#define __Z80_EXECUTETIME_H__
namespace Z80 {

class ExecuteTime
{
public:
	//Constructors
	inline ExecuteTime();
	inline ExecuteTime(unsigned int acycles, double aadditionalTime = 0);

	//Set functions
	inline void Set(unsigned int acycles, double aadditionalTime = 0);

	//Addition operators
	inline ExecuteTime operator+(const ExecuteTime& target) const;
	inline ExecuteTime& operator+=(const ExecuteTime& target);

public:
	//Data members
	unsigned int cycles;
	double additionalTime;
};

} //Close namespace Z80
#include "ExecuteTime.inl"
#endif
