#ifndef __GENERICACCESSDATAVALUEFLOAT_H__
#define __GENERICACCESSDATAVALUEFLOAT_H__
#include "GenericAccessDataValueBase.h"
#include "IGenericAccessDataValueFloat.h"

class GenericAccessDataValueFloat :public GenericAccessDataValueBase<IGenericAccessDataValueFloat>
{
public:
	//Constructors
	GenericAccessDataValueFloat(float value = 0.0f);

	//Interface version functions
	virtual unsigned int GetIGenericAccessDataValueFloatVersion() const;

	//Type functions
	virtual DataType GetType() const;

	//Value read functions
	virtual float GetValue() const;
	virtual std::wstring GetValueString() const;

	//Value write functions
	virtual bool SetValueFloat(float value);
	virtual bool SetValueDouble(double value);
	virtual bool SetValueString(const std::wstring& value);

	//Value display functions
	virtual FloatDisplayMode GetDisplayMode() const;
	virtual void SetDisplayMode(FloatDisplayMode state);
	virtual unsigned int GetMinWholeNumberChars() const;
	virtual void SetMinWholeNumberChars(unsigned int state);
	virtual unsigned int GetMinFractionalNumberChars() const;
	virtual void SetMinFractionalNumberChars(unsigned int state);

	//Value limit functions
	virtual float GetMinValue() const;
	virtual void SetMinValue(float state);
	virtual float GetMaxValue() const;
	virtual void SetMaxValue(float state);

	//Value limit functions
	virtual void ApplyLimitSettingsToCurrentValue();

private:
	float dataValue;
	FloatDisplayMode displayMode;
	unsigned int minWholeNumberChars;
	unsigned int minFractionalNumberChars;
	float minValue;
	float maxValue;
};

#endif
