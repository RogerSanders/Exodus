//Disable warning about the presence of virtual functions without a virtual destructor.
//Our structures below use virtual functions to create code barriers between assemblies so
//that they can be marshalled, but they are never derived from, so a virtual destructor is
//not required.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4265)
#endif

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
struct IProcessor::CallStackEntry
{
public:
	//Constructors
	CallStackEntry()
	{}
	CallStackEntry(unsigned int asourceAddress, unsigned int atargetAddress, unsigned int areturnAddress, const std::wstring& adisassembly)
	:sourceAddress(asourceAddress), targetAddress(atargetAddress), returnAddress(areturnAddress), disassembly(adisassembly)
	{}
	CallStackEntry(MarshalSupport::marshal_object_t, const CallStackEntry& source)
	{
		source.MarshalToTarget(sourceAddress, targetAddress, returnAddress, disassembly);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& sourceAddressMarshaller, unsigned int& targetAddressMarshaller, unsigned int& returnAddressMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& disassemblyMarshaller) const
	{
		sourceAddressMarshaller = sourceAddress;
		targetAddressMarshaller = targetAddress;
		returnAddressMarshaller = returnAddress;
		disassemblyMarshaller = disassembly;
	}

public:
	unsigned int sourceAddress;
	unsigned int targetAddress;
	unsigned int returnAddress;
	std::wstring disassembly;
};

//----------------------------------------------------------------------------------------
struct IProcessor::TraceLogEntry
{
public:
	//Constructors
	explicit TraceLogEntry(unsigned int aaddress = 0)
	:address(aaddress)
	{}
	TraceLogEntry(MarshalSupport::marshal_object_t, const TraceLogEntry& source)
	{
		source.MarshalToTarget(address, disassembly);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& addressMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& disassemblyMarshaller) const
	{
		addressMarshaller = address;
		disassemblyMarshaller = disassembly;
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

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif
