#ifndef __GENERICACCESSDATAVALUEBASE_H__
#define __GENERICACCESSDATAVALUEBASE_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

template<class B> class GenericAccessDataValueBase :public B
{
public:
	//Interface version functions
	virtual inline unsigned int GetIGenericAccessDataValueVersion() const;

	//Value write functions
	virtual inline bool SetValueBool(bool value);
	virtual inline bool SetValueInt(int value);
	virtual inline bool SetValueUInt(unsigned int value);
	virtual inline bool SetValueFloat(float value);
	virtual inline bool SetValueDouble(double value);
	virtual inline bool SetValueString(const MarshalSupport::Marshal::In<std::wstring>& value);
	virtual inline bool SetValueFilePath(const MarshalSupport::Marshal::In<std::wstring>& value);
	virtual inline bool SetValueFolderPath(const MarshalSupport::Marshal::In<std::wstring>& value);
};

#include "GenericAccessDataValueBase.inl"
#endif
