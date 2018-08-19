#include "GenericAccessDataInfo.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetIGenericAccessDataInfoVersion() const
{
	return ThisIGenericAccessDataInfoVersion();
}

//----------------------------------------------------------------------------------------
//Data info functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetID() const
{
	return _id;
}

//----------------------------------------------------------------------------------------
IGenericAccessDataValue::DataType GenericAccessDataInfo::GetType() const
{
	return _dataType;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataInfo::GetReadOnly() const
{
	return _readOnly;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataInfo::GetLockingSupported() const
{
	return _lockingSupported;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataInfo::GetHighlightUsed() const
{
	return _highlightUsed;
}

//----------------------------------------------------------------------------------------
//Data display mode functions
//----------------------------------------------------------------------------------------
IGenericAccessDataValue::IntDisplayMode GenericAccessDataInfo::GetIntDisplayMode() const
{
	return _intDisplayMode;
}

//----------------------------------------------------------------------------------------
IGenericAccessDataValue::FloatDisplayMode GenericAccessDataInfo::GetFloatDisplayMode() const
{
	return _floatDisplayMode;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetMinWholeNumberChars() const
{
	return _minWholeNumberChars;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetMinFractionalNumberChars() const
{
	return _minFractionalNumberChars;
}

//----------------------------------------------------------------------------------------
//Data limit functions
//----------------------------------------------------------------------------------------
int GenericAccessDataInfo::GetIntMinValue() const
{
	return _intMinValue;
}

//----------------------------------------------------------------------------------------
int GenericAccessDataInfo::GetIntMaxValue() const
{
	return _intMaxValue;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetUIntMinValue() const
{
	return _uintMinValue;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetUIntMaxValue() const
{
	return _uintMaxValue;
}

//----------------------------------------------------------------------------------------
float GenericAccessDataInfo::GetFloatMinValue() const
{
	return _floatMinValue;
}

//----------------------------------------------------------------------------------------
float GenericAccessDataInfo::GetFloatMaxValue() const
{
	return _floatMaxValue;
}

//----------------------------------------------------------------------------------------
double GenericAccessDataInfo::GetDoubleMinValue() const
{
	return _doubleMinValue;
}

//----------------------------------------------------------------------------------------
double GenericAccessDataInfo::GetDoubleMaxValue() const
{
	return _doubleMaxValue;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetStringMaxLength() const
{
	return _maxStringLength;
}

//----------------------------------------------------------------------------------------
//File path settings
//----------------------------------------------------------------------------------------
bool GenericAccessDataInfo::GetFilePathCreatingTarget() const
{
	return _filePathCreatingTarget;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataInfo::GetFilePathAllowScanningIntoArchives() const
{
	return _filePathAllowScanningIntoArchives;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataInfo::GetFilePathExtensionFilter() const
{
	return _filePathExtensionFilter;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessDataInfo::GetFilePathDefaultExtension() const
{
	return _filePathDefaultExtension;
}

//----------------------------------------------------------------------------------------
//Folder path settings
//----------------------------------------------------------------------------------------
bool GenericAccessDataInfo::GetFolderPathCreatingTarget() const
{
	return _folderPathCreatingTarget;
}
