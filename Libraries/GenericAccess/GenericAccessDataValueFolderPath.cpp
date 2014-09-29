#include "GenericAccessDataValueFolderPath.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueFolderPath::GenericAccessDataValueFolderPath(const std::wstring& value)
:dataValue(value)
{
	maxLength = 0;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFolderPath::GetIGenericAccessDataValueFolderPathVersion() const
{
	return ThisIGenericAccessDataValueFolderPathVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueFolderPath::DataType GenericAccessDataValueFolderPath::GetType() const
{
	return DataType::FolderPath;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
std::wstring GenericAccessDataValueFolderPath::GetValue() const
{
	return dataValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueFolderPath::GetValueInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetValue());
}

//----------------------------------------------------------------------------------------
std::wstring GenericAccessDataValueFolderPath::GetValueString() const
{
	return GetValue();
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueFolderPath::SetValueString(const std::wstring& value)
{
	return SetValueFolderPath(value);
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueFolderPath::SetValueFolderPath(const std::wstring& value)
{
	dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFolderPath::GetMaxLength() const
{
	return maxLength;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueFolderPath::SetMaxLength(unsigned int state)
{
	maxLength = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueFolderPath::ApplyLimitSettingsToCurrentValue()
{
	if((maxLength > 0) && (dataValue.length() > (size_t)maxLength))
	{
		dataValue.resize((size_t)maxLength);
	}
}
