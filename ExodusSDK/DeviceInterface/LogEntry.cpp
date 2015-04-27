#include "LogEntry.h"
#include "WindowsSupport/WindowsSupport.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel alevel)
:eventLevel(alevel)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	hour = (int)time.wHour;
	minute = (int)time.wMinute;
	second = (int)time.wSecond;
	millisecond = (int)time.wMilliseconds;
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel alevel, const std::wstring& atext)
:eventLevel(alevel)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	hour = (int)time.wHour;
	minute = (int)time.wMinute;
	second = (int)time.wSecond;
	millisecond = (int)time.wMilliseconds;
	SetText(atext);
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel alevel, const std::wstring& asource, const std::wstring& atext)
:eventLevel(alevel), source(asource)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	hour = (int)time.wHour;
	minute = (int)time.wMinute;
	second = (int)time.wSecond;
	millisecond = (int)time.wMilliseconds;
	SetText(atext);
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(const LogEntry& object)
:eventLevel(object.eventLevel), hour(object.hour), minute(object.minute), second(object.second), millisecond(object.millisecond), source(object.source)
{
	SetText(object.GetText());
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int LogEntry::GetILogEntryVersion() const
{
	return ThisILogEntryVersion();
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
LogEntry::EventLevel LogEntry::GetEventLevel() const
{
	return eventLevel;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> LogEntry::GetText() const
{
	return text.str();
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> LogEntry::GetSource() const
{
	return source;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> LogEntry::GetEventLevelString() const
{
	switch(eventLevel)
	{
	case EventLevel::Info:
		return L"1 - Information";
	case EventLevel::Debug:
		return L"2 - Debug";
	case EventLevel::Warning:
		return L"3 - Warning";
	case EventLevel::Error:
		return L"4 - Error";
	case EventLevel::Critical:
		return L"5 - Critical";
	default:
		return L"";
	}
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> LogEntry::GetTimeString() const
{
	std::wstringstream stream;
	stream << std::setw(2) << std::setfill(L'0') << hour << L':' << minute << L':' << second << L'.' << millisecond;
	return stream.str();
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void LogEntry::OverrideSourceText(const MarshalSupport::Marshal::In<std::wstring>& asource) const
{
	source = asource;
}

//----------------------------------------------------------------------------------------
//Version functions
//----------------------------------------------------------------------------------------
unsigned int LogEntry::GetInterfaceVersion() const
{
	return 1;
}
