#include "GenericAccessDataValueFloat.h"
#include "DataConversion/DataConversion.pkg"
#include <limits>

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataValueFloat::GenericAccessDataValueFloat(float value)
:_dataValue(value)
{
	_displayMode = FloatDisplayMode::Fixed;
	_minWholeNumberChars = 0;
	_minFractionalNumberChars = 0;
	_minValue = std::numeric_limits<float>::min();
	_maxValue = std::numeric_limits<float>::max();
}

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFloat::GetIGenericAccessDataValueFloatVersion() const
{
	return ThisIGenericAccessDataValueFloatVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Type functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataValueFloat::DataType GenericAccessDataValueFloat::GetType() const
{
	return DataType::Float;
}

//----------------------------------------------------------------------------------------------------------------------
// Value read functions
//----------------------------------------------------------------------------------------------------------------------
float GenericAccessDataValueFloat::GetValue() const
{
	return _dataValue;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataValueFloat::GetValueString() const
{
	std::wstring result;
	bool useScientificNotation = (_displayMode == FloatDisplayMode::Scientific);
	FloatToString(_dataValue, result, useScientificNotation, _minFractionalNumberChars, _minWholeNumberChars);
	return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Value write functions
//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessDataValueFloat::SetValueFloat(float value)
{
	_dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessDataValueFloat::SetValueDouble(double value)
{
	return SetValueFloat((float)value);
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessDataValueFloat::SetValueString(const Marshal::In<std::wstring>& value)
{
	float valueConverted;
	if (!StringToFloat(value, valueConverted))
	{
		return false;
	}
	return SetValueFloat(valueConverted);
}

//----------------------------------------------------------------------------------------------------------------------
// Value display functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataValueFloat::FloatDisplayMode GenericAccessDataValueFloat::GetDisplayMode() const
{
	return _displayMode;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::SetDisplayMode(FloatDisplayMode state)
{
	_displayMode = state;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFloat::GetMinWholeNumberChars() const
{
	return _minWholeNumberChars;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::SetMinWholeNumberChars(unsigned int state)
{
	_minWholeNumberChars = state;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFloat::GetMinFractionalNumberChars() const
{
	return _minFractionalNumberChars;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::SetMinFractionalNumberChars(unsigned int state)
{
	_minFractionalNumberChars = state;
}

//----------------------------------------------------------------------------------------------------------------------
// Value limit functions
//----------------------------------------------------------------------------------------------------------------------
float GenericAccessDataValueFloat::GetMinValue() const
{
	return _minValue;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::SetMinValue(float state)
{
	_minValue = state;
}

//----------------------------------------------------------------------------------------------------------------------
float GenericAccessDataValueFloat::GetMaxValue() const
{
	return _maxValue;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::SetMaxValue(float state)
{
	_maxValue = state;
}

//----------------------------------------------------------------------------------------------------------------------
// Value limit functions
//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueFloat::ApplyLimitSettingsToCurrentValue()
{
	_dataValue = (_dataValue < _minValue)? _minValue: _dataValue;
	_dataValue = (_dataValue > _maxValue)? _maxValue: _dataValue;
}
