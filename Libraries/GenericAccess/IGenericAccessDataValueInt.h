#ifndef __IGENERICACCESSDATAVALUEINT_H__
#define __IGENERICACCESSDATAVALUEINT_H__
#include "IGenericAccessDataValue.h"

class IGenericAccessDataValueInt :public IGenericAccessDataValue
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataValueIntVersion() { return 1; }
	virtual unsigned int GetIGenericAccessDataValueIntVersion() const = 0;

	//Value read functions
	virtual int GetValue() const = 0;

	//Value display functions
	virtual IntDisplayMode GetDisplayMode() const = 0;
	virtual void SetDisplayMode(IntDisplayMode state) = 0;
	virtual unsigned int GetMinChars() const = 0;
	virtual void SetMinChars(unsigned int state) = 0;
	virtual unsigned int CalculateDisplayChars(IntDisplayMode adisplayMode, int aminValue, int amaxValue) const = 0;

	//Value limit functions
	virtual int GetMinValue() const = 0;
	virtual void SetMinValue(int state) = 0;
	virtual int GetMaxValue() const = 0;
	virtual void SetMaxValue(int state) = 0;
};

#endif
