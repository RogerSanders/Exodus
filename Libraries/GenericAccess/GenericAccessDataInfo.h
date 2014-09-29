#ifndef __GENERICACCESSDATAINFO_H__
#define __GENERICACCESSDATAINFO_H__
#include "IGenericAccessDataInfo.h"
#include <string>

class GenericAccessDataInfo :public IGenericAccessDataInfo
{
public:
	//Constructors
	template<class T> inline GenericAccessDataInfo(T aid, IGenericAccessDataValue::DataType atype);

	//Interface version functions
	virtual unsigned int GetIGenericAccessDataInfoVersion() const;

	//Data info functions
	virtual unsigned int GetID() const;
	inline GenericAccessDataInfo* SetID(unsigned int aid);
	virtual IGenericAccessDataValue::DataType GetType() const;
	inline GenericAccessDataInfo* SetType(IGenericAccessDataValue::DataType atype);
	virtual bool GetReadOnly() const;
	inline GenericAccessDataInfo* SetReadOnly(bool areadOnly);
	virtual bool GetLockingSupported() const;
	inline GenericAccessDataInfo* SetLockingSupported(bool alockingSupported);

	//Data display mode functions
	virtual IGenericAccessDataValue::IntDisplayMode GetIntDisplayMode() const;
	inline GenericAccessDataInfo* SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode state);
	virtual IGenericAccessDataValue::FloatDisplayMode GetFloatDisplayMode() const;
	inline GenericAccessDataInfo* SetFloatDisplayMode(IGenericAccessDataValue::FloatDisplayMode state);
	virtual unsigned int GetMinWholeNumberChars() const;
	inline GenericAccessDataInfo* SetMinWholeNumberChars(unsigned int state);
	virtual unsigned int GetMinFractionalNumberChars() const;
	inline GenericAccessDataInfo* SetMinFractionalNumberChars(unsigned int state);

	//Data limit functions
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

	//File path settings
	virtual bool GetFilePathCreatingTarget() const;
	inline GenericAccessDataInfo* SetFilePathCreatingTarget(bool state);
	virtual bool GetFilePathAllowScanningIntoArchives() const;
	inline GenericAccessDataInfo* SetFilePathAllowScanningIntoArchives(bool state);
	inline std::wstring GetFilePathExtensionFilter() const;
	inline GenericAccessDataInfo* SetFilePathExtensionFilter(const std::wstring& afilePathExtensionFilter);
	inline std::wstring GetFilePathDefaultExtension() const;
	inline GenericAccessDataInfo* SetFilePathDefaultExtension(const std::wstring& afilePathDefaultExtension);

	//Folder path settings
	virtual bool GetFolderPathCreatingTarget() const;
	inline GenericAccessDataInfo* SetFolderPathCreatingTarget(bool state);

protected:
	//File path settings
	virtual void GetFilePathExtensionFilterInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetFilePathDefaultExtensionInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

private:
	unsigned int id;
	IGenericAccessDataValue::DataType dataType;
	bool readOnly;
	bool lockingSupported;

	//Display mode state
	IGenericAccessDataValue::IntDisplayMode intDisplayMode;
	IGenericAccessDataValue::FloatDisplayMode floatDisplayMode;
	unsigned int minWholeNumberChars;
	unsigned int minFractionalNumberChars;

	//Data limit state
	int intMinValue;
	int intMaxValue;
	unsigned int uintMinValue;
	unsigned int uintMaxValue;
	float floatMinValue;
	float floatMaxValue;
	double doubleMinValue;
	double doubleMaxValue;
	unsigned int maxStringLength;

	//File path settings
	bool filePathCreatingTarget;
	bool filePathAllowScanningIntoArchives;
	std::wstring filePathExtensionFilter;
	std::wstring filePathDefaultExtension;

	//Folder path settings
	bool folderPathCreatingTarget;
};

#include "GenericAccessDataInfo.inl"
#endif
