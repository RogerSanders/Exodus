#ifndef __IGENERICACCESSDATAVALUEFLOAT_H__
#define __IGENERICACCESSDATAVALUEFLOAT_H__
#include "IGenericAccessDataValue.h"

class IGenericAccessDataValueFloat :public IGenericAccessDataValue
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataValueFloatVersion();
	virtual unsigned int GetIGenericAccessDataValueFloatVersion() const = 0;

	//Value read functions
	virtual float GetValue() const = 0;

	//Value display functions
	virtual FloatDisplayMode GetDisplayMode() const = 0;
	virtual void SetDisplayMode(FloatDisplayMode state) = 0;
	virtual unsigned int GetMinWholeNumberChars() const = 0;
	virtual void SetMinWholeNumberChars(unsigned int state) = 0;
	virtual unsigned int GetMinFractionalNumberChars() const = 0;
	virtual void SetMinFractionalNumberChars(unsigned int state) = 0;

	//Value limit functions
	virtual float GetMinValue() const = 0;
	virtual void SetMinValue(float state) = 0;
	virtual float GetMaxValue() const = 0;
	virtual void SetMaxValue(float state) = 0;
};

#include "IGenericAccessDataValueFloat.inl"
#endif
