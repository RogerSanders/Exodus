#include "GenericAccessDataValueDouble.h"
#include "DataConversion/DataConversion.pkg"
#include <limits>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueDouble::GenericAccessDataValueDouble(double value)
:dataValue(value)
{
	displayMode = FLOATDISPLAYMODE_FIXED;
	minWholeNumberChars = 0;
	minFractionalNumberChars = 0;
	minValue = std::numeric_limits<double>::min();
	maxValue = std::numeric_limits<double>::max();
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueDouble::GetIGenericAccessDataValueDoubleVersion() const
{
	return ThisIGenericAccessDataValueDoubleVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueDouble::DataType GenericAccessDataValueDouble::GetType() const
{
	return DATATYPE_DOUBLE;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
double GenericAccessDataValueDouble::GetValue() const
{
	return dataValue;
}

//----------------------------------------------------------------------------------------
std::wstring GenericAccessDataValueDouble::GetValueString() const
{
	std::wstring result;
	bool useScientificNotation = (displayMode == FLOATDISPLAYMODE_SCIENTIFIC);
	DoubleToString(dataValue, result, useScientificNotation, minFractionalNumberChars, minWholeNumberChars);
	return result;
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueDouble::SetValueFloat(float value)
{
	return SetValueDouble((double)value);
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueDouble::SetValueDouble(double value)
{
	dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueDouble::SetValueString(const std::wstring& value)
{
	double valueConverted;
	if(!StringToDouble(value, valueConverted))
	{
		return false;
	}
	return SetValueDouble(valueConverted);
}

//----------------------------------------------------------------------------------------
//Value display functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueDouble::FloatDisplayMode GenericAccessDataValueDouble::GetDisplayMode() const
{
	return displayMode;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::SetDisplayMode(FloatDisplayMode state)
{
	displayMode = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueDouble::GetMinWholeNumberChars() const
{
	return minWholeNumberChars;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::SetMinWholeNumberChars(unsigned int state)
{
	minWholeNumberChars = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueDouble::GetMinFractionalNumberChars() const
{
	return minFractionalNumberChars;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::SetMinFractionalNumberChars(unsigned int state)
{
	minFractionalNumberChars = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
double GenericAccessDataValueDouble::GetMinValue() const
{
	return minValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::SetMinValue(double state)
{
	minValue = state;
}

//----------------------------------------------------------------------------------------
double GenericAccessDataValueDouble::GetMaxValue() const
{
	return maxValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::SetMaxValue(double state)
{
	maxValue = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::ApplyLimitSettingsToCurrentValue()
{
	dataValue = (dataValue < minValue)? minValue: dataValue;
	dataValue = (dataValue > maxValue)? maxValue: dataValue;
}
