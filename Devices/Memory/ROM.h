#ifndef __ROM_H__
#define __ROM_H__
#include "MemoryRead.h"
#include <vector>

class ROM :public MemoryRead
{
public:
	//Constructors
	ROM(const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual bool Construct(IHeirarchicalStorageNode& node);

	//Initialization functions
	virtual void Initialize();

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime);
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller);
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller);

private:
	std::vector<unsigned char> memory;
};

#endif
