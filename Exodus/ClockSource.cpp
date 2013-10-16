#include "ClockSource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ClockSource::ClockSource()
{
	inputClockSource = 0;

	//Initialize the default clock settings
	clockType = CLOCKTYPE_DIRECT;
	clockFrequency = 0.0;
	clockDivider = 0.0;
	clockMultiplier = 0.0;
	inputClockRate = 0.0;
}

//----------------------------------------------------------------------------------------
bool ClockSource::Construct(const ClockSourceParams& params)
{
	//Ensure that all required parameters have been defined
	if(!params.clockTypeDefined)
	{
		return false;
	}

	//Set the properties for this clock source
	clockType = params.clockType;
	switch(clockType)
	{
	case CLOCKTYPE_DIRECT:
		if(params.initialValueDefined)
		{
			clockFrequency = params.initialValue;
		}
		break;
	case CLOCKTYPE_DIVIDER:
		if(params.initialValueDefined)
		{
			clockDivider = params.initialValue;
		}
		break;
	case CLOCKTYPE_MULTIPLIER:
		if(params.initialValueDefined)
		{
			clockMultiplier = params.initialValue;
		}
		break;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool ClockSource::Construct(IHierarchicalStorageNode& node)
{
	ClockSourceParams params;

	//Load the clock type parameter
	IHierarchicalStorageAttribute* clockTypeAttribute = node.GetAttribute(L"ClockType");
	if(clockTypeAttribute != 0)
	{
		std::wstring clockTypeString = clockTypeAttribute->GetValue();
		params.clockTypeDefined = DecodeClockTypeString(clockTypeString, params.clockType);
	}

	//If we failed to successfully load the clock type, return false.
	if(!params.clockTypeDefined)
	{
		return false;
	}

	//Load the initial value parameter
	IHierarchicalStorageAttribute* initialValueAttribute = node.GetAttribute(L"InitialValue");
	if(initialValueAttribute != 0)
	{
		params.initialValueDefined = true;
		params.initialValue = initialValueAttribute->ExtractValue<double>();
	}

	//Try and construct the clock source
	return Construct(params);
}

//----------------------------------------------------------------------------------------
bool ClockSource::DecodeClockTypeString(const std::wstring& clockTypeString, ClockType& aclockType)
{
	if(clockTypeString == L"Direct")
	{
		aclockType = CLOCKTYPE_DIRECT;
	}
	else if(clockTypeString == L"Multiplier")
	{
		aclockType = CLOCKTYPE_MULTIPLIER;
	}
	else if(clockTypeString == L"Divider")
	{
		aclockType = CLOCKTYPE_DIVIDER;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ClockSource::GetIClockSourceVersion() const
{
	return ThisIClockSourceVersion();
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool ClockSource::AddReference(IBusInterface* target)
{
	busInterfaces.insert(target);
	return true;
}

//----------------------------------------------------------------------------------------
void ClockSource::RemoveReference(IBusInterface* target)
{
	std::set<IBusInterface*>::iterator i = busInterfaces.find(target);
	if(i != busInterfaces.end())
	{
		busInterfaces.erase(i);
	}
}

//----------------------------------------------------------------------------------------
//Clock type functions
//----------------------------------------------------------------------------------------
ClockSource::ClockType ClockSource::GetClockType() const
{
	return clockType;
}

//----------------------------------------------------------------------------------------
//Clock frequency functions
//----------------------------------------------------------------------------------------
double ClockSource::GetClockFrequency() const
{
	return clockFrequency;
}

//----------------------------------------------------------------------------------------
double ClockSource::GetClockDivider() const
{
	return clockDivider;
}

//----------------------------------------------------------------------------------------
double ClockSource::GetClockMultiplier() const
{
	return clockMultiplier;
}

//----------------------------------------------------------------------------------------
double ClockSource::GetEffectiveClockFrequency() const
{
	double effectiveClockFrequency = 0.0;
	switch(clockType)
	{
	case CLOCKTYPE_DIRECT:
		effectiveClockFrequency = clockFrequency;
		break;
	case CLOCKTYPE_DIVIDER:
		if(clockDivider != 0.0)
		{
			effectiveClockFrequency = inputClockRate / clockDivider;
		}
		break;
	case CLOCKTYPE_MULTIPLIER:
		effectiveClockFrequency = inputClockRate * clockMultiplier;
		break;
	}
	return effectiveClockFrequency;
}

//----------------------------------------------------------------------------------------
bool ClockSource::SetClockFrequency(double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Verify that the clock rate of this clock source is able to be set in this manner
	if(clockType != CLOCKTYPE_DIRECT)
	{
		return false;
	}

	//Set the new clock rate
	clockFrequency = clockRate;

	//Notify any dependent clock sources or devices of the change in clock frequency
	NotifyOutputClockRateChange(GetEffectiveClockFrequency(), caller, accessTime, accessContext);

	return true;
}

//----------------------------------------------------------------------------------------
bool ClockSource::SetClockDivider(double divider, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Verify that the clock rate of this clock source is able to be set in this manner
	if(clockType != CLOCKTYPE_DIVIDER)
	{
		return false;
	}

	//Set the new clock rate
	clockDivider = divider;

	//Notify any dependent clock sources or devices of the change in clock frequency
	NotifyOutputClockRateChange(GetEffectiveClockFrequency(), caller, accessTime, accessContext);

	return true;
}

//----------------------------------------------------------------------------------------
bool ClockSource::SetClockMultiplier(double multiplier, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Verify that the clock rate of this clock source is able to be set in this manner
	if(clockType != CLOCKTYPE_MULTIPLIER)
	{
		return false;
	}

	//Set the new clock rate
	clockMultiplier = multiplier;

	//Notify any dependent clock sources or devices of the change in clock frequency
	NotifyOutputClockRateChange(GetEffectiveClockFrequency(), caller, accessTime, accessContext);

	return true;
}

//----------------------------------------------------------------------------------------
bool ClockSource::TransparentSetClockFrequency(double clockRate)
{
	//Verify that the clock rate of this clock source is able to be set in this manner
	if(clockType != CLOCKTYPE_DIRECT)
	{
		return false;
	}

	//Set the new clock rate
	clockFrequency = clockRate;

	//Notify any dependent clock sources or devices of the change in clock frequency
	TransparentNotifyOutputClockRateChange(GetEffectiveClockFrequency());

	return true;
}

//----------------------------------------------------------------------------------------
bool ClockSource::TransparentSetClockDivider(double divider)
{
	//Verify that the clock rate of this clock source is able to be set in this manner
	if(clockType != CLOCKTYPE_DIVIDER)
	{
		return false;
	}

	//Set the new clock rate
	clockDivider = divider;

	//Notify any dependent clock sources or devices of the change in clock frequency
	TransparentNotifyOutputClockRateChange(GetEffectiveClockFrequency());

	return true;
}

//----------------------------------------------------------------------------------------
bool ClockSource::TransparentSetClockMultiplier(double multiplier)
{
	//Verify that the clock rate of this clock source is able to be set in this manner
	if(clockType != CLOCKTYPE_MULTIPLIER)
	{
		return false;
	}

	//Set the new clock rate
	clockMultiplier = multiplier;

	//Notify any dependent clock sources or devices of the change in clock frequency
	TransparentNotifyOutputClockRateChange(GetEffectiveClockFrequency());

	return true;
}

//----------------------------------------------------------------------------------------
//Clock dependency functions
//----------------------------------------------------------------------------------------
bool ClockSource::AddDependentClockSource(ClockSource* clockSource)
{
	//Set this clock source as the input clock source for the target
	clockSource->SetInputClockSource(this);

	//Add this clock source to the list of dependent clock sources
	dependentClockSources.insert(clockSource);

	return true;
}

//----------------------------------------------------------------------------------------
void ClockSource::RemoveDependentClockSource(ClockSource* clockSource)
{
	std::set<ClockSource*>::iterator i = dependentClockSources.find(clockSource);
	if(i != dependentClockSources.end())
	{
		(*i)->SetInputClockSource(0);
		dependentClockSources.erase(i);
	}
}

//----------------------------------------------------------------------------------------
void ClockSource::RemoveAllDependentClockSources()
{
	for(std::set<ClockSource*>::const_iterator i = dependentClockSources.begin(); i != dependentClockSources.end(); ++i)
	{
		(*i)->SetInputClockSource(0);
	}
	dependentClockSources.clear();
}

//----------------------------------------------------------------------------------------
void ClockSource::SetInputClockSource(ClockSource* clockSource)
{
	inputClockSource = clockSource;
}

//----------------------------------------------------------------------------------------
ClockSource* ClockSource::GetInputClockSource()
{
	return inputClockSource;
}

//----------------------------------------------------------------------------------------
//Clock change notification functions
//----------------------------------------------------------------------------------------
void ClockSource::PublishEffectiveClockFrequency() const
{
	TransparentNotifyOutputClockRateChange(GetEffectiveClockFrequency());
}

//----------------------------------------------------------------------------------------
void ClockSource::NotifyInputClockRateChange(double newClockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Set the new input clock rate
	inputClockRate = newClockRate;

	//Notify any dependent clock sources or devices of the change in clock frequency
	NotifyOutputClockRateChange(GetEffectiveClockFrequency(), caller, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------
void ClockSource::TransparentNotifyInputClockRateChange(double newClockRate)
{
	//Set the new input clock rate
	inputClockRate = newClockRate;

	//Notify any dependent clock sources or devices of the change in clock frequency
	TransparentNotifyOutputClockRateChange(GetEffectiveClockFrequency());
}

//----------------------------------------------------------------------------------------
void ClockSource::NotifyOutputClockRateChange(double newClockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext) const
{
	//Notify any clock sources which depend on the output from this clock source of the
	//change in clock rate.
	for(std::set<ClockSource*>::const_iterator i = dependentClockSources.begin(); i != dependentClockSources.end(); ++i)
	{
		(*i)->NotifyInputClockRateChange(newClockRate, caller, accessTime, accessContext);
	}

	//Notify any devices which are linked with this clock source of the change in clock
	//rate.
	for(std::set<IBusInterface*>::const_iterator i = busInterfaces.begin(); i != busInterfaces.end(); ++i)
	{
		(*i)->SetClockRate(newClockRate, this, caller, accessTime, accessContext);
	}
}

//----------------------------------------------------------------------------------------
void ClockSource::TransparentNotifyOutputClockRateChange(double newClockRate) const
{
	//Notify any clock sources which depend on the output from this clock source of the
	//change in clock rate.
	for(std::set<ClockSource*>::const_iterator i = dependentClockSources.begin(); i != dependentClockSources.end(); ++i)
	{
		(*i)->TransparentNotifyInputClockRateChange(newClockRate);
	}

	//Notify any devices which are linked with this clock source of the change in clock
	//rate.
	for(std::set<IBusInterface*>::const_iterator i = busInterfaces.begin(); i != busInterfaces.end(); ++i)
	{
		(*i)->TransparentSetClockRate(newClockRate, this);
	}
}
