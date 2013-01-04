#include "Z80.h"
#include "Z80Opcodes.pkg"
#include "DebugMenuHandler.h"
//##DEBUG##
#include <iostream>
namespace Z80{

//----------------------------------------------------------------------------------------
Z80::Z80(const std::wstring& ainstanceName, unsigned int amoduleID)
:Processor(L"Z80", ainstanceName, amoduleID), opcodeTable(8), opcodeTableCB(8), opcodeTableED(8), opcodeBuffer(0), memoryBus(0)
{
	//Set the default state for our device preferences
	suspendWhenBusReleased = false;

	//Initialize our CE line state
	ceLineMaskRD = 0;
	ceLineMaskWR = 0;

	//Create the menu handler
	menuHandler = new DebugMenuHandler(this);
	menuHandler->LoadMenuItems();
}

//----------------------------------------------------------------------------------------
Z80::~Z80()
{
	//Delete the opcode buffer
	delete opcodeBuffer;

	//Delete all objects stored in the opcode lists
	for(std::list<Z80Instruction*>::const_iterator i = opcodeList.begin(); i != opcodeList.end(); ++i)
	{
		delete *i;
	}
	for(std::list<Z80Instruction*>::const_iterator i = opcodeListCB.begin(); i != opcodeListCB.end(); ++i)
	{
		delete *i;
	}
	for(std::list<Z80Instruction*>::const_iterator i = opcodeListED.begin(); i != opcodeListED.end(); ++i)
	{
		delete *i;
	}

	//Delete the menu handler
	menuHandler->ClearMenuItems();
	delete menuHandler;
}

//----------------------------------------------------------------------------------------
bool Z80::Construct(IHeirarchicalStorageNode& node)
{
	bool result = Processor::Construct(node);
	IHeirarchicalStorageAttribute* suspendWhenBusReleasedAttribute = node.GetAttribute(L"SuspendWhenBusReleased");
	if(suspendWhenBusReleasedAttribute != 0)
	{
		suspendWhenBusReleased = suspendWhenBusReleasedAttribute->ExtractValue<bool>();
	}
	return result;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool Z80::BuildDevice()
{
	bool result = true;

	//Initialize our opcode tables
	opcodeTable.InitializeOpcodeTable();
	opcodeTableED.InitializeOpcodeTable();
	opcodeTableCB.InitializeOpcodeTable();

	//Delete all objects stored in the opcode lists
	for(std::list<Z80Instruction*>::const_iterator i = opcodeList.begin(); i != opcodeList.end(); ++i)
	{
		delete *i;
	}
	for(std::list<Z80Instruction*>::const_iterator i = opcodeListCB.begin(); i != opcodeListCB.end(); ++i)
	{
		delete *i;
	}
	for(std::list<Z80Instruction*>::const_iterator i = opcodeListED.begin(); i != opcodeListED.end(); ++i)
	{
		delete *i;
	}
	opcodeList.clear();
	opcodeListCB.clear();
	opcodeListED.clear();

	//Add all defined opcodes for this device to the opcode list

	//8-Bit Load Instructions
	opcodeList.push_back(new LD8());
	opcodeListED.push_back(new LD8ED());

	//16-Bit Load Instructions
	opcodeList.push_back(new LD16());
	opcodeListED.push_back(new LD16ED());
	opcodeList.push_back(new PUSH());
	opcodeList.push_back(new POP());

	//Exchange, Block Transfer, and Search Instructions
	opcodeList.push_back(new EX());
	opcodeList.push_back(new EXX());
	opcodeListED.push_back(new LDI());
	opcodeListED.push_back(new LDIR());
	opcodeListED.push_back(new LDD());
	opcodeListED.push_back(new LDDR());
	opcodeListED.push_back(new CPI());
	opcodeListED.push_back(new CPIR());
	opcodeListED.push_back(new CPD());
	opcodeListED.push_back(new CPDR());

	//8-Bit Arithmetic Instructions
	opcodeList.push_back(new ADD8());
	opcodeList.push_back(new AND8());
	opcodeList.push_back(new CP8());
	opcodeList.push_back(new DEC8());
	opcodeList.push_back(new INC8());
	opcodeList.push_back(new OR8());
	opcodeList.push_back(new SUB8());
	opcodeList.push_back(new XOR8());
	opcodeList.push_back(new ADC8());
	opcodeList.push_back(new SBC8());

	//General-Purpose Arithmetic and CPU Control Instructions
	opcodeList.push_back(new CPL());
	opcodeList.push_back(new CCF());
	opcodeList.push_back(new SCF());
	opcodeList.push_back(new NOP());
	opcodeListED.push_back(new NEG());
	opcodeList.push_back(new HALT());
	opcodeList.push_back(new DI());
	opcodeList.push_back(new EI());
	opcodeListED.push_back(new IM());
	opcodeList.push_back(new DAA());

	//16-bit Arithmetic Instructions
	opcodeList.push_back(new ADD16());
	opcodeList.push_back(new DEC16());
	opcodeList.push_back(new INC16());
	opcodeListED.push_back(new ADC16());
	opcodeListED.push_back(new SBC16());

	//Rotate and Shift Instructions
	opcodeList.push_back(new RLCA());
	opcodeList.push_back(new RLA());
	opcodeList.push_back(new RRCA());
	opcodeList.push_back(new RRA());
	opcodeListCB.push_back(new RLC());
	opcodeListCB.push_back(new RL());
	opcodeListCB.push_back(new RRC());
	opcodeListCB.push_back(new RR());
	opcodeListCB.push_back(new SLA());
	opcodeListCB.push_back(new SRA());
	opcodeListCB.push_back(new SLL());
	opcodeListCB.push_back(new SRL());
	opcodeListED.push_back(new RLD());
	opcodeListED.push_back(new RRD());

	//Bit set, reset, and test instructions
	opcodeListCB.push_back(new BIT());
	opcodeListCB.push_back(new SET());
	opcodeListCB.push_back(new RES());

	//Jump Instructions
	opcodeList.push_back(new JP());
	opcodeList.push_back(new JR());
	opcodeList.push_back(new DJNZ());

	//Call and Return Instructions
	opcodeList.push_back(new CALL());
	opcodeList.push_back(new RET());
	opcodeList.push_back(new RST());
	opcodeListED.push_back(new RETI());
	opcodeListED.push_back(new RETN());

	//##TODO## Input and Output Instructions
	//IN
	//INI
	//INIR
	//IND
	//INDR
	//OUT
	//OUTI
	//OTIR
	//OUTD
	//OTDR

	//Prepare to calculate the size of the largest opcode object allocated by this device
	size_t largestObjectSize = 0;

	//Register each constructed opcode object in the opcode table, and update the size of
	//the largest opcode object.
	for(std::list<Z80Instruction*>::const_iterator i = opcodeList.begin(); i != opcodeList.end(); ++i)
	{
		//Register this opcode in the opcode table
		Z80Instruction* opcodeObject = *i;
		if(!opcodeObject->RegisterOpcode(opcodeTable))
		{
			//Log the event
			LogEntry logEntry(LogEntry::EVENTLEVEL_CRITICAL);
			logEntry << L"Error registering opcode! Opcode name: " << opcodeObject->GetOpcodeName();
			GetDeviceContext()->WriteLogEvent(logEntry);
			result = false;
		}

		//Update our calculation of the largest opcode size
		size_t currentOpcodeObjectSize = opcodeObject->GetOpcodeClassByteSize();
		largestObjectSize = (currentOpcodeObjectSize > largestObjectSize)? currentOpcodeObjectSize: largestObjectSize;
	}

	//Register each constructed CB opcode object in the CB opcode table, and update the
	//size of the largest opcode object.
	for(std::list<Z80Instruction*>::const_iterator i = opcodeListCB.begin(); i != opcodeListCB.end(); ++i)
	{
		//Register this opcode in the opcode table
		Z80Instruction* opcodeObject = *i;
		if(!opcodeObject->RegisterOpcode(opcodeTableCB))
		{
			//Log the event
			LogEntry logEntry(LogEntry::EVENTLEVEL_CRITICAL);
			logEntry << L"Error registering opcode! Opcode name: " << opcodeObject->GetOpcodeName();
			GetDeviceContext()->WriteLogEvent(logEntry);
			result = false;
		}

		//Update our calculation of the largest opcode size
		size_t currentOpcodeObjectSize = opcodeObject->GetOpcodeClassByteSize();
		largestObjectSize = (currentOpcodeObjectSize > largestObjectSize)? currentOpcodeObjectSize: largestObjectSize;
	}

	//Register each constructed ED opcode object in the ED opcode table, and update the
	//size of the largest opcode object.
	for(std::list<Z80Instruction*>::const_iterator i = opcodeListED.begin(); i != opcodeListED.end(); ++i)
	{
		//Register this opcode in the opcode table
		Z80Instruction* opcodeObject = *i;
		if(!opcodeObject->RegisterOpcode(opcodeTableED))
		{
			//Log the event
			LogEntry logEntry(LogEntry::EVENTLEVEL_CRITICAL);
			logEntry << L"Error registering opcode! Opcode name: " << opcodeObject->GetOpcodeName();
			GetDeviceContext()->WriteLogEvent(logEntry);
			result = false;
		}

		//Update our calculation of the largest opcode size
		size_t currentOpcodeObjectSize = opcodeObject->GetOpcodeClassByteSize();
		largestObjectSize = (currentOpcodeObjectSize > largestObjectSize)? currentOpcodeObjectSize: largestObjectSize;
	}

	//Allocate a new opcode buffer, which is large enough to hold an instance of the
	//largest opcode object.
	opcodeBuffer = (void*)new unsigned char[largestObjectSize];

	return result;
}

//----------------------------------------------------------------------------------------
bool Z80::ValidateDevice()
{
	return (memoryBus != 0);
}

//----------------------------------------------------------------------------------------
void Z80::Initialize()
{
	afreg = 0;
	af2reg = 0;
	bcreg = 0;
	bc2reg = 0;
	dereg = 0;
	de2reg = 0;
	hlreg = 0;
	hl2reg = 0;
	bafreg = 0;
	baf2reg = 0;
	bbcreg = 0;
	bbc2reg = 0;
	bdereg = 0;
	bde2reg = 0;
	bhlreg = 0;
	bhl2reg = 0;

	ireg = 0;
	rreg = 0;
	ixreg = 0;
	iyreg = 0;
	spreg = 0;
	pcreg = 0;
	bireg = 0;
	brreg = 0;
	bixreg = 0;
	biyreg = 0;
	bspreg = 0;
	bpcreg = 0;

	interruptMode = 0;
	iff1 = false;
	iff2 = false;
	maskInterruptsNextOpcode = false;
	binterruptMode = 0;
	biff1 = false;
	biff2 = false;
	bmaskInterruptsNextOpcode = false;

	processorStopped = false;
	bprocessorStopped = false;

	lineAccessPending = false;
	resetLineState = false;
	busreqLineState = false;
	intLineState = false;
	nmiLineState = false;
	lastTimesliceLength = 0;
	lineAccessBuffer.clear();
	suspendUntilLineStateChangeReceived = false;

	Reset();
}

//----------------------------------------------------------------------------------------
void Z80::Reset()
{
	processorStopped = false;
	maskInterruptsNextOpcode = false;

	//These defaults obtained from "The Undocumented Z80 Documented" by Sean Young. Some
	//additional info from http://www.z80.info/interrup.htm says the I and R registers
	//should also be zeroed here, which makes sense.
	SetSP(Z80Word(0xFFFF));
	SetAF(Z80Word(0xFFFF));
	SetPC(Z80Word(0));
	SetI(Z80Byte(0));
	SetR(Z80Byte(0));
	SetIFF1(false);
	SetIFF2(false);
	SetInterruptMode(0);

	//These defaults are suggested by "The Undocumented Z80 Documented", but apparently
	//these registers aren't actually initialized during power on.
	SetBC(Z80Word(0xFFFF));
	SetDE(Z80Word(0xFFFF));
	SetHL(Z80Word(0xFFFF));
	SetAF2(Z80Word(0xFFFF));
	SetBC2(Z80Word(0xFFFF));
	SetDE2(Z80Word(0xFFFF));
	SetHL2(Z80Word(0xFFFF));
	SetIX(Z80Word(0xFFFF));
	SetIY(Z80Word(0xFFFF));

	//##FIX## Refer to http://gendev.spritesmind.net/forum/viewtopic.php?t=779 thread
	//"Williams Arcade Classics sound" for lots of information about how the Z80 is
	//supposed to behave on a reset. According to Charles MacDonald:
	//-I, R, PC reset to zero. 
	//-IM 0 selected and interrupts are disabled (like DI). 
	//-All other registers (SP, AF, AF', IX, etc.) retain their values.
	//Also note that the HALT status is apparently supposed to be cleared here, as Charles
	//says "On the Genesis you can definitely do a DI; HALT sequence and still recover
	//from a 68K-triggered reset without being 'stuck' in the HALT state."

	ClearCallStack();
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool Z80::AddReference(const wchar_t* referenceName, IBusInterface* target)
{
	std::wstring referenceNameString = referenceName;
	if(referenceNameString == L"BusInterface")
	{
		memoryBus = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool Z80::RemoveReference(IBusInterface* target)
{
	if(memoryBus == target)
	{
		memoryBus = 0;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Suspend functions
//----------------------------------------------------------------------------------------
bool Z80::UsesExecuteSuspend() const
{
	return suspendWhenBusReleased;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void Z80::ExecuteRollback()
{
	afreg = bafreg;	af2reg = baf2reg;
	bcreg = bbcreg;	bc2reg = bbc2reg;
	dereg = bdereg;	de2reg = bde2reg;
	hlreg = bhlreg;	hl2reg = bhl2reg;
	ireg = bireg;
	rreg = brreg;
	ixreg = bixreg;
	iyreg = biyreg;
	spreg = bspreg;
	pcreg = bpcreg;
	interruptMode = binterruptMode;
	iff1 = biff1;
	iff2 = biff2;
	maskInterruptsNextOpcode = bmaskInterruptsNextOpcode;
	processorStopped = bprocessorStopped;

	lastTimesliceLength = blastTimesliceLength;
	lineAccessBuffer = blineAccessBuffer;
	lineAccessPending = !lineAccessBuffer.empty();

	suspendUntilLineStateChangeReceived = bsuspendUntilLineStateChangeReceived;
	resetLineState = bresetLineState;
	busreqLineState = bbusreqLineState;
	intLineState = bintLineState;
	nmiLineState = bnmiLineState;

	Processor::ExecuteRollback();
}

//----------------------------------------------------------------------------------------
void Z80::ExecuteCommit()
{
	bafreg = afreg;	baf2reg = af2reg;
	bbcreg = bcreg;	bbc2reg = bc2reg;
	bdereg = dereg;	bde2reg = de2reg;
	bhlreg = hlreg;	bhl2reg = hl2reg;
	bireg = ireg;
	brreg = rreg;
	bixreg = ixreg;
	biyreg = iyreg;
	bspreg = spreg;
	bpcreg = pcreg;
	binterruptMode = interruptMode;
	biff1 = iff1;
	biff2 = iff2;
	bmaskInterruptsNextOpcode = maskInterruptsNextOpcode;
	bprocessorStopped = processorStopped;

	blastTimesliceLength = lastTimesliceLength;
	if(lineAccessPending)
	{
		blineAccessBuffer = lineAccessBuffer;
	}
	else
	{
		blineAccessBuffer.clear();
	}

	bsuspendUntilLineStateChangeReceived = suspendUntilLineStateChangeReceived;
	bresetLineState = resetLineState;
	bbusreqLineState = busreqLineState;
	bintLineState = intLineState;
	bnmiLineState = nmiLineState;

	Processor::ExecuteCommit();
}

//----------------------------------------------------------------------------------------
bool Z80::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void Z80::NotifyUpcomingTimeslice(double nanoseconds)
{
	//Reset lastLineCheckTime for the beginning of the new timeslice, and force any
	//remaining line state changes to be evaluated at the start of the new timeslice.
	lastLineCheckTime = 0;
	for(std::list<LineAccess>::iterator i = lineAccessBuffer.begin(); i != lineAccessBuffer.end(); ++i)
	{
		//We rebase accessTime here to the start of the new time block, in order to allow
		//line state changes to be flagged ahead of the time they actually take effect.
		//This rebasing allows changes flagged ahead of time to safely cross timeslice
		//boundaries.
		i->accessTime -= lastTimesliceLength;
	}
	lastTimesliceLength = nanoseconds;
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int Z80::GetLineID(const wchar_t* lineName) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"RESET")
	{
		return LINE_RESET;
	}
	else if(lineNameString == L"BUSREQ")
	{
		return LINE_BUSREQ;
	}
	else if(lineNameString == L"BUSACK")
	{
		return LINE_BUSACK;
	}
	else if(lineNameString == L"INT")
	{
		return LINE_INT;
	}
	else if(lineNameString == L"NMI")
	{
		return LINE_NMI;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
const wchar_t* Z80::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_RESET:
		return L"RESET";
	case LINE_BUSREQ:
		return L"BUSREQ";
	case LINE_BUSACK:
		return L"BUSACK";
	case LINE_INT:
		return L"INT";
	case LINE_NMI:
		return L"NMI";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int Z80::GetLineWidth(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_RESET:
		return 1;
	case LINE_BUSREQ:
		return 1;
	case LINE_BUSACK:
		return 1;
	case LINE_INT:
		return 1;
	case LINE_NMI:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void Z80::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(lineMutex);

	//Flag that an entry exists in the buffer. This flag is used to skip the expensive
	//locking operation in the active thread for this device when no line changes are
	//pending. Note that we set this flag before we've actually written the entry into
	//the buffer, as we want to force the active thread to lock on the beginning of the
	//next cycle while this function is executing, so that the current timeslice progress
	//of the device doesn't change after we've read it.
	lineAccessPending = true;

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(lastLineCheckTime > accessTime)
	{
		GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//Insert the line access into the buffer. Note that entries in the buffer are sorted
	//by access time from lowest to highest.
	std::list<LineAccess>::reverse_iterator i = lineAccessBuffer.rbegin();
	while((i != lineAccessBuffer.rend()) && (i->accessTime > accessTime))
	{
		++i;
	}
	lineAccessBuffer.insert(i.base(), LineAccess(targetLine, lineData, accessTime));

	//Resume the main execution thread if it is currently suspended waiting for a line
	//state change to be received.
	if(suspendUntilLineStateChangeReceived)
	{
		GetDeviceContext()->ResumeTimesliceExecution();
	}
}

//----------------------------------------------------------------------------------------
void Z80::RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(lineMutex);

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(lastLineCheckTime > accessTime)
	{
		GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//Find the matching line state change entry in the line access buffer
	std::list<LineAccess>::reverse_iterator i = lineAccessBuffer.rbegin();
	bool foundTargetEntry = false;
	while(!foundTargetEntry && (i != lineAccessBuffer.rend()))
	{
		if((i->lineID == targetLine) && (i->state == lineData) && (i->accessTime == reportedTime))
		{
			foundTargetEntry = true;
			continue;
		}
		++i;
	}

	//Erase the target line state change entry from the line access buffer
	if(foundTargetEntry)
	{
		lineAccessBuffer.erase((++i).base());
	}
	else
	{
		//##DEBUG##
		std::wcout << "Failed to find matching line state change in RevokeSetLineState! " << GetLineName(targetLine) << '\t' << lineData.GetData() << '\t' << reportedTime << '\t' << accessTime << '\n';
	}

	//Update the lineAccessPending flag
	lineAccessPending = !lineAccessBuffer.empty();
}

//----------------------------------------------------------------------------------------
void Z80::ApplyLineStateChange(unsigned int targetLine, const Data& lineData, boost::mutex::scoped_lock& lock)
{
	//##DEBUG##
//	std::wstringstream message;
//	message << "Z80 line state change applied\t" << targetLine << '\t' << lineData.GetData() << "\n";
//	std::wcout << message.str();

	switch(targetLine)
	{
	case LINE_RESET:
		resetLineState = lineData.NonZero();
		break;
	case LINE_BUSREQ:{
		bool newState = lineData.NonZero();
		if(busreqLineState != newState)
		{
			busreqLineState = newState;

			//Release our lock on lineMutex. This is critical in order to avoid
			//deadlocks between devices if another device attempts to update the line
			//state for this device while we are updating the line state for that same
			//device, either directly or indirectly. There must never be a blocking
			//mutex held which would prevent a call to SetLineState on this device
			//succeeding when we are in tern calling SetLineState.
			lock.unlock();

			//If we're processing a change to the BUSREQ line, we need to now change the
			//state of the BUSACK line to match.
			memoryBus->SetLineState(LINE_BUSACK, lineData, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);

			//Re-acquire the lock now that we've completed our external call
			lock.lock();
		}
		break;}
	case LINE_INT:
		intLineState = lineData.NonZero();
		break;
	case LINE_NMI:
		nmiLineState = lineData.NonZero();
		break;
	}

	//Flag whether we want to suspend until another line state change is received, or we
	//reach the end of the current timeslice. We do this so that the Z80 doesn't advance
	//past the state of other devices in response to, for example, bus requests, when we
	//expect those events often to be brief. If the Z80 advances too far ahead, when the
	//bus is released for example, a rollback would need to be generated. This is an
	//optimization to try and avoid excessive rollbacks.
	suspendUntilLineStateChangeReceived = suspendWhenBusReleased && (resetLineState || busreqLineState);
}

//----------------------------------------------------------------------------------------
//Clock source functions
//----------------------------------------------------------------------------------------
unsigned int Z80::GetClockSourceID(const wchar_t* clockSourceName) const
{
	std::wstring lineNameString = clockSourceName;
	if(lineNameString == L"CLK")
	{
		return CLOCK_CLK;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
const wchar_t* Z80::GetClockSourceName(unsigned int clockSourceID) const
{
	switch(clockSourceID)
	{
	case CLOCK_CLK:
		return L"CLK";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
void Z80::SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//We push clock rate changes through the normal line state change tracking system
	//here, since line state changes and clock changes are basically the same problem.
	boost::mutex::scoped_lock lock(lineMutex);

	//Flag that an entry exists in the buffer. This flag is used to skip the expensive
	//locking operation in the active thread for this device when no line changes are
	//pending. Note that we set this flag before we've actually written the entry into
	//the buffer, as we want to force the active thread to lock on the beginning of the
	//next cycle while this function is executing, so that the current timeslice progress
	//of the device doesn't change after we've read it.
	lineAccessPending = true;

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(lastLineCheckTime > accessTime)
	{
		GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//Insert the line access into the buffer. Note that entries in the buffer are sorted
	//by access time from lowest to highest.
	std::list<LineAccess>::reverse_iterator i = lineAccessBuffer.rbegin();
	while((i != lineAccessBuffer.rend()) && (i->accessTime > accessTime))
	{
		++i;
	}
	lineAccessBuffer.insert(i.base(), LineAccess(clockInput, clockRate, accessTime));

	//Resume the main execution thread if it is currently suspended waiting for a line
	//state change to be received.
	if(suspendUntilLineStateChangeReceived)
	{
		GetDeviceContext()->ResumeTimesliceExecution();
	}
}

//----------------------------------------------------------------------------------------
void Z80::TransparentSetClockSourceRate(unsigned int clockInput, double clockRate)
{
	ApplyClockStateChange(clockInput, clockRate);
}

//----------------------------------------------------------------------------------------
void Z80::ApplyClockStateChange(unsigned int targetClock, double clockRate)
{
	//Apply the input clock rate change
	if(targetClock == CLOCK_CLK)
	{
		SetClockSpeed(clockRate);
	}
}

//----------------------------------------------------------------------------------------
//Instruction functions
//----------------------------------------------------------------------------------------
double Z80::ExecuteStep()
{
	unsigned int cyclesExecuted = 1;
	double additionalTime = 0;

	//If we have any pending line state changes waiting, apply any which we have now
	//reached.
	if(lineAccessPending)
	{
		//##DEBUG##
//		std::wcout << "Z80 line access pending\n";

		boost::mutex::scoped_lock lock(lineMutex);
		double currentTimesliceProgress = GetCurrentTimesliceProgress();
		bool done = false;
		while(!done)
		{
			//Remove the next line access that we've reached from the front of the line
			//access buffer. Note that our lineMutex lock may be released while applying
			//some line state changes, so we can't keep active reference to an iterator.
			std::list<LineAccess>::iterator i = lineAccessBuffer.begin();
			if((i == lineAccessBuffer.end()) || (i->accessTime > currentTimesliceProgress))
			{
				done = true;
				continue;
			}
			LineAccess lineAccess = *i;
			lineAccessBuffer.pop_front();
			lineAccessPending = !lineAccessBuffer.empty();

			//##DEBUG##
			//std::wstringstream logMessage;
			//logMessage << L"Z80 Line state change: " << std::setprecision(16) << lineaccess.accessTime << '\t' << currentTimesliceProgress << '\t' << lineAccess.lineID << '\t' << i->state.GetData();
			//LogEntry logEntry(LogEntry::EVENTLEVEL_DEBUG, logMessage.str());
			//GetDeviceContext()->WriteLogEvent(logEntry);
			//std::wcout << "Z80 Line state change: " << std::setprecision(16) << lineAccess.accessTime << '\t' << currentTimesliceProgress << '\n';

			//Apply the line state change
			if(lineAccess.clockRateChange)
			{
				ApplyClockStateChange(lineAccess.lineID, lineAccess.clockRate);
			}
			else
			{
				ApplyLineStateChange(lineAccess.lineID, lineAccess.state, lock);
			}
		}
	}
	lastLineCheckTime = GetCurrentTimesliceProgress();

	//If no line access is pending, and we've decided to suspend until another line state
	//change is received, suspend execution waiting for another line state change to be
	//received, unless execution suspension has now been disabled.
	if(!lineAccessPending && suspendUntilLineStateChangeReceived && !GetDeviceContext()->TimesliceSuspensionDisabled())
	{
		//Check lineAccessPending again after taking a lock on lineMutex. This will ensure
		//we never enter a suspend state when there are actually line access events
		//sitting in the buffer.
		boost::mutex::scoped_lock lock(lineMutex);
		if(!lineAccessPending)
		{
			//Suspend timeslice execution, release the lock on lineMutex, then wait until
			//execution is resumed. It is essential to perform these steps, in this order.
			//By waiting to release the lock until after we have suspended timeslice
			//execution, we ensure that no line state changes have sneaked into the buffer
			//since we tested the value of the lineAccessPending variable. We need to
			//release this lock before we enter our wait state however, so that other
			//devices can resume execution of this device by triggering a line state
			//change. After releasing the lock, we can now safely enter our wait state,
			//since a command to resume timeslice execution between the calls we make here
			//to suspend and wait for resume will be respected, and the wait call will
			//return immediately.
			GetDeviceContext()->SuspendTimesliceExecution();
			lock.unlock();
			GetDeviceContext()->WaitForTimesliceExecutionResume();
		}
	}

	//Apply any active effects from input lines to the processor
	bool processorNotExecuting = resetLineState || busreqLineState || !GetDeviceContext()->DeviceEnabled();
	if(processorNotExecuting)
	{
		if(resetLineState)
		{
			Reset();
		}
		return CalculateExecutionTime(cyclesExecuted) + additionalTime;
	}

	//If interrupts are not being suppressed for this opcode, and we've received either
	//a non-maskable interrupt, or a normal interrupt while interrupts are not being
	//masked, begin interrupt processing.
	//Note the maskInterruptsNextOpcode flag, which is used for interrupt suppression.
	//This is set in the EI instruction, and cleared immediately after we check for
	//interrupts here. This emulates the behaviour of the Z80 where interrupts are not
	//accepted until after the instruction immediately following an EI opcode. This is
	//necessary so that a maskable interrupt routine can call EI followed by RETI to
	//return from the handler, without another interrupt being triggered before RETI is
	//executed. We also use the maskInterruptsNextOpcode flag when we encounter
	//consecutive DD or FD prefix bytes during the decode process, where we use this flag
	//to mask interrupts until the end of the series is reached.
	if(!maskInterruptsNextOpcode && (nmiLineState || (intLineState && GetIFF1())))
	{
		//Clear the stopped state
		processorStopped = false;
		if(nmiLineState)
		{
			//Process a non-maskable interrupt
			//##NOTE## Contrary to official documentation, according to "The Undocumented
			//Z80 Documented" by Sean Young, IFF1 is NOT copied into IFF2 here. See
			//section 7.1, "Errors in official documentation".
			//Negate IFF1
			SetIFF1(false);
			//Write the return address to the stack
			EffectiveAddress sp;
			sp.SetMode(EffectiveAddress::MODE_SP_PREDEC);
			sp.Write(this, GetPC(), GetPC());
			//Update the debug call stack, and branch to the interrupt handler
			PushCallStack(GetPC().GetData(), 0x66, GetPC().GetData(), L"NMI", true);
			SetPC(Z80Word(0x66));

			cyclesExecuted = 11;
			AddRefresh(1);
			nmiLineState = false;
		}
		else if(intLineState)
		{
			//Process a maskable interrupt
			//Negate both IFF flags
			SetIFF1(false);
			SetIFF2(false);
			if(GetInterruptMode() == 0)
			{
				//##TODO## Implement this interrupt mode
				//##DEBUG##
				std::wcout << "Z80 Attempted interrupt mode 0\n";
				GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Z80 Attempted interrupt mode 0"));

				cyclesExecuted = 13;
			}
			else if(GetInterruptMode() == 1)
			{
				//##DEBUG##
				//std::wcout << "Z80 executed interrupt mode 1\n";

				//Write the return address to the stack
				EffectiveAddress sp;
				sp.SetMode(EffectiveAddress::MODE_SP_PREDEC);
				sp.Write(this, GetPC(), GetPC());
				//Update the debug call stack, and branch to the interrupt handler
				PushCallStack(GetPC().GetData(), 0x38, GetPC().GetData(), L"INT - mode 1", true);
				SetPC(Z80Word(0x38));

				cyclesExecuted = 13;
			}
			else if(GetInterruptMode() == 2)
			{
				//##TODO## Implement this interrupt mode
				//##DEBUG##
				std::wcout << "Z80 Attempted interrupt mode 2\n";
				GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"Z80 Attempted interrupt mode 2"));

				cyclesExecuted = 19;
			}

			//##FIX## What about the IORQ line? This line when asserted with the M1 line
			//indicates an interrupt acknowledge cycle, and we believe this needs to be
			//handled by the VDP in order to clear the Z80 INT line at the appropriate
			//time. Perhaps an overloaded INTAK line would be appropriate to define to
			//indicate this case. Both IORQ and M1 have other meanings.
			AddRefresh(1);
		}

		return CalculateExecutionTime(cyclesExecuted) + additionalTime;
	}
	maskInterruptsNextOpcode = false;

	//If the processor isn't stopped, fetch the next opcode
	if(!processorStopped)
	{
		//Update the trace log, and test for breakpoints
		RecordTrace(GetPC().GetData());
		CheckExecution(GetPC().GetData());

		cyclesExecuted = 0;
		bool mandatoryIndexOffset = false;
		Z80Byte indexOffset;
		const Z80Instruction* nextOpcodeType = 0;
		Z80Word readLocation = GetPC();
		Z80Word instructionLocation = GetPC();
		unsigned int instructionSize = 0;
		Z80Byte opcode;
		EffectiveAddress::IndexState indexState = EffectiveAddress::INDEX_NONE;

		//Read the first byte of the instruction
		additionalTime += ReadMemory(readLocation++, opcode, false);
		++instructionSize;
		//If the first byte is a prefix byte, process it, and read the second byte.
		if((opcode == 0xDD) || (opcode == 0xFD))
		{
			AddRefresh(1);
			if(opcode == 0xDD)
			{
				indexState = EffectiveAddress::INDEX_IX;
			}
			else if(opcode == 0xFD)
			{
				indexState = EffectiveAddress::INDEX_IY;
			}
			additionalTime += ReadMemory(readLocation++, opcode, false);
			++instructionSize;

			//Read the displacement value for the index register. Note that we have not
			//added it to the instruction size or incremented the read location. At this
			//point, we don't know if it's required or not yet. In most cases, this is
			//only determined during the decode function for the opcode.
			additionalTime += ReadMemory(readLocation, indexOffset, false);

			cyclesExecuted += 4;
		}
		//If we've encountered two prefix bytes back to back, force the opcode to a NOP
		//instruction.
		if((opcode == 0xDD) || (opcode == 0xFD))
		{
			opcode = 0;
			//We set this flag here to prevent interrupts being accepted when we start
			//the next cycle. According to "The Undocumented Z80 Documented", section
			//5.5, page 22, interrupts are not accepted during continuous blocks of
			//prefix bytes.
			maskInterruptsNextOpcode = true;
		}

		//Select the decode table to use, and extract the opcode
		if(opcode == 0xCB)
		{
			if(indexState != EffectiveAddress::INDEX_NONE)
			{
				//Decode a DDCB or an FDCB opcode. These opcodes include a mandatory index
				//displacement value as the third byte of the opcode.
				mandatoryIndexOffset = true;
				additionalTime += ReadMemory(readLocation++, indexOffset, false);
				++instructionSize;
			}
			else
			{
				//Decode the CB opcode
				//According to the undocumented z80 documented, section 6.1, this opcode
				//group increases the refresh register by 2, regardless of whether it was
				//prefixed or not. We emulate this here by only adding the refresh
				//increment for the non-prefixed version. In the case of the prefixed
				//versions, the prefix read will add the increment itself. The second
				//increment is always added for all opcodes later in this function.
				AddRefresh(1);
			}
			additionalTime += ReadMemory(readLocation++, opcode, false);
			++instructionSize;
			nextOpcodeType = opcodeTableCB.GetInstruction(opcode.GetData());
		}
		else if(opcode == 0xED)
		{
			//Override any DD or FD prefix. The DD and FD prefix bytes are ignored for ED
			//prefixed opcodes.
			indexState = EffectiveAddress::INDEX_NONE;

			AddRefresh(1);
			additionalTime += ReadMemory(readLocation++, opcode, false);
			++instructionSize;
			nextOpcodeType = opcodeTableED.GetInstruction(opcode.GetData());

			//If we've encountered an invalid ED prefixed opcode, force the opcode to a
			//NOP instruction.
			//##NOTE## We've only disabled this because the ED opcode table is currently
			//incomplete, and we want to be notified about attempts to use unimplemented
			//opcodes. This catch should be re-enabled when the ED opcode table has all
			//valid opcodes defined.
			//if(nextOpcode == 0)
			//{
			//	opcode = 0;
			//	nextOpcode = opcodeTable.GetInstruction(opcode.GetData());
			//}
		}
		else
		{
			nextOpcodeType = opcodeTable.GetInstruction(opcode.GetData());
		}
		AddRefresh(1);

		//Process the opcode
		if(nextOpcodeType != 0)
		{
			Z80Instruction* nextOpcode = 0;
//			nextOpcode = nextOpcode->Clone();
			nextOpcode = nextOpcodeType->ClonePlacement(opcodeBuffer);

			nextOpcode->SetInstructionSize(instructionSize);
			nextOpcode->SetInstructionLocation(instructionLocation);
			nextOpcode->SetInstructionRegister(opcode);
			nextOpcode->SetIndexState(indexState);
			nextOpcode->SetIndexOffset(indexOffset, mandatoryIndexOffset);
			nextOpcode->Z80Decode(this, nextOpcode->GetInstructionLocation(), nextOpcode->GetInstructionRegister(), nextOpcode->GetTransparentFlag());
			ExecuteTime opcodeExecuteTime = nextOpcode->Z80Execute(this, nextOpcode->GetInstructionLocation());
			cyclesExecuted += opcodeExecuteTime.cycles;
			additionalTime += opcodeExecuteTime.additionalTime;

			nextOpcode->~Z80Instruction();
//			delete nextOpcode;
		}
		else
		{
			//##TODO## Complete the Z80 opcode tables, and remove this catch.
			//##DEBUG##
			std::wcout << "Z80 Unemulated opcode " << opcode.GetData() << " at " << GetPC().GetData() << '\n';
			SetPC(GetPC() + instructionSize);
		}
	}

	return CalculateExecutionTime(cyclesExecuted) + additionalTime;
}

//----------------------------------------------------------------------------------------
Z80::OpcodeInfo Z80::GetOpcodeInfo(unsigned int location)
{
	OpcodeInfo opcodeInfo;
	opcodeInfo.valid = false;

	Z80Word instructionLocation = location;
	Z80Word readLocation = location;
	bool mandatoryIndexOffset = false;
	Z80Byte indexOffset;
	const Z80Instruction* nextOpcodeType = 0;
	unsigned int instructionSize = 0;
	Z80Byte opcode;
	EffectiveAddress::IndexState indexState = EffectiveAddress::INDEX_NONE;

	//Read the first byte of the instruction
	ReadMemory(readLocation++, opcode, true);
	++instructionSize;
	//If the first byte is a prefix byte, process it, and read the second byte.
	if((opcode == 0xDD) || (opcode == 0xFD))
	{
		if(opcode == 0xDD)
		{
			indexState = EffectiveAddress::INDEX_IX;
		}
		else if(opcode == 0xFD)
		{
			indexState = EffectiveAddress::INDEX_IY;
		}
		ReadMemory(readLocation++, opcode, true);
		++instructionSize;

		ReadMemory(readLocation, indexOffset, true);
	}
	//If we've encountered two prefix bytes back to back, force the opcode to a NOP
	//instruction.
	if((opcode == 0xDD) || (opcode == 0xFD))
	{
		opcode = 0;
	}

	//Select the decode table to use, and extract the opcode
	if(opcode == 0xCB)
	{
		if(indexState != EffectiveAddress::INDEX_NONE)
		{
			//Decode a DDCB or an FDCB opcode. These opcodes include a mandatory index
			//displacement value as the third byte of the opcode.
			mandatoryIndexOffset = true;
			ReadMemory(readLocation++, indexOffset, true);
			++instructionSize;
		}
		ReadMemory(readLocation++, opcode, true);
		++instructionSize;
		nextOpcodeType = opcodeTableCB.GetInstruction(opcode.GetData());
	}
	else if(opcode == 0xED)
	{
		//Override any DD or FD prefix. The DD and FD prefix bytes are ignored for ED
		//prefixed opcodes.
		indexState = EffectiveAddress::INDEX_NONE;

		ReadMemory(readLocation++, opcode, true);
		++instructionSize;
		nextOpcodeType = opcodeTableED.GetInstruction(opcode.GetData());
	}
	else
	{
		nextOpcodeType = opcodeTable.GetInstruction(opcode.GetData());
	}

	//Process the opcode
	if(nextOpcodeType != 0)
	{
		Z80Instruction* nextOpcode = nextOpcodeType->Clone();

		nextOpcode->SetTransparentFlag(true);
		nextOpcode->SetInstructionSize(instructionSize);
		nextOpcode->SetInstructionLocation(instructionLocation);
		nextOpcode->SetInstructionRegister(opcode);
		nextOpcode->SetIndexState(indexState);
		nextOpcode->SetIndexOffset(indexOffset, mandatoryIndexOffset);

		nextOpcode->Z80Decode(this, nextOpcode->GetInstructionLocation(), nextOpcode->GetInstructionRegister(), nextOpcode->GetTransparentFlag());
		Z80Instruction::Disassembly disassembly = nextOpcode->Z80Disassemble();

		opcodeInfo.valid = true;
		opcodeInfo.opcodeSize = nextOpcode->GetInstructionSize();
		opcodeInfo.disassemblyOpcode = disassembly.disassemblyOpcode;
		opcodeInfo.disassemblyArguments = disassembly.disassemblyArguments;
		opcodeInfo.disassemblyComment = disassembly.disassemblyComment;

		delete nextOpcode;
	}

	return opcodeInfo;
}

//----------------------------------------------------------------------------------------
Data Z80::GetRawData(unsigned int location)
{
	Z80Byte data;
	ReadMemory(location, data, true);
	return data;
}

//----------------------------------------------------------------------------------------
unsigned int Z80::GetCurrentPC() const
{
	return GetPC().GetData();
}

//----------------------------------------------------------------------------------------
unsigned int Z80::GetPCWidth() const
{
	return 16;
}

//----------------------------------------------------------------------------------------
unsigned int Z80::GetAddressBusWidth() const
{
	return 16;
}

//----------------------------------------------------------------------------------------
unsigned int Z80::GetDataBusWidth() const
{
	return 8;
}

//----------------------------------------------------------------------------------------
unsigned int Z80::GetMinimumOpcodeByteSize() const
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Memory access functions
//----------------------------------------------------------------------------------------
double Z80::ReadMemory(const Z80Word& location, Data& data, bool transparent) const
{
	IBusInterface::AccessResult result;

	if(!transparent)
	{
		CheckMemoryRead(location.GetData(), data.GetData());
	}

	switch(data.GetBitCount())
	{
	case BITCOUNT_BYTE:{
		Z80Byte temp;
		if(transparent)
		{
			PerformanceLock lock(ceLineStateMutex);
			memoryAccessTransparentRD = true;
			memoryAccessTransparentWR = false;
			memoryBus->TransparentReadMemory(location.GetData(), temp, GetDeviceContext(), 0);
		}
		else
		{
			memoryAccessRD = true;
			memoryAccessWR = false;
			result = memoryBus->ReadMemory(location.GetData(), temp, GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		}
		data = temp;
		break;}
	case BITCOUNT_WORD:{
		Z80Byte byteLow;
		Z80Byte byteHigh;
		if(transparent)
		{
			PerformanceLock lock(ceLineStateMutex);
			memoryAccessTransparentRD = true;
			memoryAccessTransparentWR = false;
			memoryBus->TransparentReadMemory(location.GetData(), byteLow, GetDeviceContext(), 0);
			memoryBus->TransparentReadMemory((location + 1).GetData(), byteHigh, GetDeviceContext(), 0);
		}
		else
		{
			IBusInterface::AccessResult result2;
			memoryAccessRD = true;
			memoryAccessWR = false;
			result = memoryBus->ReadMemory(location.GetData(), byteLow, GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
			result2 = memoryBus->ReadMemory((location + 1).GetData(), byteHigh, GetDeviceContext(), GetCurrentTimesliceProgress() + result.executionTime, 0);
			result.executionTime += result2.executionTime;
		}
		data.SetLowerBits(byteLow);
		data.SetUpperBits(byteHigh);
		break;}
	}

	return result.executionTime;
}

//----------------------------------------------------------------------------------------
double Z80::WriteMemory(const Z80Word& location, const Data& data, bool transparent) const
{
	IBusInterface::AccessResult result;

	if(!transparent)
	{
		CheckMemoryWrite(location.GetData(), data.GetData());
	}

	switch(data.GetBitCount())
	{
	case BITCOUNT_BYTE:{
		if(transparent)
		{
			PerformanceLock lock(ceLineStateMutex);
			memoryAccessTransparentRD = false;
			memoryAccessTransparentWR = true;
			memoryBus->TransparentWriteMemory(location.GetData(), data, GetDeviceContext(), 0);
		}
		else
		{
			memoryAccessRD = false;
			memoryAccessWR = true;
			result = memoryBus->WriteMemory(location.GetData(), data, GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		}
		break;}
	case BITCOUNT_WORD:{
		if(transparent)
		{
			PerformanceLock lock(ceLineStateMutex);
			Z80Byte byte1(data.GetLowerBits(BITCOUNT_BYTE));
			Z80Byte byte2(data.GetUpperBits(BITCOUNT_BYTE));
			memoryAccessTransparentRD = false;
			memoryAccessTransparentWR = true;
			memoryBus->TransparentWriteMemory(location.GetData(), byte1, GetDeviceContext(), 0);
			memoryBus->TransparentWriteMemory((location + 1).GetData(), byte2, GetDeviceContext(), 0);
		}
		else
		{
			IBusInterface::AccessResult result2;
			Z80Byte byte1(data.GetLowerBits(BITCOUNT_BYTE));
			Z80Byte byte2(data.GetUpperBits(BITCOUNT_BYTE));
			memoryAccessRD = false;
			memoryAccessWR = true;
			result = memoryBus->WriteMemory(location.GetData(), byte1, GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
			result2 = memoryBus->WriteMemory((location + 1).GetData(), byte2, GetDeviceContext(), GetCurrentTimesliceProgress() + result.executionTime, 0);
			result.executionTime += result2.executionTime;
		}
		break;}
	}

	return result.executionTime;
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int Z80::GetCELineID(const wchar_t* lineName, bool inputLine) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"RD")
	{
		return CELINE_RD;
	}
	else if(lineNameString == L"WR")
	{
		return CELINE_WR;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void Z80::SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch(lineID)
	{
	case CELINE_RD:
		ceLineMaskRD = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_WR:
		ceLineMaskWR = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
unsigned int Z80::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, double accessTime) const
{
	unsigned int ceLineState = 0;
	if(caller == this->GetDeviceContext())
	{
		ceLineState |= memoryAccessRD? ceLineMaskRD: 0x0;
		ceLineState |= memoryAccessWR? ceLineMaskWR: 0x0;
	}
	return ceLineState;
}

//----------------------------------------------------------------------------------------
unsigned int Z80::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller) const
{
	unsigned int ceLineState = 0;
	if(caller == this->GetDeviceContext())
	{
		ceLineState |= memoryAccessTransparentRD? ceLineMaskRD: 0x0;
		ceLineState |= memoryAccessTransparentWR? ceLineMaskWR: 0x0;
	}
	return ceLineState;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void Z80::LoadState(IHeirarchicalStorageNode& node)
{
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"Register")
		{
			IHeirarchicalStorageAttribute* nameAttribute = (*i)->GetAttribute(L"name");
			if(nameAttribute != 0)
			{
				std::wstring registerName = nameAttribute->GetValue();
				if(registerName == L"AF")		afreg = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"BC")	bcreg = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"DE")	dereg = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"HL")	hlreg = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"AF2")	af2reg = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"BC2")	bc2reg = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"DE2")	de2reg = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"HL2")	hl2reg = (*i)->ExtractHexData<unsigned int>();

				else if(registerName == L"I")	ireg = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"R")	rreg = (*i)->ExtractHexData<unsigned int>();

				else if(registerName == L"IX")	ixreg = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"IY")	iyreg = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"SP")	spreg = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"PC")	pcreg = (*i)->ExtractHexData<unsigned int>();

				else if(registerName == L"InterruptMode")	interruptMode = (*i)->ExtractData<unsigned int>();
				else if(registerName == L"IFF1")			iff1 = (*i)->ExtractData<bool>();
				else if(registerName == L"IFF2")			iff2 = (*i)->ExtractData<bool>();
				else if(registerName == L"MaskInterruptsNextOpcode")	maskInterruptsNextOpcode = (*i)->ExtractData<bool>();
				else if(registerName == L"ProcessorStopped")		processorStopped = (*i)->ExtractData<bool>();

				else if(registerName == L"LastTimesliceLength")		lastTimesliceLength = (*i)->ExtractData<double>();
				else if(registerName == L"SuspendUntilLineStateChangeReceived")		suspendUntilLineStateChangeReceived = (*i)->ExtractData<bool>();
				else if(registerName == L"ResetLineState")			resetLineState = (*i)->ExtractData<bool>();
				else if(registerName == L"BusreqLineState")			busreqLineState = (*i)->ExtractData<bool>();
				else if(registerName == L"INTLineState")			intLineState = (*i)->ExtractData<bool>();
				else if(registerName == L"NMILineState")			nmiLineState = (*i)->ExtractData<bool>();
			}
		}
		//Restore the lineAccessBuffer state
		else if((*i)->GetName() == L"LineAccessBuffer")
		{
			IHeirarchicalStorageNode& lineAccessBufferNode = *(*i);
			std::list<IHeirarchicalStorageNode*> lineAccessBufferChildList = lineAccessBufferNode.GetChildList();
			for(std::list<IHeirarchicalStorageNode*>::iterator lineAccessBufferEntry = lineAccessBufferChildList.begin(); lineAccessBufferEntry != lineAccessBufferChildList.end(); ++lineAccessBufferEntry)
			{
				if((*lineAccessBufferEntry)->GetName() == L"LineAccess")
				{
					IHeirarchicalStorageAttribute* lineNameAttribute = (*lineAccessBufferEntry)->GetAttribute(L"LineName");
					IHeirarchicalStorageAttribute* clockRateChangeAttribute = (*lineAccessBufferEntry)->GetAttribute(L"ClockRateChange");
					IHeirarchicalStorageAttribute* accessTimeAttribute = (*lineAccessBufferEntry)->GetAttribute(L"AccessTime");
					if((lineNameAttribute != 0) && (clockRateChangeAttribute != 0) && (accessTimeAttribute != 0))
					{
						//Extract the entry from the XML stream
						LineAccess lineAccess(0, 0.0, 0.0);
						bool lineAccessDefined = false;
						std::wstring lineName = lineNameAttribute->ExtractValue<std::wstring>();
						bool clockRateChange = clockRateChangeAttribute->ExtractValue<bool>();
						double accessTime = accessTimeAttribute->ExtractValue<double>();
						if(clockRateChange)
						{
							IHeirarchicalStorageAttribute* clockRateAttribute = (*lineAccessBufferEntry)->GetAttribute(L"ClockRate");
							if(clockRateAttribute != 0)
							{
								unsigned int lineID = GetClockSourceID(lineName.c_str());
								if(lineID != 0)
								{
									double clockRate;
									clockRateAttribute->ExtractValue(clockRate);
									lineAccess = LineAccess(lineID, clockRate, accessTime);
									lineAccessDefined = true;
								}
							}
						}
						else
						{
							IHeirarchicalStorageAttribute* lineStateAttribute = (*lineAccessBufferEntry)->GetAttribute(L"LineState");
							if(lineStateAttribute != 0)
							{
								unsigned int lineID = GetLineID(lineName.c_str());
								if(lineID != 0)
								{
									Data lineState(GetLineWidth(lineID));
									lineStateAttribute->ExtractValue(lineState);
									lineAccess = LineAccess(lineID, lineState, accessTime);
									lineAccessDefined = true;
								}
							}
						}

						//Find the correct location in the list to insert the entry. The
						//list must be sorted from earliest to latest.
						if(lineAccessDefined)
						{
							std::list<LineAccess>::reverse_iterator j = lineAccessBuffer.rbegin();
							while((j != lineAccessBuffer.rend()) && (j->accessTime > lineAccess.accessTime))
							{
								++j;
							}
							lineAccessBuffer.insert(j.base(), lineAccess);
						}
					}
				}
			}
			lineAccessPending = !lineAccessBuffer.empty();
		}
	}

	Processor::LoadState(node);
}

//----------------------------------------------------------------------------------------
void Z80::SaveState(IHeirarchicalStorageNode& node) const
{
	node.CreateChildHex(L"Register", afreg.GetData(), afreg.GetHexCharCount()).CreateAttribute(L"name", L"AF");
	node.CreateChildHex(L"Register", bcreg.GetData(), bcreg.GetHexCharCount()).CreateAttribute(L"name", L"BC");
	node.CreateChildHex(L"Register", dereg.GetData(), dereg.GetHexCharCount()).CreateAttribute(L"name", L"DE");
	node.CreateChildHex(L"Register", hlreg.GetData(), hlreg.GetHexCharCount()).CreateAttribute(L"name", L"HL");
	node.CreateChildHex(L"Register", af2reg.GetData(), af2reg.GetHexCharCount()).CreateAttribute(L"name", L"AF2");
	node.CreateChildHex(L"Register", bc2reg.GetData(), bc2reg.GetHexCharCount()).CreateAttribute(L"name", L"BC2");
	node.CreateChildHex(L"Register", de2reg.GetData(), de2reg.GetHexCharCount()).CreateAttribute(L"name", L"DE2");
	node.CreateChildHex(L"Register", hl2reg.GetData(), hl2reg.GetHexCharCount()).CreateAttribute(L"name", L"HL2");

	node.CreateChildHex(L"Register", ireg.GetData(), ireg.GetHexCharCount()).CreateAttribute(L"name", L"I");
	node.CreateChildHex(L"Register", rreg.GetData(), rreg.GetHexCharCount()).CreateAttribute(L"name", L"R");

	node.CreateChildHex(L"Register", ixreg.GetData(), ixreg.GetHexCharCount()).CreateAttribute(L"name", L"IX");
	node.CreateChildHex(L"Register", iyreg.GetData(), iyreg.GetHexCharCount()).CreateAttribute(L"name", L"IY");
	node.CreateChildHex(L"Register", spreg.GetData(), spreg.GetHexCharCount()).CreateAttribute(L"name", L"SP");
	node.CreateChildHex(L"Register", pcreg.GetData(), pcreg.GetHexCharCount()).CreateAttribute(L"name", L"PC");

	node.CreateChild(L"Register", interruptMode).CreateAttribute(L"name", L"InterruptMode");
	node.CreateChild(L"Register", iff1).CreateAttribute(L"name", L"IFF1");
	node.CreateChild(L"Register", iff2).CreateAttribute(L"name", L"IFF2");
	node.CreateChild(L"Register", maskInterruptsNextOpcode).CreateAttribute(L"name", L"MaskInterruptsNextOpcode");
	node.CreateChild(L"Register", processorStopped).CreateAttribute(L"name", L"ProcessorStopped");

	node.CreateChild(L"Register", lastTimesliceLength).CreateAttribute(L"name", L"LastTimesliceLength");
	node.CreateChild(L"Register", suspendUntilLineStateChangeReceived).CreateAttribute(L"name", L"SuspendUntilLineStateChangeReceived");
	node.CreateChild(L"Register", resetLineState).CreateAttribute(L"name", L"ResetLineState");
	node.CreateChild(L"Register", busreqLineState).CreateAttribute(L"name", L"BusreqLineState");
	node.CreateChild(L"Register", intLineState).CreateAttribute(L"name", L"INTLineState");
	node.CreateChild(L"Register", nmiLineState).CreateAttribute(L"name", L"NMILineState");

	//Save the lineAccessBuffer state
	if(lineAccessPending)
	{
		IHeirarchicalStorageNode& lineAccessState = node.CreateChild(L"LineAccessBuffer");
		for(std::list<LineAccess>::const_iterator i = lineAccessBuffer.begin(); i != lineAccessBuffer.end(); ++i)
		{
			IHeirarchicalStorageNode& lineAccessEntry = lineAccessState.CreateChild(L"LineAccess");
			lineAccessEntry.CreateAttribute(L"ClockRateChange", i->clockRateChange);
			if(i->clockRateChange)
			{
				lineAccessEntry.CreateAttribute(L"LineName", GetClockSourceName(i->lineID));
				lineAccessEntry.CreateAttribute(L"ClockRate", i->clockRate);
			}
			else
			{
				lineAccessEntry.CreateAttribute(L"LineName", GetLineName(i->lineID));
				lineAccessEntry.CreateAttribute(L"LineState", i->state);
			}
			lineAccessEntry.CreateAttribute(L"AccessTime", i->accessTime);
		}
	}

	Processor::SaveState(node);
}

} //Close namespace Z80
