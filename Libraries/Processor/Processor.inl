//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum Processor::DisassemblyEntryType
{
	DISASSEMBLYENTRYTYPE_CODE,
	DISASSEMBLYENTRYTYPE_CODE_AUTODETECT,
	DISASSEMBLYENTRYTYPE_DATA,
	DISASSEMBLYENTRYTYPE_OFFSETCODE,
	DISASSEMBLYENTRYTYPE_OFFSETDATA
};

//----------------------------------------------------------------------------------------
enum Processor::DisassemblyDataType
{
	DISASSEMBLYDATATYPE_INTEGER,
	DISASSEMBLYDATATYPE_FLOAT,
	DISASSEMBLYDATATYPE_CHARACTER
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct Processor::OpcodeInfo
{
	OpcodeInfo()
	:valid(false), opcodeSize(1), undeterminedResultantPCLocation(false)
	{}

	bool valid;
	unsigned int opcodeSize;
	std::wstring disassemblyOpcode;
	std::wstring disassemblyArguments;
	std::wstring disassemblyComment;
	std::set<unsigned int> labelTargetLocations;
	std::set<unsigned int> resultantPCLocations;
	bool undeterminedResultantPCLocation;
};

//----------------------------------------------------------------------------------------
struct Processor::LabelSubstitutionSettings
{
	LabelSubstitutionSettings()
	:enableSubstitution(false), observedLabelCount(0), detectedLabelCount(0)
	{}

	bool enableSubstitution;
	std::map<unsigned int, LabelSubstitutionEntry> labelTargetsAddressingLocation;
	std::map<unsigned int, LabelEntry> labelTargetsAtLocation;
	unsigned int observedLabelCount;
	unsigned int detectedLabelCount;
};

//----------------------------------------------------------------------------------------
struct Processor::LabelSubstitutionEntry
{
	unsigned int targetAddress;
	unsigned int labelLocation;
	int locationOffset;
	std::wstring usageLabel;
};

//----------------------------------------------------------------------------------------
struct Processor::LabelEntry
{
	LabelEntry()
	:predicted(true)
	{}

	unsigned int location;
	std::wstring label;
	bool predicted;
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
	:conflictsWithKnownCode(false), arrayStartingHereDefined(false), entryDefinedOutsideArray(false)
	{}

	//Comment info
	std::wstring comment;

	//Entry info
	DisassemblyEntryType entryType;
	unsigned int baseMemoryAddress;
	unsigned int memoryBlockSize;

	//Data info
	DisassemblyDataType dataType;

	//Conflict info
	bool conflictsWithKnownCode;

	//Offset info
	bool relativeOffset;
	unsigned int relativeOffsetBaseAddress;

	//Array info
	bool entryDefinedOutsideArray;
	bool arrayStartingHereDefined;
	unsigned int arrayIDStartingHere;
	std::set<unsigned int> arraysMemberOf;
};

//----------------------------------------------------------------------------------------
struct Processor::DisassemblyArrayInfo
{
	DisassemblyArrayInfo()
	:conflictsWithKnownCode(false)
	{}

	unsigned int arrayID;
	unsigned int baseMemoryAddress;
	unsigned int arrayEntrySize;
	unsigned int arrayEntryCount;
	DisassemblyDataType dataType;
	bool conflictsWithKnownCode;
};

//----------------------------------------------------------------------------------------
struct Processor::DisassemblyJumpTableInfo
{
	unsigned int baseMemoryAddress;
	unsigned int entrySize;
	std::set<unsigned int> knownEntries;
};

//----------------------------------------------------------------------------------------
struct Processor::ActiveDisassemblyAnalysisData
{
	~ActiveDisassemblyAnalysisData()
	{
		Initialize();
	}
	void Initialize()
	{
		//Erase all our allocated data items
		for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = disassemblyCodeSorted.begin(); i != disassemblyCodeSorted.end(); ++i)
		{
			delete i->second;
		}
		for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = predictedCodeEntries.begin(); i != predictedCodeEntries.end(); ++i)
		{
			delete i->second;
		}
		for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = predictedDataEntries.begin(); i != predictedDataEntries.end(); ++i)
		{
			delete i->second;
		}
		for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = predictedOffsetEntries.begin(); i != predictedOffsetEntries.end(); ++i)
		{
			delete i->second;
		}
		disassemblyCodeSorted.clear();
		disassemblyDataSorted.clear();
		disassemblyOffsetSorted.clear();
		disassemblyArraySorted.clear();
		predictedCodeEntries.clear();
		predictedDataEntries.clear();
		predictedOffsetEntries.clear();
		disassemblyAddressInfo.clear();
		labelSettings.labelTargetsAddressingLocation.clear();
		labelSettings.labelTargetsAtLocation.clear();
	}

	unsigned int minAddress;
	unsigned int maxAddress;
	double offsetArrayIncreaseTolerance;
	unsigned int minimumArrayEntryCount;
	unsigned int offsetArrayDistanceTolerance;
	unsigned int jumpTableDistanceTolerance;
	Processor::LabelSubstitutionSettings labelSettings;
	std::map<unsigned int, DisassemblyAddressInfo*> disassemblyCodeSorted;
	std::map<unsigned int, DisassemblyAddressInfo*> disassemblyDataSorted;
	std::map<unsigned int, DisassemblyAddressInfo*> disassemblyOffsetSorted;
	std::map<unsigned int, DisassemblyArrayInfo> disassemblyArraySorted;
	std::map<unsigned int, DisassemblyAddressInfo*> predictedCodeEntries;
	std::map<unsigned int, DisassemblyAddressInfo*> predictedDataEntries;
	std::map<unsigned int, DisassemblyAddressInfo*> predictedOffsetEntries;
	std::vector<std::list<DisassemblyAddressInfo*>> disassemblyAddressInfo;
};
