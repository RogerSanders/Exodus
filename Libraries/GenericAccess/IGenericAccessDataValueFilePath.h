#ifndef __IGENERICACCESSDATAVALUEFILEPATH_H__
#define __IGENERICACCESSDATAVALUEFILEPATH_H__
#include "IGenericAccessDataValue.h"
#include "InteropSupport/InteropSupport.pkg"
#include <string>

class IGenericAccessDataValueFilePath :public IGenericAccessDataValue
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataValueFilePathVersion();
	virtual unsigned int GetIGenericAccessDataValueFilePathVersion() const = 0;

	//Value read functions
	inline std::wstring GetValue() const;

	//Value limit functions
	virtual unsigned int GetMaxLength() const = 0;
	virtual void SetMaxLength(unsigned int state) = 0;

protected:
	//Value read functions
	virtual void GetValueInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IGenericAccessDataValueFilePath.inl"
#endif
