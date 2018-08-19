#include "ClockSource.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ClockSource::ClockSource()
{
	_inputClockSource = 0;

	// Initialize the default clock settings
	_clockType = ClockType::Direct;
	_clockFrequency = 0.0;
	_clockDivider = 0.0;
	_clockMultiplier = 0.0;
	_inputClockRate = 0.0;
}

//----------------------------------------------------------------------------------------------------------------------
bool ClockSource::Construct(const ClockSourceParams& params)
{
	// Ensure that all required parameters have been defined
	if (!params.clockTypeDefined)
	{
		return false;
	}

	// Set the properties for this clock source
	_clockType = params.clockType;
	switch (_clockType)
	{
	case ClockType::Direct:
		if (params.initialValueDefined)
		{
			_clockFrequency = params.initialValue;
		}
		break;
	case ClockType::Divider:
		if (params.initialValueDefined)
		{
			_clockDivider = params.initialValue;
		}
		break;
	case ClockType::Multiplier:
		if (params.initialValueDefined)
		{
			_clockMultiplier = params.initialValue;
		}
		break;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ClockSource::Construct(IHierarchicalStorageNode& node)
{
	ClockSourceParams params;

	// Load the clock type parameter
	IHierarchicalStorageAttribute* clockTypeAttribute = node.GetAttribute(L"ClockType");
	if (clockTypeAttribute != 0)
	{
		std::wstring clockTypeString = clockTypeAttribute->GetValue();
		params.clockTypeDefined = DecodeClockTypeString(clockTypeString, params.clockType);
	}

	// If we failed to successfully load the clock type, return false.
	if (!params.clockTypeDefined)
	{
		return false;
	}

	// Load the initial value parameter
	IHierarchicalStorageAttribute* initialValueAttribute = node.GetAttribute(L"InitialValue");
	if (initialValueAttribute != 0)
	{
		params.initialValueDefined = true;
		params.initialValue = initialValueAttribute->ExtractValue<double>();
	}

	// Try and construct the clock source
	return Construct(params);
}

//----------------------------------------------------------------------------------------------------------------------
bool ClockSource::DecodeClockTypeString(const std::wstring& clockTypeString, ClockType& clockType)
{
	if (clockTypeString == L"Direct")
	{
		clockType = ClockType::Direct;
	}
	else if (clockTypeString == L"Multiplier")
	{
		clockType = ClockType::Multiplier;
	}
	else if (clockTypeString == L"Divider")
	{
		clockType = ClockType::Divider;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int ClockSource::GetIClockSourceVersion() const
{
	return ThisIClockSourceVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Reference functions
//----------------------------------------------------------------------------------------------------------------------
bool ClockSource::AddReference(IBusInterface* target)
{
	_busInterfaces.insert(target);
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ClockSource::RemoveReference(IBusInterface* target)
{
	std::set<IBusInterface*>::iterator i = _busInterfaces.find(target);
	if (i != _busInterfaces.end())
	{
		_busInterfaces.erase(i);
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Clock type functions
//----------------------------------------------------------------------------------------------------------------------
ClockSource::ClockType ClockSource::GetClockType() const
{
	return _clockType;
}

//----------------------------------------------------------------------------------------------------------------------
// Clock frequency functions
//----------------------------------------------------------------------------------------------------------------------
double ClockSource::GetClockFrequency() const
{
	return _clockFrequency;
}

//----------------------------------------------------------------------------------------------------------------------
double ClockSource::GetClockDivider() const
{
	return _clockDivider;
}

//----------------------------------------------------------------------------------------------------------------------
double ClockSource::GetClockMultiplier() const
{
	return _clockMultiplier;
}

//----------------------------------------------------------------------------------------------------------------------
double ClockSource::GetEffectiveClockFrequency() const
{
	double effectiveClockFrequency = 0.0;
	switch (_clockType)
	{
	case ClockType::Direct:
		effectiveClockFrequency = _clockFrequency;
		break;
	case ClockType::Divider:
		if (_clockDivider != 0.0)
		{
			effectiveClockFrequency = _inputClockRate / _clockDivider;
		}
		break;
	case ClockType::Multiplier:
		effectiveClockFrequency = _inputClockRate * _clockMultiplier;
		break;
	}
	return effectiveClockFrequency;
}

//----------------------------------------------------------------------------------------------------------------------
bool ClockSource::SetClockFrequency(double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	// Verify that the clock rate of this clock source is able to be set in this manner
	if (_clockType != ClockType::Direct)
	{
		return false;
	}

	// Set the new clock rate
	_clockFrequency = clockRate;

	// Notify any dependent clock sources or devices of the change in clock frequency
	NotifyOutputClockRateChange(GetEffectiveClockFrequency(), caller, accessTime, accessContext);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ClockSource::SetClockDivider(double divider, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	// Verify that the clock rate of this clock source is able to be set in this manner
	if (_clockType != ClockType::Divider)
	{
		return false;
	}

	// Set the new clock rate
	_clockDivider = divider;

	// Notify any dependent clock sources or devices of the change in clock frequency
	NotifyOutputClockRateChange(GetEffectiveClockFrequency(), caller, accessTime, accessContext);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ClockSource::SetClockMultiplier(double multiplier, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	// Verify that the clock rate of this clock source is able to be set in this manner
	if (_clockType != ClockType::Multiplier)
	{
		return false;
	}

	// Set the new clock rate
	_clockMultiplier = multiplier;

	// Notify any dependent clock sources or devices of the change in clock frequency
	NotifyOutputClockRateChange(GetEffectiveClockFrequency(), caller, accessTime, accessContext);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ClockSource::TransparentSetClockFrequency(double clockRate)
{
	// Verify that the clock rate of this clock source is able to be set in this manner
	if (_clockType != ClockType::Direct)
	{
		return false;
	}

	// Set the new clock rate
	_clockFrequency = clockRate;

	// Notify any dependent clock sources or devices of the change in clock frequency
	TransparentNotifyOutputClockRateChange(GetEffectiveClockFrequency());

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ClockSource::TransparentSetClockDivider(double divider)
{
	// Verify that the clock rate of this clock source is able to be set in this manner
	if (_clockType != ClockType::Divider)
	{
		return false;
	}

	// Set the new clock rate
	_clockDivider = divider;

	// Notify any dependent clock sources or devices of the change in clock frequency
	TransparentNotifyOutputClockRateChange(GetEffectiveClockFrequency());

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ClockSource::TransparentSetClockMultiplier(double multiplier)
{
	// Verify that the clock rate of this clock source is able to be set in this manner
	if (_clockType != ClockType::Multiplier)
	{
		return false;
	}

	// Set the new clock rate
	_clockMultiplier = multiplier;

	// Notify any dependent clock sources or devices of the change in clock frequency
	TransparentNotifyOutputClockRateChange(GetEffectiveClockFrequency());

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Clock dependency functions
//----------------------------------------------------------------------------------------------------------------------
bool ClockSource::AddDependentClockSource(ClockSource* clockSource)
{
	// Set this clock source as the input clock source for the target
	clockSource->SetInputClockSource(this);

	// Add this clock source to the list of dependent clock sources
	_dependentClockSources.insert(clockSource);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ClockSource::RemoveDependentClockSource(ClockSource* clockSource)
{
	std::set<ClockSource*>::iterator i = _dependentClockSources.find(clockSource);
	if (i != _dependentClockSources.end())
	{
		(*i)->SetInputClockSource(0);
		_dependentClockSources.erase(i);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ClockSource::RemoveAllDependentClockSources()
{
	for (std::set<ClockSource*>::const_iterator i = _dependentClockSources.begin(); i != _dependentClockSources.end(); ++i)
	{
		(*i)->SetInputClockSource(0);
	}
	_dependentClockSources.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void ClockSource::SetInputClockSource(ClockSource* clockSource)
{
	_inputClockSource = clockSource;
}

//----------------------------------------------------------------------------------------------------------------------
ClockSource* ClockSource::GetInputClockSource()
{
	return _inputClockSource;
}

//----------------------------------------------------------------------------------------------------------------------
// Clock change notification functions
//----------------------------------------------------------------------------------------------------------------------
void ClockSource::PublishEffectiveClockFrequency() const
{
	TransparentNotifyOutputClockRateChange(GetEffectiveClockFrequency());
}

//----------------------------------------------------------------------------------------------------------------------
void ClockSource::NotifyInputClockRateChange(double newClockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	// Set the new input clock rate
	_inputClockRate = newClockRate;

	// Notify any dependent clock sources or devices of the change in clock frequency
	NotifyOutputClockRateChange(GetEffectiveClockFrequency(), caller, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------------------------------------
void ClockSource::TransparentNotifyInputClockRateChange(double newClockRate)
{
	// Set the new input clock rate
	_inputClockRate = newClockRate;

	// Notify any dependent clock sources or devices of the change in clock frequency
	TransparentNotifyOutputClockRateChange(GetEffectiveClockFrequency());
}

//----------------------------------------------------------------------------------------------------------------------
void ClockSource::NotifyOutputClockRateChange(double newClockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext) const
{
	// Notify any clock sources which depend on the output from this clock source of the
	// change in clock rate.
	for (std::set<ClockSource*>::const_iterator i = _dependentClockSources.begin(); i != _dependentClockSources.end(); ++i)
	{
		(*i)->NotifyInputClockRateChange(newClockRate, caller, accessTime, accessContext);
	}

	// Notify any devices which are linked with this clock source of the change in clock
	// rate.
	for (std::set<IBusInterface*>::const_iterator i = _busInterfaces.begin(); i != _busInterfaces.end(); ++i)
	{
		(*i)->SetClockRate(newClockRate, this, caller, accessTime, accessContext);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ClockSource::TransparentNotifyOutputClockRateChange(double newClockRate) const
{
	// Notify any clock sources which depend on the output from this clock source of the
	// change in clock rate.
	for (std::set<ClockSource*>::const_iterator i = _dependentClockSources.begin(); i != _dependentClockSources.end(); ++i)
	{
		(*i)->TransparentNotifyInputClockRateChange(newClockRate);
	}

	// Notify any devices which are linked with this clock source of the change in clock
	// rate.
	for (std::set<IBusInterface*>::const_iterator i = _busInterfaces.begin(); i != _busInterfaces.end(); ++i)
	{
		(*i)->TransparentSetClockRate(newClockRate, this);
	}
}
