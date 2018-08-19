#ifndef __LOGENTRY_H__
#define __LOGENTRY_H__
#include "ILogEntry.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace MarshalSupport::Operators;

class LogEntry :public ILogEntry
{
public:
	// Constructors
	LogEntry(EventLevel level);
	LogEntry(EventLevel level, const std::wstring& text);
	LogEntry(EventLevel level, const std::wstring& source, const std::wstring& text);
	LogEntry(const LogEntry& object);

	// Interface version functions
	virtual unsigned int GetILogEntryVersion() const;

	// Getters
	virtual EventLevel GetEventLevel() const;
	virtual Marshal::Ret<std::wstring> GetText() const;
	virtual Marshal::Ret<std::wstring> GetSource() const;
	virtual Marshal::Ret<std::wstring> GetEventLevelString() const;
	virtual Marshal::Ret<std::wstring> GetTimeString() const;

	// Setters
	inline void SetText(const std::wstring& text);
	inline void SetSource(const std::wstring& source);
	inline void SetEventLevel(EventLevel level);
	virtual void OverrideSourceText(const Marshal::In<std::wstring>& source) const;

	// Version functions
	virtual unsigned int GetInterfaceVersion() const;

	// Text-based stream functions
	template<class T> inline LogEntry& operator>>(T& data);
	template<class T> inline LogEntry& operator<<(const T& data);

private:
	EventLevel _eventLevel;
	int _hour;
	int _minute;
	int _second;
	int _millisecond;
	mutable std::wstring _source;
	std::wstringstream _text;
};

#include "LogEntry.inl"
#endif
