#include "M68000.h"
#include "M68000Opcodes.pkg"
#include "Debug/Debug.pkg"
#include "DebugMenuHandler.h"
#include <iostream>
#include <iomanip>
#include <sstream>
namespace M68000 {

//----------------------------------------------------------------------------------------
M68000::M68000(const std::wstring& ainstanceName, unsigned int amoduleID)
:Processor(L"M68000", ainstanceName, amoduleID), opcodeTable(16), opcodeBuffer(0), memoryBus(0)
{
	//Create the menu handler
	menuHandler = new DebugMenuHandler(this);
	menuHandler->LoadMenuItems();

	//Initialize our CE line state
	ceLineMaskLowerDataStrobe = 0;
	ceLineMaskUpperDataStrobe = 0;
	ceLineMaskReadHighWriteLow = 0;
	ceLineMaskAddressStrobe = 0;
	ceLineMaskFunctionCode = 0;
	ceLineBitNumberFunctionCode = 0;
	ceLineMaskFCCPUSpace = 0;
	ceLineMaskRMWCycleInProgress = 0;
	ceLineMaskRMWCycleFirstOperation = 0;

	//Initialize our debugger state
	exceptionListEmpty = true;
	logAllExceptions = false;
	breakOnAllExceptions = false;
	disableAllExceptions = false;
	debugExceptionTriggerPending = false;
}

//----------------------------------------------------------------------------------------
M68000::~M68000()
{
	//Delete the opcode buffer
	delete opcodeBuffer;

	//Delete all objects stored in the opcode list
	for(std::list<M68000Instruction*>::const_iterator i = opcodeList.begin(); i != opcodeList.end(); ++i)
	{
		delete *i;
	}

	//Delete any remaining exception debugging objects
	for(ExceptionList::iterator i = exceptionList.begin(); i != exceptionList.end(); ++i)
	{
		delete *i;
	}
	exceptionList.clear();
	exceptionListEmpty = true;

	//Delete the menu handler
	menuHandler->ClearMenuItems();
	delete menuHandler;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool M68000::BuildDevice()
{
	bool result = true;

	//Initialize our opcode table
	opcodeTable.InitializeOpcodeTable();

	//Delete all objects stored in the opcode list
	for(std::list<M68000Instruction*>::const_iterator i = opcodeList.begin(); i != opcodeList.end(); ++i)
	{
		delete *i;
	}
	opcodeList.clear();

	//Add all defined opcodes for this device to the opcode list

	//Arithmetic instructions
	opcodeList.push_back(new ADD());
	opcodeList.push_back(new ADDA());
	opcodeList.push_back(new ADDI());
	opcodeList.push_back(new ADDQ());
	opcodeList.push_back(new ADDX());
	opcodeList.push_back(new CLR());
	opcodeList.push_back(new CMP());
	opcodeList.push_back(new CMPA());
	opcodeList.push_back(new CMPI());
	opcodeList.push_back(new CMPM());
	opcodeList.push_back(new DIVS());
	opcodeList.push_back(new DIVU());
	opcodeList.push_back(new EXT());
	opcodeList.push_back(new MULS());
	opcodeList.push_back(new MULU());
	opcodeList.push_back(new NEG());
	opcodeList.push_back(new NEGX());
	opcodeList.push_back(new NOP());
	opcodeList.push_back(new SUB());
	opcodeList.push_back(new SUBA());
	opcodeList.push_back(new SUBI());
	opcodeList.push_back(new SUBQ());
	opcodeList.push_back(new SUBX());

	//BCD instructions
	opcodeList.push_back(new ABCD());
	opcodeList.push_back(new NBCD());
	opcodeList.push_back(new SBCD());

	//Logic instructions
	opcodeList.push_back(new AND());
	opcodeList.push_back(new ANDI());
	opcodeList.push_back(new EOR());
	opcodeList.push_back(new EORI());
	opcodeList.push_back(new NOT());
	opcodeList.push_back(new OR());
	opcodeList.push_back(new ORI());
	opcodeList.push_back(new Scc());
	opcodeList.push_back(new TST());

	//Shift and Rotate instructions
	opcodeList.push_back(new ASL());
	opcodeList.push_back(new ASR());
	opcodeList.push_back(new LSL());
	opcodeList.push_back(new LSR());
	opcodeList.push_back(new ROL());
	opcodeList.push_back(new ROR());
	opcodeList.push_back(new ROXL());
	opcodeList.push_back(new ROXR());
	opcodeList.push_back(new SWAP());

	//Bit Manipulation Instructions
	opcodeList.push_back(new BCHG());
	opcodeList.push_back(new BCLR());
	opcodeList.push_back(new BSET());
	opcodeList.push_back(new BTST());

	//Data Transfer instructions
	opcodeList.push_back(new EXG());
	opcodeList.push_back(new LEA());
	opcodeList.push_back(new LINK());
	opcodeList.push_back(new MOVE());
	opcodeList.push_back(new MOVEA());
	opcodeList.push_back(new MOVEM());
	opcodeList.push_back(new MOVEQ());
	opcodeList.push_back(new MOVEP());
	opcodeList.push_back(new PEA());
	opcodeList.push_back(new UNLK());

	//Flow Control instructions
	opcodeList.push_back(new Bcc());
	opcodeList.push_back(new BRA());
	opcodeList.push_back(new BSR());
	opcodeList.push_back(new DBcc());
	opcodeList.push_back(new JMP());
	opcodeList.push_back(new JSR());
	opcodeList.push_back(new RTR());
	opcodeList.push_back(new RTS());

	//CCR Related instructions
	opcodeList.push_back(new ANDI_to_CCR());
	opcodeList.push_back(new EORI_to_CCR());
	opcodeList.push_back(new MOVE_to_CCR());
	opcodeList.push_back(new ORI_to_CCR());

	//Exception instructions
	opcodeList.push_back(new CHK());
	opcodeList.push_back(new ILLEGAL());
	opcodeList.push_back(new TRAP());
	opcodeList.push_back(new TRAPV());

	//Multiprocessor instructions
	opcodeList.push_back(new TAS());

	//Privileged instructions
	opcodeList.push_back(new ANDI_to_SR());
	opcodeList.push_back(new EORI_to_SR());
	opcodeList.push_back(new MOVE_from_SR());
	opcodeList.push_back(new MOVE_to_SR());
	opcodeList.push_back(new MOVE_USP());
	opcodeList.push_back(new ORI_to_SR());
	opcodeList.push_back(new RESET());
	opcodeList.push_back(new RTE());
	opcodeList.push_back(new STOP());

	//Register each constructed opcode object in the opcode table, and calculate the size
	//of the largest opcode object.
	size_t largestObjectSize = 0;
	for(std::list<M68000Instruction*>::const_iterator i = opcodeList.begin(); i != opcodeList.end(); ++i)
	{
		//Register this opcode in the opcode table
		M68000Instruction* opcodeObject = *i;
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

	//Allocate a new opcode buffer, which is large enough to hold an instance of the
	//largest opcode object.
	opcodeBuffer = (void*)new unsigned char[largestObjectSize];

	//##TODO## Remove this debug code, and make a proper interface for controlling active
	//disassembly.
	//##DEBUG## Enable active disassembly in the ROM region for the MegaDrive
	//EnableActiveDisassembly(0x0, 0x400000);

	return result;
}

//----------------------------------------------------------------------------------------
bool M68000::ValidateDevice()
{
	return (memoryBus != 0);
}

//----------------------------------------------------------------------------------------
void M68000::Initialize()
{
	for(int i = 0; i < (addressRegCount - 1); ++i)
	{
		a[i] = 0;
	}
	for(int i = 0; i < dataRegCount; ++i)
	{
		d[i] = 0;
	}
	pc = 0;
	sr = 0;
	ssp = 0;
	usp = 0;
	wordIsPrefetched = false;

	//Abandon currently pending interrupts, and restore normal processor state
	interruptPendingLevel = 0;
	lineAccessPending = false;
	lastTimesliceLength = 0;
	blastTimesliceLength = 0;
	lineAccessBuffer.clear();
	resetLineState = false;
	haltLineState = false;
	brLineState = false;
	bgLineState = false;
	forceInterrupt = false;
	processorState = STATE_NORMAL;
	lastReadBusData = 0;

	//Trigger a reset exception to start execution
	Reset();
}

//----------------------------------------------------------------------------------------
void M68000::Reset()
{
	//Queue the reset exception for the next cycle
	group0vector = EX_RESET;
	group0ExceptionPending = true;
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool M68000::AddReference(const wchar_t* referenceName, IBusInterface* target)
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
bool M68000::RemoveReference(IBusInterface* target)
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
//Line functions
//----------------------------------------------------------------------------------------
unsigned int M68000::GetLineID(const wchar_t* lineName) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"RESET") //IO
	{
		return LINE_RESET;
	}
	else if(lineNameString == L"BR") //I
	{
		return LINE_BR;
	}
	else if(lineNameString == L"BG") //O
	{
		return LINE_BG;
	}
	else if(lineNameString == L"HALT") //I
	{
		return LINE_HALT;
	}
	else if(lineNameString == L"IPL") //I
	{
		return LINE_IPL;
	}
	else if(lineNameString == L"VPA") //I
	{
		return LINE_VPA;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
const wchar_t* M68000::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_RESET:
		return L"RESET";
	case LINE_BR:
		return L"BR";
	case LINE_BG:
		return L"BG";
	case LINE_HALT:
		return L"HALT";
	case LINE_IPL:
		return L"IPL";
	case LINE_VPA:
		return L"VPA";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int M68000::GetLineWidth(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_RESET:
		return 1;
	case LINE_BR:
		return 1;
	case LINE_BG:
		return 1;
	case LINE_HALT:
		return 1;
	case LINE_IPL:
		return 3;
	case LINE_VPA:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void M68000::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime)
{
	//##DEBUG##
//	std::wcout << "M68000SetLineState\t" << targetLine << '\n';

	//Check if this is a line which we need to process immediately. This would generally
	//indicate a response to a request which this device is currently asserting.
	switch(targetLine)
	{
	case LINE_VPA:
		autoVectorPendingInterrupt = true;
		return;
	case LINE_BR:
		//If the M68000 currently doesn't have bus ownership, and an external device is
		//releasing the bus, reclaim bus ownership.
		if(bgLineState)
		{
			if(lineData.Zero())
			{
				brLineState = false;
				bgLineState = false;
				memoryBus->SetLine(LINE_BG, Data(GetLineWidth(LINE_BG), (unsigned int)bgLineState), GetDeviceContext(), GetDeviceContext(), accessTime);
			}
			return;
		}
		//If the M68000 currently has the bus, we leave the bus request pending until the
		//M68000 reaches a point where it can release the bus.
		break;
	}

	//If this line state change is not a response to a request, we need to obtain an
	//exclusive lock.
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
		GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime);
	}

	//Insert the line access into the buffer. Note that entries in the buffer are sorted
	//by access time from lowest to highest.
	std::list<LineAccess>::reverse_iterator i = lineAccessBuffer.rbegin();
	while((i != lineAccessBuffer.rend()) && (i->accessTime > accessTime))
	{
		++i;
	}
	lineAccessBuffer.insert(i.base(), LineAccess(targetLine, lineData, accessTime));
}

//----------------------------------------------------------------------------------------
void M68000::ApplyLineStateChange(unsigned int targetLine, const Data& lineData)
{
//##DEBUG##
//	std::wstringstream message;
//	message << "M68000 line state change applied\t" << targetLine << '\t' << lineData.GetData() << "\n";
//	std::wcout << message.str();

	switch(targetLine)
	{
	case LINE_RESET:
		resetLineState = !lineData.Zero();
		break;
	case LINE_BR:
		brLineState = !lineData.Zero();
		if(brLineState)
		{
			//If the BR line has been asserted, grant bus ownership to the external device
			//requesting the bus. If the BR line has been negated, reclaim bus ownership.
			bgLineState = true;
			memoryBus->SetLine(LINE_BG, Data(GetLineWidth(LINE_BG), (unsigned int)bgLineState), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress());
		}
		break;
	case LINE_HALT:
		haltLineState = !lineData.Zero();
		break;
	case LINE_IPL:{
		//Calculate the new level of the pending interrupt
		unsigned int newInterruptPendingLevel = lineData.GetData();

		//If IPL has just switched from some lower level to indicate a level 7 interrupt,
		//we set a flag here to allow us to force the interrupt to be taken. As described
		//in The M68000 Users Manual, section 6.3.2, "An interrupt is generated each time
		//the interrupt request level changes from some lower level to level 7". We
		//detect this case here. This is the only special case handling that exists for
		//level 7 interrupts.
		forceInterrupt = ((interruptPendingLevel != 7) && (newInterruptPendingLevel == 7));

		//Update the latched IPL to match the new state
		interruptPendingLevel = newInterruptPendingLevel;

		break;}
	}
}

//----------------------------------------------------------------------------------------
//Exception functions
//----------------------------------------------------------------------------------------
void M68000::PushStackFrame(const M68000Long& apc, const M68000Word& asr, bool processingInstruction)
{
	//Push Group 1/2 exception stack frame
	M68000Long stackPointer = GetSSP();
	stackPointer -= apc.GetByteSize();
	WriteMemory(stackPointer, apc, FUNCTIONCODE_SUPERVISORDATA, apc, processingInstruction, 0, false, false);
	stackPointer -= asr.GetByteSize();
	WriteMemory(stackPointer, asr, FUNCTIONCODE_SUPERVISORDATA, apc, processingInstruction, 0, false, false);
	SetSSP(stackPointer);
}

//----------------------------------------------------------------------------------------
void M68000::PushStackFrame(const M68000Long& apc, const M68000Word& asr, const M68000Word& ainstructionRegister, const M68000Long& aaccessAddress, bool aread, bool aprocessingInstruction, FunctionCode afunctionCode)
{
	//Push Group 0 Bus/Address error exception stack frame
	M68000Word statusFlags = afunctionCode;
	statusFlags.SetDataSegment(0, 3, afunctionCode);
	statusFlags.SetBit(3, !aprocessingInstruction);
	statusFlags.SetBit(4, aread);

	M68000Long stackPointer = GetSSP();
	stackPointer -= apc.GetByteSize();
	WriteMemory(stackPointer, apc, FUNCTIONCODE_SUPERVISORDATA, apc, false, 0, false, false);
	stackPointer -= asr.GetByteSize();
	WriteMemory(stackPointer, asr, FUNCTIONCODE_SUPERVISORDATA, apc, false, 0, false, false);
	stackPointer -= ainstructionRegister.GetByteSize();
	WriteMemory(stackPointer, ainstructionRegister, FUNCTIONCODE_SUPERVISORDATA, apc, false, 0, false, false);
	stackPointer -= aaccessAddress.GetByteSize();
	WriteMemory(stackPointer, aaccessAddress, FUNCTIONCODE_SUPERVISORDATA, apc, false, 0, false, false);
	stackPointer -= statusFlags.GetByteSize();
	WriteMemory(stackPointer, statusFlags, FUNCTIONCODE_SUPERVISORDATA, apc, false, 0, false, false);
	SetSSP(stackPointer);
}

//----------------------------------------------------------------------------------------
ExecuteTime M68000::ProcessException(unsigned int vector)
{
	//Log or break on this exception if requested
	ExceptionLogIfRequested(vector);
	ExceptionBreakIfRequested(vector);

	SetSR_S(true);
	SetSR_T(false);
	M68000Long interruptVector;
	SetProcessorState(STATE_EXCEPTION);

	//##DEBUG##
//	std::wcout << "Interrupt\t" << vector << '\n';

	unsigned int vectorOffset = vector * interruptVector.GetByteSize();
	//According to the M68000 Users Manual, section 6.3.9, the PC is loaded with the
	//address of the exception vector while reading the vector from the vector table. We
	//emulate this behaviour here, to ensure the contents of the PC are correct when they
	//are pushed to the stack, if an address or bus error occurs during exception
	//processing.
	M68000Long oldPC = GetPC();
	SetPC(M68000Long(vectorOffset));
	if(vector == EX_RESET)
	{
		ClearCallStack();
		SetSR_IPM(7);
		M68000Long newSSP;
		ReadMemory(vectorOffset, newSSP, FUNCTIONCODE_SUPERVISORPROGRAM, 0, false, 0, false, false);
		SetA(SP, newSSP);
		ReadMemory(vectorOffset + newSSP.GetByteSize(), interruptVector, FUNCTIONCODE_SUPERVISORPROGRAM, 0, false, 0, false, false);
	}
	else
	{
		ReadMemory(vectorOffset, interruptVector, FUNCTIONCODE_SUPERVISORDATA, 0, false, 0, false, false);
	}

	PushCallStack(oldPC.GetData(), interruptVector.GetData(), oldPC.GetData(), L"Exception", true);
	SetPC(interruptVector);
	SetProcessorState(STATE_NORMAL);
	return GetExceptionProcessingTime(vector);
}

//----------------------------------------------------------------------------------------
ExecuteTime M68000::GetExceptionProcessingTime(unsigned int vector) const
{
	//These figures largely come from the M68000 Users Manual, section 8, page 11. We've
	//tweaked a couple of the numbers however. The users manual doesn't specify whether
	//the execution times listed for group 2 exceptions include the instruction execution
	//times. In the case of several instructions such as the trap instructions, it seems
	//the instruction execution times may be included. We've used several other references
	//and a few educated guesses to adjust some of the figures. There's a lot of dispute
	//between most sources about exactly how many clock cycles are required for each
	//exception, but these numbers should be pretty close.
	switch(vector)
	{
	case EX_ADDRESS_ERROR:
		return ExecuteTime(50, 4, 7);
	case EX_BUS_ERROR:
		return ExecuteTime(50, 4, 7);
	case EX_CHK_INSTRUCTION:
		return ExecuteTime(40, 4, 3);
	case EX_ZERO_DIVIDE:
		return ExecuteTime(38, 4, 3);
	case EX_LINE_1010:
	case EX_LINE_1111:
	case EX_ILLEGAL_INSTRUCTION:
		return ExecuteTime(34, 4, 3);
	case EX_PRIVILEGE_VIOLATION:
		return ExecuteTime(34, 4, 3);
	case EX_RESET:
		return ExecuteTime(40, 6, 0);
	case EX_TRACE:
		return ExecuteTime(34, 4, 3);
	case EX_INTERRUPT_TRAP + 0x0:
	case EX_INTERRUPT_TRAP + 0x1:
	case EX_INTERRUPT_TRAP + 0x2:
	case EX_INTERRUPT_TRAP + 0x3:
	case EX_INTERRUPT_TRAP + 0x4:
	case EX_INTERRUPT_TRAP + 0x5:
	case EX_INTERRUPT_TRAP + 0x6:
	case EX_INTERRUPT_TRAP + 0x7:
	case EX_INTERRUPT_TRAP + 0x8:
	case EX_INTERRUPT_TRAP + 0x9:
	case EX_INTERRUPT_TRAP + 0xA:
	case EX_INTERRUPT_TRAP + 0xB:
	case EX_INTERRUPT_TRAP + 0xC:
	case EX_INTERRUPT_TRAP + 0xD:
	case EX_INTERRUPT_TRAP + 0xE:
	case EX_INTERRUPT_TRAP + 0xF:
		return ExecuteTime(34, 4, 3);
	case EX_TRAPV_INSTRUCTION:
		return ExecuteTime(30, 3, 3);
	default:
		//All other interrupts
		return ExecuteTime(44, 5, 3);
	}
}

//----------------------------------------------------------------------------------------
std::wstring M68000::GetExceptionName(unsigned int vector) const
{
	switch(vector)
	{
	case 0x00:
		return L"Reset";
	case 0x01:
		return L"Invalid";
	case 0x02:
		return L"Bus Error";
	case 0x03:
		return L"Address Error";
	case 0x04:
		return L"Illegal Instruction";
	case 0x05:
		return L"Zero Divide";
	case 0x06:
		return L"CHK Instruction";
	case 0x07:
		return L"TRAPV Instruction";
	case 0x08:
		return L"Privilege Violation";
	case 0x09:
		return L"Trace";
	case 0x0A:
		return L"Line 1010 Emulator";
	case 0x0B:
		return L"Line 1111 Emulator";
	case 0x0C:
	case 0x0D:
	case 0x0E:
		return L"Reserved";
	case 0x0F:
		return L"Uninitialized Interrupt Vector";
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17:
		return L"Reserved";
	case 0x18:
		return L"Spurious Interrupt";
	case 0x19:
		return L"L1 Interrupt Autovector";
	case 0x1A:
		return L"L2 Interrupt Autovector";
	case 0x1B:
		return L"L3 Interrupt Autovector";
	case 0x1C:
		return L"L4 Interrupt Autovector";
	case 0x1D:
		return L"L5 Interrupt Autovector";
	case 0x1E:
		return L"L6 Interrupt Autovector";
	case 0x1F:
		return L"L7 Interrupt Autovector";
	case 0x20:
		return L"Trap $0";
	case 0x21:
		return L"Trap $1";
	case 0x22:
		return L"Trap $2";
	case 0x23:
		return L"Trap $3";
	case 0x24:
		return L"Trap $4";
	case 0x25:
		return L"Trap $5";
	case 0x26:
		return L"Trap $6";
	case 0x27:
		return L"Trap $7";
	case 0x28:
		return L"Trap $8";
	case 0x29:
		return L"Trap $9";
	case 0x2A:
		return L"Trap $A";
	case 0x2B:
		return L"Trap $B";
	case 0x2C:
		return L"Trap $C";
	case 0x2D:
		return L"Trap $D";
	case 0x2E:
		return L"Trap $E";
	case 0x2F:
		return L"Trap $F";
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	case 0x34:
	case 0x35:
	case 0x36:
	case 0x37:
	case 0x38:
	case 0x39:
	case 0x3A:
	case 0x3B:
	case 0x3C:
	case 0x3D:
	case 0x3E:
	case 0x3F:
		return L"Reserved";
	}
	std::wstringstream nameStream;
	nameStream << L"User Interrupt 0x" << std::hex << std::uppercase << vector;
	return nameStream.str();
}

//----------------------------------------------------------------------------------------
bool M68000::ExceptionDisabled(unsigned int vector)
{
	//Check if all exceptions are disabled
	if(disableAllExceptions)
	{
		return true;
	}

	//Check if this specific exception is disabled
	if(exceptionListEmpty)
	{
		boost::mutex::scoped_lock lock(debugMutex);
		for(ExceptionList::const_iterator i = exceptionList.begin(); i != exceptionList.end(); ++i)
		{
			ExceptionDebuggingEntry* entry = *i;
			if(entry->vectorNumber == vector)
			{
				return entry->disable;
			}
		}
	}

	//If no rule could be found to disable this exception, return false.
	return false;
}

//----------------------------------------------------------------------------------------
void M68000::ExceptionLogIfRequested(unsigned int vector)
{
	if(logAllExceptions)
	{
		//Log the event
		LogEntry logEntry(LogEntry::EVENTLEVEL_DEBUG);
		logEntry << L"Exception triggered. Exception number: 0x" << std::hex << std::uppercase << vector << ", Exception address: 0x" << vector * 4 << ", Exception name: " << GetExceptionName(vector) << ", Current PC: 0x" << GetPC().GetData();
		GetDeviceContext()->WriteLogEvent(logEntry);
		return;
	}

	if(!exceptionListEmpty)
	{
		boost::mutex::scoped_lock lock(debugMutex);
		for(ExceptionList::const_iterator i = exceptionList.begin(); i != exceptionList.end(); ++i)
		{
			ExceptionDebuggingEntry* entry = *i;
			if(entry->vectorNumber == vector)
			{
				if(entry->enableLogging)
				{
					//Log the event
					LogEntry logEntry(LogEntry::EVENTLEVEL_DEBUG);
					logEntry << L"Exception triggered. Exception number: 0x" << std::hex << std::uppercase << vector << ", Exception address: 0x" << vector * 4 << ", Exception name: " << GetExceptionName(vector) << ", Current PC: 0x" << GetPC().GetData();
					GetDeviceContext()->WriteLogEvent(logEntry);
					return;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void M68000::ExceptionBreakIfRequested(unsigned int vector)
{
	if(breakOnAllExceptions)
	{
		TriggerBreak();
		return;
	}

	if(!exceptionListEmpty)
	{
		boost::mutex::scoped_lock lock(debugMutex);
		for(ExceptionList::const_iterator i = exceptionList.begin(); i != exceptionList.end(); ++i)
		{
			ExceptionDebuggingEntry* entry = *i;
			if(entry->vectorNumber == vector)
			{
				if(entry->enableBreak)
				{
					TriggerBreak();
					return;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void M68000::TriggerExceptionFromDebugger(unsigned int vector)
{
	debugExceptionTriggerVector = vector;
	debugExceptionTriggerPending = true;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
//[Group 0]
//-Process Reset
//-Process queued bus error or address error exception
//
//[Group 1]
//-Process the queued external interrupt
//-Process privilege violation or illegal instruction opcodes if they occur while
// fetching the next opcode.
//-Process a trace exception if we executed an opcode with the trace bit set
//
//[Group 2]
//-Group 2 exceptions should be processed and taken during the execution of the opcode
// from which they are generated. If a divide by 0 occurs during a div instruction for
// example, when that opcode terminates, we should already be in the exception handler
// for the divide by 0 exception.
//----------------------------------------------------------------------------------------
double M68000::ExecuteStep()
{
	unsigned int cyclesExecuted = 1;
	double additionalTime = 0;

	//If we have any pending line state changes waiting, apply any which we have now
	//reached.
	if(lineAccessPending)
	{
		//##DEBUG##
		//std::wcout << "M68000 line access pending\n";

		boost::mutex::scoped_lock lock(lineMutex);
		double currentTimesliceProgress = GetCurrentTimesliceProgress();
		bool done = false;
		std::list<LineAccess>::iterator i = lineAccessBuffer.begin();
		while(!done && (i != lineAccessBuffer.end()))
		{
			if(i->accessTime <= currentTimesliceProgress)
			{
				ApplyLineStateChange(i->lineID, i->state);
				++i;
			}
			else
			{
				done = true;
			}
		}

		//Clear any completed entries from the list
		lineAccessBuffer.erase(lineAccessBuffer.begin(), i);
		lineAccessPending = !lineAccessBuffer.empty();
	}
	lastLineCheckTime = GetCurrentTimesliceProgress();

	//If the reset line is being asserted, trigger a reset, and abort any further
	//processing.
	if(resetLineState)
	{
		Reset();
		return CalculateExecutionTime(cyclesExecuted) + additionalTime;
	}

	//If we don't have the bus or the HALT line is asserted, abort any further processing.
	if(bgLineState || haltLineState)
	{
		return CalculateExecutionTime(cyclesExecuted) + additionalTime;
	}

	//If an exception has been triggered from the debugger, process it.
	if(debugExceptionTriggerPending)
	{
		debugExceptionTriggerPending = false;

		//Push the appropriate stack frame for this exception
		if(debugExceptionTriggerVector == EX_RESET)
		{
			//No exception stack frame for a reset exception
		}
		else if((debugExceptionTriggerVector == EX_BUS_ERROR) || (debugExceptionTriggerVector == EX_ADDRESS_ERROR))
		{
			//Push a dummy group 0 exception stack frame. It's not really possible to give
			//real data here, since a real exception didn't occur.
			PushStackFrame(GetPC(), GetSR(), 0, 0, false, false, GetFunctionCode(true));
		}
		else
		{
			//Push a group 1/2 exception stack frame
			PushStackFrame(GetPC(), GetSR(), false);
		}

		//Process the exception
		cyclesExecuted = ProcessException(debugExceptionTriggerVector).cycles;
		return CalculateExecutionTime(cyclesExecuted) + additionalTime;
	}

	//Group 0 exception processing
	if(group0ExceptionPending)
	{
		group0ExceptionPending = false;
		if(!ExceptionDisabled(group0vector))
		{
			if(group0vector != EX_RESET)
			{
				PushStackFrame(group0pc, group0sr, group0instructionRegister, group0address, group0readWriteFlag, group0instructionFlag, group0functionCode);
			}
			cyclesExecuted = ProcessException(group0vector).cycles;

			//If we've triggered a double bus fault, enter the halted state
			if(group0ExceptionPending && (group0vector != EX_RESET))
			{
				group0ExceptionPending = false;
				SetProcessorState(STATE_HALTED);
			}
			return CalculateExecutionTime(cyclesExecuted) + additionalTime;
		}
	}

	//If we're in a halted state, terminate instruction processing
	if(GetProcessorState() == STATE_HALTED)
	{
		return CalculateExecutionTime(cyclesExecuted) + additionalTime;
	}

	//If an external interrupt is pending, process it.
	if(forceInterrupt || (interruptPendingLevel > GetSR_IPM()))
	{
		forceInterrupt = false;

		//Build the address for the interrupt acknowledge cycle. The format for the
		//address field is given below, as defined in the M68000 Users manual, section
		//5.1.4 "CPU Space Cycle".
		//	----------------------------------------------------------------------------------------------------------------------------------
		//	|31 |30 |29 |28 |27 |26 |25 |24 |23 |22 |21 |20 |19 |18 |17 |16 ||15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//	|---------------------------------------------------------------||---------------------------------------------------------------|
		//	| 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 || 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | Int Level | 1 |
		//	----------------------------------------------------------------------------------------------------------------------------------
		//                                                  |CPU Space Type |
		M68000Long interruptCycleAddress = 0xFFFFFFFF;
		interruptCycleAddress.SetDataSegment(1, 3, interruptPendingLevel);

		//Attempt to read the interrupt vector number from the bus by performing an
		//interrupt acknowledge cycle.
		autoVectorPendingInterrupt = false;
		M68000Word interruptVectorNumber = 0;
		memoryAccessFunctionCode = FUNCTIONCODE_CPUSPACE;
		memoryAccessUpperDataStrobe = true;
		memoryAccessLowerDataStrobe = true;
		memoryAccessReadHighWriteLow = true;
		memoryAccessRMWCycleInProgress = false;
		memoryAccessRMWCycleFirstOperation = false;
		IBusInterface::AccessResult accessResult = memoryBus->ReadMemory(interruptCycleAddress.GetData(), interruptVectorNumber, GetDeviceContext(), GetCurrentTimesliceProgress());

		//Mask the resulting interrupt vector number to a single byte. According to the
		//M68000 users manual, section 5.1.4, the M68000 does perform a 16-bit read for
		//the interrupt vector number, but only the lower byte is used: "Although a vector
		//number is one byte, both data strobes are asserted due to the microcode used for
		//exception processing. The processor does not recognize anything on data lines D8
		//through D15 at this time".
		interruptVectorNumber &= 0xFF;

		//Determine the result of the interrupt acknowledge cycle
		if(accessResult.busError)
		{
			//If a bus error occurred during the interrupt acknowledge cycle, a spurious
			//interrupt is triggered rather than a bus error exception.
			interruptVectorNumber = EX_INTERRUPT_SPURIOUS;
		}
		else if(autoVectorPendingInterrupt)
		{
			//If VPA was asserted during the interrupt acknowledge cycle, autovector the
			//interrupt.
			interruptVectorNumber = EX_INTERRUPT_AUTOVECTOR + (interruptPendingLevel - 1);
		}
		else if(!accessResult.deviceReplied)
		{
			//Critical error. No device responded to the interrupt acknowledge cycle. The
			//processor is now locked in the interrupt acknowledge cycle forever.
			SetProcessorState(STATE_HALTED);
			return CalculateExecutionTime(cyclesExecuted) + additionalTime;
		}
		//##TODO## Confirm this behaviour. This is based on a best guess, that the M68000
		//probably drives the data lines low before starting the read, and considers the
		//interrupt vector number to be uninitialized if the resulting vector number is 0.
		//The official documentation doesn't clarify the exact conditions which can result
		//in this interrupt vector occurring however.
		else if(interruptVectorNumber == 0)
		{
			//If a device responded to the interrupt acknowledge cycle by asserting DTACK,
			//but failed to provide a vector number for the interrupt, we have an
			//uninitialized interrupt.
			interruptVectorNumber = EX_INTERRUPT_UNINITIALIZED;
		}

		//Add any additional time from the memory read operation to the execution time
		additionalTime += accessResult.executionTime;

		//Process the interrupt
		if(!ExceptionDisabled(interruptVectorNumber.GetData()))
		{
			PushStackFrame(GetPC(), GetSR(), false);
			SetSR_IPM(interruptPendingLevel);
			cyclesExecuted = ProcessException(interruptVectorNumber.GetData()).cycles;
			return CalculateExecutionTime(cyclesExecuted) + additionalTime;
		}
	}

	//If the processor isn't stopped, fetch the next opcode
	if(GetProcessorState() != STATE_STOPPED)
	{
		//Update the trace log, and test for breakpoints
		RecordTrace(GetPC().GetData());
		CheckExecution(GetPC().GetData());

		M68000Word opcode = prefetchedWord;
		if(!wordIsPrefetched || (prefetchedWordAddress != GetPC()))
		{
			ReadMemory(GetPC(), opcode, GetFunctionCode(false), GetPC(), false, 0, false, false);
		}
		wordIsPrefetched = false;
		const M68000Instruction* nextOpcodeType = opcodeTable.GetInstruction(opcode.GetData());
		if(nextOpcodeType == 0)
		{
			//Generate an exception if we've encountered an unimplemented opcode
			unsigned int exception = EX_ILLEGAL_INSTRUCTION;
			if((opcode & 0xF000) == 0xA000)
			{
				exception = EX_LINE_1010;
			}
			else if((opcode & 0xF000) == 0xF000)
			{
				exception = EX_LINE_1111;
			}
			if(!ExceptionDisabled(exception))
			{
				PushStackFrame(GetPC(), GetSR(), false);
				cyclesExecuted = ProcessException(exception).cycles;
			}
		}
		else
		{
			M68000Instruction* nextOpcode = 0;
//			nextOpcode = nextOpcodeType->Clone();
			nextOpcode = nextOpcodeType->ClonePlacement(opcodeBuffer);
			if(nextOpcode->Privileged() && !GetSR_S() && !ExceptionDisabled(EX_PRIVILEGE_VIOLATION))
			{
				//Generate a privilege violation if the instruction is privileged and
				//we're not in supervisor mode.
				PushStackFrame(GetPC(), GetSR(), false);
				cyclesExecuted = ProcessException(EX_PRIVILEGE_VIOLATION).cycles;
			}
			else
			{
				bool trace = GetSR_T();

				//Decode the instruction
				nextOpcode->SetInstructionSize(2);
				nextOpcode->SetInstructionLocation(GetPC());
				nextOpcode->SetInstructionRegister(opcode);
				nextOpcode->M68000Decode(this, nextOpcode->GetInstructionLocation(), nextOpcode->GetInstructionRegister(), nextOpcode->GetTransparentFlag());

				//Record this code location to assist in disassembly
				AddDisassemblyAddressInfoCode(GetPC().GetData(), nextOpcode->GetInstructionSize());

				//We read the next data word here, just to try and get the right data
				//stored as the last data to move through the data bus. We don't have
				//correct support for the prefetch pipeline currently, which would
				//normally have read an instruction word or two ahead. In the case of
				//reads from devices which don't assert all the data lines, the last data
				//to move over the bus is exposed, so we try and simulate that here.
				//Correct support will require implementing accurate instruction decoding
				//stages, as well as correct prefetch support.
				wordIsPrefetched = true;
				prefetchedWordAddress = GetPC() + nextOpcode->GetInstructionSize();
				ReadMemory(prefetchedWordAddress, prefetchedWord, GetFunctionCode(false), GetPC(), false, 0, false, false);

				//Execute the instruction
				ExecuteTime opcodeExecuteTime = nextOpcode->M68000Execute(this, GetPC());
				cyclesExecuted = opcodeExecuteTime.cycles;
				additionalTime += opcodeExecuteTime.additionalTime;

				//Generate a trace exception if we were in trace mode before executing
				//the instruction, and a group 0 exception hasn't occurred. Note that we
				//have to check the trace bit in this manner, as apparently, a trace
				//should still be generated after instruction execution where a level 2
				//exception is triggered, which in our emulator clears the trace flag
				//immediately. The real M68000 probably does something slightly different
				//internally, but this implementation should have identical behaviour.
				if(trace && !group0ExceptionPending && !ExceptionDisabled(EX_TRACE))
				{
					PushStackFrame(GetPC(), GetSR(), false);
					cyclesExecuted += ProcessException(EX_TRACE).cycles;
					return cyclesExecuted;
				}
			}
			nextOpcode->~M68000Instruction();
//			delete nextOpcode;
		}
	}

	return CalculateExecutionTime(cyclesExecuted) + additionalTime;
}

//----------------------------------------------------------------------------------------
void M68000::ExecuteRollback()
{
	for(unsigned int i = 0; i < addressRegCount - 1; ++i)
	{
		a[i] = ba[i];
	}
	for(unsigned int i = 0; i < dataRegCount; ++i)
	{
		d[i] = bd[i];
	}
	ssp = bssp;
	usp = busp;
	pc = bpc;
	sr = bsr;
	processorState = bprocessorState;
	lastReadBusData = blastReadBusData;
	wordIsPrefetched = bwordIsPrecached;
	prefetchedWord = bprecachedWord;
	prefetchedWordAddress = bprecachedWordAddress;

	for(unsigned int i = 0; i < addressRegCount - 1; ++i)
	{
		aDisassemblyInfo[i] = baDisassemblyInfo[i];
	}
	for(unsigned int i = 0; i < dataRegCount; ++i)
	{
		dDisassemblyInfo[i] = bdDisassemblyInfo[i];
	}

	lastTimesliceLength = blastTimesliceLength;
	lineAccessBuffer = blineAccessBuffer;
	lineAccessPending = !lineAccessBuffer.empty();

	resetLineState = bresetLineState;
	haltLineState = bhaltLineState;
	brLineState = bbrLineState;
	bgLineState = bbgLineState;

	group0ExceptionPending = bgroup0ExceptionPending;
	group0instructionRegister = bgroup0instructionRegister;
	group0address = bgroup0address;
	group0pc = bgroup0pc;
	group0sr = bgroup0sr;
	group0readWriteFlag = bgroup0readWriteFlag;
	group0instructionFlag = bgroup0instructionFlag;
	group0vector = bgroup0vector;
	group0functionCode = bgroup0functionCode;

	Processor::ExecuteRollback();
}

//----------------------------------------------------------------------------------------
void M68000::ExecuteCommit()
{
	for(unsigned int i = 0; i < addressRegCount - 1; ++i)
	{
		ba[i] = a[i];
	}
	for(unsigned int i = 0; i < dataRegCount; ++i)
	{
		bd[i] = d[i];
	}
	bssp = ssp;
	busp = usp;
	bpc = pc;
	bsr = sr;
	bprocessorState = processorState;
	blastReadBusData = lastReadBusData;
	bwordIsPrecached = wordIsPrefetched;
	bprecachedWord = prefetchedWord;
	bprecachedWordAddress = prefetchedWordAddress;

	for(unsigned int i = 0; i < addressRegCount - 1; ++i)
	{
		baDisassemblyInfo[i] = aDisassemblyInfo[i];
	}
	for(unsigned int i = 0; i < dataRegCount; ++i)
	{
		bdDisassemblyInfo[i] = dDisassemblyInfo[i];
	}

	blastTimesliceLength = lastTimesliceLength;
	if(lineAccessPending)
	{
		blineAccessBuffer = lineAccessBuffer;
	}
	else
	{
		blineAccessBuffer.clear();
	}

	bresetLineState = resetLineState;
	bhaltLineState = haltLineState;
	bbrLineState = brLineState;
	bbgLineState = bgLineState;

	bgroup0ExceptionPending = group0ExceptionPending;
	bgroup0instructionRegister = group0instructionRegister;
	bgroup0address = group0address;
	bgroup0pc = group0pc;
	bgroup0sr = group0sr;
	bgroup0readWriteFlag = group0readWriteFlag;
	bgroup0instructionFlag = group0instructionFlag;
	bgroup0vector = group0vector;
	bgroup0functionCode = group0functionCode;

	Processor::ExecuteCommit();
}

//----------------------------------------------------------------------------------------
bool M68000::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void M68000::NotifyUpcomingTimeslice(double nanoseconds)
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
M68000::OpcodeInfo M68000::GetOpcodeInfo(unsigned int location)
{
	OpcodeInfo opcodeInfo;
	opcodeInfo.valid = false;

	M68000Long instructionLocation = location;
	M68000Word opcode;
	ReadMemoryTransparent(instructionLocation, opcode, FUNCTIONCODE_SUPERVISORPROGRAM, false, false);

	const M68000Instruction* targetOpcodeType = 0;
	targetOpcodeType = opcodeTable.GetInstruction(opcode.GetData());
	if(targetOpcodeType != 0)
	{
		M68000Instruction* targetOpcode = targetOpcodeType->Clone();

		targetOpcode->SetTransparentFlag(true);
		targetOpcode->SetInstructionSize(2);
		targetOpcode->SetInstructionLocation(instructionLocation);
		targetOpcode->SetInstructionRegister(opcode);

		targetOpcode->M68000Decode(this, targetOpcode->GetInstructionLocation(), targetOpcode->GetInstructionRegister(), targetOpcode->GetTransparentFlag());
		M68000Instruction::Disassembly disassembly = targetOpcode->M68000Disassemble();

		opcodeInfo.valid = true;
		opcodeInfo.opcodeSize = targetOpcode->GetInstructionSize();
		opcodeInfo.disassemblyOpcode = disassembly.disassemblyOpcode;
		opcodeInfo.disassemblyArguments = disassembly.disassemblyArguments;
		opcodeInfo.disassemblyComment = disassembly.disassemblyComment;

		delete targetOpcode;
	}

	return opcodeInfo;
}

//----------------------------------------------------------------------------------------
Data M68000::GetRawData(unsigned int location)
{
	M68000Word data;
	ReadMemoryTransparent(location, data, FUNCTIONCODE_SUPERVISORPROGRAM, false, false);
	return data;
}

//----------------------------------------------------------------------------------------
unsigned int M68000::GetCurrentPC() const
{
	return GetPC().GetData();
}

//----------------------------------------------------------------------------------------
unsigned int M68000::GetPCWidth() const
{
	return 32;
}

//----------------------------------------------------------------------------------------
unsigned int M68000::GetAddressBusWidth() const
{
	return 24;
}

//----------------------------------------------------------------------------------------
unsigned int M68000::GetDataBusWidth() const
{
	return 16;
}

//----------------------------------------------------------------------------------------
unsigned int M68000::GetMinimumOpcodeByteSize() const
{
	return 2;
}

//----------------------------------------------------------------------------------------
//Disassembly functions
//----------------------------------------------------------------------------------------
bool M68000::DisassemblyGetAddressRegisterUnmodified(unsigned int regNo, unsigned int& sourceLocation) const
{
	if(regNo < (addressRegCount - 1))
	{
		sourceLocation = aDisassemblyInfo[regNo].sourceLocation;
		return aDisassemblyInfo[regNo].unmodified;
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool M68000::DisassemblyGetDataRegisterUnmodified(unsigned int regNo, unsigned int& sourceLocation) const
{
	if(regNo < dataRegCount)
	{
		sourceLocation = dDisassemblyInfo[regNo].sourceLocation;
		return dDisassemblyInfo[regNo].unmodified;
	}
	return false;
}

//----------------------------------------------------------------------------------------
void M68000::DisassemblySetAddressRegisterUnmodified(unsigned int regNo, bool state, unsigned int dataSize, unsigned int sourceLocation)
{
	if(regNo < (addressRegCount - 1))
	{
		aDisassemblyInfo[regNo].unmodified = state;
		aDisassemblyInfo[regNo].dataSize = dataSize;
		aDisassemblyInfo[regNo].sourceLocation = sourceLocation;
	}
}

//----------------------------------------------------------------------------------------
void M68000::DisassemblySetDataRegisterUnmodified(unsigned int regNo, bool state, unsigned int dataSize, unsigned int sourceLocation)
{
	if(regNo < dataRegCount)
	{
		dDisassemblyInfo[regNo].unmodified = state;
		dDisassemblyInfo[regNo].dataSize = dataSize;
		dDisassemblyInfo[regNo].sourceLocation = sourceLocation;
	}
}

//----------------------------------------------------------------------------------------
unsigned int M68000::DisassemblyGetAddressRegisterUnmodifiedSize(unsigned int regNo) const
{
	if(regNo < (addressRegCount - 1))
	{
		return aDisassemblyInfo[regNo].dataSize;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
unsigned int M68000::DisassemblyGetDataRegisterUnmodifiedSize(unsigned int regNo) const
{
	if(regNo < dataRegCount)
	{
		return dDisassemblyInfo[regNo].dataSize;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//Special operation functions
//----------------------------------------------------------------------------------------
void M68000::TriggerExternalReset(double resetTimeBegin, double resetTimeEnd)
{
	//Toggle the external RESET line state, to reset external devices.
	memoryBus->SetLine(M68000::LINE_RESET, Data(1, 1), GetDeviceContext(), GetDeviceContext(), resetTimeBegin);
	memoryBus->SetLine(M68000::LINE_RESET, Data(1, 0), GetDeviceContext(), GetDeviceContext(), resetTimeEnd);
}

//----------------------------------------------------------------------------------------
//Memory access functions
//----------------------------------------------------------------------------------------
M68000::FunctionCode M68000::GetFunctionCode(bool programReference)
{
	static const FunctionCode codeTable[8] = {
		FUNCTIONCODE_UNDEFINED0,		//000
		FUNCTIONCODE_USERDATA,			//001
		FUNCTIONCODE_USERPROGRAM,		//010
		FUNCTIONCODE_UNDEFINED3,		//011
		FUNCTIONCODE_UNDEFINED4,		//100
		FUNCTIONCODE_SUPERVISORDATA,	//101
		FUNCTIONCODE_SUPERVISORPROGRAM,	//110
		FUNCTIONCODE_CPUSPACE			//111
	};
	unsigned int codeTableIndex = ((unsigned int)GetSR_S() << 2) | ((unsigned int)programReference << 1) | ((unsigned int)!programReference);
	return codeTable[codeTableIndex];
}

//----------------------------------------------------------------------------------------
double M68000::ReadMemory(const M68000Long& location, Data& data, FunctionCode code, bool transparent, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	if(transparent)
	{
		ReadMemoryTransparent(location, data, code, rmwCycleInProgress, rmwCycleFirstOperation);
		return 0;
	}
	else
	{
		return ReadMemory(location, data, code, currentPC, processingInstruction, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
	}
}

//----------------------------------------------------------------------------------------
double M68000::ReadMemory(const M68000Long& location, Data& data, FunctionCode code, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	IBusInterface::AccessResult result;

	//Check for watchpoints
	CheckMemoryRead(location.GetDataSegment(0, 24), data.GetData());

	if((data.GetBitCount() > BITCOUNT_BYTE) && location.Odd())
	{
		//Generate an address error for unaligned memory access
		group0ExceptionPending = true;
		group0instructionRegister = instructionRegister;
		group0address = location;
		group0pc = currentPC;
		group0sr = GetSR();
		group0readWriteFlag = true;
		group0instructionFlag = !processingInstruction;
		group0vector = EX_ADDRESS_ERROR;
		group0functionCode = code;
	}
	else
	{
		//Note that we save the full 16-bit contents of each last data read from the bus,
		//to emulate tri-state behaviour for the data lines. When a read is made from an
		//external device, the M68000 only latches data lines when they are driven either
		//high or low. When a line is left floating, the last state of that line is
		//retained. We emulate that behaviour here by using the access mask provided by
		//the bus interface to determine which data lines were actually driven. If data
		//lines were left floating, we use our stored data from the last bus read to
		//calculate the correct values for the unset bits.
		switch(data.GetBitCount())
		{
		default:
			ReleaseAssert(true);
			break;
		case BITCOUNT_BYTE:
			{
				M68000Word temp;
				bool odd = location.Odd();
				memoryAccessFunctionCode = code;
				memoryAccessUpperDataStrobe = !odd;
				memoryAccessLowerDataStrobe = odd;
				memoryAccessReadHighWriteLow = true;
				memoryAccessRMWCycleInProgress = rmwCycleInProgress;
				memoryAccessRMWCycleFirstOperation = rmwCycleFirstOperation;
				result = memoryBus->ReadMemory(location.GetDataSegment(0, 24) & ~0x1, temp, GetDeviceContext(), GetCurrentTimesliceProgress());
				if(!result.accessMaskUsed)
				{
					lastReadBusData = temp;
				}
				else
				{
					lastReadBusData = (lastReadBusData & ~result.accessMask) | (temp & result.accessMask);
				}
				if(odd)
				{
					lastReadBusData.GetLowerBits(data);
				}
				else
				{
					lastReadBusData.GetUpperBits(data);
				}
				break;
			}
		case BITCOUNT_WORD:
			{
				M68000Word temp;
				memoryAccessFunctionCode = code;
				memoryAccessUpperDataStrobe = true;
				memoryAccessLowerDataStrobe = true;
				memoryAccessReadHighWriteLow = true;
				memoryAccessRMWCycleInProgress = rmwCycleInProgress;
				memoryAccessRMWCycleFirstOperation = rmwCycleFirstOperation;
				result = memoryBus->ReadMemory(location.GetDataSegment(0, 24), temp, GetDeviceContext(), GetCurrentTimesliceProgress());
				if(!result.accessMaskUsed)
				{
					lastReadBusData = temp;
				}
				else
				{
					lastReadBusData = (lastReadBusData & ~result.accessMask) | (temp & result.accessMask);
				}
				data = lastReadBusData;
				break;
			}
		case BITCOUNT_LONG:
			{
				M68000Word temp1;
				M68000Word temp2;
				IBusInterface::AccessResult result2;
				memoryAccessFunctionCode = code;
				memoryAccessUpperDataStrobe = true;
				memoryAccessLowerDataStrobe = true;
				memoryAccessReadHighWriteLow = true;
				memoryAccessRMWCycleInProgress = rmwCycleInProgress;
				memoryAccessRMWCycleFirstOperation = rmwCycleFirstOperation;
				result = memoryBus->ReadMemory(location.GetDataSegment(0, 24), temp1, GetDeviceContext(), GetCurrentTimesliceProgress());
				result2 = memoryBus->ReadMemory((location + 2).GetDataSegment(0, 24), temp2, GetDeviceContext(), GetCurrentTimesliceProgress() + result.executionTime);
				if(!result.accessMaskUsed)
				{
					lastReadBusData = temp1;
				}
				else
				{
					lastReadBusData = (lastReadBusData & ~result.accessMask) | (temp1 & result.accessMask);
				}
				temp1 = lastReadBusData;
				if(!result2.accessMaskUsed)
				{
					lastReadBusData = temp2;
				}
				else
				{
					lastReadBusData = (lastReadBusData & ~result2.accessMask) | (temp2 & result2.accessMask);
				}
				temp2 = lastReadBusData;
				data = (temp1.GetData() << temp2.GetBitCount()) | temp2.GetData();
				result.busError |= result2.busError;
				result.executionTime += result2.executionTime;
			}
		}

		if(result.busError)
		{
			//Generate a bus error if communication failed
			group0ExceptionPending = true;
			group0instructionRegister = instructionRegister;
			group0address = location;
			group0pc = currentPC;
			group0sr = GetSR();
			group0readWriteFlag = true;
			group0instructionFlag = !processingInstruction;
			group0vector = EX_BUS_ERROR;
			group0functionCode = code;
			//##DEBUG##
			//std::wcout << "Bus error triggered on read of " << std::hex << location.GetData() << '\n';
		}
	}

	return result.executionTime;
}

//----------------------------------------------------------------------------------------
void M68000::ReadMemoryTransparent(const M68000Long& location, Data& data, FunctionCode code, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	switch(data.GetBitCount())
	{
	default:
		ReleaseAssert(true);
		break;
	case BITCOUNT_BYTE:
		{
			M68000Word temp;
			if(location.Odd())
			{
				PerformanceLock lock(ceLineStateMutex);
				memoryAccessTransparentFunctionCode = code;
				memoryAccessTransparentUpperDataStrobe = false;
				memoryAccessTransparentLowerDataStrobe = true;
				memoryAccessTransparentReadHighWriteLow = true;
				memoryAccessTransparentRMWCycleInProgress = rmwCycleInProgress;
				memoryAccessTransparentRMWCycleFirstOperation = rmwCycleFirstOperation;
				memoryBus->TransparentReadMemory(location.GetDataSegment(0, 24) & ~0x1, temp, GetDeviceContext());
				temp.GetLowerBits(data);
			}
			else
			{
				PerformanceLock lock(ceLineStateMutex);
				memoryAccessTransparentFunctionCode = code;
				memoryAccessTransparentUpperDataStrobe = true;
				memoryAccessTransparentLowerDataStrobe = false;
				memoryAccessTransparentReadHighWriteLow = true;
				memoryAccessTransparentRMWCycleInProgress = rmwCycleInProgress;
				memoryAccessTransparentRMWCycleFirstOperation = rmwCycleFirstOperation;
				memoryBus->TransparentReadMemory(location.GetDataSegment(0, 24), temp, GetDeviceContext());
				temp.GetUpperBits(data);
			}
			break;
		}
	case BITCOUNT_WORD:
		{
			PerformanceLock lock(ceLineStateMutex);
			M68000Word temp;
			memoryAccessTransparentFunctionCode = code;
			memoryAccessTransparentUpperDataStrobe = true;
			memoryAccessTransparentLowerDataStrobe = true;
			memoryAccessTransparentReadHighWriteLow = true;
			memoryAccessTransparentRMWCycleInProgress = rmwCycleInProgress;
			memoryAccessTransparentRMWCycleFirstOperation = rmwCycleFirstOperation;
			memoryBus->TransparentReadMemory(location.GetDataSegment(0, 24), temp, GetDeviceContext());
			data = temp;
			break;
		}
	case BITCOUNT_LONG:
		{
			PerformanceLock lock(ceLineStateMutex);
			M68000Word temp1;
			M68000Word temp2;
			memoryAccessTransparentFunctionCode = code;
			memoryAccessTransparentUpperDataStrobe = true;
			memoryAccessTransparentLowerDataStrobe = true;
			memoryAccessTransparentReadHighWriteLow = true;
			memoryAccessTransparentRMWCycleInProgress = rmwCycleInProgress;
			memoryAccessTransparentRMWCycleFirstOperation = rmwCycleFirstOperation;
			memoryBus->TransparentReadMemory(location.GetDataSegment(0, 24), temp1, GetDeviceContext());
			memoryBus->TransparentReadMemory((location + 2).GetDataSegment(0, 24), temp2, GetDeviceContext());
			data = (temp1.GetData() << temp2.GetBitCount()) | temp2.GetData();
		}
	}
}

//----------------------------------------------------------------------------------------
double M68000::WriteMemory(const M68000Long& location, const Data& data, FunctionCode code, bool transparent, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	if(transparent)
	{
		WriteMemoryTransparent(location, data, code, rmwCycleInProgress, rmwCycleFirstOperation);
		return 0;
	}
	else
	{
		return WriteMemory(location, data, code, currentPC, processingInstruction, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
	}
}

//----------------------------------------------------------------------------------------
double M68000::WriteMemory(const M68000Long& location, const Data& data, FunctionCode code, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	IBusInterface::AccessResult result;

	//Check for watchpoints
	CheckMemoryWrite(location.GetDataSegment(0, 24), data.GetData());

	if((data.GetBitCount() > BITCOUNT_BYTE) && location.Odd())
	{
		//Generate an address error for unaligned memory access
		group0ExceptionPending = true;
		group0instructionRegister = instructionRegister;
		group0address = location;
		group0pc = currentPC;
		group0sr = GetSR();
		group0readWriteFlag = false;
		group0instructionFlag = !processingInstruction;
		group0vector = EX_ADDRESS_ERROR;
		group0functionCode = code;
	}
	else
	{
		//##DEBUG##
//		std::wcout << std::hex << std::uppercase << "M68000 Write:\t" << location.GetDataSegment(0, 24) << '\t' << data.GetData() << '\n';

		switch(data.GetBitCount())
		{
		default:
			ReleaseAssert(true);
			break;
		case BITCOUNT_BYTE:
			{
				//##NOTE## When the M68000 performs a byte write, it mirrors the 8-bit
				//data in both the upper and lower halves of the data bus. Refer to "The
				//M68000 User's Manual", section 3.3, table 3.1 for more info. 8-bit
				//peripheral devices are often mapped to upper data lines, and rely on
				//this behaviour.
				M68000Word tempData;
				tempData.SetLowerBits(M68000Byte(data));
				tempData.SetUpperBits(M68000Byte(data));

				if(location.Odd())
				{
					memoryAccessFunctionCode = code;
					memoryAccessUpperDataStrobe = false;
					memoryAccessLowerDataStrobe = true;
					memoryAccessReadHighWriteLow = false;
					memoryAccessRMWCycleInProgress = rmwCycleInProgress;
					memoryAccessRMWCycleFirstOperation = rmwCycleFirstOperation;
					result = memoryBus->WriteMemory(location.GetDataSegment(0, 24) & ~0x1, tempData, GetDeviceContext(), GetCurrentTimesliceProgress());
				}
				else
				{
					memoryAccessFunctionCode = code;
					memoryAccessUpperDataStrobe = true;
					memoryAccessLowerDataStrobe = false;
					memoryAccessReadHighWriteLow = false;
					memoryAccessRMWCycleInProgress = rmwCycleInProgress;
					memoryAccessRMWCycleFirstOperation = rmwCycleFirstOperation;
					result = memoryBus->WriteMemory(location.GetDataSegment(0, 24), tempData, GetDeviceContext(), GetCurrentTimesliceProgress());
				}
				break;
			}
		case BITCOUNT_WORD:
			{
				memoryAccessFunctionCode = code;
				memoryAccessUpperDataStrobe = true;
				memoryAccessLowerDataStrobe = true;
				memoryAccessReadHighWriteLow = false;
				memoryAccessRMWCycleInProgress = rmwCycleInProgress;
				memoryAccessRMWCycleFirstOperation = rmwCycleFirstOperation;
				result = memoryBus->WriteMemory(location.GetDataSegment(0, 24), M68000Word(data), GetDeviceContext(), GetCurrentTimesliceProgress());
				break;
			}
		case BITCOUNT_LONG:
			{
				M68000Word word1(data.GetUpperBits(BITCOUNT_WORD));
				M68000Word word2(data.GetLowerBits(BITCOUNT_WORD));
				IBusInterface::AccessResult result2;
				memoryAccessFunctionCode = code;
				memoryAccessUpperDataStrobe = true;
				memoryAccessLowerDataStrobe = true;
				memoryAccessReadHighWriteLow = false;
				memoryAccessRMWCycleInProgress = rmwCycleInProgress;
				memoryAccessRMWCycleFirstOperation = rmwCycleFirstOperation;
				result = memoryBus->WriteMemory(location.GetDataSegment(0, 24), word1, GetDeviceContext(), GetCurrentTimesliceProgress());
				result2 = memoryBus->WriteMemory((location + 2).GetDataSegment(0, 24), word2, GetDeviceContext(), GetCurrentTimesliceProgress() + result.executionTime);
				result.busError |= result2.busError;
				result.executionTime += result2.executionTime;
				break;
			}
		}

		if(result.busError)
		{
			//Generate a bus error if communication failed
			group0ExceptionPending = true;
			group0instructionRegister = instructionRegister;
			group0address = location;
			group0pc = currentPC;
			group0sr = GetSR();
			group0readWriteFlag = false;
			group0instructionFlag = !processingInstruction;
			group0vector = EX_BUS_ERROR;
			group0functionCode = code;
			//##DEBUG##
			//std::wcout << "Bus error triggered on write of " << std::hex << location.GetData() << '\n';
		}
	}

	return result.executionTime;
}

//----------------------------------------------------------------------------------------
void M68000::WriteMemoryTransparent(const M68000Long& location, const Data& data, FunctionCode code, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	IBusInterface::AccessResult result;

	switch(data.GetBitCount())
	{
	default:
		ReleaseAssert(true);
		break;
	case BITCOUNT_BYTE:
		{
			//##NOTE## When the M68000 performs a byte write, it mirrors the 8-bit
			//data in both the upper and lower halves of the data bus. Refer to "The
			//M68000 User's Manual", section 3.3, table 3.1 for more info. 8-bit
			//peripheral devices are often mapped to upper data lines, and rely on
			//this behaviour.
			M68000Word tempData;
			tempData.SetLowerBits(M68000Byte(data));
			tempData.SetUpperBits(M68000Byte(data));

			if(location.Odd())
			{
				PerformanceLock lock(ceLineStateMutex);
				memoryAccessTransparentFunctionCode = code;
				memoryAccessTransparentUpperDataStrobe = false;
				memoryAccessTransparentLowerDataStrobe = true;
				memoryAccessTransparentReadHighWriteLow = false;
				memoryAccessTransparentRMWCycleInProgress = rmwCycleInProgress;
				memoryAccessTransparentRMWCycleFirstOperation = rmwCycleFirstOperation;
				memoryBus->TransparentWriteMemory(location.GetDataSegment(0, 24) & ~0x1, tempData, GetDeviceContext());
			}
			else
			{
				PerformanceLock lock(ceLineStateMutex);
				memoryAccessTransparentFunctionCode = code;
				memoryAccessTransparentUpperDataStrobe = true;
				memoryAccessTransparentLowerDataStrobe = false;
				memoryAccessTransparentReadHighWriteLow = false;
				memoryAccessTransparentRMWCycleInProgress = rmwCycleInProgress;
				memoryAccessTransparentRMWCycleFirstOperation = rmwCycleFirstOperation;
				memoryBus->TransparentWriteMemory(location.GetDataSegment(0, 24), tempData, GetDeviceContext());
			}
			break;
		}
	case BITCOUNT_WORD:
		{
			PerformanceLock lock(ceLineStateMutex);
			memoryAccessTransparentFunctionCode = code;
			memoryAccessTransparentUpperDataStrobe = true;
			memoryAccessTransparentLowerDataStrobe = true;
			memoryAccessTransparentReadHighWriteLow = false;
			memoryAccessTransparentRMWCycleInProgress = rmwCycleInProgress;
			memoryAccessTransparentRMWCycleFirstOperation = rmwCycleFirstOperation;
			memoryBus->TransparentWriteMemory(location.GetDataSegment(0, 24), M68000Word(data), GetDeviceContext());
			break;
		}
	case BITCOUNT_LONG:
		{
			PerformanceLock lock(ceLineStateMutex);
			M68000Word word1(data.GetUpperBits(BITCOUNT_WORD));
			M68000Word word2(data.GetLowerBits(BITCOUNT_WORD));
			memoryAccessTransparentFunctionCode = code;
			memoryAccessTransparentUpperDataStrobe = true;
			memoryAccessTransparentLowerDataStrobe = true;
			memoryAccessTransparentReadHighWriteLow = false;
			memoryAccessTransparentRMWCycleInProgress = rmwCycleInProgress;
			memoryAccessTransparentRMWCycleFirstOperation = rmwCycleFirstOperation;
			memoryBus->TransparentWriteMemory(location.GetDataSegment(0, 24), word1, GetDeviceContext());
			memoryBus->TransparentWriteMemory((location + 2).GetDataSegment(0, 24), word2, GetDeviceContext());
			break;
		}
	}
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int M68000::GetCELineID(const wchar_t* lineName, bool inputLine) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"LDS")
	{
		return CELINE_LDS;
	}
	else if(lineNameString == L"UDS")
	{
		return CELINE_UDS;
	}
	else if(lineNameString == L"R/W")
	{
		return CELINE_RW;
	}
	else if(lineNameString == L"AS")
	{
		return CELINE_AS;
	}
	else if(lineNameString == L"FC")
	{
		return CELINE_FC;
	}
	else if(lineNameString == L"FCCPUSpace")
	{
		return CELINE_FCCPUSPACE;
	}
	else if(lineNameString == L"RMWCycleInProgress")
	{
		return CELINE_RMWCYCLEINPROGRESS;
	}
	else if(lineNameString == L"RMWCycleFirstOperation")
	{
		return CELINE_RMWCYCLEFIRSTOPERATION;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void M68000::SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch(lineID)
	{
	case CELINE_LDS:
		ceLineMaskLowerDataStrobe = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_UDS:
		ceLineMaskUpperDataStrobe = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RW:
		ceLineMaskReadHighWriteLow = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_AS:
		ceLineMaskAddressStrobe = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_FC:
		ceLineBitNumberFunctionCode = !lineMapped? 0: lineStartBitNumber;
		ceLineMaskFunctionCode = !lineMapped? 0: 0x7 << lineStartBitNumber;
		break;
	case CELINE_RMWCYCLEINPROGRESS:
		ceLineMaskRMWCycleInProgress = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_FCCPUSPACE:
		ceLineMaskFCCPUSpace = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RMWCYCLEFIRSTOPERATION:
		ceLineMaskRMWCycleFirstOperation = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
unsigned int M68000::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, double accessTime) const
{
	unsigned int ceLineState = 0;
	if(caller == this->GetDeviceContext())
	{
		ceLineState |= memoryAccessUpperDataStrobe? ceLineMaskUpperDataStrobe: 0x0;
		ceLineState |= memoryAccessLowerDataStrobe? ceLineMaskLowerDataStrobe: 0x0;
		ceLineState |= memoryAccessReadHighWriteLow? ceLineMaskReadHighWriteLow: 0x0;
		ceLineState |= ceLineMaskAddressStrobe;
		ceLineState |= ((unsigned int)memoryAccessFunctionCode << ceLineBitNumberFunctionCode) & ceLineMaskFunctionCode;
		ceLineState |= (memoryAccessFunctionCode == 0x7)? ceLineMaskFCCPUSpace: 0x0;
		ceLineState |= memoryAccessRMWCycleInProgress? ceLineMaskRMWCycleInProgress: 0x0;
		ceLineState |= memoryAccessRMWCycleFirstOperation? ceLineMaskRMWCycleFirstOperation: 0x0;
	}
	return ceLineState;
}

//----------------------------------------------------------------------------------------
unsigned int M68000::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller) const
{
	unsigned int ceLineState = 0;
	if(caller == this->GetDeviceContext())
	{
		ceLineState |= memoryAccessTransparentUpperDataStrobe? ceLineMaskUpperDataStrobe: 0x0;
		ceLineState |= memoryAccessTransparentLowerDataStrobe? ceLineMaskLowerDataStrobe: 0x0;
		ceLineState |= memoryAccessTransparentReadHighWriteLow? ceLineMaskReadHighWriteLow: 0x0;
		ceLineState |= ceLineMaskAddressStrobe;
		ceLineState |= ((unsigned int)memoryAccessTransparentFunctionCode << ceLineBitNumberFunctionCode) & ceLineMaskFunctionCode;
		ceLineState |= (memoryAccessTransparentFunctionCode == 0x7)? ceLineMaskFCCPUSpace: 0x0;
		ceLineState |= memoryAccessTransparentRMWCycleInProgress? ceLineMaskRMWCycleInProgress: 0x0;
		ceLineState |= memoryAccessTransparentRMWCycleFirstOperation? ceLineMaskRMWCycleFirstOperation: 0x0;
	}
	return ceLineState;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void M68000::LoadState(IHeirarchicalStorageNode& node)
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
				if(registerName == L"A0")		a[0] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"A1")	a[1] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"A2")	a[2] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"A3")	a[3] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"A4")	a[4] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"A5")	a[5] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"A6")	a[6] = (*i)->ExtractHexData<unsigned int>();

				else if(registerName == L"D0")	d[0] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"D1")	d[1] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"D2")	d[2] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"D3")	d[3] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"D4")	d[4] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"D5")	d[5] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"D6")	d[6] = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"D7")	d[7] = (*i)->ExtractHexData<unsigned int>();

				else if(registerName == L"SSP")	ssp = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"USP")	usp = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"PC")	pc = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"SR")	sr = (*i)->ExtractHexData<unsigned int>();

				else if(registerName == L"LastReadBusData")	lastReadBusData = (*i)->ExtractHexData<unsigned int>();

				else if(registerName == L"ResetLineState")			resetLineState = (*i)->ExtractData<bool>();
				else if(registerName == L"HaltLineState")			haltLineState = (*i)->ExtractData<bool>();
				else if(registerName == L"BRLineState")				brLineState = (*i)->ExtractData<bool>();
				else if(registerName == L"BGLineState")				bgLineState = (*i)->ExtractData<bool>();
				else if(registerName == L"PendingInterruptLevel")	interruptPendingLevel = (*i)->ExtractData<unsigned int>();
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
					IHeirarchicalStorageAttribute* lineStateAttribute = (*lineAccessBufferEntry)->GetAttribute(L"LineState");
					IHeirarchicalStorageAttribute* accessTimeAttribute = (*lineAccessBufferEntry)->GetAttribute(L"AccessTime");
					if((lineNameAttribute != 0) && (lineStateAttribute != 0) && (accessTimeAttribute != 0))
					{
						//Extract the entry from the XML stream
						std::wstring lineName = lineNameAttribute->ExtractValue<std::wstring>();
						unsigned int lineID = GetLineID(lineName.c_str());
						if(lineID != 0)
						{
							Data lineState(GetLineWidth(lineID));
							lineStateAttribute->ExtractValue(lineState);
							double accessTime = accessTimeAttribute->ExtractValue<double>();
							LineAccess entry(lineID, lineState, accessTime);

							//Find the correct location in the list to insert the entry. The
							//list must be sorted from earliest to latest.
							std::list<LineAccess>::reverse_iterator j = lineAccessBuffer.rbegin();
							while((j != lineAccessBuffer.rend()) && (j->accessTime > entry.accessTime))
							{
								++j;
							}
							lineAccessBuffer.insert(j.base(), entry);
						}
					}
				}
			}
		}
	}

	Processor::LoadState(node);
}

//----------------------------------------------------------------------------------------
void M68000::GetState(IHeirarchicalStorageNode& node) const
{
	node.CreateChildHex(L"Register", a[0].GetData(), a[0].GetHexCharCount()).CreateAttribute(L"name", L"A0");
	node.CreateChildHex(L"Register", a[1].GetData(), a[1].GetHexCharCount()).CreateAttribute(L"name", L"A1");
	node.CreateChildHex(L"Register", a[2].GetData(), a[2].GetHexCharCount()).CreateAttribute(L"name", L"A2");
	node.CreateChildHex(L"Register", a[3].GetData(), a[3].GetHexCharCount()).CreateAttribute(L"name", L"A3");
	node.CreateChildHex(L"Register", a[4].GetData(), a[4].GetHexCharCount()).CreateAttribute(L"name", L"A4");
	node.CreateChildHex(L"Register", a[5].GetData(), a[5].GetHexCharCount()).CreateAttribute(L"name", L"A5");
	node.CreateChildHex(L"Register", a[6].GetData(), a[6].GetHexCharCount()).CreateAttribute(L"name", L"A6");

	node.CreateChildHex(L"Register", d[0].GetData(), d[0].GetHexCharCount()).CreateAttribute(L"name", L"D0");
	node.CreateChildHex(L"Register", d[1].GetData(), d[1].GetHexCharCount()).CreateAttribute(L"name", L"D1");
	node.CreateChildHex(L"Register", d[2].GetData(), d[2].GetHexCharCount()).CreateAttribute(L"name", L"D2");
	node.CreateChildHex(L"Register", d[3].GetData(), d[3].GetHexCharCount()).CreateAttribute(L"name", L"D3");
	node.CreateChildHex(L"Register", d[4].GetData(), d[4].GetHexCharCount()).CreateAttribute(L"name", L"D4");
	node.CreateChildHex(L"Register", d[5].GetData(), d[5].GetHexCharCount()).CreateAttribute(L"name", L"D5");
	node.CreateChildHex(L"Register", d[6].GetData(), d[6].GetHexCharCount()).CreateAttribute(L"name", L"D6");
	node.CreateChildHex(L"Register", d[7].GetData(), d[7].GetHexCharCount()).CreateAttribute(L"name", L"D7");

	node.CreateChildHex(L"Register", ssp.GetData(), ssp.GetHexCharCount()).CreateAttribute(L"name", L"SSP");
	node.CreateChildHex(L"Register", usp.GetData(), usp.GetHexCharCount()).CreateAttribute(L"name", L"USP");
	node.CreateChildHex(L"Register", pc.GetData(), pc.GetHexCharCount()).CreateAttribute(L"name", L"PC");
	node.CreateChildHex(L"Register", sr.GetData(), sr.GetHexCharCount()).CreateAttribute(L"name", L"SR");

	node.CreateChildHex(L"Register", lastReadBusData.GetData(), lastReadBusData.GetHexCharCount()).CreateAttribute(L"name", L"LastReadBusData");

	node.CreateChild(L"Register", resetLineState).CreateAttribute(L"name", L"ResetLineState");
	node.CreateChild(L"Register", haltLineState).CreateAttribute(L"name", L"HaltLineState");
	node.CreateChild(L"Register", brLineState).CreateAttribute(L"name", L"BRLineState");
	node.CreateChild(L"Register", bgLineState).CreateAttribute(L"name", L"BGLineState");
	node.CreateChild(L"Register", interruptPendingLevel).CreateAttribute(L"name", L"PendingInterruptLevel");

	//Save the lineAccessBuffer state
	if(lineAccessPending)
	{
		IHeirarchicalStorageNode& lineAccessState = node.CreateChild(L"LineAccessBuffer");
		for(std::list<LineAccess>::const_iterator i = lineAccessBuffer.begin(); i != lineAccessBuffer.end(); ++i)
		{
			IHeirarchicalStorageNode& lineAccessEntry = lineAccessState.CreateChild(L"LineAccess");
			lineAccessEntry.CreateAttribute(L"LineName", GetLineName(i->lineID));
			lineAccessEntry.CreateAttribute(L"LineState", i->state);
			lineAccessEntry.CreateAttribute(L"AccessTime", i->accessTime);
		}
	}

	Processor::GetState(node);
}

} //Close namespace M68000
