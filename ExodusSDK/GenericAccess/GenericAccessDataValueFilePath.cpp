#include "GenericAccessDataValueFilePath.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueFilePath::GenericAccessDataValueFilePath(const std::wstring& value)
:_dataValue(value)
{
	_maxLength = 0;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFilePath::GetIGenericAccessDataValueFilePathVersion() const
{
	return ThisIGenericAccessDataValueFilePathVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueFilePath::DataType GenericAccessDataValueFilePath::GetType() const
{
	return DataType::FilePath;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataValueFilePath::GetValue() const
{
	return _dataValue;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataValueFilePath::GetValueString() const
{
	return GetValue();
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueFilePath::SetValueString(const Marshal::In<std::wstring>& value)
{
	return SetValueFilePath(value);
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueFilePath::SetValueFilePath(const Marshal::In<std::wstring>& value)
{
	_dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFilePath::GetMaxLength() const
{
	return _maxLength;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueFilePath::SetMaxLength(unsigned int state)
{
	_maxLength = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueFilePath::ApplyLimitSettingsToCurrentValue()
{
	if((_maxLength > 0) && (_dataValue.length() > (size_t)_maxLength))
	{
		_dataValue.resize((size_t)_maxLength);
	}
}
