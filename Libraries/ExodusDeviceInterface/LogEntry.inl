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
