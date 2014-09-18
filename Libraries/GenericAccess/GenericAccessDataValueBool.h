#ifndef __GENERICACCESSDATAVALUEBOOL_H__
#define __GENERICACCESSDATAVALUEBOOL_H__
#include "GenericAccessDataValueBase.h"
#include "IGenericAccessDataValueBool.h"

class GenericAccessDataValueBool :public GenericAccessDataValueBase<IGenericAccessDataValueBool>
{
public:
	//Constructors
	GenericAccessDataValueBool(bool value = false);

	//Interface version functions
	virtual unsigned int GetIGenericAccessDataValueBoolVersion() const;

	//Type functions
	virtual DataType GetType() const;

	//Value read functions
	virtual bool GetValue() const;
	virtual std::wstring GetValueString() const;

	//Value write functions
	virtual bool SetValueBool(bool value);
	virtual bool SetValueString(const std::wstring& value);

	//Value limit functions
	virtual void ApplyLimitSettingsToCurrentValue();

private:
	bool dataValue;
};

#endif
