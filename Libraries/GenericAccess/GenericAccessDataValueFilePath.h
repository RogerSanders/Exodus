#ifndef __GENERICACCESSDATAVALUEFILEPATH_H__
#define __GENERICACCESSDATAVALUEFILEPATH_H__
#include "GenericAccessDataValueBase.h"
#include "IGenericAccessDataValueFilePath.h"

class GenericAccessDataValueFilePath :public GenericAccessDataValueBase<IGenericAccessDataValueFilePath>
{
public:
	//Constructors
	GenericAccessDataValueFilePath(const std::wstring& value = L"");

	//Interface version functions
	virtual unsigned int GetIGenericAccessDataValueFilePathVersion() const;

	//Type functions
	virtual DataType GetType() const;

	//Value read functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetValue() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetValueString() const;

	//Value write functions
	virtual bool SetValueString(const MarshalSupport::Marshal::In<std::wstring>& value);
	virtual bool SetValueFilePath(const MarshalSupport::Marshal::In<std::wstring>& value);

	//Value limit functions
	virtual unsigned int GetMaxLength() const;
	virtual void SetMaxLength(unsigned int state);

	//Value limit functions
	virtual void ApplyLimitSettingsToCurrentValue();

private:
	std::wstring dataValue;
	unsigned int maxLength;
};

#endif
