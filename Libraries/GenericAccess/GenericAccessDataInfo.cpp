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
	return id;
}

//----------------------------------------------------------------------------------------
IGenericAccessDataValue::DataType GenericAccessDataInfo::GetType() const
{
	return dataType;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataInfo::GetReadOnly() const
{
	return readOnly;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataInfo::GetLockingSupported() const
{
	return lockingSupported;
}

//----------------------------------------------------------------------------------------
//Data display mode functions
//----------------------------------------------------------------------------------------
IGenericAccessDataValue::IntDisplayMode GenericAccessDataInfo::GetIntDisplayMode() const
{
	return intDisplayMode;
}

//----------------------------------------------------------------------------------------
IGenericAccessDataValue::FloatDisplayMode GenericAccessDataInfo::GetFloatDisplayMode() const
{
	return floatDisplayMode;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetMinWholeNumberChars() const
{
	return minWholeNumberChars;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetMinFractionalNumberChars() const
{
	return minFractionalNumberChars;
}

//----------------------------------------------------------------------------------------
//Data limit functions
//----------------------------------------------------------------------------------------
int GenericAccessDataInfo::GetIntMinValue() const
{
	return intMinValue;
}

//----------------------------------------------------------------------------------------
int GenericAccessDataInfo::GetIntMaxValue() const
{
	return intMaxValue;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetUIntMinValue() const
{
	return uintMinValue;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetUIntMaxValue() const
{
	return uintMaxValue;
}

//----------------------------------------------------------------------------------------
float GenericAccessDataInfo::GetFloatMinValue() const
{
	return floatMinValue;
}

//----------------------------------------------------------------------------------------
float GenericAccessDataInfo::GetFloatMaxValue() const
{
	return floatMaxValue;
}

//----------------------------------------------------------------------------------------
double GenericAccessDataInfo::GetDoubleMinValue() const
{
	return doubleMinValue;
}

//----------------------------------------------------------------------------------------
double GenericAccessDataInfo::GetDoubleMaxValue() const
{
	return doubleMaxValue;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataInfo::GetStringMaxLength() const
{
	return maxStringLength;
}

//----------------------------------------------------------------------------------------
//File path settings
//----------------------------------------------------------------------------------------
bool GenericAccessDataInfo::GetFilePathCreatingTarget() const
{
	return filePathCreatingTarget;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataInfo::GetFilePathAllowScanningIntoArchives() const
{
	return filePathAllowScanningIntoArchives;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessDataInfo::GetFilePathExtensionFilter() const
{
	return filePathExtensionFilter;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessDataInfo::GetFilePathDefaultExtension() const
{
	return filePathDefaultExtension;
}

//----------------------------------------------------------------------------------------
//Folder path settings
//----------------------------------------------------------------------------------------
bool GenericAccessDataInfo::GetFolderPathCreatingTarget() const
{
	return folderPathCreatingTarget;
}
