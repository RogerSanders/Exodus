#include "Breakpoint.h"
#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

class Watchpoint :public Breakpoint
{
public:
	//Constructors
	inline Watchpoint();

	//Initialization functions
	inline void Initialize();

	//Breakpoint logging functions
	std::wstring GetLogString() const;

	//Read/write condition flag functions
	inline bool GetOnRead() const;
	inline void SetOnRead(bool state);
	inline bool GetOnWrite() const;
	inline void SetOnWrite(bool state);

	//Read condition functions
	inline bool GetReadConditionEnabled() const;
	inline void SetReadConditionEnabled(bool state);
	inline bool GetReadConditionNot() const;
	inline void SetReadConditionNot(bool state);
	inline Condition GetReadCondition() const;
	inline void SetReadCondition(Condition condition);
	inline unsigned int GetReadConditionData1() const;
	inline void SetReadConditionData1(unsigned int data, unsigned int dataBusWidth);
	inline unsigned int GetReadConditionData2() const;
	inline void SetReadConditionData2(unsigned int data, unsigned int dataBusWidth);
	bool PassesReadCondition(unsigned int data);

	//Write condition functions
	inline bool GetWriteConditionEnabled() const;
	inline void SetWriteConditionEnabled(bool state);
	inline bool GetWriteConditionNot() const;
	inline void SetWriteConditionNot(bool state);
	inline Condition GetWriteCondition() const;
	inline void SetWriteCondition(Condition condition);
	inline unsigned int GetWriteConditionData1() const;
	inline void SetWriteConditionData1(unsigned int data, unsigned int dataBusWidth);
	inline unsigned int GetWriteConditionData2() const;
	inline void SetWriteConditionData2(unsigned int data, unsigned int dataBusWidth);
	bool PassesWriteCondition(unsigned int data);

	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;

private:
	//Read/Write condition flag data
	bool read;
	bool write;

	//Read condition data
	bool readConditionEnabled;
	bool readConditionNot;
	Condition readCondition;
	unsigned int readConditionData1;
	unsigned int readConditionData2;

	//Write condition data
	bool writeConditionEnabled;
	bool writeConditionNot;
	Condition writeCondition;
	unsigned int writeConditionData1;
	unsigned int writeConditionData2;
};

#include "Watchpoint.inl"
#endif
