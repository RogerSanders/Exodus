#include "GenericAccessDataValueString.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueString::GenericAccessDataValueString(const std::wstring& value)
:dataValue(value)
{
	maxLength = 0;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueString::GetIGenericAccessDataValueStringVersion() const
{
	return ThisIGenericAccessDataValueStringVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueString::DataType GenericAccessDataValueString::GetType() const
{
	return DataType::String;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessDataValueString::GetValue() const
{
	return dataValue;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessDataValueString::GetValueString() const
{
	return dataValue;
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueString::SetValueString(const MarshalSupport::Marshal::In<std::wstring>& value)
{
	dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueString::GetMaxLength() const
{
	return maxLength;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueString::SetMaxLength(unsigned int state)
{
	maxLength = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueString::ApplyLimitSettingsToCurrentValue()
{
	if((maxLength > 0) && (dataValue.length() > (size_t)maxLength))
	{
		dataValue.resize((size_t)maxLength);
	}
}
