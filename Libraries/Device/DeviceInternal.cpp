#include "DeviceInternal.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DeviceInternal::DeviceInternal()
:deviceContext(0)
{}

//----------------------------------------------------------------------------------------
DeviceInternal::~DeviceInternal()
{
	delete deviceContext;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int DeviceInternal::GetIDeviceVersion() const
{
	return ThisIDeviceVersion();
}

//----------------------------------------------------------------------------------------
//Device context functions
//----------------------------------------------------------------------------------------
IDeviceContext* DeviceInternal::GetDeviceContext() const
{
	return deviceContext;
}

//----------------------------------------------------------------------------------------
double DeviceInternal::GetCurrentTimesliceProgress() const
{
	return deviceContext->GetCurrentTimesliceProgress();
}
