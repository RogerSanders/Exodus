#ifndef __CLOCKSOURCE_H__
#define __CLOCKSOURCE_H__
#include "SystemInterface/SystemInterface.pkg"
#include <set>

class ClockSource :public IClockSource
{
public:
	//Structures
	struct ClockSourceParams;

public:
	//Constructors
	ClockSource();
	bool Construct(IHierarchicalStorageNode& node);
	bool Construct(const ClockSourceParams& params);
	static bool DecodeClockTypeString(const std::wstring& clockTypeString, ClockType& aclockType);

	//Interface version functions
	virtual unsigned int GetIClockSourceVersion() const;

	//Reference functions
	bool AddReference(IBusInterface* target);
	void RemoveReference(IBusInterface* target);

	//Clock type functions
	virtual ClockType GetClockType() const;

	//Clock frequency functions
	double GetClockFrequency() const;
	double GetClockDivider() const;
	double GetClockMultiplier() const;
	double GetEffectiveClockFrequency() const;
	virtual bool SetClockFrequency(double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual bool SetClockDivider(double divider, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual bool SetClockMultiplier(double multiplier, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual bool TransparentSetClockFrequency(double clockRate);
	virtual bool TransparentSetClockDivider(double divider);
	virtual bool TransparentSetClockMultiplier(double multiplier);

	//Clock change notification functions
	void PublishEffectiveClockFrequency() const;

	//Clock dependency functions
	bool AddDependentClockSource(ClockSource* clockSource);
	void RemoveDependentClockSource(ClockSource* clockSource);
	void RemoveAllDependentClockSources();
	ClockSource* GetInputClockSource();
	void SetInputClockSource(ClockSource* clockSource);

private:
	//Clock change notification functions
	void NotifyInputClockRateChange(double newClockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	void TransparentNotifyInputClockRateChange(double newClockRate);
	void NotifyOutputClockRateChange(double newClockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext) const;
	void TransparentNotifyOutputClockRateChange(double newClockRate) const;

private:
	std::set<IBusInterface*> busInterfaces;
	ClockType clockType;
	double clockFrequency;
	double clockDivider;
	double clockMultiplier;
	double inputClockRate;
	std::set<ClockSource*> dependentClockSources;
	ClockSource* inputClockSource;
};

#include "ClockSource.inl"
#endif
