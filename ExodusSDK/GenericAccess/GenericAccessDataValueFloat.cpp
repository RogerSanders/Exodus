#include "GenericAccessDataValueFloat.h"
#include "DataConversion/DataConversion.pkg"
#include <limits>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueFloat::GenericAccessDataValueFloat(float value)
:dataValue(value)
{
	displayMode = FloatDisplayMode::Fixed;
	minWholeNumberChars = 0;
	minFractionalNumberChars = 0;
	minValue = std::numeric_limits<float>::min();
	maxValue = std::numeric_limits<float>::max();
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFloat::GetIGenericAccessDataValueFloatVersion() const
{
	return ThisIGenericAccessDataValueFloatVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueFloat::DataType GenericAccessDataValueFloat::GetType() const
{
	return DataType::Float;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
float GenericAccessDataValueFloat::GetValue() const
{
	return dataValue;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessDataValueFloat::GetValueString() const
{
	std::wstring result;
	bool useScientificNotation = (displayMode == FloatDisplayMode::Scientific);
	FloatToString(dataValue, result, useScientificNotation, minFractionalNumberChars, minWholeNumberChars);
	return result;
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueFloat::SetValueFloat(float value)
{
	dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueFloat::SetValueDouble(double value)
{
	return SetValueFloat((float)value);
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueFloat::SetValueString(const MarshalSupport::Marshal::In<std::wstring>& value)
{
	float valueConverted;
	if(!StringToFloat(value, valueConverted))
	{
		return false;
	}
	return SetValueFloat(valueConverted);
}

//----------------------------------------------------------------------------------------
//Value display functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueFloat::FloatDisplayMode GenericAccessDataValueFloat::GetDisplayMode() const
{
	return displayMode;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::SetDisplayMode(FloatDisplayMode state)
{
	displayMode = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFloat::GetMinWholeNumberChars() const
{
	return minWholeNumberChars;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::SetMinWholeNumberChars(unsigned int state)
{
	minWholeNumberChars = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFloat::GetMinFractionalNumberChars() const
{
	return minFractionalNumberChars;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::SetMinFractionalNumberChars(unsigned int state)
{
	minFractionalNumberChars = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
float GenericAccessDataValueFloat::GetMinValue() const
{
	return minValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::SetMinValue(float state)
{
	minValue = state;
}

//----------------------------------------------------------------------------------------
float GenericAccessDataValueFloat::GetMaxValue() const
{
	return maxValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::SetMaxValue(float state)
{
	maxValue = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::ApplyLimitSettingsToCurrentValue()
{
	dataValue = (dataValue < minValue)? minValue: dataValue;
	dataValue = (dataValue > maxValue)? maxValue: dataValue;
}
