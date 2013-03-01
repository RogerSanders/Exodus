#ifndef __LOGENTRY_H__
#define __LOGENTRY_H__
#include "ILogEntry.h"
#include "WindowFunctions/WindowFunctions.pkg"
#include <string>
#include <sstream>
#include <iomanip>

class LogEntry :public ILogEntry
{
public:
	//Constructors
	inline LogEntry(EventLevel alevel);
	inline LogEntry(EventLevel alevel, const std::wstring& atext);
	inline LogEntry(EventLevel alevel, const std::wstring& asource, const std::wstring& atext);
	inline LogEntry(const LogEntry& object);

	//Interface version functions
	virtual unsigned int GetILogEntryVersion() const;

	//Getters
	virtual EventLevel GetEventLevel() const;

	//Setters
	inline void SetText(const std::wstring& atext);
	inline void SetSource(const std::wstring& asource);
	inline void SetEventLevel(EventLevel alevel);

	//Version functions
	virtual unsigned int GetInterfaceVersion() const;

	//Text-based stream functions
	template<class T> inline LogEntry& operator>>(T& data);
	template<class T> inline LogEntry& operator<<(const T& data);

protected:
	//Getters
	virtual const wchar_t* GetTextInternal() const;
	virtual const wchar_t* GetSourceInternal() const;
	virtual const wchar_t* GetEventLevelStringInternal() const;
	virtual const wchar_t* GetTimeStringInternal() const;

	//Setters
	virtual void OverrideSourceTextInternal(const wchar_t* asource) const;

private:
	EventLevel eventLevel;
	SYSTEMTIME time;
	mutable std::wstring source;
	std::wstringstream text;
	mutable std::wstring timeStringCached;
	mutable std::wstring textStringCached;
};

#include "LogEntry.inl"
#endif
