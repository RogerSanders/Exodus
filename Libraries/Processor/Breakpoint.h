#include "IBreakpoint.h"
#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"

class Breakpoint :public IBreakpoint
{
public:
	//Constructors
	inline Breakpoint(unsigned int aaddressBusWidth, unsigned int adataBusWidth, unsigned int aaddressBusCharWidth);

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
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);
	std::wstring GenerateName() const;

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
	virtual void SetHitCounter(unsigned int ahitCounter);
	inline unsigned int GetLiveHitCounter() const;
	inline void IncrementHitCounter();
	inline void PreIncrementHitCounter();
	virtual bool GetBreakOnCounter() const;
	virtual void SetBreakOnCounter(bool state);
	virtual unsigned int GetBreakCounter() const;
	virtual void SetBreakCounter(unsigned int abreakCounter);
	bool CheckHitCounter();

	//Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;

protected:
	//Name functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void GenerateNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

private:
	//Bus sizes
	unsigned int addressBusWidth;
	unsigned int dataBusWidth;
	unsigned int addressBusCharWidth;

	//Breakpoint name
	std::wstring name;

	//Breakpoint event triggers
	bool enabled;
	bool logEvent;
	bool breakEvent;

	//Location condition data
	bool locationConditionNot;
	Condition locationCondition;
	unsigned int locationConditionData1;
	unsigned int locationConditionData2;
	unsigned int locationMask;

	//Hit counter data
	unsigned int hitCounter;
	unsigned int hitCounterIncrement;
	bool preIncrementCounter;
	bool breakOnCounter;
	unsigned int breakCounter;
};

#include "Breakpoint.inl"
#endif
