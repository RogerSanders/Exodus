#ifndef __IGENERICACCESSDATAVALUEBOOL_H__
#define __IGENERICACCESSDATAVALUEBOOL_H__
#include "IGenericAccessDataValue.h"

class IGenericAccessDataValueBool :public IGenericAccessDataValue
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessDataValueBoolVersion() { return 1; }
	virtual unsigned int GetIGenericAccessDataValueBoolVersion() const = 0;

	//Value read functions
	virtual bool GetValue() const = 0;
};

#endif
