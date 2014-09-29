//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class ILogEntry::EventLevel
{
	Info,          //Informational events record changes which are not unexpected or abnormal, but of which the user might want to be made aware. These will mostly be output by the system itself, rather than the emulation cores.
	Debug,         //Debug events such as breakpoints being triggered.
	Warning,       //Warnings are for any events that the user should be notified about, but are not necessarily errors. This may include warnings from devices about reliance on unpredictable or undefined behaviour.
	Error,         //An error is raised when a component of the system attempts to do something which it cannot do. Errors should be logged on exceptional behaviour, such as when a device hangs or locks, to assist in locating and debugging faulty code.
	Critical       //Used for internal critical events. Reserved for error checking the consistency of emulation cores or the system itself. Think of this as an assertion failure.
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ILogEntry::ThisILogEntryVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
std::wstring ILogEntry::GetText() const
{
	std::wstring result;
	GetTextInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring ILogEntry::GetSource() const
{
	std::wstring result;
	GetSourceInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring ILogEntry::GetEventLevelString() const
{
	std::wstring result;
	GetEventLevelStringInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring ILogEntry::GetTimeString() const
{
	std::wstring result;
	GetTimeStringInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void ILogEntry::OverrideSourceText(const std::wstring& asource) const
{
	OverrideSourceTextInternal(InteropSupport::STLObjectSource<std::wstring>(asource));
}
