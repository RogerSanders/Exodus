#include <limits>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> GenericAccessDataInfo::GenericAccessDataInfo(T aid, IGenericAccessDataValue::DataType atype)
:id((unsigned int)aid), dataType(atype)
{
	//Initialize the general data info to defaults
	lockingSupported = false;
	readOnly = false;

	//Initialize the display mode state to defaults
	intDisplayMode = IGenericAccessDataValue::IntDisplayMode::Decimal;
	floatDisplayMode = IGenericAccessDataValue::FloatDisplayMode::Fixed;
	minWholeNumberChars = 0;
	minFractionalNumberChars = 0;

	//Initialize the data limit state to defaults
	intMinValue = std::numeric_limits<int>::min();
	intMaxValue = std::numeric_limits<int>::max();
	uintMinValue = std::numeric_limits<unsigned int>::min();
	uintMaxValue = std::numeric_limits<unsigned int>::max();
	floatMinValue = std::numeric_limits<float>::min();
	floatMaxValue = std::numeric_limits<float>::max();
	doubleMinValue = std::numeric_limits<double>::min();
	doubleMaxValue = std::numeric_limits<double>::max();
	maxStringLength = 0;

	//Initialize the file path settings to defaults
	filePathCreatingTarget = false;
	filePathAllowScanningIntoArchives = false;

	//Folder path settings
	folderPathCreatingTarget = false;
}

//----------------------------------------------------------------------------------------
//Data info functions
//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetID(unsigned int aid)
{
	id = aid;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetType(IGenericAccessDataValue::DataType atype)
{
	dataType = atype;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetReadOnly(bool areadOnly)
{
	readOnly = areadOnly;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetLockingSupported(bool alockingSupported)
{
	lockingSupported = alockingSupported;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetHighlightUsed(bool ahighlightUsed)
{
	highlightUsed = ahighlightUsed;
	return this;
}

//----------------------------------------------------------------------------------------
//Data display mode functions
//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode state)
{
	intDisplayMode = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFloatDisplayMode(IGenericAccessDataValue::FloatDisplayMode state)
{
	floatDisplayMode = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetMinWholeNumberChars(unsigned int state)
{
	minWholeNumberChars = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetMinFractionalNumberChars(unsigned int state)
{
	minFractionalNumberChars = state;
	return this;
}

//----------------------------------------------------------------------------------------
//Data limit functions
//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetIntMinValue(int state)
{
	intMinValue = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetIntMaxValue(int state)
{
	intMaxValue = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetUIntMinValue(unsigned int state)
{
	uintMinValue = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetUIntMaxValue(unsigned int state)
{
	uintMaxValue = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFloatMinValue(float state)
{
	floatMinValue = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFloatMaxValue(float state)
{
	floatMaxValue = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetDoubleMinValue(double state)
{
	doubleMinValue = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetDoubleMaxValue(double state)
{
	doubleMaxValue = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetStringMaxLength(unsigned int state)
{
	maxStringLength = state;
	return this;
}

//----------------------------------------------------------------------------------------
//File path settings
//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFilePathCreatingTarget(bool state)
{
	filePathCreatingTarget = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFilePathAllowScanningIntoArchives(bool state)
{
	filePathAllowScanningIntoArchives = state;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFilePathExtensionFilter(const std::wstring& afilePathExtensionFilter)
{
	filePathExtensionFilter = afilePathExtensionFilter;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFilePathDefaultExtension(const std::wstring& afilePathDefaultExtension)
{
	filePathDefaultExtension = afilePathDefaultExtension;
	return this;
}

//----------------------------------------------------------------------------------------
//Folder path settings
//----------------------------------------------------------------------------------------
GenericAccessDataInfo* GenericAccessDataInfo::SetFolderPathCreatingTarget(bool state)
{
	folderPathCreatingTarget = state;
}
