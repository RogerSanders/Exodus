#ifndef __ROM8VARIABLE_H__
#define __ROM8VARIABLE_H__
#include "ROM8.h"

class ROM8Variable :public ROM8
{
public:
	//Constructors
	ROM8Variable(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext);
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext);
};

#endif
