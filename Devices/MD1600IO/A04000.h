#ifndef __A04000_H__
#define __A04000_H__
#include "SystemInterface/SystemInterface.pkg"
#include "Device/Device.pkg"
#include <vector>

//Test stub for YM2612
//##TODO## Delete this device. It's no longer used or required, now that we have a real
//YM2612 core.
class A04000 :public Device
{
public:
//Constructors
	A04000(const std::wstring& ainstanceName, unsigned int amoduleID);

//Initialization functions
	virtual void Initialize();

//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
};

#endif
