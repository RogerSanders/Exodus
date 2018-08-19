#include "GenericAccessDataValueFolderPath.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueFolderPath::GenericAccessDataValueFolderPath(const std::wstring& value)
:_dataValue(value)
{
	_maxLength = 0;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFolderPath::GetIGenericAccessDataValueFolderPathVersion() const
{
	return ThisIGenericAccessDataValueFolderPathVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueFolderPath::DataType GenericAccessDataValueFolderPath::GetType() const
{
	return DataType::FolderPath;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataValueFolderPath::GetValue() const
{
	return _dataValue;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataValueFolderPath::GetValueString() const
{
	return GetValue();
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueFolderPath::SetValueString(const Marshal::In<std::wstring>& value)
{
	return SetValueFolderPath(value);
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueFolderPath::SetValueFolderPath(const Marshal::In<std::wstring>& value)
{
	_dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueFolderPath::GetMaxLength() const
{
	return _maxLength;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueFolderPath::SetMaxLength(unsigned int state)
{
	_maxLength = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueFolderPath::ApplyLimitSettingsToCurrentValue()
{
	if ((_maxLength > 0) && (_dataValue.length() > (size_t)_maxLength))
	{
		_dataValue.resize((size_t)_maxLength);
	}
}
