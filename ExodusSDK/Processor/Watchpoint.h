#include "IWatchpoint.h"
#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"

class Watchpoint :public IWatchpoint
{
public:
	//Constructors
	inline Watchpoint(unsigned int addressBusWidth, unsigned int dataBusWidth, unsigned int addressBusCharWidth);

	//Interface version functions
	virtual unsigned int GetIWatchpointVersion() const;

	//Execute functions
	inline void Commit();
	inline void Rollback();

	//Watchpoint logging functions
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

	//Read/write condition flag functions
	virtual bool GetOnRead() const;
	virtual void SetOnRead(bool state);
	virtual bool GetOnWrite() const;
	virtual void SetOnWrite(bool state);

	//Read condition functions
	virtual bool GetReadConditionEnabled() const;
	virtual void SetReadConditionEnabled(bool state);
	virtual bool GetReadConditionNot() const;
	virtual void SetReadConditionNot(bool state);
	virtual Condition GetReadCondition() const;
	virtual void SetReadCondition(Condition condition);
	virtual unsigned int GetReadConditionData1() const;
	virtual void SetReadConditionData1(unsigned int data);
	virtual unsigned int GetReadConditionData2() const;
	virtual void SetReadConditionData2(unsigned int data);
	virtual bool PassesReadCondition(unsigned int data);

	//Write condition functions
	virtual bool GetWriteConditionEnabled() const;
	virtual void SetWriteConditionEnabled(bool state);
	virtual bool GetWriteConditionNot() const;
	virtual void SetWriteConditionNot(bool state);
	virtual Condition GetWriteCondition() const;
	virtual void SetWriteCondition(Condition condition);
	virtual unsigned int GetWriteConditionData1() const;
	virtual void SetWriteConditionData1(unsigned int data);
	virtual unsigned int GetWriteConditionData2() const;
	virtual void SetWriteConditionData2(unsigned int data);
	virtual bool PassesWriteCondition(unsigned int data);

	//Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;

private:
	//Bus sizes
	unsigned int _addressBusWidth;
	unsigned int _dataBusWidth;
	unsigned int _addressBusCharWidth;

	//Watchpoint name
	std::wstring _name;

	//Watchpoint event triggers
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

	//Read/Write condition flag data
	bool _read;
	bool _write;

	//Read condition data
	bool _readConditionEnabled;
	bool _readConditionNot;
	Condition _readCondition;
	unsigned int _readConditionData1;
	unsigned int _readConditionData2;

	//Write condition data
	bool _writeConditionEnabled;
	bool _writeConditionNot;
	Condition _writeCondition;
	unsigned int _writeConditionData1;
	unsigned int _writeConditionData2;
};

#include "Watchpoint.inl"
#endif
