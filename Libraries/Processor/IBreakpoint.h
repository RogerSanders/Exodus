#ifndef __IBREAKPOINT_H__
#define __IBREAKPOINT_H__
#include "InteropSupport/InteropSupport.pkg"
#include <string>

class IBreakpoint
{
public:
	//Enumerations
	enum class Condition;

public:
	//Constructors
	virtual ~IBreakpoint() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIBreakpointVersion();
	virtual unsigned int GetIBreakpointVersion() const = 0;

	//Breakpoint event triggers
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

protected:
	//Name functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GenerateNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IBreakpoint.inl"
#endif
