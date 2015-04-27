#ifndef __IGENERICACCESSDATAVALUESTRING_H__
#define __IGENERICACCESSDATAVALUESTRING_H__
#include "IGenericAccessDataValue.h"
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

class IGenericAccessDataValueString :public IGenericAccessDataValue
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataValueStringVersion() { return 1; }
	virtual unsigned int GetIGenericAccessDataValueStringVersion() const = 0;

	//Value read functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetValue() const = 0;

	//Value limit functions
	virtual unsigned int GetMaxLength() const = 0;
	virtual void SetMaxLength(unsigned int state) = 0;
};

#endif
