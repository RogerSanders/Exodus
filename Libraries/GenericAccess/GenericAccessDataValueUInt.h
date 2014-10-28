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
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetValueString() const;

	//Value write functions
	virtual bool SetValueInt(int value);
	virtual bool SetValueUInt(unsigned int value);
	virtual bool SetValueString(const MarshalSupport::Marshal::In<std::wstring>& value);

	//Value display functions
	virtual IntDisplayMode GetDisplayMode() const;
	virtual void SetDisplayMode(IntDisplayMode state);
	virtual unsigned int GetMinChars() const;
	virtual void SetMinChars(unsigned int state);
	virtual unsigned int CalculateDisplayChars(IntDisplayMode adisplayMode, unsigned int aminValue, unsigned int amaxValue) const;

	//Value limit functions
	virtual unsigned int GetMinValue() const;
	virtual void SetMinValue(unsigned int state);
	virtual unsigned int GetMaxValue() const;
	virtual void SetMaxValue(unsigned int state);

	//Value limit functions
	virtual void ApplyLimitSettingsToCurrentValue();

private:
	unsigned int dataValue;
	IntDisplayMode displayMode;
	unsigned int minChars;
	unsigned int minValue;
	unsigned int maxValue;
};

#endif
