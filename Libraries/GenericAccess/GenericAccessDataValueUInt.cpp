#include "GenericAccessDataValueUInt.h"
#include "DataConversion/DataConversion.pkg"
#include <limits>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueUInt::GenericAccessDataValueUInt(unsigned int value)
:dataValue(value)
{
	displayMode = INTDISPLAYMODE_DECIMAL;
	minChars = 0;
	minValue = std::numeric_limits<unsigned int>::min();
	maxValue = std::numeric_limits<unsigned int>::max();
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueUInt::GetIGenericAccessDataValueUIntVersion() const
{
	return ThisIGenericAccessDataValueUIntVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueUInt::DataType GenericAccessDataValueUInt::GetType() const
{
	return DATATYPE_UINT;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueUInt::GetValue() const
{
	return dataValue;
}

//----------------------------------------------------------------------------------------
std::wstring GenericAccessDataValueUInt::GetValueString() const
{
	std::wstring result;
	switch(displayMode)
	{
	case INTDISPLAYMODE_BINARY:
		IntToStringBase2(dataValue, result, minChars);
		break;
	case INTDISPLAYMODE_OCTAL:
		IntToStringBase8(dataValue, result, minChars);
		break;
	case INTDISPLAYMODE_DECIMAL:
		IntToStringBase10(dataValue, result, minChars);
		break;
	case INTDISPLAYMODE_HEXADECIMAL:
		IntToStringBase16(dataValue, result, minChars);
		break;
	}
	return result;
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueUInt::SetValueInt(int value)
{
	return SetValueUInt((unsigned int)value);
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueUInt::SetValueUInt(unsigned int value)
{
	dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueUInt::SetValueString(const std::wstring& value)
{
	//Calculate the default base to use for the specified number based on the specified
	//display mode
	unsigned int defaultBase = 10;
	switch(displayMode)
	{
	case INTDISPLAYMODE_BINARY:
		defaultBase = 2;
		break;
	case INTDISPLAYMODE_OCTAL:
		defaultBase = 8;
		break;
	case INTDISPLAYMODE_DECIMAL:
		defaultBase = 10;
		break;
	case INTDISPLAYMODE_HEXADECIMAL:
		defaultBase = 16;
		break;
	}

	//Attempt to convert the string to an integer
	unsigned int valueConverted;
	if(!StringToInt(value, valueConverted, defaultBase))
	{
		return false;
	}

	//Attempt to set this value to the specified integer
	return SetValueUInt(valueConverted);
}

//----------------------------------------------------------------------------------------
//Value display functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueUInt::IntDisplayMode GenericAccessDataValueUInt::GetDisplayMode() const
{
	return displayMode;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueUInt::SetDisplayMode(IntDisplayMode state)
{
	displayMode = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueUInt::GetMinChars() const
{
	return minChars;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueUInt::SetMinChars(unsigned int state)
{
	minChars = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueUInt::CalculateDisplayChars(IntDisplayMode adisplayMode, unsigned int aminValue, unsigned int amaxValue) const
{
	//If the display mode for this integer is set to decimal, shortcut any further
	//evaluation and return 1, since we default to using the minimum number of display
	//characters for decimal numbers.
	if(adisplayMode == INTDISPLAYMODE_DECIMAL)
	{
		return 1;
	}

	//Build strings for the max and min values
	std::wstring minValueString;
	std::wstring maxValueString;
	switch(adisplayMode)
	{
	case INTDISPLAYMODE_BINARY:
		IntToStringBase2(aminValue, minValueString, 0, false);
		IntToStringBase2(amaxValue, maxValueString, 0, false);
		break;
	case INTDISPLAYMODE_OCTAL:
		IntToStringBase8(aminValue, minValueString, 0, false);
		IntToStringBase8(amaxValue, maxValueString, 0, false);
		break;
	case INTDISPLAYMODE_HEXADECIMAL:
		IntToStringBase16(aminValue, minValueString, 0, false);
		IntToStringBase16(amaxValue, maxValueString, 0, false);
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
unsigned int GenericAccessDataValueUInt::GetMinValue() const
{
	return minValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueUInt::SetMinValue(unsigned int state)
{
	minValue = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueUInt::GetMaxValue() const
{
	return maxValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueUInt::SetMaxValue(unsigned int state)
{
	maxValue = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueUInt::ApplyLimitSettingsToCurrentValue()
{
	dataValue = (dataValue < minValue)? minValue: dataValue;
	dataValue = (dataValue > maxValue)? maxValue: dataValue;
}
