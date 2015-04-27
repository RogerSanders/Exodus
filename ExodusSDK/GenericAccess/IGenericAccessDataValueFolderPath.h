#ifndef __IGENERICACCESSDATAVALUEFOLDERPATH_H__
#define __IGENERICACCESSDATAVALUEFOLDERPATH_H__
#include "IGenericAccessDataValue.h"
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

class IGenericAccessDataValueFolderPath :public IGenericAccessDataValue
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataValueFolderPathVersion() { return 1; }
	virtual unsigned int GetIGenericAccessDataValueFolderPathVersion() const = 0;

	//Value read functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetValue() const = 0;

	//Value limit functions
	virtual unsigned int GetMaxLength() const = 0;
	virtual void SetMaxLength(unsigned int state) = 0;
};

#endif
