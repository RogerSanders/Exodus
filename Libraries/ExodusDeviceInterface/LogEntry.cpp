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
void LogEntry::GetTextInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetText());
}

//----------------------------------------------------------------------------------------
void LogEntry::GetSourceInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetSource());
}

//----------------------------------------------------------------------------------------
void LogEntry::GetEventLevelStringInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetEventLevelString());
}

//----------------------------------------------------------------------------------------
void LogEntry::GetTimeStringInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetTimeString());
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void LogEntry::OverrideSourceTextInternal(const InteropSupport::ISTLObjectSource<std::wstring>& sourceMarshaller) const
{
	sourceMarshaller.MarshalTo(source);
}

//----------------------------------------------------------------------------------------
//Version functions
//----------------------------------------------------------------------------------------
unsigned int LogEntry::GetInterfaceVersion() const
{
	return 1;
}
