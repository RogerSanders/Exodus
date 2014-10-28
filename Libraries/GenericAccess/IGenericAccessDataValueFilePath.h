#ifndef __IGENERICACCESSDATAVALUEFILEPATH_H__
#define __IGENERICACCESSDATAVALUEFILEPATH_H__
#include "IGenericAccessDataValue.h"
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

class IGenericAccessDataValueFilePath :public IGenericAccessDataValue
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataValueFilePathVersion() { return 1; }
	virtual unsigned int GetIGenericAccessDataValueFilePathVersion() const = 0;

	//Value read functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetValue() const = 0;

	//Value limit functions
	virtual unsigned int GetMaxLength() const = 0;
	virtual void SetMaxLength(unsigned int state) = 0;
};

#endif
