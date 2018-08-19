#include "LogEntry.h"
#include "WindowsSupport/WindowsSupport.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel level)
:_eventLevel(level)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	_hour = (int)time.wHour;
	_minute = (int)time.wMinute;
	_second = (int)time.wSecond;
	_millisecond = (int)time.wMilliseconds;
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel level, const std::wstring& text)
:_eventLevel(level)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	_hour = (int)time.wHour;
	_minute = (int)time.wMinute;
	_second = (int)time.wSecond;
	_millisecond = (int)time.wMilliseconds;
	SetText(text);
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel level, const std::wstring& source, const std::wstring& text)
:_eventLevel(level), _source(source)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	_hour = (int)time.wHour;
	_minute = (int)time.wMinute;
	_second = (int)time.wSecond;
	_millisecond = (int)time.wMilliseconds;
	SetText(text);
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(const LogEntry& object)
:_eventLevel(object._eventLevel), _hour(object._hour), _minute(object._minute), _second(object._second), _millisecond(object._millisecond), _source(object._source)
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
	return _eventLevel;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LogEntry::GetText() const
{
	return _text.str();
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LogEntry::GetSource() const
{
	return _source;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LogEntry::GetEventLevelString() const
{
	switch(_eventLevel)
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
Marshal::Ret<std::wstring> LogEntry::GetTimeString() const
{
	std::wstringstream stream;
	stream << std::setw(2) << std::setfill(L'0') << _hour << L':' << _minute << L':' << _second << L'.' << _millisecond;
	return stream.str();
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void LogEntry::OverrideSourceText(const Marshal::In<std::wstring>& asource) const
{
	_source = asource;
}

//----------------------------------------------------------------------------------------
//Version functions
//----------------------------------------------------------------------------------------
unsigned int LogEntry::GetInterfaceVersion() const
{
	return 1;
}
