#ifndef __GENERICACCESSDATAINFO_H__
#define __GENERICACCESSDATAINFO_H__
#include "IGenericAccessDataInfo.h"
#include <string>

class GenericAccessDataInfo :public IGenericAccessDataInfo
{
public:
	// Constructors
	template<class T> inline GenericAccessDataInfo(T id, IGenericAccessDataValue::DataType type);

	// Interface version functions
	virtual unsigned int GetIGenericAccessDataInfoVersion() const;

	// Data info functions
	virtual unsigned int GetID() const;
	inline GenericAccessDataInfo* SetID(unsigned int id);
	virtual IGenericAccessDataValue::DataType GetType() const;
	inline GenericAccessDataInfo* SetType(IGenericAccessDataValue::DataType type);
	virtual bool GetReadOnly() const;
	inline GenericAccessDataInfo* SetReadOnly(bool readOnly);
	virtual bool GetLockingSupported() const;
	inline GenericAccessDataInfo* SetLockingSupported(bool lockingSupported);
	virtual bool GetHighlightUsed() const;
	inline GenericAccessDataInfo* SetHighlightUsed(bool highlightUsed);

	// Data display mode functions
	virtual IGenericAccessDataValue::IntDisplayMode GetIntDisplayMode() const;
	inline GenericAccessDataInfo* SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode state);
	virtual IGenericAccessDataValue::FloatDisplayMode GetFloatDisplayMode() const;
	inline GenericAccessDataInfo* SetFloatDisplayMode(IGenericAccessDataValue::FloatDisplayMode state);
	virtual unsigned int GetMinWholeNumberChars() const;
	inline GenericAccessDataInfo* SetMinWholeNumberChars(unsigned int state);
	virtual unsigned int GetMinFractionalNumberChars() const;
	inline GenericAccessDataInfo* SetMinFractionalNumberChars(unsigned int state);

	// Data limit functions
	virtual int GetIntMinValue() const;
	inline GenericAccessDataInfo* SetIntMinValue(int state);
	virtual int GetIntMaxValue() const;
	inline GenericAccessDataInfo* SetIntMaxValue(int state);
	virtual unsigned int GetUIntMinValue() const;
	inline GenericAccessDataInfo* SetUIntMinValue(unsigned int state);
	virtual unsigned int GetUIntMaxValue() const;
	inline GenericAccessDataInfo* SetUIntMaxValue(unsigned int state);
	virtual float GetFloatMinValue() const;
	inline GenericAccessDataInfo* SetFloatMinValue(float state);
	virtual float GetFloatMaxValue() const;
	inline GenericAccessDataInfo* SetFloatMaxValue(float state);
	virtual double GetDoubleMinValue() const;
	inline GenericAccessDataInfo* SetDoubleMinValue(double state);
	virtual double GetDoubleMaxValue() const;
	inline GenericAccessDataInfo* SetDoubleMaxValue(double state);
	virtual unsigned int GetStringMaxLength() const;
	inline GenericAccessDataInfo* SetStringMaxLength(unsigned int state);

	// File path settings
	virtual bool GetFilePathCreatingTarget() const;
	inline GenericAccessDataInfo* SetFilePathCreatingTarget(bool state);
	virtual bool GetFilePathAllowScanningIntoArchives() const;
	inline GenericAccessDataInfo* SetFilePathAllowScanningIntoArchives(bool state);
	virtual Marshal::Ret<std::wstring> GetFilePathExtensionFilter() const;
	inline GenericAccessDataInfo* SetFilePathExtensionFilter(const std::wstring& filePathExtensionFilter);
	virtual Marshal::Ret<std::wstring> GetFilePathDefaultExtension() const;
	inline GenericAccessDataInfo* SetFilePathDefaultExtension(const std::wstring& filePathDefaultExtension);

	// Folder path settings
	virtual bool GetFolderPathCreatingTarget() const;
	inline GenericAccessDataInfo* SetFolderPathCreatingTarget(bool state);

private:
	unsigned int _id;
	IGenericAccessDataValue::DataType _dataType;
	bool _readOnly;
	bool _lockingSupported;
	bool _highlightUsed;

	// Display mode state
	IGenericAccessDataValue::IntDisplayMode _intDisplayMode;
	IGenericAccessDataValue::FloatDisplayMode _floatDisplayMode;
	unsigned int _minWholeNumberChars;
	unsigned int _minFractionalNumberChars;

	// Data limit state
	int _intMinValue;
	int _intMaxValue;
	unsigned int _uintMinValue;
	unsigned int _uintMaxValue;
	float _floatMinValue;
	float _floatMaxValue;
	double _doubleMinValue;
	double _doubleMaxValue;
	unsigned int _maxStringLength;

	// File path settings
	bool _filePathCreatingTarget;
	bool _filePathAllowScanningIntoArchives;
	std::wstring _filePathExtensionFilter;
	std::wstring _filePathDefaultExtension;

	// Folder path settings
	bool _folderPathCreatingTarget;
};

#include "GenericAccessDataInfo.inl"
#endif
