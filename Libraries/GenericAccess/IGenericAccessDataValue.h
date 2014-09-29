#ifndef __IGENERICACCESSDATAVALUE_H__
#define __IGENERICACCESSDATAVALUE_H__
#include "InteropSupport/InteropSupport.pkg"
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
	static inline unsigned int ThisIGenericAccessDataValueVersion();
	virtual unsigned int GetIGenericAccessDataValueVersion() const = 0;

	//Type functions
	virtual DataType GetType() const = 0;

	//Value read functions
	inline std::wstring GetValueString() const;

	//Value write functions
	template<class T> inline bool SetValue(const T& value);
	template<> inline bool SetValue<bool>(const bool& value);
	template<> inline bool SetValue<volatile bool>(const volatile bool& value);
	template<> inline bool SetValue<int>(const int& value);
	template<> inline bool SetValue<volatile int>(const volatile int& value);
	template<> inline bool SetValue<unsigned int>(const unsigned int& value);
	template<> inline bool SetValue<volatile unsigned int>(const volatile unsigned int& value);
	template<> inline bool SetValue<float>(const float& value);
	template<> inline bool SetValue<volatile float>(const volatile float& value);
	template<> inline bool SetValue<double>(const double& value);
	template<> inline bool SetValue<volatile double>(const volatile double& value);
	template<> inline bool SetValue<std::wstring>(const std::wstring& value);
	virtual bool SetValueBool(bool value) = 0;
	virtual bool SetValueInt(int value) = 0;
	virtual bool SetValueUInt(unsigned int value) = 0;
	virtual bool SetValueFloat(float value) = 0;
	virtual bool SetValueDouble(double value) = 0;
	inline bool SetValueString(const std::wstring& value);
	inline bool SetValueFilePath(const std::wstring& value);
	inline bool SetValueFolderPath(const std::wstring& value);

	//Value limit functions
	virtual void ApplyLimitSettingsToCurrentValue() = 0;

protected:
	//Value read functions
	virtual void GetValueStringInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Value write functions
	virtual bool SetValueStringInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual bool SetValueFilePathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual bool SetValueFolderPathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
};

#include "IGenericAccessDataValue.inl"
#endif
