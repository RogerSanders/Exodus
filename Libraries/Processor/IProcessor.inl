//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IProcessor::IProcessorDataSource
{
	BreakpointName = 1,
	BreakpointEnable,
	BreakpointLog,
	BreakpointBreak,
	BreakpointNotCondition,
	BreakpointCondition,
	BreakpointLocation1,
	BreakpointLocation2,
	BreakpointLocationMask,
	BreakpointHitCounter,
	BreakpointEnableBreakInterval,
	BreakpointBreakInterval,
	End = 0x1000
};

//----------------------------------------------------------------------------------------
enum class IProcessor::IProcessorCommand
{
	BreakpointResetHitCounter = 1,
	BreakpointDelete,
	BreakpointNew,
	BreakpointEnableAll,
	BreakpointDisableAll,
	BreakpointDeleteAll,
	End = 0x1000
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct IProcessor::CallStackEntry :protected InteropSupport::IMarshallingObject
{
public:
	//Constructors
	CallStackEntry()
	{}
	CallStackEntry(unsigned int asourceAddress, unsigned int atargetAddress, unsigned int areturnAddress, const std::wstring& adisassembly)
	:sourceAddress(asourceAddress), targetAddress(atargetAddress), returnAddress(areturnAddress), disassembly(adisassembly)
	{}

protected:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& source)
	{
		const CallStackEntry& sourceResolved = static_cast<const CallStackEntry&>(source);
		sourceResolved.MarshalToTarget(sourceAddress, targetAddress, returnAddress, InteropSupport::STLObjectTarget<std::wstring>(disassembly));
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& sourceAddressMarshaller, unsigned int& targetAddressMarshaller, unsigned int& returnAddressMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& disassemblyMarshaller) const
	{
		sourceAddressMarshaller = sourceAddress;
		targetAddressMarshaller = targetAddress;
		returnAddressMarshaller = returnAddress;
		disassemblyMarshaller.MarshalFrom(disassembly);
	}

public:
	unsigned int sourceAddress;
	unsigned int targetAddress;
	unsigned int returnAddress;
	std::wstring disassembly;
};

//----------------------------------------------------------------------------------------
struct IProcessor::TraceLogEntry :protected InteropSupport::IMarshallingObject
{
public:
	//Constructors
	explicit TraceLogEntry(unsigned int aaddress = 0)
	:address(aaddress)
	{}

protected:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& source)
	{
		const TraceLogEntry& sourceResolved = static_cast<const TraceLogEntry&>(source);
		sourceResolved.MarshalToTarget(address, InteropSupport::STLObjectTarget<std::wstring>(disassembly));
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& addressMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& disassemblyMarshaller) const
	{
		addressMarshaller = address;
		disassemblyMarshaller.MarshalFrom(disassembly);
	}

public:
	unsigned int address;
	std::wstring disassembly;
};

//----------------------------------------------------------------------------------------
struct IProcessor::BreakpointDataContext :public IGenericAccess::DataContext
{
	BreakpointDataContext(IBreakpoint* abreakpoint)
	:breakpoint(abreakpoint)
	{}

	IBreakpoint* breakpoint;
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IProcessor::ThisIProcessorVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Breakpoint functions
//----------------------------------------------------------------------------------------
std::list<IBreakpoint*> IProcessor::GetBreakpointList() const
{
	std::list<IBreakpoint*> result;
	GetBreakpointListInternal(InteropSupport::STLObjectTarget<std::list<IBreakpoint*>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Watchpoint functions
//----------------------------------------------------------------------------------------
std::list<IWatchpoint*> IProcessor::GetWatchpointList() const
{
	std::list<IWatchpoint*> result;
	GetWatchpointListInternal(InteropSupport::STLObjectTarget<std::list<IWatchpoint*>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Call stack functions
//----------------------------------------------------------------------------------------
std::list<IProcessor::CallStackEntry> IProcessor::GetCallStack() const
{
	std::list<CallStackEntry> result(0);
	GetCallStackInternal(InteropSupport::STLObjectTarget<std::list<CallStackEntry>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Trace functions
//----------------------------------------------------------------------------------------
std::list<IProcessor::TraceLogEntry> IProcessor::GetTraceLog() const
{
	std::list<TraceLogEntry> result(0);
	GetTraceLogInternal(InteropSupport::STLObjectTarget<std::list<TraceLogEntry>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Active disassembly analysis functions
//----------------------------------------------------------------------------------------
bool IProcessor::ActiveDisassemblyExportAnalysisToASMFile(const std::wstring& filePath) const
{
	return ActiveDisassemblyExportAnalysisToASMFileInternal(InteropSupport::STLObjectSource<std::wstring>(filePath));
}

//----------------------------------------------------------------------------------------
bool IProcessor::ActiveDisassemblyExportAnalysisToTextFile(const std::wstring& filePath) const
{
	return ActiveDisassemblyExportAnalysisToTextFileInternal(InteropSupport::STLObjectSource<std::wstring>(filePath));
}

//----------------------------------------------------------------------------------------
bool IProcessor::ActiveDisassemblyExportAnalysisToIDCFile(const std::wstring& filePath) const
{
	return ActiveDisassemblyExportAnalysisToIDCFileInternal(InteropSupport::STLObjectSource<std::wstring>(filePath));
}
