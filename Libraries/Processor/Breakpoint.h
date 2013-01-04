#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
#include <string>

class Breakpoint
{
public:
	//Enumerations
	enum Condition;

	//Constructors
	inline Breakpoint();
	virtual ~Breakpoint();

	//Initialization functions
	inline void Initialize();

	//Execute functions
	inline void Commit();
	inline void Rollback();

	//Breakpoint logging functions
	std::wstring GetLogString() const;

	//Breakpoint event triggers
	inline bool GetLogEvent() const;
	inline void SetLogEvent(bool state);
	inline bool GetBreakEvent() const;
	inline void SetBreakEvent(bool state);

	//Name functions
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);
	std::wstring GenerateName(unsigned int addressCharWidth) const;

	//Location condition functions
	inline bool GetLocationConditionNot() const;
	inline void SetLocationConditionNot(bool state);
	inline Condition GetLocationCondition() const;
	inline void SetLocationCondition(Condition condition);
	inline unsigned int GetLocationConditionData1() const;
	inline void SetLocationConditionData1(unsigned int data, unsigned int addressBusWidth);
	inline unsigned int GetLocationConditionData2() const;
	inline void SetLocationConditionData2(unsigned int data, unsigned int addressBusWidth);
	bool PassesLocationCondition(unsigned int location);

	//Hit counter functions
	inline unsigned int GetHitCounter() const;
	inline void SetHitCounter(unsigned int ahitCounter);
	inline unsigned int GetLiveHitCounter() const;
	inline void IncrementHitCounter();
	inline void PreIncrementHitCounter();
	inline bool GetBreakOnCounter() const;
	inline void SetBreakOnCounter(bool state);
	inline unsigned int GetBreakCounter() const;
	inline void SetBreakCounter(unsigned int abreakCounter);
	bool CheckHitCounter();

	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;

private:
	//Breakpoint name
	std::wstring name;

	//Breakpoint event triggers
	bool logEvent;
	bool breakEvent;

	//Location condition data
	bool locationConditionNot;
	Condition locationCondition;
	unsigned int locationConditionData1;
	unsigned int locationConditionData2;

	//Hit counter data
	unsigned int hitCounter;
	unsigned int hitCounterIncrement;
	bool preIncrementCounter;
	bool breakOnCounter;
	unsigned int breakCounter;
};

#include "Breakpoint.inl"
#endif
