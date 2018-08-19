#include "GenericAccessDataValueDouble.h"
#include "DataConversion/DataConversion.pkg"
#include <limits>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueDouble::GenericAccessDataValueDouble(double value)
:_dataValue(value)
{
	_displayMode = FloatDisplayMode::Fixed;
	_minWholeNumberChars = 0;
	_minFractionalNumberChars = 0;
	_minValue = std::numeric_limits<double>::min();
	_maxValue = std::numeric_limits<double>::max();
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
	return DataType::Double;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
double GenericAccessDataValueDouble::GetValue() const
{
	return _dataValue;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataValueDouble::GetValueString() const
{
	std::wstring result;
	bool useScientificNotation = (_displayMode == FloatDisplayMode::Scientific);
	DoubleToString(_dataValue, result, useScientificNotation, _minFractionalNumberChars, _minWholeNumberChars);
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
	_dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueDouble::SetValueString(const Marshal::In<std::wstring>& value)
{
	double valueConverted;
	if (!StringToDouble(value, valueConverted))
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
	return _displayMode;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::SetDisplayMode(FloatDisplayMode state)
{
	_displayMode = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueDouble::GetMinWholeNumberChars() const
{
	return _minWholeNumberChars;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::SetMinWholeNumberChars(unsigned int state)
{
	_minWholeNumberChars = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueDouble::GetMinFractionalNumberChars() const
{
	return _minFractionalNumberChars;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::SetMinFractionalNumberChars(unsigned int state)
{
	_minFractionalNumberChars = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
double GenericAccessDataValueDouble::GetMinValue() const
{
	return _minValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::SetMinValue(double state)
{
	_minValue = state;
}

//----------------------------------------------------------------------------------------
double GenericAccessDataValueDouble::GetMaxValue() const
{
	return _maxValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::SetMaxValue(double state)
{
	_maxValue = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueDouble::ApplyLimitSettingsToCurrentValue()
{
	_dataValue = (_dataValue < _minValue)? _minValue: _dataValue;
	_dataValue = (_dataValue > _maxValue)? _maxValue: _dataValue;
}
