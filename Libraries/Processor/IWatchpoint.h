#ifndef __IWATCHPOINT_H__
#define __IWATCHPOINT_H__
#include "InteropSupport/InteropSupport.pkg"
#include <string>

class IWatchpoint
{
public:
	//Enumerations
	enum class Condition;

public:
	//Constructors
	virtual ~IWatchpoint() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIWatchpointVersion();
	virtual unsigned int GetIWatchpointVersion() const = 0;

	//Watchpoint event triggers
	virtual bool GetEnabled() const = 0;
	virtual void SetEnabled(bool state) = 0;
	virtual bool GetLogEvent() const = 0;
	virtual void SetLogEvent(bool state) = 0;
	virtual bool GetBreakEvent() const = 0;
	virtual void SetBreakEvent(bool state) = 0;

	//Name functions
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);
	inline std::wstring GenerateName() const;

	//Location condition functions
	virtual bool GetLocationConditionNot() const = 0;
	virtual void SetLocationConditionNot(bool state) = 0;
	virtual Condition GetLocationCondition() const = 0;
	virtual void SetLocationCondition(Condition condition) = 0;
	virtual unsigned int GetLocationConditionData1() const = 0;
	virtual void SetLocationConditionData1(unsigned int data) = 0;
	virtual unsigned int GetLocationConditionData2() const = 0;
	virtual void SetLocationConditionData2(unsigned int data) = 0;
	virtual unsigned int GetLocationMask() const = 0;
	virtual void SetLocationMask(unsigned int data) = 0;
	virtual bool PassesLocationCondition(unsigned int location) = 0;

	//Hit counter functions
	virtual unsigned int GetHitCounter() const = 0;
	virtual void SetHitCounter(unsigned int ahitCounter) = 0;
	virtual bool GetBreakOnCounter() const = 0;
	virtual void SetBreakOnCounter(bool state) = 0;
	virtual unsigned int GetBreakCounter() const = 0;
	virtual void SetBreakCounter(unsigned int abreakCounter) = 0;

	//Read/write condition flag functions
	virtual bool GetOnRead() const = 0;
	virtual void SetOnRead(bool state) = 0;
	virtual bool GetOnWrite() const = 0;
	virtual void SetOnWrite(bool state) = 0;

	//Read condition functions
	virtual bool GetReadConditionEnabled() const = 0;
	virtual void SetReadConditionEnabled(bool state) = 0;
	virtual bool GetReadConditionNot() const = 0;
	virtual void SetReadConditionNot(bool state) = 0;
	virtual Condition GetReadCondition() const = 0;
	virtual void SetReadCondition(Condition condition) = 0;
	virtual unsigned int GetReadConditionData1() const = 0;
	virtual void SetReadConditionData1(unsigned int data) = 0;
	virtual unsigned int GetReadConditionData2() const = 0;
	virtual void SetReadConditionData2(unsigned int data) = 0;
	virtual bool PassesReadCondition(unsigned int data) = 0;

	//Write condition functions
	virtual bool GetWriteConditionEnabled() const = 0;
	virtual void SetWriteConditionEnabled(bool state) = 0;
	virtual bool GetWriteConditionNot() const = 0;
	virtual void SetWriteConditionNot(bool state) = 0;
	virtual Condition GetWriteCondition() const = 0;
	virtual void SetWriteCondition(Condition condition) = 0;
	virtual unsigned int GetWriteConditionData1() const = 0;
	virtual void SetWriteConditionData1(unsigned int data) = 0;
	virtual unsigned int GetWriteConditionData2() const = 0;
	virtual void SetWriteConditionData2(unsigned int data) = 0;
	virtual bool PassesWriteCondition(unsigned int data) = 0;

protected:
	//Name functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GenerateNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IWatchpoint.inl"
#endif
