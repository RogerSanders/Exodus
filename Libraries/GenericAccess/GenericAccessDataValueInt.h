#ifndef __GENERICACCESSDATAVALUEINT_H__
#define __GENERICACCESSDATAVALUEINT_H__
#include "GenericAccessDataValueBase.h"
#include "IGenericAccessDataValueInt.h"

class GenericAccessDataValueInt :public GenericAccessDataValueBase<IGenericAccessDataValueInt>
{
public:
	//Constructors
	GenericAccessDataValueInt(int value = 0);

	//Interface version functions
	virtual unsigned int GetIGenericAccessDataValueIntVersion() const;

	//Type functions
	virtual DataType GetType() const;

	//Value read functions
	virtual int GetValue() const;
	virtual std::wstring GetValueString() const;

	//Value write functions
	virtual bool SetValueInt(int value);
	virtual bool SetValueUInt(unsigned int value);
	virtual bool SetValueString(const std::wstring& value);

	//Value display functions
	virtual IntDisplayMode GetDisplayMode() const;
	virtual void SetDisplayMode(IntDisplayMode state);
	virtual unsigned int GetMinChars() const;
	virtual void SetMinChars(unsigned int state);
	virtual unsigned int CalculateDisplayChars(IntDisplayMode adisplayMode, int aminValue, int amaxValue) const;

	//Value limit functions
	virtual int GetMinValue() const;
	virtual void SetMinValue(int state);
	virtual int GetMaxValue() const;
	virtual void SetMaxValue(int state);

	//Value limit functions
	virtual void ApplyLimitSettingsToCurrentValue();

private:
	int dataValue;
	IntDisplayMode displayMode;
	unsigned int minChars;
	int minValue;
	int maxValue;
};

#endif
