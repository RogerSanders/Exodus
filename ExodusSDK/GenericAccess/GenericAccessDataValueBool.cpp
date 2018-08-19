#include "GenericAccessDataValueBool.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueBool::GenericAccessDataValueBool(bool value)
:_dataValue(value)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueBool::GetIGenericAccessDataValueBoolVersion() const
{
	return ThisIGenericAccessDataValueBoolVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueBool::DataType GenericAccessDataValueBool::GetType() const
{
	return DataType::Bool;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueBool::GetValue() const
{
	return _dataValue;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataValueBool::GetValueString() const
{
	std::wstring result;
	BoolToString(_dataValue, result);
	return result;
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueBool::SetValueBool(bool value)
{
	_dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueBool::SetValueString(const Marshal::In<std::wstring>& value)
{
	bool valueConverted;
	if (!StringToBool(value, valueConverted))
	{
		return false;
	}
	return SetValueBool(valueConverted);
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueBool::ApplyLimitSettingsToCurrentValue()
{}
