#include <limits>

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
template<class T> GenericAccessDataInfo::GenericAccessDataInfo(T id, IGenericAccessDataValue::DataType type)
:_id((unsigned int)id), _dataType(type)
{
	// Initialize the general data info to defaults
	_lockingSupported = false;
	_readOnly = false;

	// Initialize the display mode state to defaults
	_intDisplayMode = IGenericAccessDataValue::IntDisplayMode::Decimal;
	_floatDisplayMode = IGenericAccessDataValue::FloatDisplayMode::Fixed;
	_minWholeNumberChars = 0;
	_minFractionalNumberChars = 0;

	// Initialize the data limit state to defaults
	_intMinValue = std::numeric_limits<int>::min();
	_intMaxValue = std::numeric_limits<int>::max();
	_uintMinValue = std::numeric_limits<unsigned int>::min();
	_uintMaxValue = std::numeric_limits<unsigned int>::max();
	_floatMinValue = std::numeric_limits<float>::min();
	_floatMaxValue = std::numeric_limits<float>::max();
	_doubleMinValue = std::numeric_limits<double>::min();
	_doubleMaxValue = std::numeric_limits<double>::max();
	_maxStringLength = 0;

	// Initialize the file path settings to defaults
	_filePathCreatingTarget = false;
	_filePathAllowScanningIntoArchives = false;

	// Folder path settings
	_folderPathCreatingTarget = false;
}

//----------------------------------------------------------------------------------------------------------------------
// Data info functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetID(unsigned int id)
{
	_id = id;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetType(IGenericAccessDataValue::DataType type)
{
	_dataType = type;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetReadOnly(bool readOnly)
{
	_readOnly = readOnly;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetLockingSupported(bool lockingSupported)
{
	_lockingSupported = lockingSupported;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetHighlightUsed(bool highlightUsed)
{
	_highlightUsed = highlightUsed;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
// Data display mode functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode state)
{
	_intDisplayMode = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFloatDisplayMode(IGenericAccessDataValue::FloatDisplayMode state)
{
	_floatDisplayMode = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetMinWholeNumberChars(unsigned int state)
{
	_minWholeNumberChars = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetMinFractionalNumberChars(unsigned int state)
{
	_minFractionalNumberChars = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
// Data limit functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetIntMinValue(int state)
{
	_intMinValue = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetIntMaxValue(int state)
{
	_intMaxValue = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetUIntMinValue(unsigned int state)
{
	_uintMinValue = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetUIntMaxValue(unsigned int state)
{
	_uintMaxValue = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFloatMinValue(float state)
{
	_floatMinValue = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFloatMaxValue(float state)
{
	_floatMaxValue = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetDoubleMinValue(double state)
{
	_doubleMinValue = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetDoubleMaxValue(double state)
{
	_doubleMaxValue = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetStringMaxLength(unsigned int state)
{
	_maxStringLength = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
// File path settings
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFilePathCreatingTarget(bool state)
{
	_filePathCreatingTarget = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFilePathAllowScanningIntoArchives(bool state)
{
	_filePathAllowScanningIntoArchives = state;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFilePathExtensionFilter(const std::wstring& filePathExtensionFilter)
{
	_filePathExtensionFilter = filePathExtensionFilter;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFilePathDefaultExtension(const std::wstring& filePathDefaultExtension)
{
	_filePathDefaultExtension = filePathDefaultExtension;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
// Folder path settings
//----------------------------------------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFolderPathCreatingTarget(bool state)
{
	_folderPathCreatingTarget = state;
}
