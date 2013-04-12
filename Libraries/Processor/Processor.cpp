#include "Processor.h"
#include "SystemInterface/SystemInterface.pkg"
#include "DataConversion/DataConversion.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "Stream/Stream.pkg"
#include "DebugMenuHandler.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <boost/bind.hpp>
#include <boost/function.hpp>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::Processor(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(aimplementationName, ainstanceName, amoduleID),
menuHandler(0),
clockSpeed(0), reportedClockSpeed(0), clockSpeedOverridden(false),
traceEnabled(false), traceDisassemble(false), traceLength(200),
stackDisassemble(false), stepOver(false), stepOut(false),
breakOnNextOpcode(false), breakpointExists(false), watchpointExists(false)
{
	//Initialize active disassembly info
	activeDisassemblyAnalysis = new ActiveDisassemblyAnalysisData();
	activeDisassemblyEnabled = false;
	activeDisassemblyArrayNextFreeID = 1;
	activeDisassemblyStartLocation = 0;
	activeDisassemblyEndLocation = 0;
	activeDisassemblyAnalysisStartLocation = activeDisassemblyStartLocation;
	activeDisassemblyAnalysisEndLocation = activeDisassemblyEndLocation;
	activeDisassemblyAnalyzeCode = true;
	activeDisassemblyAnalyzeData = true;
	activeDisassemblyAnalyzeCodeOffsets = true;
	activeDisassemblyAnalyzeDataOffsets = true;
	activeDisassemblyAnalyzePredictedArrays = true;
	activeDisassemblyAnalyzePredictedJumpTables = true;
	activeDisassemblyExploreCodePaths = true;
	activeDisassemblyPerformLabelSubstitution = true;
	activeDisassemblyDetectOffsets = true;
	activeDisassemblyDetectJumpTables = true;
	activeDisassemblyDetectData = true;
	activeDisassemblyDetectDataArrays = true;
	activeDisassemblyDetectTextData = true;
	activeDisassemblyOffsetArrayIncreaseTolerance = 2.1;
	activeDisassemblyMinimumArrayEntryCount = 4;
	activeDisassemblyOffsetArrayDistanceTolerance = 0x40;
	activeDisassemblyJumpTableDistanceTolerance = 0x100;
}

//----------------------------------------------------------------------------------------
Processor::~Processor()
{
	//Delete any remaining breakpoint objects
	for(BreakpointList::const_iterator i = breakpoints.begin(); i != breakpoints.end(); ++i)
	{
		delete *i;
	}
	breakpoints.clear();

	//Delete any remaining watchpoint objects
	for(WatchpointList::const_iterator i = watchpoints.begin(); i != watchpoints.end(); ++i)
	{
		delete *i;
	}
	watchpoints.clear();

	//Delete all disassembly info
	for(std::set<DisassemblyAddressInfo*>::const_iterator i = activeDisassemblyAddressInfoSet.begin(); i != activeDisassemblyAddressInfoSet.end(); ++i)
	{
		delete *i;
	}
	activeDisassemblyAddressInfoSet.clear();
	activeDisassemblyAddressInfo.clear();
	activeDisassemblyJumpTableInfo.clear();
	activeDisassemblyArrayInfo.clear();
	delete activeDisassemblyAnalysis;

	//Delete the menu handler
	if(menuHandler != 0)
	{
		menuHandler->ClearMenuItems();
		delete menuHandler;
	}
}

//----------------------------------------------------------------------------------------
bool Processor::Construct(IHeirarchicalStorageNode& node)
{
	bool result = Device::Construct(node);

	IHeirarchicalStorageAttribute* clockSpeedAttribute = node.GetAttribute(L"ClockSpeed");
	if(clockSpeedAttribute != 0)
	{
		clockSpeed = clockSpeedAttribute->ExtractValue<double>();
	}

	return result;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void Processor::ExecuteRollback()
{
	boost::mutex::scoped_lock lock(debugMutex);

	//Clock speed
	clockSpeed = bclockSpeed;
	if(!clockSpeedOverridden)
	{
		reportedClockSpeed = clockSpeed;
	}

	//Call stack and trace log
	callStack = bcallStack;
	trace = btrace;

	//Breakpoint and Watchpoint hit counters
	if(breakpointExists)
	{
		for(BreakpointList::iterator i = breakpoints.begin(); i != breakpoints.end(); ++i)
		{
			(*i)->Rollback();
		}
	}
	if(watchpointExists)
	{
		for(WatchpointList::iterator i = watchpoints.begin(); i != watchpoints.end(); ++i)
		{
			(*i)->Rollback();
		}
	}

	//Step over/out flags
	breakOnNextOpcode = bbreakOnNextOpcode;
	stepOver = bstepOver;
	stepOut = bstepOut;
	stackLevel = bstackLevel;
}

//----------------------------------------------------------------------------------------
void Processor::ExecuteCommit()
{
	boost::mutex::scoped_lock lock(debugMutex);

	//Clock speed
	bclockSpeed = clockSpeed;

	//Call stack and trace log
	bcallStack = callStack;
	btrace = trace;

	//Breakpoint and Watchpoint hit counters
	if(breakpointExists)
	{
		for(BreakpointList::iterator i = breakpoints.begin(); i != breakpoints.end(); ++i)
		{
			(*i)->Commit();
		}
	}
	if(watchpointExists)
	{
		for(WatchpointList::iterator i = watchpoints.begin(); i != watchpoints.end(); ++i)
		{
			(*i)->Commit();
		}
	}

	//Step over/out flags
	bbreakOnNextOpcode = breakOnNextOpcode;
	bstepOver = stepOver;
	bstepOut = stepOut;
	bstackLevel = stackLevel;
}

//----------------------------------------------------------------------------------------
Processor::UpdateMethod Processor::GetUpdateMethod() const
{
	return UPDATEMETHOD_STEP;
}

//----------------------------------------------------------------------------------------
//Control functions
//----------------------------------------------------------------------------------------
double Processor::GetClockSpeed() const
{
	return reportedClockSpeed;
}

//----------------------------------------------------------------------------------------
void Processor::SetClockSpeed(double aclockSpeed)
{
	clockSpeed = aclockSpeed;
	if(!clockSpeedOverridden)
	{
		reportedClockSpeed = clockSpeed;
	}
}

//----------------------------------------------------------------------------------------
void Processor::OverrideClockSpeed(double aclockSpeed)
{
	clockSpeedOverridden = true;
	reportedClockSpeed = aclockSpeed;
}

//----------------------------------------------------------------------------------------
void Processor::RestoreClockSpeed()
{
	clockSpeedOverridden = false;
	reportedClockSpeed = clockSpeed;
}

//----------------------------------------------------------------------------------------
double Processor::CalculateExecutionTime(unsigned int cycles) const
{
	return ((double)cycles * (1000000000.0 / GetClockSpeed()));
}

//----------------------------------------------------------------------------------------
//Instruction functions
//----------------------------------------------------------------------------------------
Processor::OpcodeInfo Processor::GetOpcodeInfo(unsigned int location) const
{
	Processor::OpcodeInfo opcodeInfo;
	opcodeInfo.valid = true;
	opcodeInfo.disassemblyOpcode = L"NOT SUPPORTED";
	opcodeInfo.opcodeSize = GetMinimumOpcodeByteSize();
	return opcodeInfo;
}

//----------------------------------------------------------------------------------------
Data Processor::GetRawData(unsigned int location) const
{
	return Data(GetDataBusWidth(), 0);
}

//----------------------------------------------------------------------------------------
unsigned int Processor::GetPCCharWidth() const
{
	return ((GetPCWidth() + 3) / 4);
}

//----------------------------------------------------------------------------------------
unsigned int Processor::GetAddressBusCharWidth() const
{
	return ((GetAddressBusWidth() + 3) / 4);
}

//----------------------------------------------------------------------------------------
unsigned int Processor::GetDataBusCharWidth() const
{
	return ((GetDataBusWidth() + 3) / 4);
}

//----------------------------------------------------------------------------------------
unsigned int Processor::GetPCMask() const
{
	return ((1 << GetPCWidth()) - 1);
}

//----------------------------------------------------------------------------------------
unsigned int Processor::GetAddressBusMask() const
{
	return ((1 << GetAddressBusWidth()) - 1);
}

//----------------------------------------------------------------------------------------
unsigned int Processor::GetDataBusMask() const
{
	return ((1 << GetDataBusWidth()) - 1);
}

//----------------------------------------------------------------------------------------
//Breakpoint functions
//----------------------------------------------------------------------------------------
void Processor::CheckExecution(unsigned int location) const
{
	if(breakpointExists || breakOnNextOpcode || stepOver)
	{
		boost::mutex::scoped_lock lock(debugMutex);

		bool breakOnInstruction = breakOnNextOpcode;
		breakOnNextOpcode = false;
		Breakpoint* triggerBreakpoint = 0;
		for(BreakpointList::iterator i = breakpoints.begin(); i != breakpoints.end(); ++i)
		{
			Breakpoint* breakpoint = *i;
			//Evaluate location
			if(breakpoint->GetEnabled() && breakpoint->PassesLocationCondition(location))
			{
				//Update hitcounter
				if(breakpoint->CheckHitCounter())
				{
					if(breakpoint->GetLogEvent())
					{
						//Log the event
						GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_DEBUG, breakpoint->GetLogString()));
					}
					if(breakpoint->GetBreakEvent())
					{
						breakOnInstruction = true;
						triggerBreakpoint = *i;
					}
				}
			}
		}

		if(breakOnInstruction)
		{
			TriggerBreakpoint(triggerBreakpoint);
		}

		//If we're in step over mode and the stack level is less than or equal to 0, set
		//the breakOnNextOpcode flag to ensure that a breakpoint will be triggered when
		//stepping over a non-branching instruction. If the instruction triggers a branch,
		//this flag will be cleared when a new entry is pushed to the call stack.
		if(stepOver && (stackLevel <= 0))
		{
			breakOnNextOpcode = true;
		}
	}
}

//----------------------------------------------------------------------------------------
void Processor::CheckMemoryRead(unsigned int location, unsigned int data) const
{
	if(watchpointExists)
	{
		boost::mutex::scoped_lock lock(debugMutex);

		bool breakOnInstruction = false;
		Watchpoint* triggerWatchpoint = 0;
		for(WatchpointList::iterator i = watchpoints.begin(); i != watchpoints.end(); ++i)
		{
			//Evaluate location
			Watchpoint* watchpoint = *i;
			if(watchpoint->GetEnabled() && watchpoint->PassesLocationCondition(location) && watchpoint->GetOnRead() && watchpoint->PassesReadCondition(data))
			{
				//Update hitcounter
				if(watchpoint->CheckHitCounter())
				{
					if(watchpoint->GetLogEvent())
					{
						//Log the event
						GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_DEBUG, watchpoint->GetLogString()));
					}
					if(watchpoint->GetBreakEvent())
					{
						breakOnInstruction = true;
						triggerWatchpoint = *i;
					}
				}
			}
		}

		if(breakOnInstruction)
		{
			TriggerBreakpoint(triggerWatchpoint);
		}
	}
}

//----------------------------------------------------------------------------------------
void Processor::CheckMemoryWrite(unsigned int location, unsigned int data) const
{
	if(watchpointExists)
	{
		boost::mutex::scoped_lock lock(debugMutex);

		bool breakOnInstruction = false;
		Watchpoint* triggerWatchpoint = 0;
		for(WatchpointList::iterator i = watchpoints.begin(); i != watchpoints.end(); ++i)
		{
			Watchpoint* watchpoint = *i;
			if(watchpoint->GetEnabled() && watchpoint->PassesLocationCondition(location) && watchpoint->GetOnWrite() && watchpoint->PassesWriteCondition(data))
			{
				//Update hitcounter
				if(watchpoint->CheckHitCounter())
				{
					if(watchpoint->GetLogEvent())
					{
						//Log the event
						GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_DEBUG, watchpoint->GetLogString()));
					}
					if(watchpoint->GetBreakEvent())
					{
						breakOnInstruction = true;
						triggerWatchpoint = *i;
					}
				}
			}
		}

		if(breakOnInstruction)
		{
			TriggerBreakpoint(triggerWatchpoint);
		}
	}
}

//----------------------------------------------------------------------------------------
void Processor::TriggerBreak() const
{
	TriggerBreakpoint(0);
}

//----------------------------------------------------------------------------------------
void Processor::TriggerBreakpoint(Breakpoint* breakpoint) const
{
//	if(!breakpointTriggered)
//	{
	//boost::function<void()> function = boost::bind(&Processor::BreakpointCallback, this, breakpoint);
	//GetDeviceContext()->SetSystemRollback(GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), &function);
	//boost::function<void()> function = boost::bind(&Processor::BreakpointCallback, this, breakpoint);
	BreakpointCallbackParams* params = new BreakpointCallbackParams();
	params->object = (Processor*)this;
	params->breakpoint = breakpoint;
	GetDeviceContext()->SetSystemRollback(GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0, BreakpointCallbackRaw, params);

//		GetDeviceContext()->SetSystemRollback(0, GetCurrentTimesliceProgress());
//		GetDeviceContext()->GetSystem()->FlagStopSystem();

//		breakpointsEnabled = false;
//		breakpointTriggered = true;
//	}
//	else
//	{
//		breakpointTriggered = false;
//		breakOnNextOpcode = false;
//	}
}

//----------------------------------------------------------------------------------------
void Processor::BreakpointCallbackRaw(void* aparams)
{
	BreakpointCallbackParams* params = (BreakpointCallbackParams*)aparams;
	params->object->BreakpointCallback(params->breakpoint);
	delete params;
}

//----------------------------------------------------------------------------------------
void Processor::BreakpointCallback(Breakpoint* breakpoint) const
{
	if(breakpoint != 0)
	{
		breakpoint->PreIncrementHitCounter();
	}
	GetDeviceContext()->FlagStopSystem();
}

//----------------------------------------------------------------------------------------
void Processor::SetStepOver(bool state)
{
	boost::mutex::scoped_lock lock(debugMutex);
	stepOver = state;
	stepOut = false;
	breakOnNextOpcode = false;
	stackLevel = 0;
}

//----------------------------------------------------------------------------------------
void Processor::SetStepOut(bool state)
{
	boost::mutex::scoped_lock lock(debugMutex);
	stepOut = state;
	stepOver = false;
	breakOnNextOpcode = false;
	stackLevel = 0;
}

//----------------------------------------------------------------------------------------
//Stack functions
//----------------------------------------------------------------------------------------
void Processor::PushCallStack(unsigned int sourceAddress, unsigned int targetAddress, unsigned int returnAddress, const std::wstring& entry, bool fixedDisassembly)
{
	boost::mutex::scoped_lock lock(debugMutex);
	if(stepOut || stepOver)
	{
		++stackLevel;

		//If we're in step over mode and a new entry has just been added to the call
		//stack, clear the breakOnNextOpcode flag to ensure that a breakpoint will no
		//longer be triggered until the call returns.
		breakOnNextOpcode = false;
	}

	CallStackEntry stackEntry(sourceAddress, targetAddress, returnAddress, entry);
	if(stackDisassemble && !fixedDisassembly)
	{
		OpcodeInfo opcodeInfo = GetOpcodeInfo(sourceAddress);
		stackEntry.disassembly = opcodeInfo.disassemblyOpcode + L'\t' + opcodeInfo.disassemblyArguments;
	}

	callStack.push_front(stackEntry);
	if(callStack.size() > 500)
	{
		callStack.pop_back();
	}
}

//----------------------------------------------------------------------------------------
void Processor::PopCallStack(unsigned int returnAddress)
{
	boost::mutex::scoped_lock lock(debugMutex);

	//This call stack has a form of self-healing built in, to deal with direct stack
	//manipulation of return addresses. When a pop is triggered, we pop the stack back to
	//the first matching return address on the call stack. This allows us to deal with
	//multiple-level return cases where the return address is manually popped off the
	//stack. Where no return address matches in the stack, we ignore the pop. This allows
	//us to deal with cases where the return address is modified on the stack. The pop
	//effectively becomes a non-returnable branch, which should not recorded on the
	//stack. When that branch returns, it will appear to do a multiple-level return. This
	//behaviour accurately reflects the actual execution path the code takes.
	for(CallStack::iterator i = callStack.begin(); i != callStack.end(); ++i)
	{
		if(i->returnAddress == returnAddress)
		{
			callStack.erase(callStack.begin(), ++i);
			break;
		}
	}

	if(stepOut || stepOver)
	{
		--stackLevel;
	}
	if(stepOut && (stackLevel < 0))
	{
		stepOut = false;
		breakOnNextOpcode = true;
	}
	else if(stepOver && (stackLevel <= 0))
	{
		stepOver = false;
		breakOnNextOpcode = true;
	}
}

//----------------------------------------------------------------------------------------
void Processor::ClearCallStack()
{
	boost::mutex::scoped_lock lock(debugMutex);
	callStack.clear();
}

//----------------------------------------------------------------------------------------
//Trace functions
//----------------------------------------------------------------------------------------
void Processor::RecordTrace(unsigned int pc)
{
	if(GetTraceEnabled())
	{
		boost::mutex::scoped_lock lock(debugMutex);

		TraceLogEntry traceEntry(pc);
		if(traceDisassemble)
		{
			OpcodeInfo opcodeInfo = GetOpcodeInfo(pc);
			traceEntry.disassembly = opcodeInfo.disassemblyOpcode + L'\t' + opcodeInfo.disassemblyArguments;
		}

		//Add the entry to the running trace log
		trace.push_front(traceEntry);
		while(trace.size() > GetTraceLength())
		{
			trace.pop_back();
		}
	}
}

//----------------------------------------------------------------------------------------
bool Processor::GetTraceEnabled() const
{
	return traceEnabled;
}

//----------------------------------------------------------------------------------------
void Processor::SetTraceEnabled(bool astate)
{
	traceEnabled = astate;
}

//----------------------------------------------------------------------------------------
unsigned int Processor::GetTraceLength() const
{
	return traceLength;
}

//----------------------------------------------------------------------------------------
void Processor::SetTraceLength(unsigned int length)
{
	traceLength = length;
}

//----------------------------------------------------------------------------------------
void Processor::ClearTraceLog()
{
	trace.clear();
}

//----------------------------------------------------------------------------------------
//Active disassembly functions
//----------------------------------------------------------------------------------------
bool Processor::ActiveDisassemblyEnabled() const
{
	return activeDisassemblyEnabled;
}

//----------------------------------------------------------------------------------------
void Processor::EnableActiveDisassembly(unsigned int startLocation, unsigned int endLocation)
{
	//Flag that an active disassembly is not in progress
	activeDisassemblyEnabled = false;

	//Validate the start and end locations for the disassembly
	if(activeDisassemblyEndLocation <= activeDisassemblyStartLocation)
	{
		return;
	}

	//Set the new start and end locations for the disassembly, and record the previous
	//start and end locations being used.
	unsigned int oldActiveDisassemblyStartLocation = activeDisassemblyStartLocation;
	unsigned int oldActiveDisassemblyEndLocation = activeDisassemblyEndLocation;
	activeDisassemblyStartLocation = startLocation;
	activeDisassemblyEndLocation = endLocation;

	//Resize the array holding the disassembly address info
	unsigned int requiredDisassemblyAddressInfoArraySize = activeDisassemblyEndLocation - activeDisassemblyStartLocation;
	if((activeDisassemblyStartLocation != oldActiveDisassemblyStartLocation) || (activeDisassemblyEndLocation != oldActiveDisassemblyEndLocation) || ((unsigned int)activeDisassemblyAddressInfo.size() != requiredDisassemblyAddressInfoArraySize))
	{
		//Clear disassemblyAddressInfo and resize it correctly for the new region
		activeDisassemblyAddressInfo.clear();
		activeDisassemblyAddressInfo.resize(requiredDisassemblyAddressInfoArraySize);

		//Insert current entries into the resized array, and remove any existing entries
		//which now fall outsize the active disassembly region.
		std::set<DisassemblyAddressInfo*> entriesToRemove;
		for(std::set<DisassemblyAddressInfo*>::const_iterator i = activeDisassemblyAddressInfoSet.begin(); i != activeDisassemblyAddressInfoSet.end(); ++i)
		{
			DisassemblyAddressInfo* entry = *i;
			if((entry->baseMemoryAddress < activeDisassemblyStartLocation) || ((entry->baseMemoryAddress + entry->memoryBlockSize) >= activeDisassemblyEndLocation))
			{
				entriesToRemove.insert(entry);
			}
			else
			{
				AddDisassemblyAddressInfoEntryToArray(*i);
			}
		}
		for(std::set<DisassemblyAddressInfo*>::const_iterator i = entriesToRemove.begin(); i != entriesToRemove.end(); ++i)
		{
			activeDisassemblyAddressInfoSet.erase(*i);
			delete *i;
		}

		//Remove any jump table definitions which now begin outside the active disassembly
		//region, and remove any entries which now extend outside the active disassembly
		//region.
		std::set<unsigned int> jumpTableEntriesToRemove;
		for(DisassemblyJumpTableInfoMap::iterator i = activeDisassemblyJumpTableInfo.begin(); i != activeDisassemblyJumpTableInfo.end(); ++i)
		{
			DisassemblyJumpTableInfo& entry = i->second;
			if((entry.baseMemoryAddress < activeDisassemblyStartLocation) || ((entry.baseMemoryAddress + entry.entrySize) > activeDisassemblyEndLocation))
			{
				jumpTableEntriesToRemove.insert(entry.baseMemoryAddress);
			}
			std::set<unsigned int> tableEntriesToRemove;
			for(std::set<unsigned int>::const_iterator entryIterator = entry.knownEntries.begin(); entryIterator != entry.knownEntries.end(); ++entryIterator)
			{
				unsigned int tableEntry = *entryIterator;
				if((tableEntry + entry.entrySize) > activeDisassemblyEndLocation)
				{
					tableEntriesToRemove.insert(tableEntry);
				}
			}
			for(std::set<unsigned int>::const_iterator entryIterator = tableEntriesToRemove.begin(); entryIterator != tableEntriesToRemove.end(); ++entryIterator)
			{
				unsigned int tableEntry = *entryIterator;
				entry.knownEntries.erase(tableEntry);
			}
		}
		for(std::set<unsigned int>::const_iterator i = jumpTableEntriesToRemove.begin(); i != jumpTableEntriesToRemove.end(); ++i)
		{
			activeDisassemblyJumpTableInfo.erase(*i);
		}

		//Remove any arrays which now begin outside the active disassembly region, and
		//truncate any arrays which extend beyond the active disassembly region.
		std::set<unsigned int> arrayEntriesToRemove;
		for(DisassemblyArrayInfoMap::iterator i = activeDisassemblyArrayInfo.begin(); i != activeDisassemblyArrayInfo.end(); ++i)
		{
			DisassemblyArrayInfo& entry = i->second;
			if((entry.baseMemoryAddress < activeDisassemblyStartLocation) || ((entry.baseMemoryAddress + entry.arrayEntrySize) >= activeDisassemblyEndLocation))
			{
				arrayEntriesToRemove.insert(entry.arrayID);
			}
			else if((entry.baseMemoryAddress + (entry.arrayEntryCount * entry.arrayEntrySize)) >= activeDisassemblyEndLocation)
			{
				entry.arrayEntryCount = (activeDisassemblyEndLocation - entry.baseMemoryAddress) / entry.arrayEntryCount;
			}
		}
		for(std::set<unsigned int>::const_iterator i = arrayEntriesToRemove.begin(); i != arrayEntriesToRemove.end(); ++i)
		{
			activeDisassemblyArrayInfo.erase(*i);
		}
	}

	//Flag that an active disassembly is in progress
	activeDisassemblyEnabled = true;
}

//----------------------------------------------------------------------------------------
void Processor::DisableActiveDisassembly()
{
	//Flag that an active disassembly is not in progress
	activeDisassemblyEnabled = false;
}

//----------------------------------------------------------------------------------------
void Processor::ClearActiveDisassembly()
{
	//Delete all disassembly info
	for(std::set<DisassemblyAddressInfo*>::const_iterator i = activeDisassemblyAddressInfoSet.begin(); i != activeDisassemblyAddressInfoSet.end(); ++i)
	{
		delete *i;
	}
	activeDisassemblyAddressInfoSet.clear();
	activeDisassemblyAddressInfo.clear();
	activeDisassemblyJumpTableInfo.clear();
	activeDisassemblyArrayInfo.clear();

	//If active disassembly is currently enabled, re-allocate the disassembly array using
	//the current array size.
	if(activeDisassemblyEnabled)
	{
		unsigned int disassemblyAddressInfoArraySize = activeDisassemblyEndLocation - activeDisassemblyStartLocation;
		activeDisassemblyAddressInfo.resize(disassemblyAddressInfoArraySize);
	}
}

//----------------------------------------------------------------------------------------
unsigned int Processor::MakeDataArrayAtLocation(unsigned int location, unsigned int dataSize, DisassemblyDataType dataType)
{
	//Verify that active disassembly is enabled
	if(!activeDisassemblyEnabled)
	{
		return 0;
	}

	//Verify that this address falls within the target area
	boost::mutex::scoped_lock lock(debugMutex);
	if((location < activeDisassemblyStartLocation) || (location >= activeDisassemblyEndLocation))
	{
		return 0;
	}

	//Try and find existing data references at the same address which define the start of
	//an array. If one is found, return the existing array ID for that data reference.
	std::list<DisassemblyAddressInfo*>& addressList = activeDisassemblyAddressInfo[location - activeDisassemblyStartLocation];
	for(std::list<DisassemblyAddressInfo*>::const_iterator i = addressList.begin(); i != addressList.end(); ++i)
	{
		DisassemblyAddressInfo* entry = *i;
		if((entry->entryType == DISASSEMBLYENTRYTYPE_DATA) && (entry->baseMemoryAddress == location) && (entry->memoryBlockSize == dataSize) && (entry->dataType == dataType) && entry->arrayStartingHereDefined)
		{
			return entry->arrayIDStartingHere;
		}
	}

	//If no data entry exists which defines an array starting at this address, define a
	//new array, and return the array ID.
	DisassemblyArrayInfo arrayInfo;
	arrayInfo.arrayID = activeDisassemblyArrayNextFreeID++;
	arrayInfo.arrayEntryCount = 0;
	arrayInfo.baseMemoryAddress = location;
	arrayInfo.arrayEntrySize = dataSize;
	arrayInfo.dataType = dataType;
	arrayInfo.conflictsWithKnownCode = false;
	activeDisassemblyArrayInfo.insert(DisassemblyArrayInfoMapEntry(arrayInfo.arrayID, arrayInfo));
	return arrayInfo.arrayID;
}

//----------------------------------------------------------------------------------------
void Processor::AddDisassemblyAddressInfoCode(unsigned int location, unsigned int dataSize, const std::wstring& comment)
{
	//Verify that active disassembly is enabled
	if(!activeDisassemblyEnabled)
	{
		return;
	}

	//Verify that this address falls within the target area
	boost::mutex::scoped_lock lock(debugMutex);
	if((location < activeDisassemblyStartLocation) || (location >= activeDisassemblyEndLocation))
	{
		return;
	}

	//Try and find identical existing references at the same address
	bool foundExistingCodeReference = false;
	std::list<DisassemblyAddressInfo*>& addressList = activeDisassemblyAddressInfo[location - activeDisassemblyStartLocation];
	for(std::list<DisassemblyAddressInfo*>::const_iterator i = addressList.begin(); i != addressList.end(); ++i)
	{
		DisassemblyAddressInfo* entry = *i;
		if((entry->baseMemoryAddress == location) && (entry->memoryBlockSize == dataSize))
		{
			//Check if this entry refers to an opcode
			if(entry->entryType == DISASSEMBLYENTRYTYPE_CODE)
			{
				//Flag if we've found an existing reference to an opcode at this location
				foundExistingCodeReference = true;
			}
			else if(entry->entryType == DISASSEMBLYENTRYTYPE_CODE_AUTODETECT)
			{
				//If we've found a reference to an auto-detected opcode at this location,
				//change it to a confirmed opcode, and flag that we've found an existing
				//opcode reference here.
				entry->entryType = DISASSEMBLYENTRYTYPE_CODE;
				foundExistingCodeReference = true;
			}
		}
	}

	//If we found an identical existing reference, we don't have anything to do. Abort any
	//further processing.
	if(foundExistingCodeReference)
	{
		return;
	}

	//Create a new DisassemblyAddressInfo object to describe the entry, and insert it into
	//the set of DisassemblyAddressInfo objects.
	DisassemblyAddressInfo* newEntry = new DisassemblyAddressInfo();
	newEntry->entryType = DISASSEMBLYENTRYTYPE_CODE;
	newEntry->baseMemoryAddress = location;
	newEntry->memoryBlockSize = dataSize;
	newEntry->comment = comment;
	newEntry->entryDefinedOutsideArray = true;
	activeDisassemblyAddressInfoSet.insert(newEntry);

	//Add the new reference to the reference list at each address location it occupies,
	//and set conflict flags where appropriate.
	AddDisassemblyAddressInfoEntryToArray(newEntry);
}

//----------------------------------------------------------------------------------------
void Processor::AddDisassemblyAddressInfoData(unsigned int location, unsigned int dataSize, DisassemblyDataType dataType, unsigned int arrayID, const std::wstring& comment)
{
	//Verify that active disassembly is enabled
	if(!activeDisassemblyEnabled)
	{
		return;
	}

	//Verify that this address falls within the target area
	boost::mutex::scoped_lock lock(debugMutex);
	if((location < activeDisassemblyStartLocation) || (location >= activeDisassemblyEndLocation))
	{
		return;
	}

	//Try and find identical existing references at the same address
	bool foundExistingDataReference = false;
	DisassemblyAddressInfo* targetEntry = 0;
	std::list<DisassemblyAddressInfo*>& addressList = activeDisassemblyAddressInfo[location - activeDisassemblyStartLocation];
	std::list<DisassemblyAddressInfo*>::const_iterator addressListIterator = addressList.begin();
	while(!foundExistingDataReference && (addressListIterator != addressList.end()))
	{
		DisassemblyAddressInfo* entry = *addressListIterator;
		if((entry->entryType == DISASSEMBLYENTRYTYPE_DATA) && (entry->baseMemoryAddress == location) && (entry->memoryBlockSize == dataSize) && (entry->dataType == dataType))
		{
			foundExistingDataReference = true;
			targetEntry = entry;
			continue;
		}
		++addressListIterator;
	}

	//If we didn't manage to find an identical existing reference, create a new entry for
	//this data reference.
	if(!foundExistingDataReference)
	{
		//Create a new DisassemblyAddressInfo object to describe the entry, and insert it
		//into the set of DisassemblyAddressInfo objects.
		targetEntry = new DisassemblyAddressInfo();
		targetEntry->entryType = DISASSEMBLYENTRYTYPE_DATA;
		targetEntry->baseMemoryAddress = location;
		targetEntry->memoryBlockSize = dataSize;
		targetEntry->dataType = dataType;
		targetEntry->comment = comment;
		targetEntry->entryDefinedOutsideArray = (arrayID == 0);
		activeDisassemblyAddressInfoSet.insert(targetEntry);

		//Add the new reference to the reference list at each address location it
		//occupies, and set conflict flags where appropriate.
		AddDisassemblyAddressInfoEntryToArray(targetEntry);
	}

	//Add this data entry to the target array if an array ID has been specified
	if(arrayID != 0)
	{
		DisassemblyArrayInfoMap::iterator arrayInfoIterator = activeDisassemblyArrayInfo.find(arrayID);
		if(arrayInfoIterator != activeDisassemblyArrayInfo.end())
		{
			//Validate this entry before adding it to the array. A new array entry can
			//only be pushed back to the end of the array, and it must match the array
			//entry size.
			DisassemblyArrayInfo& arrayInfo = arrayInfoIterator->second;
			if((arrayInfo.arrayEntrySize == dataSize) && (arrayInfo.dataType == dataType) && ((arrayInfo.baseMemoryAddress + (arrayInfo.arrayEntryCount * arrayInfo.arrayEntrySize)) == location))
			{
				//Add this data entry to the target array
				++arrayInfo.arrayEntryCount;
				arrayInfo.conflictsWithKnownCode |= targetEntry->conflictsWithKnownCode;
				targetEntry->arraysMemberOf.insert(arrayID);

				//If this data entry is the first entry in the array, mark it as such.
				if(arrayInfo.baseMemoryAddress == targetEntry->baseMemoryAddress)
				{
					targetEntry->arrayIDStartingHere = arrayID;
					targetEntry->arrayStartingHereDefined = true;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void Processor::AddDisassemblyAddressInfoOffset(unsigned int location, unsigned int dataSize, bool offsetToCode, bool relativeOffset, unsigned int relativeOffsetBaseAddress)
{
	//Verify that active disassembly is enabled
	if(!activeDisassemblyEnabled)
	{
		return;
	}

	//Verify that this address falls within the target area
	boost::mutex::scoped_lock lock(debugMutex);
	if((location < activeDisassemblyStartLocation) || (location >= activeDisassemblyEndLocation))
	{
		return;
	}

	//Determine the type of offset being added
	DisassemblyEntryType entryType = offsetToCode? DISASSEMBLYENTRYTYPE_OFFSETCODE: DISASSEMBLYENTRYTYPE_OFFSETDATA;

	//Try and find identical existing references at the same address
	bool foundExistingDataReference = false;
	std::list<DisassemblyAddressInfo*>& addressList = activeDisassemblyAddressInfo[location - activeDisassemblyStartLocation];
	for(std::list<DisassemblyAddressInfo*>::const_iterator i = addressList.begin(); i != addressList.end(); ++i)
	{
		DisassemblyAddressInfo* entry = *i;
		if((entry->entryType == entryType) && (entry->baseMemoryAddress == location) && (entry->memoryBlockSize == dataSize))
		{
			foundExistingDataReference = true;
		}
	}

	//If we found an identical existing reference, we don't have anything to do. Abort any
	//further processing.
	if(foundExistingDataReference)
	{
		return;
	}

	//Create a new DisassemblyAddressInfo object to describe the entry, and insert it into
	//the set of DisassemblyAddressInfo objects.
	DisassemblyAddressInfo* newEntry = new DisassemblyAddressInfo();
	newEntry->entryType = entryType;
	newEntry->baseMemoryAddress = location;
	newEntry->memoryBlockSize = dataSize;
	newEntry->relativeOffset = relativeOffset;
	newEntry->relativeOffsetBaseAddress = relativeOffsetBaseAddress;
	newEntry->entryDefinedOutsideArray = true;
	activeDisassemblyAddressInfoSet.insert(newEntry);

	//Add the new reference to the reference list at each address location it occupies,
	//and set conflict flags where appropriate.
	AddDisassemblyAddressInfoEntryToArray(newEntry);
}

//----------------------------------------------------------------------------------------
void Processor::AddDisassemblyPossibleBranchTable(unsigned int baseAddress, unsigned int confirmedEntry, unsigned int entrySize)
{
	//Verify that active disassembly is enabled
	if(!activeDisassemblyEnabled)
	{
		return;
	}

	//Verify that this address falls within the target area
	boost::mutex::scoped_lock lock(debugMutex);
	if((baseAddress < activeDisassemblyStartLocation) || (baseAddress >= activeDisassemblyEndLocation))
	{
		return;
	}

	//Record info on this jump table entry
	std::map<unsigned int, DisassemblyJumpTableInfo>::iterator jumpTableInfoIterator = activeDisassemblyJumpTableInfo.find(baseAddress);
	if(jumpTableInfoIterator != activeDisassemblyJumpTableInfo.end())
	{
		//If a jump table is already known at the indicated base address, and the size of
		//this entry matches the recorded entry size, add this entry to the list of known
		//entries in this table.
		DisassemblyJumpTableInfo& jumpTableInfo = jumpTableInfoIterator->second;
		if(jumpTableInfo.entrySize == entrySize)
		{
			jumpTableInfo.knownEntries.insert(confirmedEntry);
		}
	}
	else
	{
		//If this is the first time we've encountered a possible jump table entry with
		//this base address, record information on this suspected jump table.
		DisassemblyJumpTableInfo& jumpTableInfo = activeDisassemblyJumpTableInfo[baseAddress];
		jumpTableInfo.baseMemoryAddress = baseAddress;
		jumpTableInfo.entrySize = entrySize;
		jumpTableInfo.knownEntries.insert(confirmedEntry);
	}
}

//----------------------------------------------------------------------------------------
void Processor::AddDisassemblyAddressInfoEntryToArray(DisassemblyAddressInfo* newEntry)
{
	//Verify that the start address of this entry lies within the array bounds
	if(newEntry->baseMemoryAddress < activeDisassemblyStartLocation)
	{
		return;
	}

	//Add the new reference to the reference list at each address location it occupies,
	//and set conflict flags where appropriate.
	for(unsigned int locationOffset = 0; (locationOffset < newEntry->memoryBlockSize) && ((newEntry->baseMemoryAddress + locationOffset) < activeDisassemblyEndLocation); ++locationOffset)
	{
		//Iterate over each address which falls within the range of this new entry, and
		//set the conflict flags appropriately if there are any known code clashes.
		std::list<DisassemblyAddressInfo*>& addressList = activeDisassemblyAddressInfo[(newEntry->baseMemoryAddress - activeDisassemblyStartLocation) + locationOffset];
		for(std::list<DisassemblyAddressInfo*>::const_iterator i = addressList.begin(); i != addressList.end(); ++i)
		{
			DisassemblyAddressInfo* entry = *i;

			if(newEntry->entryType == DISASSEMBLYENTRYTYPE_CODE)
			{
				//If we've found an existing entry which appears to overlap with the new
				//code entry at this address, mark the existing entry as conflicted.
				if(entry->entryType != DISASSEMBLYENTRYTYPE_CODE)
				{
					entry->conflictsWithKnownCode = true;

					//Mark any arrays this entry is a member of as conflicted
					for(std::set<unsigned int>::const_iterator arrayIterator = entry->arraysMemberOf.begin(); arrayIterator != entry->arraysMemberOf.end(); ++arrayIterator)
					{
						DisassemblyArrayInfoMap::iterator arrayInfoIterator = activeDisassemblyArrayInfo.find(*arrayIterator);
						if(arrayInfoIterator != activeDisassemblyArrayInfo.end())
						{
							arrayInfoIterator->second.conflictsWithKnownCode = true;
						}
					}
				}

				//If we have an actual known code entry which seems to overlap, mark both
				//code entries as conflicted.
				if((entry->entryType == DISASSEMBLYENTRYTYPE_CODE) && ((entry->baseMemoryAddress != newEntry->baseMemoryAddress) || (entry->memoryBlockSize != newEntry->memoryBlockSize)))
				{
					entry->conflictsWithKnownCode = true;
					newEntry->conflictsWithKnownCode = true;
				}
			}
			else
			{
				//If we've found a code entry which overlaps with our new data entry, mark
				//the data entry as conflicted.
				if((entry->entryType == DISASSEMBLYENTRYTYPE_CODE))
				{
					newEntry->conflictsWithKnownCode = true;
				}
			}
		}

		//Add the new reference to the list
		addressList.push_front(newEntry);
	}
}

//----------------------------------------------------------------------------------------
bool Processor::PerformActiveDisassemblyAnalysis(unsigned int minAddress, unsigned int maxAddress, ActiveDisassemblyAnalysisData& analysis) const
{
	//Set the boundaries of this analysis session, and build an address array to perform
	//the analysis. Note that we cannot rely on the active disassembly location array
	//here, since it may not be valid if active disassembly is currently disabled.
	analysis.minAddress = minAddress;
	analysis.maxAddress = maxAddress;
	unsigned int disassemblyAddressInfoArraySize = analysis.maxAddress - analysis.minAddress;
	analysis.disassemblyAddressInfo.resize(disassemblyAddressInfoArraySize);

	//Latch current active disassembly analysis options as the settings for this analysis
	//session
	analysis.labelSettings.enableSubstitution = activeDisassemblyPerformLabelSubstitution;

	//Latch the current specified tolerance values as the tolerance values for this
	//analysis session
	analysis.offsetArrayIncreaseTolerance = activeDisassemblyOffsetArrayIncreaseTolerance;
	analysis.minimumArrayEntryCount = activeDisassemblyMinimumArrayEntryCount;
	analysis.offsetArrayDistanceTolerance = activeDisassemblyOffsetArrayDistanceTolerance;
	analysis.jumpTableDistanceTolerance = activeDisassemblyJumpTableDistanceTolerance;

	//Load the active disassembly data into our analysis arrays
	std::map<unsigned int, const DisassemblyAddressInfo*> disassemblyDataSortedRaw;
	std::map<unsigned int, const DisassemblyAddressInfo*> disassemblyOffsetSortedRaw;
	for(std::set<DisassemblyAddressInfo*>::const_iterator i = activeDisassemblyAddressInfoSet.begin(); i != activeDisassemblyAddressInfoSet.end(); ++i)
	{
		DisassemblyAddressInfo* entry = *i;

		//Exclude entries which lie outside the analysis region
		if((entry->baseMemoryAddress < analysis.minAddress) || ((entry->baseMemoryAddress + entry->memoryBlockSize) > analysis.maxAddress))
		{
			continue;
		}

		//Exclude entries which directly conflict with known code
		if(entry->conflictsWithKnownCode)
		{
			continue;
		}

		//Exclude entries which are only defined as members of arrays that conflict
		//with known code
		bool definedInNonConflictedArray = entry->entryDefinedOutsideArray;
		if(!definedInNonConflictedArray)
		{
			std::set<unsigned int>::const_iterator arrayMembershipIterator = entry->arraysMemberOf.begin();
			while(!definedInNonConflictedArray && (arrayMembershipIterator != entry->arraysMemberOf.end()))
			{
				DisassemblyArrayInfoMap::const_iterator arrayInfoIterator = activeDisassemblyArrayInfo.find(*arrayMembershipIterator);
				if(arrayInfoIterator != activeDisassemblyArrayInfo.end())
				{
					definedInNonConflictedArray = !arrayInfoIterator->second.conflictsWithKnownCode;
				}
				++arrayMembershipIterator;
			}
		}
		if(!definedInNonConflictedArray)
		{
			continue;
		}

		//Add this entry to the appropriate analysis structure
		switch(entry->entryType)
		{
		case DISASSEMBLYENTRYTYPE_CODE:
			if(activeDisassemblyAnalyzeCode)
			{
				DisassemblyAddressInfo* copiedEntry = new DisassemblyAddressInfo(*entry);
				analysis.disassemblyCodeSorted.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(copiedEntry->baseMemoryAddress, copiedEntry));
			}
			break;
		case DISASSEMBLYENTRYTYPE_DATA:
			if(activeDisassemblyAnalyzeData && entry->entryDefinedOutsideArray)
			{
				disassemblyDataSortedRaw.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(entry->baseMemoryAddress, entry));
			}
			break;
		case DISASSEMBLYENTRYTYPE_OFFSETCODE:
			if(activeDisassemblyAnalyzeCodeOffsets)
			{
				disassemblyOffsetSortedRaw.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(entry->baseMemoryAddress, entry));
			}
			break;
		case DISASSEMBLYENTRYTYPE_OFFSETDATA:
			if(activeDisassemblyAnalyzeDataOffsets)
			{
				disassemblyOffsetSortedRaw.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(entry->baseMemoryAddress, entry));
			}
			break;
		default:
			continue;
		}

		//Add this entry to the memory array for the analysis
		for(unsigned int byteOffset = 0; byteOffset < entry->memoryBlockSize; ++byteOffset)
		{
			analysis.disassemblyAddressInfo[(entry->baseMemoryAddress + byteOffset) - analysis.minAddress].push_back(entry);
		}
	}

	//Load the active disassembly array info into our analysis arrays
	std::map<unsigned int, DisassemblyArrayInfo> disassemblyArraySortedRaw;
	if(activeDisassemblyAnalyzePredictedArrays)
	{
		for(DisassemblyArrayInfoMap::const_iterator i = activeDisassemblyArrayInfo.begin(); i != activeDisassemblyArrayInfo.end(); ++i)
		{
			const DisassemblyArrayInfo& entry = i->second;
			if(!entry.conflictsWithKnownCode && (entry.arrayEntryCount > 1))
			{
				disassemblyArraySortedRaw.insert(std::pair<unsigned int, DisassemblyArrayInfo>(entry.baseMemoryAddress, entry));
			}
		}
	}

	//Detect offset arrays
	bool offsetEntrySaved = false;
	const DisassemblyAddressInfo* offsetArrayFirstEntry = 0;
	unsigned int offsetArrayFirstKnownEntryLocation;
	unsigned int offsetArrayLastKnownEntryLocation;
	unsigned int offsetArrayEntrySize;
	DisassemblyEntryType offsetArrayOffsetType;
	bool offsetArrayRelativeOffset;
	unsigned int offsetArrayBaseAddress;
	std::map<unsigned int, const DisassemblyAddressInfo*> knownOffsetArrayEntries;
	for(std::map<unsigned int, const DisassemblyAddressInfo*>::const_iterator i = disassemblyOffsetSortedRaw.begin(); i != disassemblyOffsetSortedRaw.end(); ++i)
	{
		const DisassemblyAddressInfo* currentEntry = i->second;
		if(activeDisassemblyDetectOffsets && offsetEntrySaved && ActiveDisassemblyEntrySeemsToBePartOfArray(analysis, currentEntry, offsetArrayFirstKnownEntryLocation, offsetArrayLastKnownEntryLocation, offsetArrayEntrySize, offsetArrayOffsetType, offsetArrayRelativeOffset, offsetArrayBaseAddress))
		{
			//Add this entry to the offset array
			knownOffsetArrayEntries.insert(std::pair<unsigned int, const DisassemblyAddressInfo*>(currentEntry->baseMemoryAddress, currentEntry));
			offsetArrayLastKnownEntryLocation = currentEntry->baseMemoryAddress;
		}
		else
		{
			//If a previous value has been encountered, save the last value as an array or
			//offset value as appropriate.
			if(offsetEntrySaved)
			{
				if(offsetArrayFirstKnownEntryLocation != offsetArrayLastKnownEntryLocation)
				{
					ActiveDisassemblyGeneratePredictedOffsetArrayEntries(analysis, knownOffsetArrayEntries, offsetArrayFirstKnownEntryLocation, offsetArrayLastKnownEntryLocation, offsetArrayEntrySize, offsetArrayOffsetType, offsetArrayRelativeOffset, offsetArrayBaseAddress);
				}
				else
				{
					DisassemblyAddressInfo* copiedEntry = new DisassemblyAddressInfo(*offsetArrayFirstEntry);
					analysis.disassemblyOffsetSorted.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(copiedEntry->baseMemoryAddress, copiedEntry));
				}
			}

			//Try and create a new offset array from this entry
			offsetEntrySaved = true;
			offsetArrayFirstEntry = currentEntry;
			offsetArrayFirstKnownEntryLocation = currentEntry->baseMemoryAddress;
			offsetArrayLastKnownEntryLocation = currentEntry->baseMemoryAddress;
			offsetArrayEntrySize = currentEntry->memoryBlockSize;
			offsetArrayRelativeOffset = currentEntry->relativeOffset;
			offsetArrayOffsetType = currentEntry->entryType;
			offsetArrayBaseAddress = currentEntry->relativeOffsetBaseAddress;
			knownOffsetArrayEntries.clear();
			knownOffsetArrayEntries.insert(std::pair<unsigned int, const DisassemblyAddressInfo*>(currentEntry->baseMemoryAddress, currentEntry));
		}
	}
	if(offsetEntrySaved)
	{
		if(offsetArrayFirstKnownEntryLocation != offsetArrayLastKnownEntryLocation)
		{
			ActiveDisassemblyGeneratePredictedOffsetArrayEntries(analysis, knownOffsetArrayEntries, offsetArrayFirstKnownEntryLocation, offsetArrayLastKnownEntryLocation, offsetArrayEntrySize, offsetArrayOffsetType, offsetArrayRelativeOffset, offsetArrayBaseAddress);
		}
		else
		{
			DisassemblyAddressInfo* copiedEntry = new DisassemblyAddressInfo(*offsetArrayFirstEntry);
			analysis.disassemblyOffsetSorted.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(copiedEntry->baseMemoryAddress, copiedEntry));
		}
	}

	//Validate our suspected jump table entries, and attempt to predict the boundaries
	//of the tables.
	if(activeDisassemblyAnalyzePredictedJumpTables && activeDisassemblyDetectJumpTables)
	{
		for(DisassemblyJumpTableInfoMap::const_iterator i = activeDisassemblyJumpTableInfo.begin(); i != activeDisassemblyJumpTableInfo.end(); ++i)
		{
			const DisassemblyJumpTableInfo& entry = i->second;
			if(ActiveDisassemblySuspectedJumpTableSeemsToBeValid(analysis, entry))
			{
				ActiveDisassemblyGeneratePredictedJumpTableEntries(analysis, entry);
			}
		}
	}

	//Scan from all known opcode locations and identify any other opcodes which can be
	//determined
	if(activeDisassemblyExploreCodePaths)
	{
		std::set<unsigned int> opcodeLocationsToCheck;
		std::set<unsigned int> opcodeLocationsChecked;
		for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.disassemblyCodeSorted.begin(); i != analysis.disassemblyCodeSorted.end(); ++i)
		{
			opcodeLocationsToCheck.insert(i->first);
		}
		for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.predictedCodeEntries.begin(); i != analysis.predictedCodeEntries.end(); ++i)
		{
			opcodeLocationsToCheck.insert(i->first);
		}
		while(!opcodeLocationsToCheck.empty())
		{
			//Grab the next opcode location to check from the list, and add it to the list
			//of checked opcode locations.
			unsigned int targetAddress = *opcodeLocationsToCheck.begin();
			opcodeLocationsToCheck.erase(targetAddress);
			opcodeLocationsChecked.insert(targetAddress);

			//Retrieve the opcode data for the target address
			OpcodeInfo opcodeInfo = GetOpcodeInfo(targetAddress);
			if(!opcodeInfo.valid)
			{
				continue;
			}

			//Ensure this opcode lies within the analysis region
			if((targetAddress < analysis.minAddress) || ((targetAddress + opcodeInfo.opcodeSize)) >= analysis.maxAddress)
			{
				continue;
			}

			//Add all unchecked resultant PC locations from this opcode to the list of
			//opcode locations to check.
			for(std::set<unsigned int>::const_iterator i = opcodeInfo.resultantPCLocations.begin(); i != opcodeInfo.resultantPCLocations.end(); ++i)
			{
				if(opcodeLocationsChecked.find(*i) == opcodeLocationsChecked.end())
				{
					opcodeLocationsToCheck.insert(*i);
				}
			}

			//If this opcode hasn't already been identified, add a new entry for this
			//predicted code location.
			if((analysis.disassemblyCodeSorted.find(targetAddress) == analysis.disassemblyCodeSorted.end()) && (analysis.predictedCodeEntries.find(targetAddress) == analysis.predictedCodeEntries.end()))
			{
				DisassemblyAddressInfo* newCodeEntry = new DisassemblyAddressInfo();
				newCodeEntry->entryType = DISASSEMBLYENTRYTYPE_CODE_AUTODETECT;
				newCodeEntry->baseMemoryAddress = targetAddress;
				newCodeEntry->memoryBlockSize = opcodeInfo.opcodeSize;
				newCodeEntry->comment = L"Predicted (Code-scan)";
				if(opcodeInfo.undeterminedResultantPCLocation)
				{
					newCodeEntry->comment += L" (Uncertain target!)";
				}
				newCodeEntry->entryDefinedOutsideArray = true;
				analysis.predictedCodeEntries.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(newCodeEntry->baseMemoryAddress, newCodeEntry));
			}
		}
	}

	//Add all predicted entries to the memory array for the analysis
	for(std::map<unsigned int, DisassemblyAddressInfo*>::iterator i = analysis.predictedCodeEntries.begin(); i != analysis.predictedCodeEntries.end(); ++i)
	{
		DisassemblyAddressInfo* entry = i->second;
		for(unsigned int byteOffset = 0; byteOffset < entry->memoryBlockSize; ++byteOffset)
		{
			analysis.disassemblyAddressInfo[(entry->baseMemoryAddress + byteOffset) - analysis.minAddress].push_back(entry);
		}
	}
	for(std::map<unsigned int, DisassemblyAddressInfo*>::iterator i = analysis.predictedOffsetEntries.begin(); i != analysis.predictedOffsetEntries.end(); ++i)
	{
		DisassemblyAddressInfo* entry = i->second;
		for(unsigned int byteOffset = 0; byteOffset < entry->memoryBlockSize; ++byteOffset)
		{
			analysis.disassemblyAddressInfo[(entry->baseMemoryAddress + byteOffset) - analysis.minAddress].push_back(entry);
		}
	}
	for(std::map<unsigned int, DisassemblyAddressInfo*>::iterator i = analysis.predictedDataEntries.begin(); i != analysis.predictedDataEntries.end(); ++i)
	{
		DisassemblyAddressInfo* entry = i->second;
		for(unsigned int byteOffset = 0; byteOffset < entry->memoryBlockSize; ++byteOffset)
		{
			analysis.disassemblyAddressInfo[(entry->baseMemoryAddress + byteOffset) - analysis.minAddress].push_back(entry);
		}
	}

	//Generate labels for absolute and relative address references within the target
	//region
	if(analysis.labelSettings.enableSubstitution)
	{
		//Generate labels for offset address references
		for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.disassemblyOffsetSorted.begin(); i != analysis.disassemblyOffsetSorted.end(); ++i)
		{
			const DisassemblyAddressInfo* entry = i->second;
			ActiveDisassemblyGenerateLabelsForOffset(analysis, entry, false);
		}
		for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.predictedOffsetEntries.begin(); i != analysis.predictedOffsetEntries.end(); ++i)
		{
			const DisassemblyAddressInfo* entry = i->second;
			ActiveDisassemblyGenerateLabelsForOffset(analysis, entry, true);
		}

		//Generate labels for code address references
		for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.disassemblyCodeSorted.begin(); i != analysis.disassemblyCodeSorted.end(); ++i)
		{
			const DisassemblyAddressInfo* entry = i->second;
			OpcodeInfo opcodeInfo = GetOpcodeInfo(entry->baseMemoryAddress);
			if(opcodeInfo.valid)
			{
				for(std::set<unsigned int>::const_iterator targetLocationIterator = opcodeInfo.labelTargetLocations.begin(); targetLocationIterator != opcodeInfo.labelTargetLocations.end(); ++targetLocationIterator)
				{
					ActiveDisassemblyAddLabelForTarget(analysis, *targetLocationIterator, false);
				}
			}
		}
		for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.predictedCodeEntries.begin(); i != analysis.predictedCodeEntries.end(); ++i)
		{
			const DisassemblyAddressInfo* entry = i->second;
			OpcodeInfo opcodeInfo = GetOpcodeInfo(entry->baseMemoryAddress);
			if(opcodeInfo.valid)
			{
				for(std::set<unsigned int>::const_iterator targetLocationIterator = opcodeInfo.labelTargetLocations.begin(); targetLocationIterator != opcodeInfo.labelTargetLocations.end(); ++targetLocationIterator)
				{
					ActiveDisassemblyAddLabelForTarget(analysis, *targetLocationIterator, true);
				}
			}
		}
	}

	//Generate our counts of detected and predicted labels from this analysis operation
	analysis.labelSettings.observedLabelCount = 0;
	analysis.labelSettings.detectedLabelCount = 0;
	for(std::map<unsigned int, LabelEntry>::const_iterator i = analysis.labelSettings.labelTargetsAtLocation.begin(); i != analysis.labelSettings.labelTargetsAtLocation.end(); ++i)
	{
		if(i->second.predicted)
		{
			++analysis.labelSettings.detectedLabelCount;
		}
		else
		{
			++analysis.labelSettings.observedLabelCount;
		}
	}

	//Process all recorded data locations, combining adjacent data reads with the same
	//data type and size into arrays.
	std::set<const DisassemblyAddressInfo*> dataEntriesPrevious;
	DisassemblyArrayInfo dataArrayEntry;
	for(std::map<unsigned int, const DisassemblyAddressInfo*>::iterator i = disassemblyDataSortedRaw.begin(); i != disassemblyDataSortedRaw.end(); ++i)
	{
		const DisassemblyAddressInfo* entry = i->second;

		//If an offset has been recorded at the same location as this data entry, skip it.
		//We expect that an address will often be flagged as data, then later flagged as
		//an offset when that data is used as an offset. In this case, the offset entry
		//silently overrides the data entry. Note that we do combine comment data however,
		//in the case where comment information has been supplied for the data entry and
		//not the offset entry, or where an offset entry has been predicted. We do this
		//because the data entry may contain important context information that was no
		//longer known when the data was used as an offset.
		std::map<unsigned int, DisassemblyAddressInfo*>::iterator knownOffsetIterator = analysis.disassemblyOffsetSorted.find(entry->baseMemoryAddress);
		std::map<unsigned int, DisassemblyAddressInfo*>::iterator predictedOffsetIterator = analysis.predictedOffsetEntries.find(entry->baseMemoryAddress);
		if(knownOffsetIterator != analysis.disassemblyOffsetSorted.end())
		{
			if(knownOffsetIterator->second->comment.empty())
			{
				knownOffsetIterator->second->comment = entry->comment;
			}
			continue;
		}
		if(predictedOffsetIterator != analysis.predictedOffsetEntries.end())
		{
			predictedOffsetIterator->second->comment = entry->comment + L" (Predicted offset)";
			continue;
		}

		//Fold this data entry into an array with the previous entry if appropriate
		if(!dataEntriesPrevious.empty() && (dataArrayEntry.dataType == entry->dataType) && (dataArrayEntry.arrayEntrySize == entry->memoryBlockSize) && entry->entryDefinedOutsideArray)
		{
			unsigned int dataArrayEntryEnd = dataArrayEntry.baseMemoryAddress + (dataArrayEntry.arrayEntryCount * dataArrayEntry.arrayEntrySize);
			if(dataArrayEntryEnd == entry->baseMemoryAddress)
			{
				//Only combine this entry into an array if there isn't a comment
				//specified, and if there isn't a label that needs to be placed before
				//this entry.
				if(entry->comment.empty() && (analysis.labelSettings.labelTargetsAtLocation.find(entry->baseMemoryAddress) == analysis.labelSettings.labelTargetsAtLocation.end()))
				{
					dataEntriesPrevious.insert(entry);
					++dataArrayEntry.arrayEntryCount;
					dataArrayEntry.conflictsWithKnownCode |= entry->conflictsWithKnownCode;
					continue;
				}
			}
		}

		//Save off the previous unsaved array or data entry
		if(!dataEntriesPrevious.empty())
		{
			if(dataEntriesPrevious.size() >= analysis.minimumArrayEntryCount)
			{
				disassemblyArraySortedRaw.insert(std::pair<unsigned int, DisassemblyArrayInfo>(dataArrayEntry.baseMemoryAddress, dataArrayEntry));
			}
			else
			{
				for(std::set<const DisassemblyAddressInfo*>::iterator dataEntriesPreviousIterator = dataEntriesPrevious.begin(); dataEntriesPreviousIterator != dataEntriesPrevious.end(); ++dataEntriesPreviousIterator)
				{
					DisassemblyAddressInfo* copiedEntry = new DisassemblyAddressInfo(*(*dataEntriesPreviousIterator));
					analysis.disassemblyDataSorted.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(copiedEntry->baseMemoryAddress, copiedEntry));
				}
			}
		}

		//Attempt to start a new array definition with this data entry
		dataArrayEntry.baseMemoryAddress = entry->baseMemoryAddress;
		dataArrayEntry.arrayEntrySize = entry->memoryBlockSize;
		dataArrayEntry.dataType = entry->dataType;
		dataArrayEntry.arrayEntryCount = 1;
		dataArrayEntry.conflictsWithKnownCode = entry->conflictsWithKnownCode;
		dataEntriesPrevious.clear();
		dataEntriesPrevious.insert(entry);
	}
	if(!dataEntriesPrevious.empty())
	{
		if(dataEntriesPrevious.size() >= analysis.minimumArrayEntryCount)
		{
			disassemblyArraySortedRaw.insert(std::pair<unsigned int, DisassemblyArrayInfo>(dataArrayEntry.baseMemoryAddress, dataArrayEntry));
		}
		else
		{
			for(std::set<const DisassemblyAddressInfo*>::iterator dataEntriesPreviousIterator = dataEntriesPrevious.begin(); dataEntriesPreviousIterator != dataEntriesPrevious.end(); ++dataEntriesPreviousIterator)
			{
				DisassemblyAddressInfo* copiedEntry = new DisassemblyAddressInfo(*(*dataEntriesPreviousIterator));
				analysis.disassemblyDataSorted.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(copiedEntry->baseMemoryAddress, copiedEntry));
			}
		}
	}

	//Combine adjacent or overlapping arrays with the same data type and element size
	if(activeDisassemblyDetectDataArrays)
	{
		bool unsavedArrayEntryPresent = false;
		DisassemblyArrayInfo unsavedArrayEntry;
		for(std::map<unsigned int, DisassemblyArrayInfo>::const_iterator i = disassemblyArraySortedRaw.begin(); i != disassemblyArraySortedRaw.end(); ++i)
		{
			//Combine this entry with the unsaved entry if the two arrays directly follow
			//each other, or they overlap, and they have the same element type, size, and
			//alignment.
			const DisassemblyArrayInfo& arrayInfo = i->second;
			if(unsavedArrayEntryPresent
			&& (unsavedArrayEntry.dataType == arrayInfo.dataType)
			&& (unsavedArrayEntry.arrayEntrySize == arrayInfo.arrayEntrySize)
			&& (arrayInfo.baseMemoryAddress >= unsavedArrayEntry.baseMemoryAddress) && (arrayInfo.baseMemoryAddress <= (unsavedArrayEntry.baseMemoryAddress + (unsavedArrayEntry.arrayEntryCount * unsavedArrayEntry.arrayEntrySize)))
			&& ((arrayInfo.baseMemoryAddress % arrayInfo.arrayEntrySize) == (unsavedArrayEntry.baseMemoryAddress % unsavedArrayEntry.arrayEntrySize)))
			{
				//Ensure we don't combine arrays where a label needs to be placed between
				//the two
				unsigned int unsavedArrayEndLocation = unsavedArrayEntry.baseMemoryAddress + (unsavedArrayEntry.arrayEntryCount * unsavedArrayEntry.arrayEntrySize);
				if(analysis.labelSettings.labelTargetsAtLocation.find(unsavedArrayEndLocation) == analysis.labelSettings.labelTargetsAtLocation.end())
				{
					unsigned int currentArrayEndLocation = arrayInfo.baseMemoryAddress + (arrayInfo.arrayEntryCount * arrayInfo.arrayEntrySize);
					unsigned int combinedArrayEndLocation = (unsavedArrayEndLocation >= currentArrayEndLocation)? unsavedArrayEndLocation: currentArrayEndLocation;
					unsigned int combinedArrayEntryCount = (combinedArrayEndLocation - unsavedArrayEntry.baseMemoryAddress) / unsavedArrayEntry.arrayEntrySize;
					unsavedArrayEntry.arrayEntryCount = combinedArrayEntryCount;
					unsavedArrayEntry.conflictsWithKnownCode |= arrayInfo.conflictsWithKnownCode;
					continue;
				}
			}

			//Commit the current unsaved entry, and make this the new unsaved entry.
			if(unsavedArrayEntryPresent)
			{
				if(unsavedArrayEntry.arrayEntryCount >= analysis.minimumArrayEntryCount)
				{
					analysis.disassemblyArraySorted.insert(std::pair<unsigned int, DisassemblyArrayInfo>(unsavedArrayEntry.baseMemoryAddress, unsavedArrayEntry));
				}
			}
			unsavedArrayEntry = i->second;
			unsavedArrayEntryPresent = true;
		}
		if(unsavedArrayEntryPresent)
		{
			analysis.disassemblyArraySorted.insert(std::pair<unsigned int, DisassemblyArrayInfo>(unsavedArrayEntry.baseMemoryAddress, unsavedArrayEntry));
		}
	}

	//Detect arrays which appear to contain printable text
	if(activeDisassemblyDetectTextData)
	{
		for(std::map<unsigned int, DisassemblyArrayInfo>::iterator i = analysis.disassemblyArraySorted.begin(); i != analysis.disassemblyArraySorted.end(); ++i)
		{
			//Check the basic array type and size information first to see if this could
			//possibly be a character array, and skip it if it could not.
			DisassemblyArrayInfo& arrayinfo = i->second;
			if((arrayinfo.dataType != DISASSEMBLYDATATYPE_INTEGER) || (arrayinfo.arrayEntrySize != 1))
			{
				continue;
			}

			//Read all values in the array, and add them to a list of values.
			std::list<Data> dataElements;
			const unsigned int bitsPerByte = 8;
			unsigned int entrySizeInBits = arrayinfo.arrayEntrySize * bitsPerByte;
			for(unsigned int i = 0; i < arrayinfo.arrayEntryCount; ++i)
			{
				//Read in this array entry
				unsigned int entryLocation = arrayinfo.baseMemoryAddress + (i * arrayinfo.arrayEntrySize);
				Data entryData(entrySizeInBits, 0);
				unsigned int bitsRead = 0;
				while(bitsRead < entrySizeInBits)
				{
					Data dataSegment = GetRawData(entryLocation + (bitsRead / bitsPerByte));
					unsigned int bitsUsed = (dataSegment.GetBitCount() > (entrySizeInBits - bitsRead))? (entrySizeInBits - bitsRead): dataSegment.GetBitCount();
					entryData.SetDataSegment(entrySizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
					bitsRead += bitsUsed;
				}

				//Add the value of this entry into our entry set
				dataElements.push_back(entryData);
			}

			//If it's possible this array could be a character array, change the array
			//datatype to a character array.
			if(ActiveDisassemblyArraySeemsToBeCharArray(arrayinfo.dataType, arrayinfo.arrayEntrySize, dataElements))
			{
				arrayinfo.dataType = DISASSEMBLYDATATYPE_CHARACTER;
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool Processor::ActiveDisassemblyEntrySeemsToBePartOfArray(ActiveDisassemblyAnalysisData& analysis, const DisassemblyAddressInfo* currentEntry, unsigned int firstKnownEntryLocation, unsigned int lastKnownEntryLocation, unsigned int entrySize, DisassemblyEntryType offsetType, bool relativeOffset, unsigned int relativeOffsetBaseAddress) const
{
	//Check if the current entry is the same size as the last entry
	if(currentEntry->memoryBlockSize != entrySize)
	{
		return false;
	}

	//Ensure the offset type matches the last entry
	if((currentEntry->entryType != offsetType) || (currentEntry->relativeOffset != relativeOffset) || (currentEntry->relativeOffset && (currentEntry->relativeOffsetBaseAddress != relativeOffsetBaseAddress)))
	{
		return false;
	}

	//Check if the current entry is correctly aligned with the last entry
	if(((currentEntry->baseMemoryAddress - firstKnownEntryLocation) % entrySize) != 0)
	{
		return false;
	}

	//Check if the current entry is within the maximum distance from the last entry
	const unsigned int maxPredictedOffsetArrayEntryStride = analysis.offsetArrayDistanceTolerance;
	unsigned int maxOffsetArrayDistance = maxPredictedOffsetArrayEntryStride * entrySize;
	if(currentEntry->baseMemoryAddress > (lastKnownEntryLocation + maxOffsetArrayDistance))
	{
		return false;
	}

	//Ensure that no instructions have been detected between the last entry and this entry
	bool foundOverlappingOpcode = false;
	unsigned int memoryBaseOffset = 0;
	while(!foundOverlappingOpcode && (memoryBaseOffset < (currentEntry->baseMemoryAddress - lastKnownEntryLocation)))
	{
		unsigned int entryByteLocation = lastKnownEntryLocation + memoryBaseOffset;
		for(std::list<DisassemblyAddressInfo*>::const_iterator i = analysis.disassemblyAddressInfo[entryByteLocation - analysis.minAddress].begin(); i != analysis.disassemblyAddressInfo[entryByteLocation - analysis.minAddress].end(); ++i)
		{
			if((*i)->entryType == DISASSEMBLYENTRYTYPE_CODE)
			{
				foundOverlappingOpcode = true;
			}
		}
		++memoryBaseOffset;
	}
	if(foundOverlappingOpcode)
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
void Processor::ActiveDisassemblyGeneratePredictedOffsetArrayEntries(ActiveDisassemblyAnalysisData& analysis, std::map<unsigned int, const DisassemblyAddressInfo*> knownOffsetArrayEntries, unsigned int firstKnownEntryLocation, unsigned int lastKnownEntryLocation, unsigned int entrySize, DisassemblyEntryType offsetType, bool relativeOffset, unsigned int relativeOffsetBaseAddress) const
{
	//Analyze all known entries in the array, and see if we can determine a common stride
	//value between each entry. This is required in order to support offset arrays which
	//are not tightly packed. It's possible we have an array of structures here, where
	//there is an offset, then an unknown number of values between them. In such a case,
	//each entry will have a common stride value between each valid element. If the array
	//appears to be tightly packed, or if there are not enough known values to reliably
	//detect the stride, consider the stride to be the same as the offset array entry
	//size.
	unsigned int entryStride = entrySize;
	const unsigned int minimumKnownOffsetArrayEntriesForStridePrediction = 4;
	if(knownOffsetArrayEntries.size() >= minimumKnownOffsetArrayEntriesForStridePrediction)
	{
		//Build a set of stride values between all known entries
		std::set<unsigned int> strideEntries;
		bool previousEntryLocationRead = false;
		unsigned int previousEntryLocation;
		for(std::map<unsigned int, const DisassemblyAddressInfo*>::const_iterator i = knownOffsetArrayEntries.begin(); i != knownOffsetArrayEntries.end(); ++i)
		{
			unsigned int currentEntryLocation = i->first;
			if(previousEntryLocationRead)
			{
				unsigned int currentEntryStride = currentEntryLocation - previousEntryLocation;
				strideEntries.insert(currentEntryStride);
			}
			previousEntryLocation = currentEntryLocation;
			previousEntryLocationRead = true;
		}

		//Count the occurrence of each valid whole number divisor for each detected stride
		//value
		std::map<unsigned int, unsigned int> divisorCount;
		for(std::set<unsigned int>::const_iterator strideEntryIterator = strideEntries.begin(); strideEntryIterator != strideEntries.end(); ++strideEntryIterator)
		{
			unsigned int strideEntry = *strideEntryIterator;
			for(unsigned int i = strideEntry; i > 0; --i)
			{
				if((strideEntry % i) == 0)
				{
					++divisorCount[i];
				}
			}
		}

		//Search for the greatest common divisor among all our stride values. There should
		//always be a common divisor, with the smallest one being the size of an entry in
		//the offset array. If there is a greater common divisor than this, it is our
		//stride value.
		bool foundCommonDivisor = false;
		unsigned int commonDivisor;
		unsigned int strideEntryCount = (unsigned int)strideEntries.size();
		std::map<unsigned int, unsigned int>::const_reverse_iterator divisorCountIterator = divisorCount.rbegin();
		while(!foundCommonDivisor && (divisorCountIterator != divisorCount.rend()))
		{
			unsigned int divisorCountForEntry = divisorCountIterator->second;
			if(divisorCountForEntry == strideEntryCount)
			{
				commonDivisor = divisorCountIterator->first;
				foundCommonDivisor = true;
				continue;
			}
			++divisorCountIterator;
		}

		//If we found a common divisor, assign it as the offset entry stride. We should
		//always manage to find a common divisor.
		if(foundCommonDivisor)
		{
			entryStride = commonDivisor;
		}
	}

	//Read all detected values in the array, and add them to a set of values.
	std::set<unsigned int> valuesInArray;
	unsigned int predictedArrayEntryCount = ((lastKnownEntryLocation - firstKnownEntryLocation) / entryStride) + 1;
	const unsigned int bitsPerByte = 8;
	unsigned int entrySizeInBits = entrySize * bitsPerByte;
	for(unsigned int i = 0; i < predictedArrayEntryCount; ++i)
	{
		//Read in this array entry
		unsigned int entryLocation = firstKnownEntryLocation + (i * entryStride);
		Data entryData(entrySizeInBits, 0);
		unsigned int bitsRead = 0;
		while(bitsRead < entrySizeInBits)
		{
			Data dataSegment = GetRawData(entryLocation + (bitsRead / bitsPerByte));
			unsigned int bitsUsed = (dataSegment.GetBitCount() > (entrySizeInBits - bitsRead))? (entrySizeInBits - bitsRead): dataSegment.GetBitCount();
			entryData.SetDataSegment(entrySizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
			bitsRead += bitsUsed;
		}

		//Add the value of this entry into our entry set
		valuesInArray.insert(entryData.GetData());
	}

	//Predict the start location of the array
	unsigned int firstPredictedEntryLocation = firstKnownEntryLocation;
	bool predictedArrayStartLocationUsingRelativeBase = false;
	if(relativeOffset && (relativeOffsetBaseAddress >= analysis.minAddress) && (firstKnownEntryLocation >= relativeOffsetBaseAddress))
	{
		//If this array defines a relative offset array, and the base address of the array
		//is before our first known entry, scan back from the first entry to the base
		//address to see if the base address appears to be the start of the array.

		//Scan back from the first known array entry location to the relative offset base
		//address looking for collisions with other items.
		bool collisionDetected = false;
		unsigned int currentAddress = firstKnownEntryLocation;
		while(!collisionDetected && (currentAddress > relativeOffsetBaseAddress) && (currentAddress >= analysis.minAddress))
		{
			--currentAddress;
			if(!analysis.disassemblyAddressInfo[currentAddress - analysis.minAddress].empty())
			{
				collisionDetected = true;
			}
		}

		//If no collisions were encountered with other items, consider the base address to
		//be the start address for the array, and add all predicted array items to the
		//array info.
		if(!collisionDetected)
		{
			//Set the predicted array start address to the relative offset base address,
			//and flag that we managed to predict the array start location using the
			//relative base address.
			firstPredictedEntryLocation = relativeOffsetBaseAddress;
			predictedArrayStartLocationUsingRelativeBase = true;

			//Load data for each predicted item into the array
			unsigned int entriesToAdd = (firstKnownEntryLocation - firstPredictedEntryLocation) / entryStride;
			for(unsigned int i = 0; i < entriesToAdd; ++i)
			{
				//Read in the next predicted array entry
				unsigned int entryLocation = firstPredictedEntryLocation + (i * entryStride);
				Data entryData(entrySizeInBits, 0);
				unsigned int bitsRead = 0;
				while(bitsRead < entrySizeInBits)
				{
					Data dataSegment = GetRawData(entryLocation + (bitsRead / bitsPerByte));
					unsigned int bitsUsed = (dataSegment.GetBitCount() > (entrySizeInBits - bitsRead))? (entrySizeInBits - bitsRead): dataSegment.GetBitCount();
					entryData.SetDataSegment(entrySizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
					bitsRead += bitsUsed;
				}

				//Add this entry to the array
				valuesInArray.insert(entryData.GetData());
				++predictedArrayEntryCount;
			}
		}
	}
	if(!predictedArrayStartLocationUsingRelativeBase)
	{
		//If we didn't manage to predict the start of the array using a relative offset
		//base address, use a content-based search method.
		bool previousItemAddToArrayFailed = false;
		while(!previousItemAddToArrayFailed)
		{
			//Calculate the minimum and maximum allowable values for the next item to be
			//considered an entry in the array
			unsigned int lowestArrayEntryValue = *valuesInArray.begin();
			unsigned int highestArrayEntryValue = *valuesInArray.rbegin();
			double allowedMaxDeviance = analysis.offsetArrayIncreaseTolerance;
			unsigned int arrayEntryDifferenceLowestToHighest = (highestArrayEntryValue - lowestArrayEntryValue);
			unsigned int allowedMaxDifferenceForNextEntry = (unsigned int)(((double)arrayEntryDifferenceLowestToHighest / (double)valuesInArray.size()) * allowedMaxDeviance);
			unsigned int allowedMaxValueForNextEntry = (((unsigned int)~0u - highestArrayEntryValue) <= allowedMaxDifferenceForNextEntry)? (unsigned int)~0u: highestArrayEntryValue + allowedMaxDifferenceForNextEntry;
			unsigned int allowedMinValueForNextEntry = (lowestArrayEntryValue <= allowedMaxDifferenceForNextEntry)? 0: lowestArrayEntryValue - allowedMaxDifferenceForNextEntry;

			//Verify that at least one more previous entry could exist in the memory space
			if(firstPredictedEntryLocation < entryStride)
			{
				previousItemAddToArrayFailed = true;
				continue;
			}

			//Validate the next possible entry location
			unsigned int entryLocation = firstPredictedEntryLocation - entryStride;
			if(entryLocation < analysis.minAddress)
			{
				previousItemAddToArrayFailed = true;
				continue;
			}

			//Read in the next possible array entry
			Data entryData(entrySizeInBits, 0);
			unsigned int bitsRead = 0;
			while(bitsRead < entrySizeInBits)
			{
				Data dataSegment = GetRawData(entryLocation + (bitsRead / bitsPerByte));
				unsigned int bitsUsed = (dataSegment.GetBitCount() > (entrySizeInBits - bitsRead))? (entrySizeInBits - bitsRead): dataSegment.GetBitCount();
				entryData.SetDataSegment(entrySizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
				bitsRead += bitsUsed;
			}

			//Validate the next possible array entry data
			if((entryData.GetData() < allowedMinValueForNextEntry) || (entryData.GetData() > allowedMaxValueForNextEntry))
			{
				previousItemAddToArrayFailed = true;
				continue;
			}

			//Ensure this item doesn't collide with a previously known item
			bool collisionDetected = false;
			unsigned int currentAddress = entryLocation;
			while(!collisionDetected && (currentAddress < (entryLocation + entrySize)) && (currentAddress < analysis.maxAddress))
			{
				if(!analysis.disassemblyAddressInfo[currentAddress - analysis.minAddress].empty())
				{
					collisionDetected = true;
				}
				++currentAddress;
			}
			if(collisionDetected)
			{
				previousItemAddToArrayFailed = true;
				continue;
			}

			//Extend the array to include this entry
			valuesInArray.insert(entryData.GetData());
			++predictedArrayEntryCount;
			firstPredictedEntryLocation = entryLocation;
		}
	}

	//Predict the end location of the array
	unsigned int lastPredictedEntryLocation = firstPredictedEntryLocation + (predictedArrayEntryCount * entryStride);
	bool nextItemAddToArrayFailed = false;
	while(!nextItemAddToArrayFailed)
	{
		//Calculate the minimum and maximum allowable values for the next item to be
		//considered an entry in the array
		unsigned int lowestArrayEntryValue = *valuesInArray.begin();
		unsigned int highestArrayEntryValue = *valuesInArray.rbegin();
		double allowedMaxDeviance = analysis.offsetArrayIncreaseTolerance;
		unsigned int arrayEntryDifferenceLowestToHighest = (highestArrayEntryValue - lowestArrayEntryValue);
		unsigned int allowedMaxDifferenceForNextEntry = (unsigned int)(((double)arrayEntryDifferenceLowestToHighest / (double)valuesInArray.size()) * allowedMaxDeviance);
		unsigned int allowedMaxValueForNextEntry = (((unsigned int)~0u - highestArrayEntryValue) <= allowedMaxDifferenceForNextEntry)? (unsigned int)~0u: highestArrayEntryValue + allowedMaxDifferenceForNextEntry;
		unsigned int allowedMinValueForNextEntry = (lowestArrayEntryValue <= allowedMaxDifferenceForNextEntry)? 0: lowestArrayEntryValue - allowedMaxDifferenceForNextEntry;

		//Validate the next possible entry location
		unsigned int entryLocation = firstPredictedEntryLocation + (predictedArrayEntryCount * entryStride);
		if((entryLocation + entrySize) > analysis.maxAddress)
		{
			nextItemAddToArrayFailed = true;
			continue;
		}

		//Read in the next possible array entry
		Data entryData(entrySizeInBits, 0);
		unsigned int bitsRead = 0;
		while(bitsRead < entrySizeInBits)
		{
			Data dataSegment = GetRawData(entryLocation + (bitsRead / bitsPerByte));
			unsigned int bitsUsed = (dataSegment.GetBitCount() > (entrySizeInBits - bitsRead))? (entrySizeInBits - bitsRead): dataSegment.GetBitCount();
			entryData.SetDataSegment(entrySizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
			bitsRead += bitsUsed;
		}

		//Validate the next possible array entry data
		if((entryData.GetData() < allowedMinValueForNextEntry) || (entryData.GetData() > allowedMaxValueForNextEntry))
		{
			nextItemAddToArrayFailed = true;
			continue;
		}

		//Ensure this item doesn't collide with a previously known item
		bool collisionDetected = false;
		unsigned int currentAddress = entryLocation;
		while(!collisionDetected && (currentAddress < (entryLocation + entrySize)) && (currentAddress < analysis.maxAddress))
		{
			if(!analysis.disassemblyAddressInfo[currentAddress - analysis.minAddress].empty())
			{
				collisionDetected = true;
			}
			++currentAddress;
		}
		if(collisionDetected)
		{
			nextItemAddToArrayFailed = true;
			continue;
		}

		//If this is a relative offset array, ensure this item doesn't collide with any
		//predicted or known targets of this array itself.
		if(relativeOffset && ((entryLocation + entrySize) > (relativeOffsetBaseAddress + lowestArrayEntryValue)))
		{
			nextItemAddToArrayFailed = true;
			continue;
		}

		//If this is a relative offset array, ensure this item doesn't collide with the
		//offset array itself.
		unsigned int entryTarget = relativeOffsetBaseAddress + entryData.GetData();
		if(relativeOffset && ((entryTarget >= firstPredictedEntryLocation) && (entryTarget < (lastPredictedEntryLocation + entrySize))))
		{
			nextItemAddToArrayFailed = true;
			continue;
		}

		//Extend the array to include this entry
		valuesInArray.insert(entryData.GetData());
		++predictedArrayEntryCount;
		lastPredictedEntryLocation = entryLocation;
	}

	//##TODO## Define an actual array structure for this offset array? On the other hand,
	//this isn't really a data array, and none of the rules for how we deal with it are
	//the same. That said, we've detected an array structure here, and that information
	//should be preserved, as well as jump table information. I would recommend we output
	//information on these structures separately from data arrays.
	//DisassemblyArrayInfo arrayEntry;
	//arrayEntry.baseMemoryAddress = firstPredictedEntryLocation;
	//arrayEntry.arrayEntrySize = entrySize;
	//arrayEntry.arrayEntryCount = predictedArrayEntryCount;
	//arrayEntry.conflictsWithKnownCode = false;
	//analysis.disassemblyArraySorted.insert(std::pair<unsigned int, DisassemblyArrayInfo>(arrayEntry.baseMemoryAddress, arrayEntry));

	//Now that we've predicted the beginning and end locations of this array, generate
	//entries for each item in the array.
	for(unsigned int i = 0; i < predictedArrayEntryCount; ++i)
	{
		//If this offset array entry is already known, add it to the known offset list,
		//and advance to the next entry.
		unsigned int entryLocation = firstPredictedEntryLocation + (i * entryStride);
		std::map<unsigned int, const DisassemblyAddressInfo*>::const_iterator knownOffsetsIterator = knownOffsetArrayEntries.find(entryLocation);
		if(knownOffsetsIterator != knownOffsetArrayEntries.end())
		{
			DisassemblyAddressInfo* copiedEntry = new DisassemblyAddressInfo(*knownOffsetsIterator->second);
			analysis.disassemblyOffsetSorted.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(copiedEntry->baseMemoryAddress, copiedEntry));
			continue;
		}

		//Add a predicted offset entry for this offset array entry
		DisassemblyAddressInfo* newEntry = new DisassemblyAddressInfo();
		newEntry->entryType = offsetType;
		newEntry->baseMemoryAddress = entryLocation;
		newEntry->memoryBlockSize = entrySize;
		newEntry->relativeOffset = relativeOffset;
		newEntry->relativeOffsetBaseAddress = relativeOffsetBaseAddress;
		newEntry->comment = L"Predicted";
		newEntry->entryDefinedOutsideArray = true;
		analysis.predictedOffsetEntries.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(newEntry->baseMemoryAddress, newEntry));

		//If this predicted entry is an offset to code, insert a predicted code entry at
		//the target address.
		if(newEntry->entryType == DISASSEMBLYENTRYTYPE_OFFSETCODE)
		{
			//Read in the offset data
			unsigned int entryLocation = newEntry->baseMemoryAddress;
			Data entryData(entrySizeInBits, 0);
			unsigned int bitsRead = 0;
			while(bitsRead < entrySizeInBits)
			{
				Data dataSegment = GetRawData(entryLocation + (bitsRead / bitsPerByte));
				unsigned int bitsUsed = (dataSegment.GetBitCount() > (entrySizeInBits - bitsRead))? (entrySizeInBits - bitsRead): dataSegment.GetBitCount();
				entryData.SetDataSegment(entrySizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
				bitsRead += bitsUsed;
			}

			//Build the target address
			unsigned int targetAddress = entryData.SignExtend(GetPCWidth()).GetData();
			if(newEntry->relativeOffset)
			{
				targetAddress += newEntry->relativeOffsetBaseAddress;
			}

			//If this is a relative offset and the value of this entry is 0, don't
			//generate a code location for this offset entry. We deal with this as a
			//special case, since it's quite likely to encounter offset arrays where a
			//particular value is invalid and unused, but that invalid value lies between
			//valid values. We consider the target of an offset value of 0 for a relative
			//offset array entry to be invalid, since we know it will overlap with the
			//array itself, but we don't reject this offset value itself just because of
			//the invalid target in this case.
			if(relativeOffset && (targetAddress == relativeOffsetBaseAddress))
			{
				continue;
			}

			//Add the predicted code target address to the offset comment
			std::wstringstream commentStream;
			commentStream << L" (Code target predicted at 0x" << std::hex << std::uppercase << targetAddress << L")";
			newEntry->comment += commentStream.str();

			//If we haven't recorded any information on this target opcode, add it
			//to the list of predicted code locations.
			if((analysis.disassemblyCodeSorted.find(targetAddress) == analysis.disassemblyCodeSorted.end()) && (analysis.predictedCodeEntries.find(targetAddress) == analysis.predictedCodeEntries.end()))
			{
				//Get the opcode info for the target
				OpcodeInfo opcodeInfo = GetOpcodeInfo(targetAddress);

				//If the target appears to be invalid, don't add a predicted code location
				//at the target.
				if(!opcodeInfo.valid)
				{
					continue;
				}

				//Build the initial set of resulting PC locations from executing the
				//target opcode.
				std::list<unsigned int> locationsToExamine;
				std::set<unsigned int> locationsToExamineSet;
				for(std::set<unsigned int>::const_iterator pcLocationIterator = opcodeInfo.resultantPCLocations.begin(); pcLocationIterator != opcodeInfo.resultantPCLocations.end(); ++pcLocationIterator)
				{
					unsigned int possibleLocationToExamine = *pcLocationIterator;
					locationsToExamine.push_back(possibleLocationToExamine);
					locationsToExamineSet.insert(possibleLocationToExamine);
				}

				//Examine a certain number of opcodes from this location down the call
				//tree to see if they're all valid. If we hit a bad opcode, we consider
				//that this is possibly a bad code reference.
				const unsigned int maxOpcodesToVerify = 100;
				std::set<unsigned int> locationsExamined;
				unsigned int opcodesVerified = 0;
				bool allCodePathsTraversed = locationsToExamine.empty();
				bool opcodeVerificationFailed = false;
				while(!opcodeVerificationFailed && !allCodePathsTraversed && (opcodesVerified < maxOpcodesToVerify))
				{
					//Grab the next address from the list of locations to examine
					unsigned int currentLocation = *locationsToExamine.begin();
					locationsToExamine.pop_front();
					locationsToExamineSet.erase(currentLocation);

					//Obtain and validate information for the opcode at the next address
					OpcodeInfo opcodeInfoForCurrentLocation = GetOpcodeInfo(currentLocation);
					if(!opcodeInfoForCurrentLocation.valid)
					{
						opcodeVerificationFailed = true;
						continue;
					}

					//Insert all unexamined resultant PC locations from this opcode into
					//the list of locations to examine.
					for(std::set<unsigned int>::const_iterator pcLocationIterator = opcodeInfoForCurrentLocation.resultantPCLocations.begin(); pcLocationIterator != opcodeInfoForCurrentLocation.resultantPCLocations.end(); ++pcLocationIterator)
					{
						unsigned int possibleLocationToExamine = *pcLocationIterator;
						if(locationsExamined.find(possibleLocationToExamine) != locationsExamined.end())
						{
							continue;
						}
						if(locationsToExamineSet.find(possibleLocationToExamine) != locationsToExamineSet.end())
						{
							continue;
						}
						locationsToExamine.push_back(possibleLocationToExamine);
						locationsToExamineSet.insert(possibleLocationToExamine);
					}
					allCodePathsTraversed = locationsToExamine.empty();

					//Increment the count of verified opcodes
					++opcodesVerified;
				}

				//If we hit a bad opcode while attempting to verify the code target, flag
				//this code reference to be suspect, and don't generate a predicted code
				//entry at the target.
				if(opcodeVerificationFailed)
				{
					newEntry->comment += L" [Code verification failed! Opcode target not generated.]";
					continue;
				}

				//Add a new entry for this predicted code location if it lies within the
				//analysis region
				if((targetAddress >= analysis.minAddress) && ((targetAddress + opcodeInfo.opcodeSize) <= analysis.maxAddress))
				{
					DisassemblyAddressInfo* newCodeEntry = new DisassemblyAddressInfo();
					newCodeEntry->entryType = DISASSEMBLYENTRYTYPE_CODE;
					newCodeEntry->baseMemoryAddress = targetAddress;
					newCodeEntry->memoryBlockSize = opcodeInfo.opcodeSize;
					newCodeEntry->comment = L"Predicted (Offset array entry)";
					newCodeEntry->entryDefinedOutsideArray = true;
					analysis.predictedCodeEntries.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(newCodeEntry->baseMemoryAddress, newCodeEntry));
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
bool Processor::ActiveDisassemblySuspectedJumpTableSeemsToBeValid(ActiveDisassemblyAnalysisData& analysis, const DisassemblyJumpTableInfo& jumpTableInfo) const
{
	//Ensure that at least two entries have been detected
	if(jumpTableInfo.knownEntries.size() < 2)
	{
		return false;
	}

	//Validate each known entry in the jump table
	for(std::set<unsigned int>::const_iterator i = jumpTableInfo.knownEntries.begin(); i != jumpTableInfo.knownEntries.end(); ++i)
	{
		//Ensure that this entry has the correct alignment from the base address
		unsigned int entryLocation = *i;
		if(((entryLocation - jumpTableInfo.baseMemoryAddress) % jumpTableInfo.entrySize) != 0)
		{
			return false;
		}

		//Obtain the opcode information for the target entry
		OpcodeInfo opcodeInfo = GetOpcodeInfo(entryLocation);
		if(!opcodeInfo.valid)
		{
			return false;
		}

		//Ensure that this opcode is the required size
		if(opcodeInfo.opcodeSize != jumpTableInfo.entrySize)
		{
			return false;
		}

		//Ensure that this opcode defines a resultant PC location and has no undetermined
		//resultant PC locations
		if(opcodeInfo.resultantPCLocations.empty() || opcodeInfo.undeterminedResultantPCLocation)
		{
			return false;
		}

		//Ensure that this opcode has a single resultant PC location which is different
		//from the following opcode address
		if((opcodeInfo.resultantPCLocations.size() != 1) || (*opcodeInfo.resultantPCLocations.begin() == (entryLocation + opcodeInfo.opcodeSize)))
		{
			return false;
		}
	}

	//Do some sanity checking on the second known entry in the suspected jump table. If
	//it seems too far from the first entry, reject this table definition.
	const unsigned int maxEntriesToSecondKnownEntry = analysis.jumpTableDistanceTolerance;
	unsigned int maxDistanceToSecondKnownEntry = maxEntriesToSecondKnownEntry * jumpTableInfo.entrySize;
	unsigned int firstEntry = *(jumpTableInfo.knownEntries.begin());
	unsigned int secondEntry = *(++jumpTableInfo.knownEntries.begin());
	if((secondEntry - firstEntry) > maxDistanceToSecondKnownEntry)
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
void Processor::ActiveDisassemblyGeneratePredictedJumpTableEntries(ActiveDisassemblyAnalysisData& analysis, const DisassemblyJumpTableInfo& jumpTableInfo) const
{
	unsigned int firstKnownEntryLocation = *jumpTableInfo.knownEntries.begin();
	unsigned int lastKnownEntryLocation = *jumpTableInfo.knownEntries.rbegin();
	unsigned int predictedArrayEntryCount = ((lastKnownEntryLocation - firstKnownEntryLocation) / jumpTableInfo.entrySize) + 1;

	//Predict the start location of the array
	unsigned int firstPredictedEntryLocation = firstKnownEntryLocation;

	//Scan back from the first known array entry location to the relative offset base
	//address looking for collisions with other items.
	bool collisionDetected = false;
	unsigned int currentAddress = firstKnownEntryLocation;
	while(!collisionDetected && (currentAddress > jumpTableInfo.baseMemoryAddress) && (currentAddress >= analysis.minAddress))
	{
		--currentAddress;
		if(!analysis.disassemblyAddressInfo[currentAddress - analysis.minAddress].empty())
		{
			collisionDetected = true;
		}
	}
	bool reachedBaseAddress = (currentAddress == jumpTableInfo.baseMemoryAddress);

	//If no collisions were encountered with other items, and we were able to scan back to
	//the base address, consider the base address to be the start address for the array,
	//and add all predicted array items to the array info.
	if(!collisionDetected && reachedBaseAddress)
	{
		//Set the predicted array start address to the relative offset base address,
		//and flag that we managed to predict the array start location using the
		//relative base address.
		firstPredictedEntryLocation = jumpTableInfo.baseMemoryAddress;
	}
	else
	{
		//If we didn't manage to predict the start of the array using a relative offset
		//base address, use a content-based search method.
		bool previousItemAddToArrayFailed = false;
		while(!previousItemAddToArrayFailed)
		{
			//Verify that at least one more previous entry could exist in the memory space
			if(firstPredictedEntryLocation < jumpTableInfo.entrySize)
			{
				previousItemAddToArrayFailed = true;
				continue;
			}

			//Validate the next possible entry location
			unsigned int entryLocation = firstPredictedEntryLocation - jumpTableInfo.entrySize;
			if(entryLocation < analysis.minAddress)
			{
				previousItemAddToArrayFailed = true;
				continue;
			}

			//Read in the next possible array entry
			OpcodeInfo opcodeInfo = GetOpcodeInfo(entryLocation);
			if(!opcodeInfo.valid)
			{
				previousItemAddToArrayFailed = true;
				continue;
			}

			//Ensure that this opcode is the required size
			if(opcodeInfo.opcodeSize != jumpTableInfo.entrySize)
			{
				previousItemAddToArrayFailed = true;
				continue;
			}

			//Ensure that this opcode defines a resultant PC location and has no
			//undetermined resultant PC locations
			if(opcodeInfo.resultantPCLocations.empty() || opcodeInfo.undeterminedResultantPCLocation)
			{
				previousItemAddToArrayFailed = true;
				continue;
			}

			//Ensure that this opcode has a single resultant PC location which is
			//different from the following opcode address
			if((opcodeInfo.resultantPCLocations.size() != 1) || (*opcodeInfo.resultantPCLocations.begin() == (entryLocation + opcodeInfo.opcodeSize)))
			{
				previousItemAddToArrayFailed = true;
				continue;
			}

			//Ensure this item doesn't collide with a previously known item
			bool collisionDetected = false;
			unsigned int currentAddress = entryLocation;
			while(!collisionDetected && (currentAddress < (entryLocation + jumpTableInfo.entrySize)) && (currentAddress < analysis.maxAddress))
			{
				if(!analysis.disassemblyAddressInfo[currentAddress - analysis.minAddress].empty())
				{
					collisionDetected = true;
				}
				++currentAddress;
			}
			if(collisionDetected)
			{
				previousItemAddToArrayFailed = true;
				continue;
			}

			//Extend the array to include this entry
			++predictedArrayEntryCount;
			firstPredictedEntryLocation = entryLocation;
		}
	}

	//Predict the end location of the array
	unsigned int lastPredictedEntryLocation = firstPredictedEntryLocation + (predictedArrayEntryCount * jumpTableInfo.entrySize);
	bool nextItemAddToArrayFailed = false;
	while(!nextItemAddToArrayFailed)
	{
		//Validate the next possible entry location
		unsigned int entryLocation = firstPredictedEntryLocation + (predictedArrayEntryCount * jumpTableInfo.entrySize);
		if((entryLocation + jumpTableInfo.entrySize) > analysis.maxAddress)
		{
			nextItemAddToArrayFailed = true;
			continue;
		}

		//Read in the next possible array entry
		OpcodeInfo opcodeInfo = GetOpcodeInfo(entryLocation);
		if(!opcodeInfo.valid)
		{
			nextItemAddToArrayFailed = true;
			continue;
		}

		//Ensure that this opcode is the required size
		if(opcodeInfo.opcodeSize != jumpTableInfo.entrySize)
		{
			nextItemAddToArrayFailed = true;
			continue;
		}

		//Ensure that this opcode defines a resultant PC location and has no undetermined
		//resultant PC locations
		if(opcodeInfo.resultantPCLocations.empty() || opcodeInfo.undeterminedResultantPCLocation)
		{
			nextItemAddToArrayFailed = true;
			continue;
		}

		//Ensure that this opcode has a single resultant PC location which is
		//different from the following opcode address
		if((opcodeInfo.resultantPCLocations.size() != 1) || (*opcodeInfo.resultantPCLocations.begin() == (entryLocation + opcodeInfo.opcodeSize)))
		{
			nextItemAddToArrayFailed = true;
			continue;
		}

		//Ensure this item doesn't collide with a previously known item
		//##TODO## Check predicted code locations here
		bool collisionDetected = false;
		unsigned int currentAddress = entryLocation;
		while(!collisionDetected && (currentAddress < (entryLocation + jumpTableInfo.entrySize)) && (currentAddress < analysis.maxAddress))
		{
			if(!analysis.disassemblyAddressInfo[currentAddress - analysis.minAddress].empty())
			{
				collisionDetected = true;
			}
			++currentAddress;
		}
		if(collisionDetected)
		{
			nextItemAddToArrayFailed = true;
			continue;
		}

		//Extend the array to include this entry
		++predictedArrayEntryCount;
		lastPredictedEntryLocation = entryLocation;
	}

	//Now that we've predicted the beginning and end locations of this array, generate
	//entries for each item in the array.
	for(unsigned int i = 0; i < predictedArrayEntryCount; ++i)
	{
		//Add a new entry for this predicted code location, if it is not already known.
		unsigned int entryLocation = firstPredictedEntryLocation + (i * jumpTableInfo.entrySize);
		if(jumpTableInfo.knownEntries.find(entryLocation) == jumpTableInfo.knownEntries.end())
		{
			//Add a new entry for this predicted code location if it lies within the
			//analysis region
			if((entryLocation >= analysis.minAddress) && ((entryLocation + jumpTableInfo.entrySize) <= analysis.maxAddress))
			{
				DisassemblyAddressInfo* newCodeEntry = new DisassemblyAddressInfo();
				newCodeEntry->entryType = DISASSEMBLYENTRYTYPE_CODE;
				newCodeEntry->baseMemoryAddress = entryLocation;
				newCodeEntry->memoryBlockSize = jumpTableInfo.entrySize;
				newCodeEntry->comment = L"Predicted (Jump Table Entry)";
				newCodeEntry->entryDefinedOutsideArray = true;
				analysis.predictedCodeEntries.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(newCodeEntry->baseMemoryAddress, newCodeEntry));
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void Processor::ActiveDisassemblyGenerateLabelsForOffset(ActiveDisassemblyAnalysisData& analysis, const DisassemblyAddressInfo* entry, bool predicted) const
{
	const unsigned int bitsPerByte = 8;
	unsigned int entrySizeInBits = entry->memoryBlockSize * bitsPerByte;

	//Read in the offset data
	unsigned int entryLocation = entry->baseMemoryAddress;
	Data entryData(entrySizeInBits, 0);
	unsigned int bitsRead = 0;
	while(bitsRead < entrySizeInBits)
	{
		Data dataSegment = GetRawData(entryLocation + (bitsRead / bitsPerByte));
		unsigned int bitsUsed = (dataSegment.GetBitCount() > (entrySizeInBits - bitsRead))? (entrySizeInBits - bitsRead): dataSegment.GetBitCount();
		entryData.SetDataSegment(entrySizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
		bitsRead += bitsUsed;
	}

	//Build the target address
	unsigned int targetAddress = entryData.SignExtend(GetPCWidth()).GetData();
	if(entry->relativeOffset)
	{
		targetAddress += entry->relativeOffsetBaseAddress;
	}

	//Record a label for the target address
	ActiveDisassemblyAddLabelForTarget(analysis, targetAddress, predicted);

	//If this is a relative offset, record a label for the relative offset base address.
	if(entry->relativeOffset)
	{
		ActiveDisassemblyAddLabelForTarget(analysis, entry->relativeOffsetBaseAddress, predicted);
	}
}

//----------------------------------------------------------------------------------------
void Processor::ActiveDisassemblyAddLabelForTarget(ActiveDisassemblyAnalysisData& analysis, unsigned int targetAddress, bool predicted) const
{
	//Ensure the target address lies within our analysis region
	if((targetAddress < analysis.minAddress) || (targetAddress > analysis.maxAddress))
	{
		return;
	}

	//Check for collisions with this label target, and calculate an offset to the next
	//free location where a label can be placed.
	int locationOffset = 0;
	unsigned int adjustedAddress = targetAddress;
	const std::list<DisassemblyAddressInfo*>& entriesForAddress = analysis.disassemblyAddressInfo[targetAddress - analysis.minAddress];
	for(std::list<DisassemblyAddressInfo*>::const_iterator i = entriesForAddress.begin(); i != entriesForAddress.end(); ++i)
	{
		const DisassemblyAddressInfo* entry = *i;
		if(entry->baseMemoryAddress != targetAddress)
		{
			adjustedAddress = entry->baseMemoryAddress + entry->memoryBlockSize;
			locationOffset = (int)targetAddress - (int)adjustedAddress;
		}
	}

	//Ensure the adjusted address lies within our analysis region
	if((adjustedAddress < analysis.minAddress) || (adjustedAddress >= analysis.maxAddress))
	{
		return;
	}

	//Generate a label for the adjusted address
	std::wstring adjustedAddressAsRawString;
	IntToStringBase16(adjustedAddress, adjustedAddressAsRawString, GetPCCharWidth(), false);
	std::wstring adjustedAddressLabel = L"loc_" + adjustedAddressAsRawString;

	//Generate a usage string addressing the target label with the calculated offset
	std::wstring usageLabel;
	if(!FormatLabelUsageForDisassembly(adjustedAddressLabel, locationOffset, usageLabel))
	{
		//If the format operation failed, and we didn't use an offset from the label,
		//generate a warning. Note that we don't generate a warning if an offset is used,
		//since it's possible this processor doesn't support offsets from label targets,
		//in which case, we want to fall back to a numeric reference.
		if(locationOffset == 0)
		{
			GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Failed to generate usage label for label with name \"" + adjustedAddressLabel + L"\" when performing active disassembly!"));
		}
		return;
	}

	//Insert an entry for the label itself at the adjusted address. If an entry already
	//exists for this label, we'll simply be re-setting the exact same data here, except
	//that we combine the predicted flags together.
	LabelEntry& labelEntry = analysis.labelSettings.labelTargetsAtLocation[adjustedAddress];
	labelEntry.label = adjustedAddressLabel;
	labelEntry.location = adjustedAddress;
	labelEntry.predicted &= predicted;

	//Insert an entry for this label usage
	LabelSubstitutionEntry& labelSubstitutionEntry = analysis.labelSettings.labelTargetsAddressingLocation[targetAddress];
	labelSubstitutionEntry.targetAddress = targetAddress;
	labelSubstitutionEntry.labelLocation = adjustedAddress;
	labelSubstitutionEntry.locationOffset = locationOffset;
	labelSubstitutionEntry.usageLabel = usageLabel;
}

//----------------------------------------------------------------------------------------
bool Processor::ActiveDisassemblyArraySeemsToBeCharArray(DisassemblyDataType dataType, unsigned int dataElementByteSize, const std::list<Data>& dataElements)
{
	//If the datatype and entry size for this array don't match the required parameters,
	//reject this array as a character array.
	if((dataType != DISASSEMBLYDATATYPE_INTEGER) || (dataElementByteSize != 1))
	{
		return false;
	}

	//If this array doesn't contain enough entries for us to detect character arrays with
	//sufficient accuracy, return false.
	const unsigned int minimumArrayEntryCountForCharDetection = 8;
	if((unsigned int)dataElements.size() < minimumArrayEntryCountForCharDetection)
	{
		return false;
	}

	//Gather information on the data elements within the array
	std::list<Data>::const_iterator dataElementsIterator = dataElements.begin();
	std::set<unsigned int> uniqueDataValues;
	unsigned int whiteSpaceCount = 0;
	unsigned int nullCount = 0;
	bool invalidCharEncountered = false;
	unsigned int arrayEntryCount = (unsigned int)dataElements.size();
	while(!invalidCharEncountered && (dataElementsIterator != dataElements.end()))
	{
		//Gather information on this array entry
		unsigned int elementData = dataElementsIterator->GetData();
		bool printableChar = (isprint(elementData) != 0);
		bool whiteSpaceChar = (isspace(elementData) != 0);
		bool nullChar = (elementData == 0);

		//Record information on this array entry
		uniqueDataValues.insert(elementData);
		invalidCharEncountered |= (!printableChar && !nullChar);
		if(nullChar)
		{
			++nullCount;
		}
		if(whiteSpaceChar)
		{
			++whiteSpaceCount;
		}

		//Advance to the next data element
		++dataElementsIterator;
	}

	//If a non-printable character was found in the array, return false.
	if(invalidCharEncountered)
	{
		return false;
	}

	//If too many entries in this array are null characters, return false.
	const unsigned int maximumValidCharToNullCharEntryRatio = 3;
	if(nullCount >= (arrayEntryCount / maximumValidCharToNullCharEntryRatio))
	{
		return false;
	}

	//If we didn't encounter enough whitespace entries in this array, return false.
	const unsigned int maximumArrayLengthWithoutWhitespace = 0x20;
	if((whiteSpaceCount == 0) && (arrayEntryCount > maximumArrayLengthWithoutWhitespace))
	{
		return false;
	}

	//If this array doesn't appear to have enough unique characters, return false.
	const unsigned int minimumArrayLengthForUniqueCharText = 0x20;
	const unsigned int minimumUniqueCharactersInArray = 5;
	unsigned int arrayEntryCountMinusWhiteSpace = (arrayEntryCount - whiteSpaceCount);
	if((arrayEntryCountMinusWhiteSpace >= minimumArrayLengthForUniqueCharText) && ((unsigned int)uniqueDataValues.size() < minimumUniqueCharactersInArray))
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool Processor::ActiveDisassemblyDecodeIDADataString(unsigned int byteSize, DisassemblyDataType dataType, std::wstring& outputString)
{
	if(dataType == DISASSEMBLYDATATYPE_INTEGER)
	{
		switch(byteSize)
		{
		case 1:
			outputString = L"FF_BYTE";
			return true;
		case 2:
			outputString = L"FF_WORD";
			return true;
		case 4:
			outputString = L"FF_DWRD";
			return true;
		case 8:
			outputString = L"FF_QWRD";
			return true;
		}
	}
	else if(dataType == DISASSEMBLYDATATYPE_FLOAT)
	{
		switch(byteSize)
		{
		case 4:
			outputString = L"FF_FLOAT";
			return true;
		case 8:
			outputString = L"FF_DOUBLE";
			return true;
		}
	}
	else if(dataType == DISASSEMBLYDATATYPE_CHARACTER)
	{
		switch(byteSize)
		{
		case 1:
			outputString = L"FF_ASCI";
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool Processor::ActiveDisassemblyDecodeIDAOffsetString(unsigned int pcWidth, std::wstring& outputString)
{
	switch(pcWidth)
	{
	case 8:
		outputString = L"REF_OFF8";
		return true;
	case 16:
		outputString = L"REF_OFF16";
		return true;
	case 32:
		outputString = L"REF_OFF32";
		return true;
	case 64:
		outputString = L"REF_OFF64";
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
std::wstring Processor::ActiveDisassemblyGenerateCommentForDataArrayLine(unsigned int dataEntryCountAlreadyWritten, unsigned int arrayStartLocation, unsigned int arrayEndLocation, bool unknownData, unsigned int addressCharWidth)
{
	//Write the index number of the starting data element in this line into the comment
	//for this array line.
	std::wstring dataStringComment;
	std::wstringstream arrayIndexStringStream;
	arrayIndexStringStream << L"0x" << std::hex << std::uppercase << dataEntryCountAlreadyWritten;
	dataStringComment = arrayIndexStringStream.str();

	//If this is the first line of data we've written for this array, add info about the
	//size and location of the array into the comment.
	if(dataEntryCountAlreadyWritten == 0)
	{
		//Write the start location, end location, and entry count for the array into the
		//comment string.
		unsigned int entriesInDataArray = arrayEndLocation - arrayStartLocation;
		std::wstring arrayStartLocationAsString;
		std::wstring arrayEndLocationAsString;
		std::wstring arrayEntryCountAsString;
		IntToStringBase16(arrayStartLocation, arrayStartLocationAsString, addressCharWidth, false);
		IntToStringBase16(arrayEndLocation, arrayEndLocationAsString, addressCharWidth, false);
		IntToStringBase16(entriesInDataArray, arrayEntryCountAsString, 0, false);
		std::wstringstream entryCountStringStream;
		entryCountStringStream << L"0x" << std::hex << std::uppercase << arrayEntryCountAsString;
		dataStringComment += L" (0x" + arrayStartLocationAsString + L"-0x" + arrayEndLocationAsString + L", Entry count: " + entryCountStringStream.str() + L")";

		//If this array contains unknown data, flag it as such.
		if(unknownData)
		{
			dataStringComment += L" [Unknown data]";
		}
	}

	return dataStringComment;
}

//----------------------------------------------------------------------------------------
std::wstring Processor::ActiveDisassemblyGenerateTextLabelForDataType(DisassemblyDataType dataType)
{
	switch(dataType)
	{
	case DISASSEMBLYDATATYPE_INTEGER:
		return L"Int";
	case DISASSEMBLYDATATYPE_FLOAT:
		return L"Float";
	case DISASSEMBLYDATATYPE_CHARACTER:
		return L"Char";
	}
	return L"Unknown";
}

//----------------------------------------------------------------------------------------
bool Processor::ActiveDisassemblyExportAnalysisToASMFile(const ActiveDisassemblyAnalysisData& analysis, const std::wstring& filePath) const
{
	//Create the output file
	Stream::File asmFile;
	if(!asmFile.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
	{
		GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Failed to create output file \"" + filePath + L"\"!"));
		return false;
	}
	Stream::ViewText asmFileView(asmFile);

	//Write a comment block at the start of the disassembly stating that this disassembly
	//was generated by our software, and listing some information about the analysis
	//settings.
	std::list<std::wstring> fileCommentHeaderLines;
	std::wstring analysisMinAddressString;
	IntToStringBase16(analysis.minAddress, analysisMinAddressString, GetPCCharWidth(), false);
	std::wstring analysisMaxAddressString;
	IntToStringBase16(analysis.maxAddress , analysisMaxAddressString, GetPCCharWidth(), false);
	fileCommentHeaderLines.push_back(L" ########################################################################################");
	fileCommentHeaderLines.push_back(L" # Generated by the active disassembly feature of the Exodus emulation platform");
	fileCommentHeaderLines.push_back(L" #");
	fileCommentHeaderLines.push_back(L" # Creation Date:   " + GetTimestamp().GetTimestamp());
	fileCommentHeaderLines.push_back(L" # Analysis Region: 0x" + analysisMinAddressString + L" - 0x" + analysisMaxAddressString);
	fileCommentHeaderLines.push_back(L" ########################################################################################");
	for(std::list<std::wstring>::const_iterator i = fileCommentHeaderLines.begin(); i != fileCommentHeaderLines.end(); ++i)
	{
		std::wstring formattedCommentLine;
		if(!FormatCommentForDisassembly(*i, formattedCommentLine))
		{
			GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Format failed for comment \"" + *i + L"\" when attempting to export active disassembly analysis to file with path \"" + filePath + L"\"!"));
			return false;
		}
		asmFileView << formattedCommentLine << L'\n';
	}
	asmFileView << L'\n';

	//Write any leading lines to the assembly file which are required for this processor
	std::list<std::wstring> requiredLeadinglines;
	if(!GetLeadingLinesForASMFile(analysis.minAddress, analysis.maxAddress, requiredLeadinglines))
	{
		GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Failed to get leading lines when attempting to export active disassembly analysis to file with path \"" + filePath + L"\"!"));
		return false;
	}
	if(!requiredLeadinglines.empty())
	{
		for(std::list<std::wstring>::const_iterator i = requiredLeadinglines.begin(); i != requiredLeadinglines.end(); ++i)
		{
			asmFileView << *i << L'\n';
		}
		asmFileView << L'\n';
	}

	//Save our active disassembly analysis to the output file
	unsigned int location = analysis.minAddress;
	const unsigned int bitsPerByte = 8;
	std::list<Data> dataElements;
	bool currentDataArrayIsFormallyDefined = false;
	unsigned int dataElementByteSize = GetMinimumDataByteSize();
	unsigned int currentDataArrayStartLocation = analysis.minAddress;
	unsigned int currentDataArrayEndLocation = analysis.maxAddress;
	DisassemblyDataType currentDataArrayDataType = DISASSEMBLYDATATYPE_INTEGER;
	while(location < analysis.maxAddress)
	{
		unsigned int nextLocation = location;
		bool lineDisassembled = false;
		bool lineLabelDefined = false;
		std::wstring lineString;
		std::wstring lineComment;
		std::wstring lineLabel;

		//Write the label for this location if one is defined
		if(analysis.labelSettings.enableSubstitution)
		{
			std::map<unsigned int, LabelEntry>::const_iterator labelIterator = analysis.labelSettings.labelTargetsAtLocation.find(location);
			if(labelIterator != analysis.labelSettings.labelTargetsAtLocation.end())
			{
				std::wstring rawLabelString = labelIterator->second.label;
				std::wstring labelString;
				if(!FormatLabelPlacementForDisassembly(rawLabelString, labelString))
				{
					GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Placement format failed for label \"" + rawLabelString + L"\" when attempting to export active disassembly analysis to file with path \"" + filePath + L"\"!"));
					return false;
				}
				lineLabel = labelString;
				lineLabelDefined = true;
			}
		}

		//Write opcode entries
		DisassemblyAddressInfo* codeEntry = 0;
		std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator codeIterator = analysis.disassemblyCodeSorted.find(location);
		std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator predictedCodeIterator = analysis.predictedCodeEntries.find(location);
		if(codeIterator != analysis.disassemblyCodeSorted.end())
		{
			codeEntry = codeIterator->second;
		}
		else if(predictedCodeIterator != analysis.predictedCodeEntries.end())
		{
			codeEntry = predictedCodeIterator->second;
		}
		if(codeEntry != 0)
		{
			std::wstring opcodePrefix;
			std::wstring opcodeArguments;
			std::wstring opcodeComments;
			if(!FormatOpcodeForDisassembly(codeEntry->baseMemoryAddress, analysis.labelSettings, opcodePrefix, opcodeArguments, opcodeComments))
			{
				LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR);
				logEntry << L"Format failed for opcode with address \"0x" << std::hex << std::uppercase << codeEntry->baseMemoryAddress << L"\" when attempting to export active disassembly analysis to file with path \"" << filePath << L"\"!";
				GetDeviceContext()->WriteLogEvent(logEntry);
				return false;
			}

			//Write the line string
			lineString = L'\t';
			lineString += opcodePrefix;
			if(!opcodeArguments.empty())
			{
				lineString += L'\t';
				lineString += opcodeArguments;
			}
			//##TODO## Use the opcode comments again
			lineComment = codeEntry->comment;

			lineDisassembled = true;
			nextLocation = location + codeEntry->memoryBlockSize;
		}

		//Write offset entries
		DisassemblyAddressInfo* offsetEntry = 0;
		std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator offsetIterator = analysis.disassemblyOffsetSorted.find(location);
		std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator predictedOffsetIterator = analysis.predictedOffsetEntries.find(location);
		if(offsetIterator != analysis.disassemblyOffsetSorted.end())
		{
			offsetEntry = offsetIterator->second;
		}
		else if(predictedOffsetIterator != analysis.predictedOffsetEntries.end())
		{
			offsetEntry = predictedOffsetIterator->second;
		}
		if(offsetEntry != 0)
		{
			//Read in the offset data
			unsigned int entryLocation = offsetEntry->baseMemoryAddress;
			unsigned int entrySizeInBits = offsetEntry->memoryBlockSize * bitsPerByte;
			Data entryData(entrySizeInBits, 0);
			unsigned int bitsRead = 0;
			while(bitsRead < entrySizeInBits)
			{
				Data dataSegment = GetRawData(entryLocation + (bitsRead / bitsPerByte));
				unsigned int bitsUsed = (dataSegment.GetBitCount() > (entrySizeInBits - bitsRead))? (entrySizeInBits - bitsRead): dataSegment.GetBitCount();
				entryData.SetDataSegment(entrySizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
				bitsRead += bitsUsed;
			}

			//Format the offset data
			std::wstring opcodeStringPrefix;
			std::wstring opcodeString;
			if(!FormatOffsetForDisassembly(entryData, offsetEntry->relativeOffset, offsetEntry->relativeOffsetBaseAddress, analysis.labelSettings, opcodeStringPrefix, opcodeString))
			{
				LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR);
				logEntry << L"Format failed for offset with address \"0x" << std::hex << std::uppercase << offsetEntry->baseMemoryAddress << L"\" when attempting to export active disassembly analysis to file with path \"" << filePath << L"\"!";
				GetDeviceContext()->WriteLogEvent(logEntry);
				return false;
			}

			//Write the line string
			lineString = L'\t';
			lineString += opcodeStringPrefix;
			if(!opcodeString.empty())
			{
				lineString += L'\t';
				lineString += opcodeString;
			}
			lineComment = offsetEntry->comment;

			lineDisassembled = true;
			nextLocation = location + offsetEntry->memoryBlockSize;
		}

		//Write data entries
		DisassemblyAddressInfo* dataEntry = 0;
		std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator dataIterator = analysis.disassemblyDataSorted.find(location);
		std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator predictedDataIterator = analysis.predictedDataEntries.find(location);
		if(dataIterator != analysis.disassemblyDataSorted.end())
		{
			dataEntry = dataIterator->second;
		}
		else if(predictedDataIterator != analysis.predictedDataEntries.end())
		{
			dataEntry = predictedDataIterator->second;
		}
		if(dataEntry != 0)
		{
			//Read in the data
			unsigned int entryLocation = dataEntry->baseMemoryAddress;
			unsigned int entrySizeInBits = dataEntry->memoryBlockSize * bitsPerByte;
			Data entryData(entrySizeInBits, 0);
			unsigned int bitsRead = 0;
			while(bitsRead < entrySizeInBits)
			{
				Data dataSegment = GetRawData(entryLocation + (bitsRead / bitsPerByte));
				unsigned int bitsUsed = (dataSegment.GetBitCount() > (entrySizeInBits - bitsRead))? (entrySizeInBits - bitsRead): dataSegment.GetBitCount();
				entryData.SetDataSegment(entrySizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
				bitsRead += bitsUsed;
			}

			//Format the data
			std::wstring dataStringPrefix;
			std::wstring dataString;
			std::vector<Data> dataEntryInArray;
			dataEntryInArray.push_back(entryData);
			if(!FormatDataForDisassembly(dataEntryInArray, entryData.GetByteSize(), dataEntry->dataType, analysis.labelSettings, dataStringPrefix, dataString))
			{
				LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR);
				logEntry << L"Format failed for data with address \"0x" << std::hex << std::uppercase << entryData.GetData() << L"\" when attempting to export active disassembly analysis to file with path \"" << filePath << L"\"!";
				GetDeviceContext()->WriteLogEvent(logEntry);
				return false;
			}

			//Write the line string
			lineString = L'\t';
			lineString += dataStringPrefix;
			lineString += L'\t';
			lineString += dataString;
			lineComment = dataEntry->comment;

			lineDisassembled = true;
			nextLocation = location + dataEntry->memoryBlockSize;
		}

		//Write array entries
		bool newArrayBeginning = false;
		unsigned int newArrayEndLocation;
		unsigned int newArrayElementByteSize;
		DisassemblyDataType newArrayDataType;
		if(!lineDisassembled)
		{
			const DisassemblyArrayInfo* arrayEntry = 0;
			std::map<unsigned int, DisassemblyArrayInfo>::const_iterator arrayIterator = analysis.disassemblyArraySorted.find(location);
			if(arrayIterator != analysis.disassemblyArraySorted.end())
			{
				arrayEntry = &arrayIterator->second;
			}
			if(arrayEntry != 0)
			{
				//If we detected an array starting at this location, begin an array definition
				//here.
				if(!arrayEntry->conflictsWithKnownCode)
				{
					newArrayBeginning = true;
					newArrayDataType = arrayEntry->dataType;
					newArrayElementByteSize = arrayEntry->arrayEntrySize;
					newArrayEndLocation = arrayEntry->baseMemoryAddress + (arrayEntry->arrayEntrySize * arrayEntry->arrayEntryCount);
				}
			}
		}

		//If we've reached the end of a data array, write all buffered array data elements
		//to the file.
		bool reachedEndOfArray = !dataElements.empty() && ((location >= currentDataArrayEndLocation) || newArrayBeginning || lineLabelDefined || lineDisassembled);
		if(reachedEndOfArray)
		{
			//Write this data array to the file
			if(!ActiveDisassemblyWriteDataArrayToASMFile(asmFileView, dataElements, currentDataArrayStartLocation, dataElementByteSize, currentDataArrayDataType, !currentDataArrayIsFormallyDefined, analysis.labelSettings, filePath))
			{
				return false;
			}

			//Clear the data elements we've just written
			dataElements.clear();
		}

		//If we need to setup our array parameters for a new array definition, reset the
		//array properties, using the defined formal array parameters if they have been
		//specified.
		if(newArrayBeginning)
		{
			currentDataArrayIsFormallyDefined = true;
			currentDataArrayDataType = newArrayDataType;
			dataElementByteSize = newArrayElementByteSize;
			currentDataArrayEndLocation = newArrayEndLocation;
		}
		else if(reachedEndOfArray)
		{
			currentDataArrayIsFormallyDefined = false;
			currentDataArrayDataType = DISASSEMBLYDATATYPE_INTEGER;
			dataElementByteSize = GetMinimumDataByteSize();
			currentDataArrayEndLocation = analysis.maxAddress;
		}

		//Write the label for this location to the disassembly file
		if(lineLabelDefined)
		{
			asmFileView << lineLabel << L'\n';
		}

		//Write this line to the disassembly file
		if(lineDisassembled)
		{
			asmFileView << lineString;
			if(!lineComment.empty())
			{
				std::wstring formattedComment;
				if(!FormatCommentForDisassembly(lineComment, formattedComment))
				{
					GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Format failed for comment \"" + lineComment + L"\" when attempting to export active disassembly analysis to file with path \"" + filePath + L"\"!"));
					return false;
				}
				asmFileView << L"\t" << formattedComment;
			}
			asmFileView << L'\n';
		}
		else
		{
			//If this is the first data element in a new array, latch the current location
			//as the start address for the array.
			if(dataElements.empty())
			{
				currentDataArrayStartLocation = location;
			}

			//If we didn't detect this value as being anything in particular, add it as an
			//element in a data array.
			unsigned int dataElementSizeInBits = dataElementByteSize * bitsPerByte;
			Data entryData(dataElementSizeInBits, 0);
			unsigned int bitsRead = 0;
			while(bitsRead < dataElementSizeInBits)
			{
				Data dataSegment = GetRawData(location + (bitsRead / bitsPerByte));
				unsigned int bitsUsed = (dataSegment.GetBitCount() > (dataElementSizeInBits - bitsRead))? (dataElementSizeInBits - bitsRead): dataSegment.GetBitCount();
				entryData.SetDataSegment(dataElementSizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
				bitsRead += bitsUsed;
			}
			dataElements.push_back(entryData);
			nextLocation = location + dataElementByteSize;
		}

		//Advance to the next location
		location = nextLocation;
	}

	//If there are any remaining unwritten buffered data elements, write them to the file.
	if(!dataElements.empty())
	{
		if(!ActiveDisassemblyWriteDataArrayToASMFile(asmFileView, dataElements, currentDataArrayStartLocation, dataElementByteSize, currentDataArrayDataType, !currentDataArrayIsFormallyDefined, analysis.labelSettings, filePath))
		{
			return false;
		}
	}

	//Write any trailing lines to the assembly file which are required for this processor
	std::list<std::wstring> requiredTrailinglines;
	if(!GetTrailingLinesForASMFile(analysis.minAddress, analysis.maxAddress, requiredTrailinglines))
	{
		GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Failed to get trailing lines when attempting to export active disassembly analysis to file with path \"" + filePath + L"\"!"));
		return false;
	}
	if(!requiredTrailinglines.empty())
	{
		asmFileView << L'\n';
		for(std::list<std::wstring>::const_iterator i = requiredTrailinglines.begin(); i != requiredTrailinglines.end(); ++i)
		{
			asmFileView << *i << L'\n';
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool Processor::ActiveDisassemblyWriteDataArrayToASMFile(Stream::ViewText& asmFileView, const std::list<Data>& dataElements, unsigned int arrayStartLocation, unsigned int dataElementByteSize, DisassemblyDataType dataType, bool unknownData, const LabelSubstitutionSettings& labelSettings, const std::wstring& filePath) const
{
	//Calculate the end location of the array
	unsigned int arrayEndLocation = arrayStartLocation + ((unsigned int)dataElements.size() * dataElementByteSize);

	//If text prediction is enabled, and this is an unknown data array, and it appears
	//that it could be a character array, change the data type to a character array.
	if(activeDisassemblyDetectTextData && unknownData && ActiveDisassemblyArraySeemsToBeCharArray(dataType, dataElementByteSize, dataElements))
	{
		dataType = DISASSEMBLYDATATYPE_CHARACTER;
	}

	//Write all data elements in the array to the file
	unsigned int arrayEntriesWritten = 0;
	std::list<Data>::const_iterator dataElementsIterator = dataElements.begin();
	while(dataElementsIterator != dataElements.end())
	{
		//Extract data from the array until we've got enough data to fill one data line,
		//or we reach the end of the array.
		const unsigned int maxDataBytesPerLine = 0x20;
		std::vector<Data> dataElementsForLine;
		while((dataElementsIterator != dataElements.end()) && ((unsigned int)dataElementsForLine.size() < maxDataBytesPerLine))
		{
			dataElementsForLine.push_back(*dataElementsIterator);
			++dataElementsIterator;
		}

		//Build a string containing all the buffered array data
		std::wstring dataStringPrefix;
		std::wstring dataString;
		if(!FormatDataForDisassembly(dataElementsForLine, dataElementByteSize, dataType, labelSettings, dataStringPrefix, dataString))
		{
			LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR);
			logEntry << L"Format failed for data with address \"0x" << std::hex << std::uppercase << dataElements.begin()->GetData() << L"\" when attempting to export active disassembly analysis to file with path \"" << filePath << L"\"!";
			GetDeviceContext()->WriteLogEvent(logEntry);
			return false;
		}

		//Generate and format a comment string for this array data line
		std::wstring formattedComment;
		std::wstring dataStringComment = ActiveDisassemblyGenerateCommentForDataArrayLine(arrayEntriesWritten, arrayStartLocation, arrayEndLocation, unknownData, GetPCCharWidth());
		if(!FormatCommentForDisassembly(dataStringComment, formattedComment))
		{
			GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Format failed for comment \"" + dataStringComment + L"\" when attempting to export active disassembly analysis to file with path \"" + filePath + L"\"!"));
			return false;
		}

		//Write this data line to the file
		asmFileView << L'\t' << dataStringPrefix << L'\t' << dataString << L' ' << formattedComment << L'\n';

		//Update the count of data entries written for this array
		arrayEntriesWritten += (unsigned int)dataElementsForLine.size();
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool Processor::ActiveDisassemblyExportAnalysisToTextFile(const ActiveDisassemblyAnalysisData& analysis, const std::wstring& filePath) const
{
	//Create the output file
	Stream::File logFile;
	if(!logFile.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
	{
		GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Failed to create output file \"" + filePath + L"\"!"));
		return false;
	}

	//Save our active disassembly analysis to the output file
	Stream::ViewText logFileView(logFile);
	logFileView << L"Code:\n";
	for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.disassemblyCodeSorted.begin(); i != analysis.disassemblyCodeSorted.end(); ++i)
	{
		logFileView << L"0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L'\t' << i->second->comment << L'\n';
	}
	logFileView << L"Predicted code:\n";
	for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.predictedCodeEntries.begin(); i != analysis.predictedCodeEntries.end(); ++i)
	{
		logFileView << L"0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L'\t' << i->second->comment << L'\n';
	}
	logFileView << L"Offset:\n";
	for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.disassemblyOffsetSorted.begin(); i != analysis.disassemblyOffsetSorted.end(); ++i)
	{
		logFileView << L"0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L'\t' << L"0x" << std::hex << std::uppercase << i->second->memoryBlockSize << L'\t' << i->second->relativeOffset << L'\t' << L"0x" << std::hex << std::uppercase << i->second->relativeOffsetBaseAddress << L'\t' << i->second->comment << L'\n';
	}
	logFileView << L"Predicted Offset:\n";
	for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.predictedOffsetEntries.begin(); i != analysis.predictedOffsetEntries.end(); ++i)
	{
		logFileView << L"0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L'\t' << L"0x" << std::hex << std::uppercase << i->second->memoryBlockSize << L'\t' << i->second->relativeOffset << L'\t' << L"0x" << std::hex << std::uppercase << i->second->relativeOffsetBaseAddress << L'\t' << i->second->comment << L'\n';
	}
	logFileView << L"Data:\n";
	for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.disassemblyDataSorted.begin(); i != analysis.disassemblyDataSorted.end(); ++i)
	{
		logFileView << L"0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L'\t' << ActiveDisassemblyGenerateTextLabelForDataType(i->second->dataType) << L'\t' << L"0x" << std::hex << std::uppercase << i->second->memoryBlockSize << L'\t' << i->second->comment << L'\n';
	}
	logFileView << L"Predicted Data:\n";
	for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.predictedDataEntries.begin(); i != analysis.predictedDataEntries.end(); ++i)
	{
		logFileView << L"0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L'\t' << ActiveDisassemblyGenerateTextLabelForDataType(i->second->dataType) << L'\t' << L"0x" << std::hex << std::uppercase << i->second->memoryBlockSize << L'\t' << i->second->comment << L'\n';
	}
	logFileView << L"Arrays:\n";
	for(std::map<unsigned int, DisassemblyArrayInfo>::const_iterator i = analysis.disassemblyArraySorted.begin(); i != analysis.disassemblyArraySorted.end(); ++i)
	{
		logFileView << L"0x" << std::hex << std::uppercase << i->second.baseMemoryAddress << L'\t' << ActiveDisassemblyGenerateTextLabelForDataType(i->second.dataType) << L'\t' << L"0x" << std::hex << std::uppercase << i->second.arrayEntrySize << L'\t' << L"0x" << std::hex << std::uppercase << i->second.arrayEntryCount << L'\n';
	}
	logFileView << L"Label usage:\n";
	for(std::map<unsigned int, LabelSubstitutionEntry>::const_iterator i = analysis.labelSettings.labelTargetsAddressingLocation.begin(); i != analysis.labelSettings.labelTargetsAddressingLocation.end(); ++i)
	{
		logFileView << L"0x" << std::hex << std::uppercase << i->second.targetAddress << L'\t' << L"0x" << std::hex << std::uppercase << i->second.labelLocation << L'\t' << i->second.usageLabel << L'\n';
	}
	logFileView << L"Label placement:\n";
	for(std::map<unsigned int, LabelEntry>::const_iterator i = analysis.labelSettings.labelTargetsAtLocation.begin(); i != analysis.labelSettings.labelTargetsAtLocation.end(); ++i)
	{
		logFileView << L"0x" << std::hex << std::uppercase << i->second.location << L'\t' << i->second.label << L'\n';
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool Processor::ActiveDisassemblyExportAnalysisToIDCFile(const ActiveDisassemblyAnalysisData& analysis, const std::wstring& filePath) const
{
	//Create the output file
	Stream::File idcFile;
	if(!idcFile.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
	{
		GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Failed to create output file \"" + filePath + L"\"!"));
		return false;
	}

	//Save an IDC script for IDA Pro which will apply our results
	Stream::ViewText idcFileView(idcFile);
	idcFileView << L"#include <idc.idc>\n";
	idcFileView << L"static main()\n";
	idcFileView << L"{\n";
	for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.disassemblyCodeSorted.begin(); i != analysis.disassemblyCodeSorted.end(); ++i)
	{
		idcFileView << L"MakeCode(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L");\n";
		if(!i->second->comment.empty())
		{
			idcFileView << L"MakeComm(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", \"" << i->second->comment << L"\");\n";
		}
	}
	for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.predictedCodeEntries.begin(); i != analysis.predictedCodeEntries.end(); ++i)
	{
		idcFileView << L"MakeCode(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L");\n";
		if(!i->second->comment.empty())
		{
			idcFileView << L"MakeComm(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", \"" << i->second->comment << L"\");\n";
		}
	}
	for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.disassemblyDataSorted.begin(); i != analysis.disassemblyDataSorted.end(); ++i)
	{
		std::wstring dataString;
		if(ActiveDisassemblyDecodeIDADataString(i->second->memoryBlockSize, i->second->dataType, dataString))
		{
			idcFileView << L"MakeData(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", " << dataString << L", 0x" << i->second->memoryBlockSize << L", 0);\n";
			if(!i->second->comment.empty())
			{
				idcFileView << L"MakeComm(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", \"" << i->second->comment << L"\");\n";
			}
		}
	}
	for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.disassemblyOffsetSorted.begin(); i != analysis.disassemblyOffsetSorted.end(); ++i)
	{
		std::wstring dataString;
		if(ActiveDisassemblyDecodeIDADataString(i->second->memoryBlockSize, DISASSEMBLYDATATYPE_INTEGER, dataString))
		{
			//Read in the next possible array entry
			const unsigned int bitsPerByte = 8;
			unsigned int entrySizeInBits = i->second->memoryBlockSize * bitsPerByte;
			unsigned int entryLocation = i->second->baseMemoryAddress;
			Data entryData(entrySizeInBits, 0);
			unsigned int bitsRead = 0;
			while(bitsRead < entrySizeInBits)
			{
				Data dataSegment = GetRawData(entryLocation + (bitsRead / bitsPerByte));
				unsigned int bitsUsed = (dataSegment.GetBitCount() > (entrySizeInBits - bitsRead))? (entrySizeInBits - bitsRead): dataSegment.GetBitCount();
				entryData.SetDataSegment(entrySizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
				bitsRead += bitsUsed;
			}

			idcFileView << L"MakeData(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", " << dataString << L", 0x" << i->second->memoryBlockSize << L", 0);\n";
			if(!i->second->relativeOffset)
			{
				idcFileView << L"OpOff(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", -1, 0x0);\n";
			}
			else
			{
				//If this offset is negative, and the offset size is less than the PC
				//register width, handle the offset definition slightly differently.
				//IDA Pro always considers offsets to be unsigned. We need to define
				//the offset as the lower 16-bit half of a 32-bit offset in order to
				//get IDA Pro to generate a correct output.
				if(entryData.Negative() && (entrySizeInBits < GetPCWidth()) && ((entrySizeInBits == 8) || (entrySizeInBits == 16)))
				{
					unsigned int offsetTarget = entryData.SignExtend(GetPCWidth()).GetData() + i->second->relativeOffsetBaseAddress;
					if(entrySizeInBits == 8)
					{
						idcFileView << L"OpOffEx(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", -1, REF_LOW8, 0x" << std::hex << std::uppercase << offsetTarget <<", 0x" << std::hex << std::uppercase << i->second->relativeOffsetBaseAddress << L", 0);\n";
					}
					else if(entrySizeInBits == 16)
					{
						idcFileView << L"OpOffEx(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", -1, REF_LOW16, 0x" << std::hex << std::uppercase << offsetTarget <<", 0x" << std::hex << std::uppercase << i->second->relativeOffsetBaseAddress << L", 0);\n";
					}
				}
				else
				{
					std::wstring offsetString;
					if(ActiveDisassemblyDecodeIDAOffsetString(GetPCWidth(), offsetString))
					{
						idcFileView << L"OpOffEx(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", -1, " << offsetString << L", -1, 0x" << std::hex << std::uppercase << i->second->relativeOffsetBaseAddress << L", 0);\n";
					}
				}
			}
			if(!i->second->comment.empty())
			{
				idcFileView << L"MakeComm(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", \"" << i->second->comment << L"\");\n";
			}
		}
	}
	for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = analysis.predictedOffsetEntries.begin(); i != analysis.predictedOffsetEntries.end(); ++i)
	{
		std::wstring dataString;
		if(ActiveDisassemblyDecodeIDADataString(i->second->memoryBlockSize, DISASSEMBLYDATATYPE_INTEGER, dataString))
		{
			//Read in the next possible array entry
			const unsigned int bitsPerByte = 8;
			unsigned int entrySizeInBits = i->second->memoryBlockSize * bitsPerByte;
			unsigned int entryLocation = i->second->baseMemoryAddress;
			Data entryData(entrySizeInBits, 0);
			unsigned int bitsRead = 0;
			while(bitsRead < entrySizeInBits)
			{
				Data dataSegment = GetRawData(entryLocation + (bitsRead / bitsPerByte));
				unsigned int bitsUsed = (dataSegment.GetBitCount() > (entrySizeInBits - bitsRead))? (entrySizeInBits - bitsRead): dataSegment.GetBitCount();
				entryData.SetDataSegment(entrySizeInBits - (bitsRead + bitsUsed), bitsUsed, dataSegment.GetDataSegment(dataSegment.GetBitCount() - bitsUsed, bitsUsed));
				bitsRead += bitsUsed;
			}

			idcFileView << L"MakeData(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", " << dataString << L", 0x" << i->second->memoryBlockSize << L", 0);\n";
			if(!i->second->relativeOffset)
			{
				idcFileView << L"OpOff(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", -1, 0x0);\n";
			}
			else
			{
				//If this offset is negative, and the offset size is less than the PC
				//register width, handle the offset definition slightly differently.
				//IDA Pro always considers offsets to be unsigned. We need to define
				//the offset as the lower 16-bit half of a 32-bit offset in order to
				//get IDA Pro to generate a correct output.
				if(entryData.Negative() && (entrySizeInBits < GetPCWidth()) && ((entrySizeInBits == 8) || (entrySizeInBits == 16)))
				{
					unsigned int offsetTarget = entryData.SignExtend(GetPCWidth()).GetData() + i->second->relativeOffsetBaseAddress;
					if(entrySizeInBits == 8)
					{
						idcFileView << L"OpOffEx(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", -1, REF_LOW8, 0x" << std::hex << std::uppercase << offsetTarget <<", 0x" << std::hex << std::uppercase << i->second->relativeOffsetBaseAddress << L", 0);\n";
					}
					else if(entrySizeInBits == 16)
					{
						idcFileView << L"OpOffEx(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", -1, REF_LOW16, 0x" << std::hex << std::uppercase << offsetTarget <<", 0x" << std::hex << std::uppercase << i->second->relativeOffsetBaseAddress << L", 0);\n";
					}
				}
				else
				{
					std::wstring offsetString;
					if(ActiveDisassemblyDecodeIDAOffsetString(GetPCWidth(), offsetString))
					{
						idcFileView << L"OpOffEx(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", -1, " << offsetString << L", -1, 0x" << std::hex << std::uppercase << i->second->relativeOffsetBaseAddress << L", 0);\n";
					}
				}
			}
			if(!i->second->comment.empty())
			{
				idcFileView << L"MakeComm(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << L", \"" << i->second->comment << L"\");\n";
			}
		}
	}
	for(std::map<unsigned int, DisassemblyArrayInfo>::const_iterator i = analysis.disassemblyArraySorted.begin(); i != analysis.disassemblyArraySorted.end(); ++i)
	{
		std::wstring dataString;
		if(ActiveDisassemblyDecodeIDADataString(i->second.arrayEntrySize, i->second.dataType, dataString))
		{
			const unsigned int maxArrayEntriesPerLine = 0x10;
			unsigned int arrayEntriesPerLine = (i->second.arrayEntrySize > maxArrayEntriesPerLine)? i->second.arrayEntrySize: maxArrayEntriesPerLine / i->second.arrayEntrySize;
			unsigned int arrayEntryWidth = (i->second.arrayEntrySize * 2) + 1;
			idcFileView << L"MakeData(0x" << std::hex << std::uppercase << i->second.baseMemoryAddress << L", " << dataString << L", 0x" << i->second.arrayEntrySize << L", 0);\n";
			idcFileView << L"MakeArray(0x" << std::hex << std::uppercase << i->second.baseMemoryAddress << L", 0x" << std::hex << std::uppercase << i->second.arrayEntryCount << L");\n";
			idcFileView << L"SetArrayFormat(0x" << std::hex << std::uppercase << i->second.baseMemoryAddress << L", AP_INDEX|AP_IDXHEX, 0x" << arrayEntriesPerLine << L", 0x" << arrayEntryWidth << L");\n";
		}
	}
	idcFileView << L"}\n";

	return true;
}

//----------------------------------------------------------------------------------------
bool Processor::ActiveDisassemblySupported() const
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Processor::GetLeadingLinesForASMFile(unsigned int analysisStartAddress, unsigned int analysisEndAddress, std::list<std::wstring>& outputLines) const
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Processor::GetTrailingLinesForASMFile(unsigned int analysisStartAddress, unsigned int analysisEndAddress, std::list<std::wstring>& outputLines) const
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Processor::FormatOpcodeForDisassembly(unsigned int opcodeAddress, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& opcodeArguments, std::wstring& opcodeComments) const
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Processor::FormatDataForDisassembly(const std::vector<Data>& dataElements, unsigned int dataElementByteSize, DisassemblyDataType dataType, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& formattedData) const
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Processor::FormatOffsetForDisassembly(const Data& offsetData, bool relativeOffset, unsigned int relativeOffsetBaseAddress, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& formattedOffset) const
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Processor::FormatCommentForDisassembly(const std::wstring& rawComment, std::wstring& formattedComment) const
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Processor::FormatLabelPlacementForDisassembly(const std::wstring& rawLabel, std::wstring& formattedLabel) const
{
	return false;
}

//----------------------------------------------------------------------------------------
bool Processor::FormatLabelUsageForDisassembly(const std::wstring& rawLabel, int labelOffset, std::wstring& formattedLabel) const
{
	return false;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void Processor::LoadState(IHeirarchicalStorageNode& node)
{
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		std::wstring keyName = (*i)->GetName();
		if(keyName == L"Register")
		{
			IHeirarchicalStorageAttribute* nameAttribute = (*i)->GetAttribute(L"name");
			if(nameAttribute != 0)
			{
				std::wstring registerName = nameAttribute->GetValue();
				if(registerName == L"ClockSpeed")	clockSpeed = (*i)->ExtractData<double>();
			}
		}
		else if(keyName == L"CallStack")
		{
			LoadCallStack(*(*i));
		}
	}

	Device::LoadState(node);
}

//----------------------------------------------------------------------------------------
void Processor::SaveState(IHeirarchicalStorageNode& node) const
{
	node.CreateChild(L"Register", clockSpeed).CreateAttribute(L"name", L"ClockSpeed");
	IHeirarchicalStorageNode& callStackNode = node.CreateChild(L"CallStack");
	SaveCallStack(callStackNode);

	Device::SaveState(node);
}

//----------------------------------------------------------------------------------------
void Processor::LoadDebuggerState(IHeirarchicalStorageNode& node)
{
	boost::mutex::scoped_lock lock(debugMutex);

	bool activeDisassemblyDataLoaded = false;
	bool activeDisassemblyStateChanged = false;
	unsigned int newActiveDisassemblyStartLocation = activeDisassemblyStartLocation;
	unsigned int newActiveDisassemblyEndLocation = activeDisassemblyEndLocation;

	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		std::wstring keyName = (*i)->GetName();
		if(keyName == L"Register")
		{
			IHeirarchicalStorageAttribute* nameAttribute = (*i)->GetAttribute(L"name");
			if(nameAttribute != 0)
			{
				std::wstring registerName = nameAttribute->GetValue();
				//Device enable
				if(registerName == L"DeviceEnabled")				GetDeviceContext()->SetDeviceEnabled((*i)->ExtractData<bool>());
				//Clock speed
				else if(registerName == L"OverriddenClockSpeed")
				{
					clockSpeedOverridden = true;
					reportedClockSpeed = (*i)->ExtractData<double>();
				}
				//Call stack
				else if(registerName == L"StackDisassemble")		stackDisassemble = (*i)->ExtractData<bool>();
				//Trace
				else if(registerName == L"TraceEnabled")			traceEnabled = (*i)->ExtractData<bool>();
				else if(registerName == L"TraceDisassemble")		traceDisassemble = (*i)->ExtractData<bool>();
				else if(registerName == L"TraceLength")				traceLength = (*i)->ExtractData<unsigned int>();
				//Active Disassembly
				else if(registerName == L"ActiveDisassemblyEnabled")
				{
					activeDisassemblyEnabled = (*i)->ExtractData<bool>();
					activeDisassemblyStateChanged = true;
				}
				else if(registerName == L"ActiveDisassemblyStartLocation")
				{
					newActiveDisassemblyStartLocation = (*i)->ExtractHexData<unsigned int>();
					activeDisassemblyStateChanged = true;
				}
				else if(registerName == L"ActiveDisassemblyEndLocation")
				{
					newActiveDisassemblyEndLocation = (*i)->ExtractHexData<unsigned int>();
					activeDisassemblyStateChanged = true;
				}
				else if(registerName == L"ActiveDisassemblyArrayNextFreeID")
				{
					activeDisassemblyArrayNextFreeID = (*i)->ExtractHexData<unsigned int>();
					activeDisassemblyStateChanged = true;
				}
				else if(registerName == L"ActiveDisassemblyAnalysisStartLocation")	activeDisassemblyAnalysisStartLocation = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"ActiveDisassemblyAnalysisEndLocation")	activeDisassemblyAnalysisEndLocation = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"ActiveDisassemblyAnalyzeCode")			activeDisassemblyAnalyzeCode = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyAnalyzeData")			activeDisassemblyAnalyzeData = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyAnalyzeCodeOffsets")		activeDisassemblyAnalyzeCodeOffsets = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyAnalyzeDataOffsets")		activeDisassemblyAnalyzeDataOffsets = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyAnalyzePredictedArrays")	activeDisassemblyAnalyzePredictedArrays = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyAnalyzePredictedJumpTables")	activeDisassemblyAnalyzePredictedJumpTables = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyExploreCodePaths")		activeDisassemblyExploreCodePaths = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyPerformLabelSubstitution")	activeDisassemblyPerformLabelSubstitution = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyDetectOffsets")			activeDisassemblyDetectOffsets = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyDetectJumpTables")		activeDisassemblyDetectJumpTables = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyDetectData")				activeDisassemblyDetectData = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyDetectDataArrays")		activeDisassemblyDetectDataArrays = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyDetectTextData")			activeDisassemblyDetectTextData = (*i)->ExtractData<bool>();
				else if(registerName == L"ActiveDisassemblyOffsetArrayIncreaseTolerance")	activeDisassemblyOffsetArrayIncreaseTolerance = (*i)->ExtractData<double>();
				else if(registerName == L"ActiveDisassemblyMinimumArrayEntryCount")			activeDisassemblyMinimumArrayEntryCount = (*i)->ExtractData<unsigned int>();
				else if(registerName == L"ActiveDisassemblyOffsetArrayDistanceTolerance")	activeDisassemblyOffsetArrayDistanceTolerance = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"ActiveDisassemblyJumpTableDistanceTolerance")		activeDisassemblyJumpTableDistanceTolerance = (*i)->ExtractHexData<unsigned int>();
			}
		}
		else if(keyName == L"BreakpointList")
		{
			for(BreakpointList::const_iterator breakpointIterator = breakpoints.begin(); breakpointIterator != breakpoints.end(); ++breakpointIterator)
			{
				delete *breakpointIterator;
			}
			breakpoints.clear();

			std::list<IHeirarchicalStorageNode*> childList = (*i)->GetChildList();
			for(std::list<IHeirarchicalStorageNode*>::iterator childNodeIterator = childList.begin(); childNodeIterator != childList.end(); ++childNodeIterator)
			{
				IHeirarchicalStorageNode& childNode = *(*childNodeIterator);
				if(childNode.GetName() == L"Breakpoint")
				{
					Breakpoint* breakpoint = new Breakpoint(GetAddressBusWidth());
					breakpoint->LoadState(childNode);
					breakpoints.push_back(breakpoint);
				}
			}
			breakpointExists = !breakpoints.empty();
		}
		else if(keyName == L"WatchpointList")
		{
			for(WatchpointList::const_iterator watchpointIterator = watchpoints.begin(); watchpointIterator != watchpoints.end(); ++watchpointIterator)
			{
				delete *watchpointIterator;
			}
			watchpoints.clear();

			std::list<IHeirarchicalStorageNode*> childList = (*i)->GetChildList();
			for(std::list<IHeirarchicalStorageNode*>::iterator childNodeIterator = childList.begin(); childNodeIterator != childList.end(); ++childNodeIterator)
			{
				IHeirarchicalStorageNode& childNode = *(*childNodeIterator);
				if(childNode.GetName() == L"Watchpoint")
				{
					Watchpoint* watchpoint = new Watchpoint(GetAddressBusWidth());
					watchpoint->LoadState(childNode);
					watchpoints.push_back(watchpoint);
				}
			}
			watchpointExists = !watchpoints.empty();
		}
		else if(keyName == L"ActiveDisassemblyData")
		{
			//Clear all current active disassembly data
			ClearActiveDisassembly();

			//Obtain and configure the data stream for the saved data
			Stream::IStream& stream = (*i)->GetBinaryDataBufferStream();
			stream.SetByteOrder(Stream::IStream::BYTEORDER_LITTLEENDIAN);
			stream.SetStreamPos(0);

			//Read the version number of the data format
			unsigned int versionNo;
			stream.ReadData(versionNo);

			//Read all address entries from the saved data
			unsigned int disassemblyEntryCount;
			stream.ReadData(disassemblyEntryCount);
			for(unsigned int entryNo = 0; entryNo < disassemblyEntryCount; ++entryNo)
			{
				DisassemblyAddressInfo* entry = new DisassemblyAddressInfo();
				unsigned int entryType;
				unsigned int dataType;
				stream.ReadData(entryType);
				entry->entryType = (DisassemblyEntryType)entryType;
				stream.ReadData(entry->baseMemoryAddress);
				stream.ReadData(entry->memoryBlockSize);
				stream.ReadData(dataType);
				entry->dataType = (DisassemblyDataType)dataType;
				stream.ReadData(entry->conflictsWithKnownCode);
				stream.ReadData(entry->relativeOffset);
				stream.ReadData(entry->relativeOffsetBaseAddress);
				stream.ReadData(entry->entryDefinedOutsideArray);
				stream.ReadData(entry->arrayStartingHereDefined);
				stream.ReadData(entry->arrayIDStartingHere);

				unsigned int arrayMembershipCount;
				stream.ReadData(arrayMembershipCount);
				for(unsigned int arrayMembershipNo = 0; arrayMembershipNo < arrayMembershipCount; ++arrayMembershipNo)
				{
					unsigned int arrayNo;
					stream.ReadData(arrayNo);
					entry->arraysMemberOf.insert(arrayNo);
				}

				unsigned int commentLength;
				stream.ReadData(commentLength);
				entry->comment.resize(commentLength);
				for(unsigned int charNo = 0; charNo < commentLength; ++charNo)
				{
					stream.ReadData(entry->comment[charNo]);
				}

				activeDisassemblyAddressInfoSet.insert(entry);
			}

			//Read all jump table entries from the saved data
			unsigned int jumpTableEntryCount;
			stream.ReadData(jumpTableEntryCount);
			for(unsigned int entryNo = 0; entryNo < jumpTableEntryCount; ++entryNo)
			{
				DisassemblyJumpTableInfo entry;
				stream.ReadData(entry.baseMemoryAddress);
				stream.ReadData(entry.entrySize);

				unsigned int tableEntryCount;
				stream.ReadData(tableEntryCount);
				for(unsigned int tableEntryNo = 0; tableEntryNo < tableEntryCount; ++tableEntryNo)
				{
					unsigned int tableEntry;
					stream.ReadData(tableEntry);
					entry.knownEntries.insert(tableEntry);
				}

				activeDisassemblyJumpTableInfo.insert(DisassemblyJumpTableInfoMapEntry(entry.baseMemoryAddress, entry));
			}

			//Read all array entries from the saved data
			unsigned int arrayEntryCount;
			stream.ReadData(arrayEntryCount);
			for(unsigned int entryNo = 0; entryNo < arrayEntryCount; ++entryNo)
			{
				DisassemblyArrayInfo entry;
				unsigned int dataType;
				stream.ReadData(entry.arrayID);
				stream.ReadData(entry.baseMemoryAddress);
				stream.ReadData(entry.arrayEntrySize);
				stream.ReadData(entry.arrayEntryCount);
				stream.ReadData(dataType);
				entry.dataType = (DisassemblyDataType)dataType;
				stream.ReadData(entry.conflictsWithKnownCode);
				activeDisassemblyArrayInfo.insert(DisassemblyArrayInfoMapEntry(entry.arrayID, entry));
			}

			//Flag that active disassembly data has been loaded
			activeDisassemblyDataLoaded = true;
		}
	}

	//If the location or enable state for active disassembly has changed, update the
	//active region, and resize the physical memory array.
	if(activeDisassemblyStateChanged)
	{
		//Set the new active disassembly location
		activeDisassemblyStartLocation = newActiveDisassemblyStartLocation;
		activeDisassemblyEndLocation = newActiveDisassemblyEndLocation;

		//Clear disassemblyAddressInfo and resize it correctly for the new region if
		//active disassembly is currently enabled.
		activeDisassemblyAddressInfo.clear();
		if(activeDisassemblyEnabled)
		{
			unsigned int disassemblyAddressInfoArraySize = activeDisassemblyEndLocation - activeDisassemblyStartLocation;
			activeDisassemblyAddressInfo.resize(disassemblyAddressInfoArraySize);
		}
	}

	//If new active disassembly data was loaded, Clear the active disassembly analysis
	//data.
	if(activeDisassemblyDataLoaded)
	{
		activeDisassemblyAnalysis->Initialize();
	}

	//If new active disassembly data was loaded, and active disassembly is currently
	//running, ensure that the physical address entry array is updated to contain the new
	//entries.
	if(activeDisassemblyDataLoaded && activeDisassemblyEnabled)
	{
		//Add all address entries to the memory array
		for(std::set<DisassemblyAddressInfo*>::const_iterator addressInfoIterator = activeDisassemblyAddressInfoSet.begin(); addressInfoIterator != activeDisassemblyAddressInfoSet.end(); ++addressInfoIterator)
		{
			AddDisassemblyAddressInfoEntryToArray(*addressInfoIterator);
		}
	}

	Device::LoadDebuggerState(node);
}

//----------------------------------------------------------------------------------------
void Processor::SaveDebuggerState(IHeirarchicalStorageNode& node) const
{
	boost::mutex::scoped_lock lock(debugMutex);

	//Device enable
	node.CreateChild(L"Register", GetDeviceContext()->DeviceEnabled()).CreateAttribute(L"name", L"DeviceEnabled");

	//Clock speed
	if(clockSpeedOverridden)
	{
		node.CreateChild(L"Register", reportedClockSpeed).CreateAttribute(L"name", L"OverriddenClockSpeed");
	}

	//Breakpoints
	IHeirarchicalStorageNode& breakpointListNode = node.CreateChild(L"BreakpointList");
	for(BreakpointList::const_iterator i = breakpoints.begin(); i != breakpoints.end(); ++i)
	{
		IHeirarchicalStorageNode& breakpointListEntry = breakpointListNode.CreateChild(L"Breakpoint");
		(*i)->SaveState(breakpointListEntry);
	}
	IHeirarchicalStorageNode& watchpointListNode = node.CreateChild(L"WatchpointList");
	for(WatchpointList::const_iterator i = watchpoints.begin(); i != watchpoints.end(); ++i)
	{
		IHeirarchicalStorageNode& watchpointListEntry = watchpointListNode.CreateChild(L"Watchpoint");
		(*i)->SaveState(watchpointListEntry);
	}

	//Call stack
	node.CreateChild(L"Register", stackDisassemble).CreateAttribute(L"name", L"StackDisassemble");

	//Trace
	node.CreateChild(L"Register", traceEnabled).CreateAttribute(L"name", L"TraceEnabled");
	node.CreateChild(L"Register", traceDisassemble).CreateAttribute(L"name", L"TraceDisassemble");
	node.CreateChild(L"Register", traceLength).CreateAttribute(L"name", L"TraceLength");

	//Active Disassembly
	node.CreateChild(L"Register", activeDisassemblyEnabled).CreateAttribute(L"name", L"ActiveDisassemblyEnabled");
	node.CreateChildHex(L"Register", activeDisassemblyStartLocation, GetAddressBusCharWidth()).CreateAttribute(L"name", L"ActiveDisassemblyStartLocation");
	node.CreateChildHex(L"Register", activeDisassemblyEndLocation, GetAddressBusCharWidth()).CreateAttribute(L"name", L"ActiveDisassemblyEndLocation");
	node.CreateChild(L"Register", activeDisassemblyArrayNextFreeID).CreateAttribute(L"name", L"ActiveDisassemblyArrayNextFreeID");
	node.CreateChildHex(L"Register", activeDisassemblyAnalysisStartLocation, GetAddressBusCharWidth()).CreateAttribute(L"name", L"ActiveDisassemblyAnalysisStartLocation");
	node.CreateChildHex(L"Register", activeDisassemblyAnalysisEndLocation, GetAddressBusCharWidth()).CreateAttribute(L"name", L"ActiveDisassemblyAnalysisEndLocation");
	node.CreateChild(L"Register", activeDisassemblyAnalyzeCode).CreateAttribute(L"name", L"ActiveDisassemblyAnalyzeCode");
	node.CreateChild(L"Register", activeDisassemblyAnalyzeData).CreateAttribute(L"name", L"ActiveDisassemblyAnalyzeData");
	node.CreateChild(L"Register", activeDisassemblyAnalyzeCodeOffsets).CreateAttribute(L"name", L"ActiveDisassemblyAnalyzeCodeOffsets");
	node.CreateChild(L"Register", activeDisassemblyAnalyzeDataOffsets).CreateAttribute(L"name", L"ActiveDisassemblyAnalyzeDataOffsets");
	node.CreateChild(L"Register", activeDisassemblyAnalyzePredictedArrays).CreateAttribute(L"name", L"ActiveDisassemblyAnalyzePredictedArrays");
	node.CreateChild(L"Register", activeDisassemblyAnalyzePredictedJumpTables).CreateAttribute(L"name", L"ActiveDisassemblyAnalyzePredictedJumpTables");
	node.CreateChild(L"Register", activeDisassemblyExploreCodePaths).CreateAttribute(L"name", L"ActiveDisassemblyExploreCodePaths");
	node.CreateChild(L"Register", activeDisassemblyPerformLabelSubstitution).CreateAttribute(L"name", L"ActiveDisassemblyPerformLabelSubstitution");
	node.CreateChild(L"Register", activeDisassemblyDetectOffsets).CreateAttribute(L"name", L"ActiveDisassemblyDetectOffsets");
	node.CreateChild(L"Register", activeDisassemblyDetectJumpTables).CreateAttribute(L"name", L"ActiveDisassemblyDetectJumpTables");
	node.CreateChild(L"Register", activeDisassemblyDetectData).CreateAttribute(L"name", L"ActiveDisassemblyDetectData");
	node.CreateChild(L"Register", activeDisassemblyDetectDataArrays).CreateAttribute(L"name", L"ActiveDisassemblyDetectDataArrays");
	node.CreateChild(L"Register", activeDisassemblyDetectTextData).CreateAttribute(L"name", L"ActiveDisassemblyDetectTextData");
	node.CreateChild(L"Register", activeDisassemblyOffsetArrayIncreaseTolerance).CreateAttribute(L"name", L"ActiveDisassemblyOffsetArrayIncreaseTolerance");
	node.CreateChild(L"Register", activeDisassemblyMinimumArrayEntryCount).CreateAttribute(L"name", L"ActiveDisassemblyMinimumArrayEntryCount");
	node.CreateChildHex(L"Register", activeDisassemblyOffsetArrayDistanceTolerance, GetAddressBusCharWidth()).CreateAttribute(L"name", L"ActiveDisassemblyOffsetArrayDistanceTolerance");
	node.CreateChildHex(L"Register", activeDisassemblyJumpTableDistanceTolerance, GetAddressBusCharWidth()).CreateAttribute(L"name", L"ActiveDisassemblyJumpTableDistanceTolerance");
	if(!activeDisassemblyAddressInfoSet.empty())
	{
		//Create and configure the child node to store our active disassembly data as a
		//binary data stream.
		IHeirarchicalStorageNode& childNode = node.CreateChild(L"ActiveDisassemblyData");
		childNode.SetBinaryDataPresent(true);
		childNode.SetInlineBinaryDataEnabled(false);
		childNode.SetBinaryDataBufferName(GetFullyQualifiedDeviceInstanceName() + L".DisassemblyAddressInfo");

		//Obtain and configure the data stream for the saved data
		Stream::IStream& stream = childNode.GetBinaryDataBufferStream();
		stream.SetByteOrder(Stream::IStream::BYTEORDER_LITTLEENDIAN);

		//Write the version number of the data format
		stream.WriteData((unsigned int)1);

		//Write all address entries to the saved data
		stream.WriteData((unsigned int)activeDisassemblyAddressInfoSet.size());
		for(std::set<DisassemblyAddressInfo*>::const_iterator i = activeDisassemblyAddressInfoSet.begin(); i != activeDisassemblyAddressInfoSet.end(); ++i)
		{
			const DisassemblyAddressInfo* entry = *i;
			stream.WriteData(entry->entryType);
			stream.WriteData(entry->baseMemoryAddress);
			stream.WriteData(entry->memoryBlockSize);
			stream.WriteData(entry->dataType);
			stream.WriteData(entry->conflictsWithKnownCode);
			stream.WriteData(entry->relativeOffset);
			stream.WriteData(entry->relativeOffsetBaseAddress);
			stream.WriteData(entry->entryDefinedOutsideArray);
			stream.WriteData(entry->arrayStartingHereDefined);
			stream.WriteData(entry->arrayIDStartingHere);
			stream.WriteData((unsigned int)entry->arraysMemberOf.size());
			for(std::set<unsigned int>::const_iterator arrayIterator = entry->arraysMemberOf.begin(); arrayIterator != entry->arraysMemberOf.end(); ++arrayIterator)
			{
				stream.WriteData(*arrayIterator);
			}
			stream.WriteData((unsigned int)entry->comment.size());
			for(unsigned int charIndex = 0; charIndex < (unsigned int)entry->comment.size(); ++charIndex)
			{
				stream.WriteData(entry->comment[charIndex]);
			}
		}

		//Write all jump table entries to the saved data
		stream.WriteData((unsigned int)activeDisassemblyJumpTableInfo.size());
		for(DisassemblyJumpTableInfoMap::const_iterator i = activeDisassemblyJumpTableInfo.begin(); i != activeDisassemblyJumpTableInfo.end(); ++i)
		{
			const DisassemblyJumpTableInfo& entry = i->second;
			stream.WriteData(entry.baseMemoryAddress);
			stream.WriteData(entry.entrySize);
			stream.WriteData((unsigned int)entry.knownEntries.size());
			for(std::set<unsigned int>::const_iterator knownEntryIterator = entry.knownEntries.begin(); knownEntryIterator != entry.knownEntries.end(); ++knownEntryIterator)
			{
				stream.WriteData(*knownEntryIterator);
			}
		}

		//Write all array entries to the saved data
		stream.WriteData((unsigned int)activeDisassemblyArrayInfo.size());
		for(DisassemblyArrayInfoMap::const_iterator i = activeDisassemblyArrayInfo.begin(); i != activeDisassemblyArrayInfo.end(); ++i)
		{
			const DisassemblyArrayInfo& entry = i->second;
			stream.WriteData(entry.arrayID);
			stream.WriteData(entry.baseMemoryAddress);
			stream.WriteData(entry.arrayEntrySize);
			stream.WriteData(entry.arrayEntryCount);
			stream.WriteData(entry.dataType);
			stream.WriteData(entry.conflictsWithKnownCode);
		}
	}

	Device::SaveDebuggerState(node);
}

//----------------------------------------------------------------------------------------
void Processor::LoadCallStack(IHeirarchicalStorageNode& node)
{
	callStack.clear();
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"CallStackEntry")
		{
			CallStackEntry callStackEntry;
			(*i)->ExtractAttributeHex(L"SourceAddress", callStackEntry.sourceAddress);
			(*i)->ExtractAttributeHex(L"TargetAddress", callStackEntry.targetAddress);
			(*i)->ExtractAttributeHex(L"ReturnAddress", callStackEntry.returnAddress);
			(*i)->ExtractAttribute(L"Disassembly", callStackEntry.disassembly);
			callStack.push_back(callStackEntry);
		}
	}
}

//----------------------------------------------------------------------------------------
void Processor::SaveCallStack(IHeirarchicalStorageNode& node) const
{
	unsigned int pcCharWidth = GetPCCharWidth();
	for(CallStack::iterator i = callStack.begin(); i != callStack.end(); ++i)
	{
		IHeirarchicalStorageNode& callStackEntry = node.CreateChild(L"CallStackEntry");
		callStackEntry.CreateAttributeHex(L"SourceAddress", i->sourceAddress, pcCharWidth);
		callStackEntry.CreateAttributeHex(L"TargetAddress", i->targetAddress, pcCharWidth);
		callStackEntry.CreateAttributeHex(L"ReturnAddress", i->returnAddress, pcCharWidth);
		callStackEntry.CreateAttribute(L"Disassembly", i->disassembly);
	}
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void Processor::CreateMenuHandlers()
{
	if(menuHandler == 0)
	{
		menuHandler = new DebugMenuHandler(this);
		menuHandler->LoadMenuItems();
	}
}

//----------------------------------------------------------------------------------------
void Processor::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Device::AddDebugMenuItems(menuSegment, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Processor::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Device::RestoreViewModelState(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->RestoreMenuViewModelOpen(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Processor::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	Device::OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
}
