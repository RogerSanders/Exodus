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
	inline std::wstring GetValue() const;
	virtual std::wstring GetValueString() const;

	//Value write functions
	virtual bool SetValueString(const std::wstring& value);
	virtual bool SetValueFolderPath(const std::wstring& value);

	//Value limit functions
	virtual unsigned int GetMaxLength() const;
	virtual void SetMaxLength(unsigned int state);

	//Value limit functions
	virtual void ApplyLimitSettingsToCurrentValue();

protected:
	//Value read functions
	virtual void GetValueInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

private:
	std::wstring dataValue;
	unsigned int maxLength;
};

#endif
