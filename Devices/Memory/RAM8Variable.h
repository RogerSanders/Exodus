#ifndef __RAM8VARIABLE_H__
#define __RAM8VARIABLE_H__
#include "RAMBase.h"

class RAM8Variable :public RAMBase<unsigned char>
{
public:
	//Constructors
	RAM8Variable(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext);
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext);

	//Debug memory access functions
	virtual unsigned int ReadMemoryEntry(unsigned int location) const;
	virtual void WriteMemoryEntry(unsigned int location, unsigned int data);
};

#endif
