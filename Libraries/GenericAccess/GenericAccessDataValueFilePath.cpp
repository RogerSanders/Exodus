#include "GenericAccessDataValueFilePath.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueFilePath::GenericAccessDataValueFilePath(const std::wstring& value)
:dataValue(value)
{
	maxLength = 0;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFilePath::GetIGenericAccessDataValueFilePathVersion() const
{
	return ThisIGenericAccessDataValueFilePathVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueFilePath::DataType GenericAccessDataValueFilePath::GetType() const
{
	return DATATYPE_FILEPATH;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
std::wstring GenericAccessDataValueFilePath::GetValue() const
{
	return dataValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueFilePath::GetValueInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetValue());
}

//----------------------------------------------------------------------------------------
std::wstring GenericAccessDataValueFilePath::GetValueString() const
{
	return GetValue();
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueFilePath::SetValueString(const std::wstring& value)
{
	return SetValueFilePath(value);
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueFilePath::SetValueFilePath(const std::wstring& value)
{
	dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFilePath::GetMaxLength() const
{
	return maxLength;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueFilePath::SetMaxLength(unsigned int state)
{
	maxLength = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueFilePath::ApplyLimitSettingsToCurrentValue()
{
	if((maxLength > 0) && (dataValue.length() > (size_t)maxLength))
	{
		dataValue.resize((size_t)maxLength);
	}
}
