#ifndef __IGENERICACCESSCOMMANDINFO_H__
#define __IGENERICACCESSCOMMANDINFO_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

class IGenericAccessCommandInfo
{
public:
	//Typedefs
	typedef void* CommandID;

public:
	//Constructors
	virtual ~IGenericAccessCommandInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIGenericAccessCommandInfoVersion() { return 1; }
	virtual unsigned int GetIGenericAccessCommandInfoVersion() const = 0;

	//Command info functions
	virtual unsigned int GetID() const = 0;
	virtual bool GetCanExecute() const = 0;
};

#endif
