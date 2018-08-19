#include "GenericAccessDataValueString.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataValueString::GenericAccessDataValueString(const std::wstring& value)
:_dataValue(value)
{
	_maxLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueString::GetIGenericAccessDataValueStringVersion() const
{
	return ThisIGenericAccessDataValueStringVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Type functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataValueString::DataType GenericAccessDataValueString::GetType() const
{
	return DataType::String;
}

//----------------------------------------------------------------------------------------------------------------------
// Value read functions
//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataValueString::GetValue() const
{
	return _dataValue;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataValueString::GetValueString() const
{
	return _dataValue;
}

//----------------------------------------------------------------------------------------------------------------------
// Value write functions
//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessDataValueString::SetValueString(const Marshal::In<std::wstring>& value)
{
	_dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Value limit functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueString::GetMaxLength() const
{
	return _maxLength;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueString::SetMaxLength(unsigned int state)
{
	_maxLength = state;
}

//----------------------------------------------------------------------------------------------------------------------
// Value limit functions
//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueString::ApplyLimitSettingsToCurrentValue()
{
	if ((_maxLength > 0) && (_dataValue.length() > (size_t)_maxLength))
	{
		_dataValue.resize((size_t)_maxLength);
	}
}
