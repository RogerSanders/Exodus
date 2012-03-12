//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum Processor::DisassemblyDataType
{
	DISASSEMBLYDATATYPE_CODE,
	DISASSEMBLYDATATYPE_CODE_AUTODETECT,
	DISASSEMBLYDATATYPE_DATA,
	DISASSEMBLYDATATYPE_OFFSETCODE,
	DISASSEMBLYDATATYPE_OFFSETDATA
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct Processor::OpcodeInfo
{
	OpcodeInfo()
	:valid(false), opcodeSize(1)
	{}

	bool valid;
	unsigned int opcodeSize;
	std::wstring disassemblyOpcode;
	std::wstring disassemblyArguments;
	std::wstring disassemblyComment;
};

//----------------------------------------------------------------------------------------
struct Processor::BreakpointCallbackParams
{
	Processor* object;
	Breakpoint* breakpoint;
};

//----------------------------------------------------------------------------------------
struct Processor::CallStackEntry
{
	CallStackEntry()
	{}
	CallStackEntry(unsigned int asourceAddress, unsigned int atargetAddress, unsigned int areturnAddress, const std::wstring& adisassembly)
	:sourceAddress(asourceAddress), targetAddress(atargetAddress), returnAddress(areturnAddress), disassembly(adisassembly)
	{}

	unsigned int sourceAddress;
	unsigned int targetAddress;
	unsigned int returnAddress;
	std::wstring disassembly;
};

//----------------------------------------------------------------------------------------
struct Processor::TraceLogEntry
{
	TraceLogEntry(unsigned int aaddress)
	:address(aaddress)
	{}

	unsigned int address;
	std::wstring disassembly;
};

//----------------------------------------------------------------------------------------
struct Processor::DisassemblyAddressInfo
{
	DisassemblyAddressInfo()
	:conflictsWithKnownCode(false)
	{}

	//Data info
	DisassemblyDataType dataType;
	unsigned int baseMemoryAddress;
	unsigned int memoryBlockSize;

	//Conflict info
	bool conflictsWithKnownCode;

	//Opcode info
	Processor::OpcodeInfo opcodeInfo;

	//Offset info
	bool relativeOffset;
	unsigned int relativeOffsetBaseAddress;

	//Array info
	std::set<unsigned int> arrayIDs;
};

//----------------------------------------------------------------------------------------
struct Processor::DisassemblyArrayInfo
{
	unsigned int arrayID;
	unsigned int baseMemoryAddress;
	unsigned int arrayEntrySize;
	unsigned int arrayEntryCount;
};
