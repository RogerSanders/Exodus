#include "GenericAccessDataValueBool.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueBool::GenericAccessDataValueBool(bool value)
:dataValue(value)
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
	return DATATYPE_BOOL;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueBool::GetValue() const
{
	return dataValue;
}

//----------------------------------------------------------------------------------------
std::wstring GenericAccessDataValueBool::GetValueString() const
{
	std::wstring result;
	BoolToString(dataValue, result);
	return result;
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueBool::SetValueBool(bool value)
{
	dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueBool::SetValueString(const std::wstring& value)
{
	bool valueConverted;
	if(!StringToBool(value, valueConverted))
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
