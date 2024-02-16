//----------------------------------------------------------------------------------------------------------------------
// Enumerations
//----------------------------------------------------------------------------------------------------------------------
enum class Processor::DisassemblyEntryType
{
	Code,
	CodeAutoDetect,
	Data,
	OffsetCode,
	OffsetData
};

//----------------------------------------------------------------------------------------------------------------------
enum class Processor::DisassemblyDataType
{
	Integer,
	Float,
	Character
};

//----------------------------------------------------------------------------------------------------------------------
// Structures
//----------------------------------------------------------------------------------------------------------------------
struct Processor::LabelSubstitutionSettings
{
	LabelSubstitutionSettings()
	:enableSubstitution(false), observedLabelCount(0), detectedLabelCount(0)
	{ }

	bool enableSubstitution;
	std::map<unsigned int, LabelSubstitutionEntry> labelTargetsAddressingLocation;
	std::map<unsigned int, LabelEntry> labelTargetsAtLocation;
	unsigned int observedLabelCount;
	unsigned int detectedLabelCount;
};

//----------------------------------------------------------------------------------------------------------------------
struct Processor::LabelSubstitutionEntry
{
	unsigned int targetAddress;
	unsigned int labelLocation;
	int locationOffset;
	std::wstring usageLabel;
};

//----------------------------------------------------------------------------------------------------------------------
struct Processor::LabelEntry
{
	LabelEntry()
	:predicted(true)
	{ }

	unsigned int location;
	std::wstring label;
	bool predicted;
};

//----------------------------------------------------------------------------------------------------------------------
struct Processor::BreakpointCallbackParams
{
	Processor* object;
	Breakpoint* breakpoint;
};

//----------------------------------------------------------------------------------------------------------------------
struct Processor::WatchpointCallbackParams
{
	Processor* object;
	Watchpoint* watchpoint;
};

//----------------------------------------------------------------------------------------------------------------------
struct Processor::DisassemblyAddressInfo
{
	DisassemblyAddressInfo()
	:conflictsWithKnownCode(false), arrayStartingHereDefined(false), entryDefinedOutsideArray(false)
	{ }

	// Comment info
	std::wstring comment;

	// Entry info
	DisassemblyEntryType entryType;
	unsigned int baseMemoryAddress;
	unsigned int memoryBlockSize;

	// Data info
	DisassemblyDataType dataType;

	// Conflict info
	bool conflictsWithKnownCode;

	// Offset info
	bool relativeOffset;
	unsigned int relativeOffsetBaseAddress;

	// Array info
	bool entryDefinedOutsideArray;
	bool arrayStartingHereDefined;
	unsigned int arrayIDStartingHere;
	std::set<unsigned int> arraysMemberOf;
};

//----------------------------------------------------------------------------------------------------------------------
struct Processor::DisassemblyArrayInfo
{
	DisassemblyArrayInfo()
	:conflictsWithKnownCode(false)
	{ }

	unsigned int arrayID;
	unsigned int baseMemoryAddress;
	unsigned int arrayEntrySize;
	unsigned int arrayEntryCount;
	DisassemblyDataType dataType;
	bool conflictsWithKnownCode;
};

//----------------------------------------------------------------------------------------------------------------------
struct Processor::DisassemblyJumpTableInfo
{
	unsigned int baseMemoryAddress;
	unsigned int entrySize;
	std::set<unsigned int> knownEntries;
};

//----------------------------------------------------------------------------------------------------------------------
struct Processor::ActiveDisassemblyAnalysisData
{
	~ActiveDisassemblyAnalysisData()
	{
		Initialize();
	}
	void Initialize()
	{
		// Erase all our allocated data items
		for (std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = disassemblyCodeSorted.begin(); i != disassemblyCodeSorted.end(); ++i)
		{
			delete i->second;
		}
		for (std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = predictedCodeEntries.begin(); i != predictedCodeEntries.end(); ++i)
		{
			delete i->second;
		}
		for (std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = predictedDataEntries.begin(); i != predictedDataEntries.end(); ++i)
		{
			delete i->second;
		}
		for (std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = predictedOffsetEntries.begin(); i != predictedOffsetEntries.end(); ++i)
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

//----------------------------------------------------------------------------------------------------------------------
// Control functions
//----------------------------------------------------------------------------------------------------------------------
double Processor::CalculateExecutionTime(unsigned int cycles) const
{
	return ((double)cycles * (1000000000.0 / _reportedClockSpeed));
}

//----------------------------------------------------------------------------------------------------------------------
// Instruction functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int Processor::GetByteCharWidth() const
{
	return ((GetByteBitCount() + 3) / 4);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Processor::GetPCCharWidth() const
{
	return ((GetPCWidth() + 3) / 4);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Processor::GetAddressBusCharWidth() const
{
	return ((GetAddressBusWidth() + 3) / 4);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Processor::GetDataBusCharWidth() const
{
	return ((GetDataBusWidth() + 3) / 4);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Processor::GetPCMask() const
{
	return ((1 << GetPCWidth()) - 1);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Processor::GetAddressBusMask() const
{
	return ((1 << GetAddressBusWidth()) - 1);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Processor::GetDataBusMask() const
{
	return ((1 << GetDataBusWidth()) - 1);
}

//----------------------------------------------------------------------------------------------------------------------
// Breakpoint functions
//----------------------------------------------------------------------------------------------------------------------
void Processor::CheckExecution(unsigned int location)
{
	// Note that we split the internals of this method outside this inline wrapper function
	// for performance. If we fold all the logic into one method, we can't effectively
	// inline it, and we get a big performance penalty in the case that this test fails,
	// which we expect it will almost all the time, due to a lack of inlining and needing
	// to prepare the stack and registers for inner variables that never get used. This has
	// been verified through profiling as a performance bottleneck.
	if (_breakpointExists || _breakOnNextOpcode || _stepOver || _transientBreakpointExists)
	{
		CheckExecutionInternal(location);
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Watchpoint functions
//----------------------------------------------------------------------------------------------------------------------
void Processor::CheckMemoryRead(unsigned int location, unsigned int data)
{
	// Note that we split the internals of this method outside this inline wrapper function
	// for performance. If we fold all the logic into one method, we can't effectively
	// inline it, and we get a big performance penalty in the case that this test fails,
	// which we expect it will almost all the time, due to a lack of inlining and needing
	// to prepare the stack and registers for inner variables that never get used. This has
	// been verified through profiling as a performance bottleneck.
	if (_watchpointExists)
	{
		CheckMemoryReadInternal(location, data);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void Processor::CheckMemoryWrite(unsigned int location, unsigned int data)
{
	// Note that we split the internals of this method outside this inline wrapper function
	// for performance. If we fold all the logic into one method, we can't effectively
	// inline it, and we get a big performance penalty in the case that this test fails,
	// which we expect it will almost all the time, due to a lack of inlining and needing
	// to prepare the stack and registers for inner variables that never get used. This has
	// been verified through profiling as a performance bottleneck.
	if (_watchpointExists)
	{
		CheckMemoryWriteInternal(location, data);
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Trace functions
//----------------------------------------------------------------------------------------------------------------------
void Processor::RecordTrace(unsigned int pc, uint64_t currentCycle, double currentTime)
{
	// Note that we split the internals of this method outside this inline wrapper function
	// for performance. If we fold all the logic into one method, we can't effectively
	// inline it, and we get a big performance penalty in the case that this test fails,
	// which we expect it will almost all the time, due to a lack of inlining and needing
	// to prepare the stack and registers for inner variables that never get used. This has
	// been verified through profiling as a performance bottleneck.
	if (_traceLogEnabled)
	{
		return RecordTraceInternal(pc, currentCycle, currentTime);
	}
}
