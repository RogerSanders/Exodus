#ifndef __M68000_EXECUTETIME_H__
#define __M68000_EXECUTETIME_H__
namespace M68000 {

class ExecuteTime
{
public:
	//Constructors
	inline ExecuteTime();
	inline ExecuteTime(unsigned int acycles, unsigned int abusRead, unsigned int abusWrite, double aadditionalTime = 0);

	//Set functions
	inline void Set(unsigned int acycles, unsigned int abusRead, unsigned int abusWrite, double aadditionalTime = 0);

	//Addition operators
	inline ExecuteTime operator+(const ExecuteTime& target) const;
	inline ExecuteTime& operator+=(const ExecuteTime& target);

public:
	//Data members
	unsigned int cycles;
	unsigned int busRead;
	unsigned int busWrite;
	double additionalTime;
};

} //Close namespace M68000
#include "ExecuteTime.inl"
#endif
