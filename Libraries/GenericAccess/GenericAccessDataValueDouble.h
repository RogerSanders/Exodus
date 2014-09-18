#ifndef __GENERICACCESSDATAVALUEDOUBLE_H__
#define __GENERICACCESSDATAVALUEDOUBLE_H__
#include "GenericAccessDataValueBase.h"
#include "IGenericAccessDataValueDouble.h"

class GenericAccessDataValueDouble :public GenericAccessDataValueBase<IGenericAccessDataValueDouble>
{
public:
	//Constructors
	GenericAccessDataValueDouble(double value = 0.0);

	//Interface version functions
	virtual unsigned int GetIGenericAccessDataValueDoubleVersion() const;

	//Type functions
	virtual DataType GetType() const;

	//Value read functions
	virtual double GetValue() const;
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
	virtual double GetMinValue() const;
	virtual void SetMinValue(double state);
	virtual double GetMaxValue() const;
	virtual void SetMaxValue(double state);

	//Value limit functions
	virtual void ApplyLimitSettingsToCurrentValue();

private:
	double dataValue;
	FloatDisplayMode displayMode;
	unsigned int minWholeNumberChars;
	unsigned int minFractionalNumberChars;
	double minValue;
	double maxValue;
};

#endif
