#ifndef __IGENERICACCESSDATAVALUEUINT_H__
#define __IGENERICACCESSDATAVALUEUINT_H__
#include "IGenericAccessDataValue.h"

class IGenericAccessDataValueUInt :public IGenericAccessDataValue
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataValueUIntVersion();
	virtual unsigned int GetIGenericAccessDataValueUIntVersion() const = 0;

	//Value read functions
	virtual unsigned int GetValue() const = 0;

	//Value display functions
	virtual IntDisplayMode GetDisplayMode() const = 0;
	virtual void SetDisplayMode(IntDisplayMode state) = 0;
	virtual unsigned int GetMinChars() const = 0;
	virtual void SetMinChars(unsigned int state) = 0;
	virtual unsigned int CalculateDisplayChars(IntDisplayMode adisplayMode, unsigned int aminValue, unsigned int amaxValue) const = 0;

	//Value limit functions
	virtual unsigned int GetMinValue() const = 0;
	virtual void SetMinValue(unsigned int state) = 0;
	virtual unsigned int GetMaxValue() const = 0;
	virtual void SetMaxValue(unsigned int state) = 0;
};

#include "IGenericAccessDataValueUInt.inl"
#endif
