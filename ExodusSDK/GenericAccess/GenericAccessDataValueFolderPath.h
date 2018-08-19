#ifndef __GENERICACCESSDATAVALUEFOLDERPATH_H__
#define __GENERICACCESSDATAVALUEFOLDERPATH_H__
#include "GenericAccessDataValueBase.h"
#include "IGenericAccessDataValueFolderPath.h"

class GenericAccessDataValueFolderPath :public GenericAccessDataValueBase<IGenericAccessDataValueFolderPath>
{
public:
	//Constructors
	GenericAccessDataValueFolderPath(const std::wstring& value = L"");

	//Interface version functions
	virtual unsigned int GetIGenericAccessDataValueFolderPathVersion() const;

	//Type functions
	virtual DataType GetType() const;

	//Value read functions
	virtual Marshal::Ret<std::wstring> GetValue() const;
	virtual Marshal::Ret<std::wstring> GetValueString() const;

	//Value write functions
	virtual bool SetValueString(const Marshal::In<std::wstring>& value);
	virtual bool SetValueFolderPath(const Marshal::In<std::wstring>& value);

	//Value limit functions
	virtual unsigned int GetMaxLength() const;
	virtual void SetMaxLength(unsigned int state);

	//Value limit functions
	virtual void ApplyLimitSettingsToCurrentValue();

private:
	std::wstring _dataValue;
	unsigned int _maxLength;
};

#endif
