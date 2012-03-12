//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel alevel)
:eventLevel(alevel)
{
	GetLocalTime(&time);
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel alevel, const std::wstring& atext)
:eventLevel(alevel)
{
	GetLocalTime(&time);
	SetText(atext);
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel alevel, const std::wstring& asource, const std::wstring& atext)
:eventLevel(alevel), source(asource)
{
	GetLocalTime(&time);
	SetText(atext);
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(const LogEntry& object)
:eventLevel(object.eventLevel), time(object.time), source(object.source)
{
	SetText(object.GetText());
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void LogEntry::SetText(const std::wstring& atext)
{
	text.str() = L"";
	text << atext;
}

//----------------------------------------------------------------------------------------
void LogEntry::SetSource(const std::wstring& asource)
{
	source = asource;
}

//----------------------------------------------------------------------------------------
void LogEntry::SetEventLevel(EventLevel alevel)
{
	eventLevel = alevel;
}

//----------------------------------------------------------------------------------------
//Text-based stream functions
//----------------------------------------------------------------------------------------
template<class T> LogEntry& LogEntry::operator>>(T& data)
{
	text << data;
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> LogEntry& LogEntry::operator<<(const T& data)
{
	text << data;
	return *this;
}
