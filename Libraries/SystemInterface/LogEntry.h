#ifndef __LOGENTRY_H__
#define __LOGENTRY_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "ILogEntry.h"
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
	inline std::wstring GetText() const;
	inline std::wstring GetSource() const;
	inline std::wstring GetEventLevelString() const;
	inline std::wstring GetTimeString() const;

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
	virtual void GetTextInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetSourceInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetEventLevelStringInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetTimeStringInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Setters
	virtual void OverrideSourceTextInternal(const InteropSupport::ISTLObjectSource<std::wstring>& sourceMarshaller) const;

private:
	EventLevel eventLevel;
	SYSTEMTIME time;
	mutable std::wstring source;
	std::wstringstream text;
};

#include "LogEntry.inl"
#endif
