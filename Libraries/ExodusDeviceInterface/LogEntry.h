#ifndef __LOGENTRY_H__
#define __LOGENTRY_H__
#include "ILogEntry.h"
#include <string>
#include <sstream>
#include <iomanip>

class LogEntry :public ILogEntry
{
public:
	//Constructors
	LogEntry(EventLevel alevel);
	LogEntry(EventLevel alevel, const std::wstring& atext);
	LogEntry(EventLevel alevel, const std::wstring& asource, const std::wstring& atext);
	LogEntry(const LogEntry& object);

	//Interface version functions
	virtual unsigned int GetILogEntryVersion() const;

	//Getters
	virtual EventLevel GetEventLevel() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetText() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetSource() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetEventLevelString() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetTimeString() const;

	//Setters
	inline void SetText(const std::wstring& atext);
	inline void SetSource(const std::wstring& asource);
	inline void SetEventLevel(EventLevel alevel);
	virtual void OverrideSourceText(const MarshalSupport::Marshal::In<std::wstring>& asource) const;

	//Version functions
	virtual unsigned int GetInterfaceVersion() const;

	//Text-based stream functions
	template<class T> inline LogEntry& operator>>(T& data);
	template<class T> inline LogEntry& operator<<(const T& data);

private:
	EventLevel eventLevel;
	int hour;
	int minute;
	int second;
	int millisecond;
	mutable std::wstring source;
	std::wstringstream text;
};

#include "LogEntry.inl"
#endif
