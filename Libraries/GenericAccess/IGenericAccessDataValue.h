#ifndef __IGENERICACCESSDATAVALUE_H__
#define __IGENERICACCESSDATAVALUE_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

class IGenericAccessDataValue
{
public:
	//Enumerations
	enum class DataType;
	enum class IntDisplayMode;
	enum class FloatDisplayMode;

public:
	//Constructors
	virtual ~IGenericAccessDataValue() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataValueVersion() { return 1; }
	virtual unsigned int GetIGenericAccessDataValueVersion() const = 0;

	//Type functions
	virtual DataType GetType() const = 0;

	//Value read functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetValueString() const = 0;

	//Value write functions
	inline bool SetValue(bool value);
	inline bool SetValue(int value);
	inline bool SetValue(unsigned int value);
	inline bool SetValue(float value);
	inline bool SetValue(double value);
	inline bool SetValue(const std::wstring& value);
	virtual bool SetValueBool(bool value) = 0;
	virtual bool SetValueInt(int value) = 0;
	virtual bool SetValueUInt(unsigned int value) = 0;
	virtual bool SetValueFloat(float value) = 0;
	virtual bool SetValueDouble(double value) = 0;
	virtual bool SetValueString(const MarshalSupport::Marshal::In<std::wstring>& value) = 0;
	virtual bool SetValueFilePath(const MarshalSupport::Marshal::In<std::wstring>& value) = 0;
	virtual bool SetValueFolderPath(const MarshalSupport::Marshal::In<std::wstring>& value) = 0;

	//Value limit functions
	virtual void ApplyLimitSettingsToCurrentValue() = 0;
};

#include "IGenericAccessDataValue.inl"
#endif
