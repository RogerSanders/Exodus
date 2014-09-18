#ifndef __GENERICACCESSDATAVALUEBASE_H__
#define __GENERICACCESSDATAVALUEBASE_H__
#include "IGenericAccessDataValue.h"

template<class B> class GenericAccessDataValueBase :public B
{
public:
	//Interface version functions
	virtual inline unsigned int GetIGenericAccessDataValueVersion() const;

	//Value read functions
	virtual std::wstring GetValueString() const = 0;

	//Value write functions
	virtual inline bool SetValueBool(bool value);
	virtual inline bool SetValueInt(int value);
	virtual inline bool SetValueUInt(unsigned int value);
	virtual inline bool SetValueFloat(float value);
	virtual inline bool SetValueDouble(double value);
	virtual inline bool SetValueString(const std::wstring& value);
	virtual inline bool SetValueFilePath(const std::wstring& value);
	virtual inline bool SetValueFolderPath(const std::wstring& value);

protected:
	//Value read functions
	virtual inline void GetValueStringInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Value write functions
	virtual inline bool SetValueStringInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual inline bool SetValueFilePathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual inline bool SetValueFolderPathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
};

#include "GenericAccessDataValueBase.inl"
#endif
