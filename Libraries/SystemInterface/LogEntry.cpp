#include "LogEntry.h"

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
