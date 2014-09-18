#ifndef __IGENERICACCESSDATAVALUEFOLDERPATH_H__
#define __IGENERICACCESSDATAVALUEFOLDERPATH_H__
#include "IGenericAccessDataValue.h"
#include "InteropSupport/InteropSupport.pkg"
#include <string>

class IGenericAccessDataValueFolderPath :public IGenericAccessDataValue
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataValueFolderPathVersion();
	virtual unsigned int GetIGenericAccessDataValueFolderPathVersion() const = 0;

	//Value read functions
	inline std::wstring GetValue() const;

	//Value limit functions
	virtual unsigned int GetMaxLength() const = 0;
	virtual void SetMaxLength(unsigned int state) = 0;

protected:
	//Value read functions
	virtual void GetValueInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IGenericAccessDataValueFolderPath.inl"
#endif
