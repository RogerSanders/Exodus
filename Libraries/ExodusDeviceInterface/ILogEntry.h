#ifndef __ILOGENTRY_H__
#define __ILOGENTRY_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

class ILogEntry
{
public:
	//Enumerations
	enum class EventLevel;

public:
	//Constructors
	virtual ~ILogEntry() = 0 {}

	//Interface version functions
	static inline unsigned int ThisILogEntryVersion() { return 1; }
	virtual unsigned int GetILogEntryVersion() const = 0;

	//Getters
	virtual EventLevel GetEventLevel() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetText() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetSource() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetEventLevelString() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetTimeString() const = 0;

	//Setters
	virtual void OverrideSourceText(const MarshalSupport::Marshal::In<std::wstring>& asource) const = 0;

	//Version functions
	virtual unsigned int GetInterfaceVersion() const = 0;
};

#include "ILogEntry.inl"
#endif
