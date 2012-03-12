#include "LogEntry.h"

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
LogEntry::EventLevel LogEntry::GetEventLevel() const
{
	return eventLevel;
}

//----------------------------------------------------------------------------------------
const wchar_t* LogEntry::GetTextInternal() const
{
	textStringCached = text.str();
	return textStringCached.c_str();
}

//----------------------------------------------------------------------------------------
const wchar_t* LogEntry::GetSourceInternal() const
{
	return source.c_str();
}

//----------------------------------------------------------------------------------------
const wchar_t* LogEntry::GetEventLevelStringInternal() const
{
	switch(eventLevel)
	{
	case EVENTLEVEL_INFO:
		return L"1 - Information";
	case EVENTLEVEL_DEBUG:
		return L"2 - Debug";
	case EVENTLEVEL_WARNING:
		return L"3 - Warning";
	case EVENTLEVEL_ERROR:
		return L"4 - Error";
	case EVENTLEVEL_CRITICAL:
		return L"5 - Critical";
	default:
		return L"";
	}
}

//----------------------------------------------------------------------------------------
const wchar_t* LogEntry::GetTimeStringInternal() const
{
	std::wstringstream stream;
	stream << std::setw(2) << std::setfill(L'0') << time.wHour << L':' << time.wMinute << L':' << time.wSecond << L'.' << time.wMilliseconds;
	timeStringCached = stream.str();
	return timeStringCached.c_str();
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void LogEntry::OverrideSourceTextInternal(const wchar_t* asource) const
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
