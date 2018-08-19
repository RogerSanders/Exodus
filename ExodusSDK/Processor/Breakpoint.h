#include "IBreakpoint.h"
#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"

class Breakpoint :public IBreakpoint
{
public:
	//Constructors
	inline Breakpoint(unsigned int addressBusWidth, unsigned int dataBusWidth, unsigned int addressBusCharWidth);

	//Interface version functions
	virtual unsigned int GetIBreakpointVersion() const;

	//Execute functions
	inline void Commit();
	inline void Rollback();

	//Breakpoint logging functions
	std::wstring GetLogString() const;

	//Breakpoint event triggers
	virtual bool GetEnabled() const;
	virtual void SetEnabled(bool state);
	virtual bool GetLogEvent() const;
	virtual void SetLogEvent(bool state);
	virtual bool GetBreakEvent() const;
	virtual void SetBreakEvent(bool state);

	//Name functions
	virtual Marshal::Ret<std::wstring> GetName() const;
	virtual void SetName(const Marshal::In<std::wstring>& name);
	virtual Marshal::Ret<std::wstring> GenerateName() const;

	//Location condition functions
	virtual bool GetLocationConditionNot() const;
	virtual void SetLocationConditionNot(bool state);
	virtual Condition GetLocationCondition() const;
	virtual void SetLocationCondition(Condition condition);
	virtual unsigned int GetLocationConditionData1() const;
	virtual void SetLocationConditionData1(unsigned int data);
	virtual unsigned int GetLocationConditionData2() const;
	virtual void SetLocationConditionData2(unsigned int data);
	virtual unsigned int GetLocationMask() const;
	virtual void SetLocationMask(unsigned int data);
	virtual bool PassesLocationCondition(unsigned int location);

	//Hit counter functions
	virtual unsigned int GetHitCounter() const;
	virtual void SetHitCounter(unsigned int hitCounter);
	inline unsigned int GetLiveHitCounter() const;
	inline void IncrementHitCounter();
	inline void PreIncrementHitCounter();
	virtual bool GetBreakOnCounter() const;
	virtual void SetBreakOnCounter(bool state);
	virtual unsigned int GetBreakCounter() const;
	virtual void SetBreakCounter(unsigned int breakCounter);
	bool CheckHitCounter();

	//Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;

private:
	//Bus sizes
	unsigned int _addressBusWidth;
	unsigned int _dataBusWidth;
	unsigned int _addressBusCharWidth;

	//Breakpoint name
	std::wstring _name;

	//Breakpoint event triggers
	bool _enabled;
	bool _logEvent;
	bool _breakEvent;

	//Location condition data
	bool _locationConditionNot;
	Condition _locationCondition;
	unsigned int _locationConditionData1;
	unsigned int _locationConditionData2;
	unsigned int _locationMask;

	//Hit counter data
	unsigned int _hitCounter;
	unsigned int _hitCounterIncrement;
	bool _preIncrementCounter;
	bool _breakOnCounter;
	unsigned int _breakCounter;
};

#include "Breakpoint.inl"
#endif
