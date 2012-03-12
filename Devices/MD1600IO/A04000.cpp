#include "A04000.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
A04000::A04000(const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(L"A04000", ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
void A04000::Initialize()
{}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult A04000::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime)
{
	data = 0;
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult A04000::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime)
{
	return true;
}
