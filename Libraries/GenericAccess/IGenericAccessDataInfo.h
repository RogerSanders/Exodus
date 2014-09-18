#ifndef __IGENERICACCESSDATAINFO_H__
#define __IGENERICACCESSDATAINFO_H__
#include "IGenericAccessDataValue.h"
#include "InteropSupport/InteropSupport.pkg"
#include <string>

class IGenericAccessDataInfo
{
public:
	//Typedefs
	typedef void* DataID;

public:
	//Constructors
	virtual ~IGenericAccessDataInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataInfoVersion();
	virtual unsigned int GetIGenericAccessDataInfoVersion() const = 0;

	//Data info functions
	virtual unsigned int GetID() const = 0;
	virtual IGenericAccessDataValue::DataType GetType() const = 0;
	virtual bool GetReadOnly() const = 0;
	virtual bool GetLockingSupported() const = 0;

	//Data display mode functions
	virtual IGenericAccessDataValue::IntDisplayMode GetIntDisplayMode() const = 0;
	virtual IGenericAccessDataValue::FloatDisplayMode GetFloatDisplayMode() const = 0;
	virtual unsigned int GetMinWholeNumberChars() const = 0;
	virtual unsigned int GetMinFractionalNumberChars() const = 0;

	//Data limit functions
	virtual int GetIntMinValue() const = 0;
	virtual int GetIntMaxValue() const = 0;
	virtual unsigned int GetUIntMinValue() const = 0;
	virtual unsigned int GetUIntMaxValue() const = 0;
	virtual float GetFloatMinValue() const = 0;
	virtual float GetFloatMaxValue() const = 0;
	virtual double GetDoubleMinValue() const = 0;
	virtual double GetDoubleMaxValue() const = 0;
	virtual unsigned int GetStringMaxLength() const = 0;

	//File path settings
	virtual bool GetFilePathCreatingTarget() const = 0;
	virtual bool GetFilePathAllowScanningIntoArchives() const = 0;
	inline std::wstring GetFilePathExtensionFilter() const;
	inline std::wstring GetFilePathDefaultExtension() const;

	//Folder path settings
	virtual bool GetFolderPathCreatingTarget() const = 0;

protected:
	//File path settings
	virtual void GetFilePathExtensionFilterInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetFilePathDefaultExtensionInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IGenericAccessDataInfo.inl"
#endif
