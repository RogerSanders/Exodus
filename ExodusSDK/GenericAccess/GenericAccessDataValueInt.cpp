#include "GenericAccessDataValueInt.h"
#include "DataConversion/DataConversion.pkg"
#include <limits>
#include <stdio.h>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueInt::GenericAccessDataValueInt(int value)
:_dataValue(value)
{
	_displayMode = IntDisplayMode::Decimal;
	_minChars = 0;
	_minValue = std::numeric_limits<int>::min();
	_maxValue = std::numeric_limits<int>::max();
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueInt::GetIGenericAccessDataValueIntVersion() const
{
	return ThisIGenericAccessDataValueIntVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueInt::DataType GenericAccessDataValueInt::GetType() const
{
	return DataType::Int;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
int GenericAccessDataValueInt::GetValue() const
{
	return _dataValue;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataValueInt::GetValueString() const
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

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueInt::SetValueInt(int value)
{
	_dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueInt::SetValueUInt(unsigned int value)
{
	return SetValueInt((int)value);
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueInt::SetValueString(const Marshal::In<std::wstring>& value)
{
	//Calculate the default base to use for the specified number based on the specified
	//display mode
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

	//Attempt to convert the string to an integer
	int valueConverted;
	if (!StringToInt(value, valueConverted, defaultBase))
	{
		return false;
	}

	//Attempt to set this value to the specified integer
	return SetValueInt(valueConverted);
}

//----------------------------------------------------------------------------------------
//Value display functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueInt::IntDisplayMode GenericAccessDataValueInt::GetDisplayMode() const
{
	return _displayMode;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueInt::SetDisplayMode(IntDisplayMode state)
{
	_displayMode = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueInt::GetMinChars() const
{
	return _minChars;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueInt::SetMinChars(unsigned int state)
{
	_minChars = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueInt::CalculateDisplayChars(IntDisplayMode displayMode, int minValue, int maxValue) const
{
	//Build strings for the max and min values
	std::wstring minValueString;
	std::wstring maxValueString;
	switch (displayMode)
	{
	case IntDisplayMode::Binary:
		IntToStringBase2(abs(minValue), minValueString, 0, false);
		IntToStringBase2(abs(maxValue), maxValueString, 0, false);
		break;
	case IntDisplayMode::Octal:
		IntToStringBase8(abs(minValue), minValueString, 0, false);
		IntToStringBase8(abs(maxValue), maxValueString, 0, false);
		break;
	case IntDisplayMode::Decimal:
		IntToStringBase10(abs(minValue), minValueString, 0, false);
		IntToStringBase10(abs(maxValue), maxValueString, 0, false);
		break;
	case IntDisplayMode::Hexadecimal:
		IntToStringBase16(abs(minValue), minValueString, 0, false);
		IntToStringBase16(abs(maxValue), maxValueString, 0, false);
		break;
	}

	//Calculate the minimum number of chars to be able to hold any value within the
	//maximum and minimum range
	unsigned int displayChars = (minValueString.length() > maxValueString.length())? (unsigned int)minValueString.length(): (unsigned int)maxValueString.length();
	return displayChars;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
int GenericAccessDataValueInt::GetMinValue() const
{
	return _minValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueInt::SetMinValue(int state)
{
	_minValue = state;
}

//----------------------------------------------------------------------------------------
int GenericAccessDataValueInt::GetMaxValue() const
{
	return _maxValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueInt::SetMaxValue(int state)
{
	_maxValue = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueInt::ApplyLimitSettingsToCurrentValue()
{
	_dataValue = (_dataValue < _minValue)? _minValue: _dataValue;
	_dataValue = (_dataValue > _maxValue)? _maxValue: _dataValue;
}
