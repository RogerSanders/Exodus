#ifndef __RAM16VARIABLE_H__
#define __RAM16VARIABLE_H__
#include "RAM16.h"

class RAM16Variable :public RAM16
{
public:
	//Constructors
	RAM16Variable(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext);
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext);
};

#endif
