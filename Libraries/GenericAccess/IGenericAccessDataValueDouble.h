#ifndef __IGENERICACCESSDATAVALUEDOUBLE_H__
#define __IGENERICACCESSDATAVALUEDOUBLE_H__
#include "IGenericAccessDataValue.h"

class IGenericAccessDataValueDouble :public IGenericAccessDataValue
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataValueDoubleVersion() { return 1; }
	virtual unsigned int GetIGenericAccessDataValueDoubleVersion() const = 0;

	//Value read functions
	virtual double GetValue() const = 0;

	//Value display functions
	virtual FloatDisplayMode GetDisplayMode() const = 0;
	virtual void SetDisplayMode(FloatDisplayMode state) = 0;
	virtual unsigned int GetMinWholeNumberChars() const = 0;
	virtual void SetMinWholeNumberChars(unsigned int state) = 0;
	virtual unsigned int GetMinFractionalNumberChars() const = 0;
	virtual void SetMinFractionalNumberChars(unsigned int state) = 0;

	//Value limit functions
	virtual double GetMinValue() const = 0;
	virtual void SetMinValue(double state) = 0;
	virtual double GetMaxValue() const = 0;
	virtual void SetMaxValue(double state) = 0;
};

#endif
