#ifndef __GENERICACCESSDATAVALUEUINT_H__
#define __GENERICACCESSDATAVALUEUINT_H__
#include "GenericAccessDataValueBase.h"
#include "IGenericAccessDataValueUInt.h"

class GenericAccessDataValueUInt :public GenericAccessDataValueBase<IGenericAccessDataValueUInt>
{
public:
	//Constructors
	GenericAccessDataValueUInt(unsigned int value = 0);

	//Interface version functions
	virtual unsigned int GetIGenericAccessDataValueUIntVersion() const;

	//Type functions
	virtual DataType GetType() const;

	//Value read functions
	virtual unsigned int GetValue() const;
	virtual Marshal::Ret<std::wstring> GetValueString() const;

	//Value write functions
	virtual bool SetValueInt(int value);
	virtual bool SetValueUInt(unsigned int value);
	virtual bool SetValueString(const Marshal::In<std::wstring>& value);

	//Value display functions
	virtual IntDisplayMode GetDisplayMode() const;
	virtual void SetDisplayMode(IntDisplayMode state);
	virtual unsigned int GetMinChars() const;
	virtual void SetMinChars(unsigned int state);
	virtual unsigned int CalculateDisplayChars(IntDisplayMode displayMode, unsigned int minValue, unsigned int maxValue) const;

	//Value limit functions
	virtual unsigned int GetMinValue() const;
	virtual void SetMinValue(unsigned int state);
	virtual unsigned int GetMaxValue() const;
	virtual void SetMaxValue(unsigned int state);

	//Value limit functions
	virtual void ApplyLimitSettingsToCurrentValue();

private:
	unsigned int _dataValue;
	IntDisplayMode _displayMode;
	unsigned int _minChars;
	unsigned int _minValue;
	unsigned int _maxValue;
};

#endif
