#include "GenericAccessDataValueUInt.h"
#include "DataConversion/DataConversion.pkg"
#include <limits>

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataValueUInt::GenericAccessDataValueUInt(unsigned int value)
:_dataValue(value)
{
	_displayMode = IntDisplayMode::Decimal;
	_minChars = 0;
	_minValue = std::numeric_limits<unsigned int>::min();
	_maxValue = std::numeric_limits<unsigned int>::max();
}

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueUInt::GetIGenericAccessDataValueUIntVersion() const
{
	return ThisIGenericAccessDataValueUIntVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Type functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataValueUInt::DataType GenericAccessDataValueUInt::GetType() const
{
	return DataType::UInt;
}

//----------------------------------------------------------------------------------------------------------------------
// Value read functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueUInt::GetValue() const
{
	return _dataValue;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataValueUInt::GetValueString() const
{
	std::wstring result;
	switch (_displayMode)
	{
	case IntDisplayMode::Binary:
		IntToStringBase2(_dataValue, result, _minChars);
		break;
	case IntDisplayMode::Octal:
		IntToStringBase8(_dataValue, result, _minChars);
		break;
	case IntDisplayMode::Decimal:
		IntToStringBase10(_dataValue, result, _minChars);
		break;
	case IntDisplayMode::Hexadecimal:
		IntToStringBase16(_dataValue, result, _minChars);
		break;
	}
	return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Value write functions
//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessDataValueUInt::SetValueInt(int value)
{
	return SetValueUInt((unsigned int)value);
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessDataValueUInt::SetValueUInt(unsigned int value)
{
	_dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessDataValueUInt::SetValueString(const Marshal::In<std::wstring>& value)
{
	// Calculate the default base to use for the specified number based on the specified
	// display mode
	unsigned int defaultBase = 10;
	switch (_displayMode)
	{
	case IntDisplayMode::Binary:
		defaultBase = 2;
		break;
	case IntDisplayMode::Octal:
		defaultBase = 8;
		break;
	case IntDisplayMode::Decimal:
		defaultBase = 10;
		break;
	case IntDisplayMode::Hexadecimal:
		defaultBase = 16;
		break;
	}

	// Attempt to convert the string to an integer
	unsigned int valueConverted;
	if (!StringToInt(value, valueConverted, defaultBase))
	{
		return false;
	}

	// Attempt to set this value to the specified integer
	return SetValueUInt(valueConverted);
}

//----------------------------------------------------------------------------------------------------------------------
// Value display functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataValueUInt::IntDisplayMode GenericAccessDataValueUInt::GetDisplayMode() const
{
	return _displayMode;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueUInt::SetDisplayMode(IntDisplayMode state)
{
	_displayMode = state;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueUInt::GetMinChars() const
{
	return _minChars;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueUInt::SetMinChars(unsigned int state)
{
	_minChars = state;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueUInt::CalculateDisplayChars(IntDisplayMode displayMode, unsigned int minValue, unsigned int maxValue) const
{
	// If the display mode for this integer is set to decimal, shortcut any further
	// evaluation and return 1, since we default to using the minimum number of display
	// characters for decimal numbers.
	if (displayMode == IntDisplayMode::Decimal)
	{
		return 1;
	}

	// Build strings for the max and min values
	std::wstring minValueString;
	std::wstring maxValueString;
	switch (displayMode)
	{
	case IntDisplayMode::Binary:
		IntToStringBase2(minValue, minValueString, 0, false);
		IntToStringBase2(maxValue, maxValueString, 0, false);
		break;
	case IntDisplayMode::Octal:
		IntToStringBase8(minValue, minValueString, 0, false);
		IntToStringBase8(maxValue, maxValueString, 0, false);
		break;
	case IntDisplayMode::Hexadecimal:
		IntToStringBase16(minValue, minValueString, 0, false);
		IntToStringBase16(maxValue, maxValueString, 0, false);
		break;
	}

	// Calculate the minimum number of chars to be able to hold any value within the
	// maximum and minimum range
	unsigned int displayChars = (minValueString.length() > maxValueString.length())? (unsigned int)minValueString.length(): (unsigned int)maxValueString.length();
	return displayChars;
}

//----------------------------------------------------------------------------------------------------------------------
// Value limit functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueUInt::GetMinValue() const
{
	return _minValue;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueUInt::SetMinValue(unsigned int state)
{
	_minValue = state;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueUInt::GetMaxValue() const
{
	return _maxValue;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueUInt::SetMaxValue(unsigned int state)
{
	_maxValue = state;
}

//----------------------------------------------------------------------------------------------------------------------
// Value limit functions
//----------------------------------------------------------------------------------------------------------------------
void GenericAccessDataValueUInt::ApplyLimitSettingsToCurrentValue()
{
	_dataValue = (_dataValue < _minValue)? _minValue: _dataValue;
	_dataValue = (_dataValue > _maxValue)? _maxValue: _dataValue;
}
