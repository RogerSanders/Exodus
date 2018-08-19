//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
Timestamp::Timestamp(const SYSTEMTIME& systemTime)
{
	std::wstringstream dateStream;
	dateStream << systemTime.wYear << L'-' << systemTime.wMonth << L'-' << systemTime.wDay;
	_date = dateStream.str();

	std::wstringstream timeStream;
	timeStream << systemTime.wHour << L':' << std::setw(2) << std::setfill(L'0') << systemTime.wMinute << L':' << std::setw(2) << systemTime.wSecond;
	_time = timeStream.str();
}

//----------------------------------------------------------------------------------------------------------------------
// Timestamp string functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring Timestamp::GetDate() const
{
	return _date;
}

//----------------------------------------------------------------------------------------------------------------------
std::wstring Timestamp::GetFilenameDate() const
{
	return _date;
}

//----------------------------------------------------------------------------------------------------------------------
std::wstring Timestamp::GetTime() const
{
	return _time;
}

//----------------------------------------------------------------------------------------------------------------------
std::wstring Timestamp::GetFilenameTime() const
{
	std::wstring filenameTime = _time;
	std::wstring::size_type pos = filenameTime.find_first_of(L':');
	while (pos != std::wstring::npos)
	{
		filenameTime[pos] = L'.';
		pos = filenameTime.find_first_of(L':');
	}
	return filenameTime;
}

//----------------------------------------------------------------------------------------------------------------------
std::wstring Timestamp::GetTimestamp() const
{
	return GetDate() + L' ' + GetTime();
}

//----------------------------------------------------------------------------------------------------------------------
std::wstring Timestamp::GetFilenameTimestamp() const
{
	return GetFilenameDate() + L' ' + GetFilenameTime();
}
