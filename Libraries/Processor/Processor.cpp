#include "Processor.h"
#include "SystemInterface/SystemInterface.pkg"
#include "DebugMenuHandler.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <boost/bind.hpp>
#include <boost/function.hpp>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::Processor(const std::wstring& aclassName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(aclassName, ainstanceName, amoduleID),
clockSpeed(0), originalClockSpeed(0),
traceEnabled(false), traceDisassemble(false), traceLength(200), traceCoverageEnabled(false), traceCoverageStart(0), traceCoverageEnd(0),
stackDisassemble(false), stepOver(false), stepOut(false),
breakOnNextOpcode(false), breakpointTriggered(false), breakpointExists(false), watchpointExists(false),
activeDisassembly(false)
{
	//Create the menu handler
	menuHandler = new DebugMenuHandler(this);
	menuHandler->LoadMenuItems();

	//Initialize disassembly info
	disassemblyArrayNextFreeID = 0;
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
	for(std::set<DisassemblyAddressInfo*>::const_iterator i = disassemblyAddressInfoSet.begin(); i != disassemblyAddressInfoSet.end(); ++i)
	{
		delete *i;
	}
	disassemblyAddressInfoSet.clear();
	disassemblyAddressInfo.clear();

	//Delete the menu handler
	menuHandler->ClearMenuItems();
	delete menuHandler;
}

//----------------------------------------------------------------------------------------
bool Processor::Construct(IHeirarchicalStorageNode& node)
{
	bool result = Device::Construct(node);
	IHeirarchicalStorageAttribute* clockSpeedAttribute = node.GetAttribute(L"ClockSpeed");
	if(clockSpeedAttribute != 0)
	{
		clockSpeed = clockSpeedAttribute->ExtractValue<unsigned int>();
		originalClockSpeed = clockSpeed;
	}
	else
	{
		result = false;
	}
	return result;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void Processor::ExecuteRollback()
{
	boost::mutex::scoped_lock lock(debugMutex);

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
	stepOver = bstepOver;
	stepOut = bstepOut;
	stackLevel = bstackLevel;
}

//----------------------------------------------------------------------------------------
void Processor::ExecuteCommit()
{
	boost::mutex::scoped_lock lock(debugMutex);

	//Call stack and trace log
	bcallStack = callStack;
	btrace = trace;

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
unsigned int Processor::GetClockSpeed() const
{
	return clockSpeed;
}

//----------------------------------------------------------------------------------------
void Processor::SetClockSpeed(unsigned int aclockSpeed)
{
	clockSpeed = aclockSpeed;
}

//----------------------------------------------------------------------------------------
void Processor::RestoreClockSpeed()
{
	clockSpeed = originalClockSpeed;
}

//----------------------------------------------------------------------------------------
double Processor::CalculateExecutionTime(unsigned int cycles) const
{
	return (cycles * (1000000000 / (double)GetClockSpeed()));
}

//----------------------------------------------------------------------------------------
//Instruction functions
//----------------------------------------------------------------------------------------
Processor::OpcodeInfo Processor::GetOpcodeInfo(unsigned int location)
{
	Processor::OpcodeInfo opcodeInfo;
	opcodeInfo.valid = true;
	opcodeInfo.disassemblyOpcode = L"NOT SUPPORTED";
	opcodeInfo.opcodeSize = 1;
	return opcodeInfo;
}

//----------------------------------------------------------------------------------------
Data Processor::GetRawData(unsigned int location)
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
//Breakpoint functions
//----------------------------------------------------------------------------------------
void Processor::CheckExecution(unsigned int location) const
{
	if(breakpointExists)
	{
		boost::mutex::scoped_lock lock(debugMutex);

		bool breakOnInstruction = breakOnNextOpcode;
		breakOnNextOpcode = false;
		Breakpoint* triggerBreakpoint = 0;
		for(BreakpointList::iterator i = breakpoints.begin(); i != breakpoints.end(); ++i)
		{
			Breakpoint* breakpoint = *i;
			//Evaluate location
			if(breakpoint->PassesLocationCondition(location))
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
			if(watchpoint->PassesLocationCondition(location) && watchpoint->GetOnRead() && watchpoint->PassesReadCondition(data))
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
			if(watchpoint->PassesLocationCondition(location) && watchpoint->GetOnWrite() && watchpoint->PassesWriteCondition(data))
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
	if(GetTraceEnabled() || GetTraceCoverageEnabled())
	{
		boost::mutex::scoped_lock lock(debugMutex);

		if(GetTraceEnabled())
		{
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

		//Add the address to the coverage log
		if(GetTraceCoverageEnabled() && (pc >= GetTraceCoverageStart()) && (pc <= GetTraceCoverageEnd()))
		{
			traceCoverage.insert(pc);
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
bool Processor::GetTraceCoverageEnabled() const
{
	return traceCoverageEnabled;
}

//----------------------------------------------------------------------------------------
void Processor::SetTraceCoverageEnabled(bool astate)
{
	traceCoverageEnabled = astate;
}

//----------------------------------------------------------------------------------------
unsigned int Processor::GetTraceCoverageStart() const
{
	return traceCoverageStart;
}

//----------------------------------------------------------------------------------------
void Processor::SetTraceCoverageStart(unsigned int astart)
{
	traceCoverageStart = astart;
}

//----------------------------------------------------------------------------------------
unsigned int Processor::GetTraceCoverageEnd() const
{
	return traceCoverageEnd;
}

//----------------------------------------------------------------------------------------
void Processor::SetTraceCoverageEnd(unsigned int aend)
{
	traceCoverageEnd = aend;
}

//----------------------------------------------------------------------------------------
std::set<unsigned int> Processor::GetTraceCoverageLog() const
{
	return traceCoverage;
}

//----------------------------------------------------------------------------------------
void Processor::ClearTraceCoverageLog()
{
	traceCoverage.clear();
}

//----------------------------------------------------------------------------------------
unsigned int Processor::GetTraceCoverageLogSize() const
{
	return (unsigned int)traceCoverage.size();
}

//----------------------------------------------------------------------------------------
//Disassembly functions
//----------------------------------------------------------------------------------------
void Processor::EnableActiveDisassembly(unsigned int startLocation, unsigned int endLocation)
{
	//Ensure that any existing disassembly info has been cleared
	DisableActiveDisassembly();

	//Set the start and end locations for the disassembly
	activeDisassemblyStartLocation = startLocation;
	activeDisassemblyEndLocation = endLocation;

	//Validate the start and end locations for the disassembly
	if(activeDisassemblyEndLocation <= activeDisassemblyStartLocation)
	{
		return;
	}

	//Initialize the array holding the disassembly address info
	disassemblyAddressInfo.resize(activeDisassemblyEndLocation - activeDisassemblyStartLocation);

	//Flag that an active disassembly is in progress
	activeDisassembly = true;
}

//----------------------------------------------------------------------------------------
void Processor::DisableActiveDisassembly()
{
	//Flag that an active disassembly is not in progress
	activeDisassembly = false;

	//Delete all stored disassembly info
	for(std::set<DisassemblyAddressInfo*>::const_iterator i = disassemblyAddressInfoSet.begin(); i != disassemblyAddressInfoSet.end(); ++i)
	{
		delete *i;
	}
	disassemblyAddressInfoSet.clear();
	disassemblyAddressInfo.clear();
	disassemblyArrayInfo.clear();
}

//----------------------------------------------------------------------------------------
void Processor::AddDisassemblyAddressInfoCode(unsigned int location, unsigned int dataSize)
{
	//Verify that active disassembly is enabled, and that this address falls within the
	//target area.
	if(activeDisassembly && (location >= activeDisassemblyStartLocation) && (location < activeDisassemblyEndLocation))
	{
		//Try and find identical existing references at the same address
		bool foundExistingCodeReference = false;
		std::list<DisassemblyAddressInfo*>& addressList = disassemblyAddressInfo[location - activeDisassemblyStartLocation];
		for(std::list<DisassemblyAddressInfo*>::const_iterator i = addressList.begin(); i != addressList.end(); ++i)
		{
			DisassemblyAddressInfo* entry = *i;
			if((entry->baseMemoryAddress == location) && (entry->memoryBlockSize == dataSize))
			{
				//Check if this entry refers to an opcode
				if(entry->dataType == DISASSEMBLYDATATYPE_CODE)
				{
					//Flag if we've found an existing reference to an opcode at this location
					foundExistingCodeReference = true;
				}
				else if(entry->dataType == DISASSEMBLYDATATYPE_CODE_AUTODETECT)
				{
					//If we've found a reference to an auto-detected opcode at this location,
					//change it to a confirmed opcode, and flag that we've found an existing
					//opcode reference here.
					entry->dataType = DISASSEMBLYDATATYPE_CODE;
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

		boost::mutex::scoped_lock lock(debugMutex);

		//Create a new DisassemblyAddressInfo object to describe the entry
		DisassemblyAddressInfo* newEntry = new DisassemblyAddressInfo();
		newEntry->dataType = DISASSEMBLYDATATYPE_CODE;
		newEntry->baseMemoryAddress = location;
		newEntry->memoryBlockSize = dataSize;

		//Add the new reference to the reference list at each address location it occupies,
		//and set conflict flags where appropriate.
		disassemblyAddressInfoSet.insert(newEntry);
		for(unsigned int locationOffset = 0; (locationOffset < dataSize) && ((location + locationOffset) < activeDisassemblyEndLocation); ++locationOffset)
		{
			//Iterate over each address which falls within the range of this opcode, and if
			//there are any data references, mark that they conflict with a known code
			//address.
			std::list<DisassemblyAddressInfo*>& addressList = disassemblyAddressInfo[(location - activeDisassemblyStartLocation) + locationOffset];
			for(std::list<DisassemblyAddressInfo*>::const_iterator i = addressList.begin(); i != addressList.end(); ++i)
			{
				DisassemblyAddressInfo* entry = *i;

				//If we've found an existing entry which appears to overlap with the new code
				//entry at this address, mark the existing entry as conflicted.
				if((entry->dataType != DISASSEMBLYDATATYPE_CODE))
				{
					entry->conflictsWithKnownCode = true;
				}

				//If we have an actual known code entry which seems to overlap, mark both code
				//entries as conflicted.
				if((entry->dataType == DISASSEMBLYDATATYPE_CODE) && ((entry->baseMemoryAddress != location) || (entry->memoryBlockSize != dataSize)))
				{
					entry->conflictsWithKnownCode = true;
					newEntry->conflictsWithKnownCode = true;
				}
			}

			//Add the new reference to the list
			addressList.push_front(newEntry);
		}
	}
}

//----------------------------------------------------------------------------------------
void Processor::AddDisassemblyAddressInfoData(unsigned int location, unsigned int dataSize)
{
	//Verify that active disassembly is enabled, and that this address falls within the
	//target area.
	if(activeDisassembly && (location >= activeDisassemblyStartLocation) && (location < activeDisassemblyEndLocation))
	{
		//Try and find identical existing references at the same address
		bool foundExistingDataReference = false;
		std::list<DisassemblyAddressInfo*>& addressList = disassemblyAddressInfo[location - activeDisassemblyStartLocation];
		for(std::list<DisassemblyAddressInfo*>::const_iterator i = addressList.begin(); i != addressList.end(); ++i)
		{
			DisassemblyAddressInfo* entry = *i;
			if((entry->dataType == DISASSEMBLYDATATYPE_DATA) && (entry->baseMemoryAddress == location) && (entry->memoryBlockSize == dataSize))
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

		boost::mutex::scoped_lock lock(debugMutex);

		//Create a new DisassemblyAddressInfo object to describe the entry
		DisassemblyAddressInfo* newEntry = new DisassemblyAddressInfo();
		newEntry->dataType = DISASSEMBLYDATATYPE_DATA;
		newEntry->baseMemoryAddress = location;
		newEntry->memoryBlockSize = dataSize;

		//Add the new reference to the reference list at each address location it occupies,
		//and set conflict flags where appropriate.
		disassemblyAddressInfoSet.insert(newEntry);
		for(unsigned int locationOffset = 0; (locationOffset < dataSize) && ((location + locationOffset) < activeDisassemblyEndLocation); ++locationOffset)
		{
			//Iterate over each address which falls within the range of this opcode, and if
			//there are any code references, mark that our new entry conflicts with a known
			//code address.
			std::list<DisassemblyAddressInfo*>& addressList = disassemblyAddressInfo[(location - activeDisassemblyStartLocation) + locationOffset];
			for(std::list<DisassemblyAddressInfo*>::const_iterator i = addressList.begin(); i != addressList.end(); ++i)
			{
				DisassemblyAddressInfo* entry = *i;

				//If we've found a code entry which overlaps with our new data entry, mark the
				//data entry as conflicted.
				if((entry->dataType == DISASSEMBLYDATATYPE_CODE))
				{
					newEntry->conflictsWithKnownCode = true;
				}
			}

			//Add the new reference to the list
			addressList.push_front(newEntry);
		}
	}
}

//----------------------------------------------------------------------------------------
void Processor::AddDisassemblyAddressInfoOffset(unsigned int location, unsigned int dataSize, bool offsetToCode, bool relativeOffset, unsigned int relativeOffsetBaseAddress)
{
	//Determine the type of offset being added
	DisassemblyDataType dataType = offsetToCode? DISASSEMBLYDATATYPE_OFFSETCODE: DISASSEMBLYDATATYPE_OFFSETDATA;

	//Verify that active disassembly is enabled, and that this address falls within the
	//target area.
	if(activeDisassembly && (location >= activeDisassemblyStartLocation) && (location < activeDisassemblyEndLocation))
	{
		//Try and find identical existing references at the same address
		bool foundExistingDataReference = false;
		std::list<DisassemblyAddressInfo*>& addressList = disassemblyAddressInfo[location - activeDisassemblyStartLocation];
		for(std::list<DisassemblyAddressInfo*>::const_iterator i = addressList.begin(); i != addressList.end(); ++i)
		{
			DisassemblyAddressInfo* entry = *i;
			if((entry->dataType == dataType) && (entry->baseMemoryAddress == location) && (entry->memoryBlockSize == dataSize))
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

		boost::mutex::scoped_lock lock(debugMutex);

		//Create a new DisassemblyAddressInfo object to describe the entry
		DisassemblyAddressInfo* newEntry = new DisassemblyAddressInfo();
		newEntry->dataType = dataType;
		newEntry->baseMemoryAddress = location;
		newEntry->memoryBlockSize = dataSize;
		newEntry->relativeOffset = relativeOffset;
		newEntry->relativeOffsetBaseAddress = relativeOffsetBaseAddress;

		//Add the new reference to the reference list at each address location it occupies,
		//and set conflict flags where appropriate.
		disassemblyAddressInfoSet.insert(newEntry);
		for(unsigned int locationOffset = 0; (locationOffset < dataSize) && ((location + locationOffset) < activeDisassemblyEndLocation); ++locationOffset)
		{
			//Iterate over each address which falls within the range of this opcode, and if
			//there are any code references, mark that our new entry conflicts with a known
			//code address.
			std::list<DisassemblyAddressInfo*>& addressList = disassemblyAddressInfo[(location - activeDisassemblyStartLocation) + locationOffset];
			for(std::list<DisassemblyAddressInfo*>::const_iterator i = addressList.begin(); i != addressList.end(); ++i)
			{
				DisassemblyAddressInfo* entry = *i;

				//If we've found a code entry which overlaps with our new data entry, mark the
				//data entry as conflicted.
				if((entry->dataType == DISASSEMBLYDATATYPE_CODE))
				{
					newEntry->conflictsWithKnownCode = true;
				}
			}

			//Add the new reference to the list
			addressList.push_front(newEntry);
		}
	}
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void Processor::LoadState(IHeirarchicalStorageNode& node)
{
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"CallStack")
		{
			LoadCallStack(*(*i));
		}
	}
}

//----------------------------------------------------------------------------------------
void Processor::GetState(IHeirarchicalStorageNode& node) const
{
	IHeirarchicalStorageNode& callStackNode = node.CreateChild(L"CallStack");
	SaveCallStack(callStackNode);
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
void Processor::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Device::AddDebugMenuItems(menuSegment, viewModelLauncher);
	menuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Processor::RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Device::RestoreViewModelState(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
	menuHandler->RestoreMenuViewModelOpen(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Processor::OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher)
{
	Device::OpenViewModel(menuHandlerName, viewModelID, viewModelLauncher);
	menuHandler->OpenViewModel(menuHandlerName, viewModelID, viewModelLauncher);
}
