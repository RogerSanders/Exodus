#include "M68000.h"
#include "M68000Opcodes.pkg"
#include "Debug/Debug.pkg"
#include <iostream>
#include <iomanip>
#include <sstream>
namespace M68000 {

//----------------------------------------------------------------------------------------
M68000::M68000(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Processor(aimplementationName, ainstanceName, amoduleID), opcodeTable(16), opcodeBuffer(0), memoryBus(0)
{
	//Set the default state for our device preferences
	suspendWhenBusReleased = false;

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
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int M68000::GetIM68000Version() const
{
	return ThisIM68000Version();
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool M68000::Construct(IHierarchicalStorageNode& node)
{
	bool result = Processor::Construct(node);
	IHierarchicalStorageAttribute* suspendWhenBusReleasedAttribute = node.GetAttribute(L"SuspendWhenBusReleased");
	if(suspendWhenBusReleasedAttribute != 0)
	{
		suspendWhenBusReleased = suspendWhenBusReleasedAttribute->ExtractValue<bool>();
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool M68000::BuildDevice()
{
	bool result = Processor::BuildDevice();

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
			LogEntry logEntry(LogEntry::EventLevel::Critical);
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

	//Register each data source with the generic data access base class
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterSRX, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterSRN, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterSRZ, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterSRV, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterSRC, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterSRT, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterSRS, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterSRIPM, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0x07)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterPC, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFFFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterSR, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterCCR, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterSP, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFFFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterSSP, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFFFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterUSP, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFFFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterA, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFFFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IM68000DataSource::RegisterD, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFFFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));

	//Register page layouts for generic access to this device
	GenericAccessGroup* addressRegistersGroup = new GenericAccessGroup(L"Address Registers");
	for(unsigned int registerNo = 0; registerNo < addressRegCount; ++registerNo)
	{
		std::wstring registerNoAsString;
		IntToString(registerNo + 1, registerNoAsString);
		addressRegistersGroup->AddEntry((new GenericAccessGroupDataEntry(IM68000DataSource::RegisterA, L"A" + registerNoAsString))->SetDataContext(new RegisterDataContext(registerNo)));
	}
	GenericAccessGroup* dataRegistersGroup = new GenericAccessGroup(L"Data Registers");
	for(unsigned int registerNo = 0; registerNo < dataRegCount; ++registerNo)
	{
		std::wstring registerNoAsString;
		IntToString(registerNo + 1, registerNoAsString);
		dataRegistersGroup->AddEntry((new GenericAccessGroupDataEntry(IM68000DataSource::RegisterD, L"D" + registerNoAsString))->SetDataContext(new RegisterDataContext(registerNo)));
	}
	GenericAccessPage* registersPage = new GenericAccessPage(L"Registers");
	registersPage->AddEntry((new GenericAccessGroup(L"User Registers"))
	                 ->AddEntry(addressRegistersGroup)
	                 ->AddEntry(dataRegistersGroup)
	                 ->AddEntry((new GenericAccessGroup(L"CCR Flags"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterSRX, L"X"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterSRN, L"N"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterSRZ, L"Z"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterSRV, L"V"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterSRC, L"C")))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterPC, L"PC")))
	             ->AddEntry((new GenericAccessGroup(L"Supervisor Registers"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterUSP, L"USP"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterSSP, L"SSP"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterSRS, L"S"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterSRT, L"T"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterSRIPM, L"IPM"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IM68000DataSource::RegisterSR, L"SR")));
	result &= AddGenericAccessPage(registersPage);

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
	//Note that hardware tests have shown that all registers are not initialized to 0 when
	//the processor is first powered on. In fact, it would appear that most likely all
	//that is initialized is what is initialized by a normal reset operation, namely, PC,
	//SSP, and SR. We know that the data registers and address registers 0-6 are
	//uninitialized, and contain values that are similar to 0xFFFFFFFF, but the precise
	//bit values change on each boot attempt, usually with one or more bits in most
	//registers reading as 0 instead of 1. We set all registers explicitly to 0xFFFFFFFF
	//here, and allow the reset process to correctly initialize the registers that are set
	//by that operation.
	for(int i = 0; i < (addressRegCount - 1); ++i)
	{
		a[i] = 0xFFFFFFFF;
	}
	for(int i = 0; i < dataRegCount; ++i)
	{
		d[i] = 0xFFFFFFFF;
	}
	sr = 0xFFFF;
	pc = 0xFFFFFFFF;
	ssp = 0xFFFFFFFF;
	usp = 0xFFFFFFFF;
	wordIsPrefetched = false;
	powerOnDelayPending = true;

	//Abandon currently pending interrupts, and restore normal processor state
	interruptPendingLevel = 0;
	lastLineCheckTime = 0;
	lineAccessPending = false;
	lastTimesliceLength = 0;
	blastTimesliceLength = 0;
	lineAccessBuffer.clear();
	suspendUntilLineStateChangeReceived = false;
	manualDeviceAdvanceInProgress = false;
	resetLineState = false;
	haltLineState = false;
	brLineState = false;
	bgLineState = false;
	forceInterrupt = false;
	processorState = State::Normal;
	lastReadBusData = 0;

	//Trigger a reset exception to start execution
	Reset();

	//Synchronize the changed register state with the current register state
	PopulateChangedRegStateFromCurrentState();
}

//----------------------------------------------------------------------------------------
void M68000::Reset()
{
	//Queue the reset exception for the next cycle
	group0Vector = Exceptions::Reset;
	group0ExceptionPending = true;
}

//----------------------------------------------------------------------------------------
void M68000::BeginExecution()
{
	//Synchronize the changed register state with the current register state
	PopulateChangedRegStateFromCurrentState();
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool M68000::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target)
{
	if(referenceName == L"BusInterface")
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
//Exception functions
//----------------------------------------------------------------------------------------
double M68000::PushStackFrame(const M68000Long& apc, const M68000Word& asr, bool processingInstruction)
{
	//Push Group 1/2 exception stack frame
	double delayTime = 0.0;
	M68000Long stackPointer = GetSSP();
	stackPointer -= apc.GetByteSize();
	delayTime += WriteMemory(stackPointer, apc, FunctionCode::SupervisorData, apc, processingInstruction, 0, false, false);
	stackPointer -= asr.GetByteSize();
	delayTime += WriteMemory(stackPointer, asr, FunctionCode::SupervisorData, apc, processingInstruction, 0, false, false);
	SetSSP(stackPointer);
	return delayTime;
}

//----------------------------------------------------------------------------------------
double M68000::PushStackFrame(const M68000Long& apc, const M68000Word& asr, const M68000Word& ainstructionRegister, const M68000Long& aaccessAddress, bool aread, bool aprocessingInstruction, FunctionCode afunctionCode)
{
	//Push Group 0 Bus/Address error exception stack frame
	M68000Word statusFlags;
	statusFlags.SetDataSegment(0, 3, (unsigned int)afunctionCode);
	statusFlags.SetBit(3, !aprocessingInstruction);
	statusFlags.SetBit(4, aread);

	double delayTime = 0.0;
	M68000Long stackPointer = GetSSP();
	stackPointer -= apc.GetByteSize();
	delayTime += WriteMemory(stackPointer, apc, FunctionCode::SupervisorData, apc, false, 0, false, false);
	stackPointer -= asr.GetByteSize();
	delayTime += WriteMemory(stackPointer, asr, FunctionCode::SupervisorData, apc, false, 0, false, false);
	stackPointer -= ainstructionRegister.GetByteSize();
	delayTime += WriteMemory(stackPointer, ainstructionRegister, FunctionCode::SupervisorData, apc, false, 0, false, false);
	stackPointer -= aaccessAddress.GetByteSize();
	delayTime += WriteMemory(stackPointer, aaccessAddress, FunctionCode::SupervisorData, apc, false, 0, false, false);
	stackPointer -= statusFlags.GetByteSize();
	delayTime += WriteMemory(stackPointer, statusFlags, FunctionCode::SupervisorData, apc, false, 0, false, false);
	SetSSP(stackPointer);

	return delayTime;
}

//----------------------------------------------------------------------------------------
ExecuteTime M68000::ProcessException(Exceptions vector)
{
	//Log or break on this exception if requested
	ExceptionLogIfRequested(vector);
	ExceptionBreakIfRequested(vector);

	SetSR_S(true);
	SetSR_T(false);
	M68000Long interruptVector;
	SetProcessorState(State::Exception);

	//##DEBUG##
//	std::wcout << "Interrupt\t" << vector << '\n';

	unsigned int vectorOffset = (unsigned int)vector * interruptVector.GetByteSize();
	//According to the M68000 Users Manual, section 6.3.9, the PC is loaded with the
	//address of the exception vector while reading the vector from the vector table. We
	//emulate this behaviour here, to ensure the contents of the PC are correct when they
	//are pushed to the stack, if an address or bus error occurs during exception
	//processing.
	M68000Long oldPC = GetPC();
	SetPC(M68000Long(vectorOffset));
	if(vector == Exceptions::Reset)
	{
		ClearCallStack();
		SetSR_IPM(7);
		M68000Long newSSP;
		ReadMemory(vectorOffset, newSSP, FunctionCode::SupervisorProgram, 0, false, 0, false, false);
		AddDisassemblyAddressInfoOffset(vectorOffset, interruptVector.GetByteSize(), false, false, 0);
		SetA(SP, newSSP);
		ReadMemory(vectorOffset + newSSP.GetByteSize(), interruptVector, FunctionCode::SupervisorProgram, 0, false, 0, false, false);
		AddDisassemblyAddressInfoOffset(vectorOffset + newSSP.GetByteSize(), interruptVector.GetByteSize(), true, false, 0);
	}
	else
	{
		ReadMemory(vectorOffset, interruptVector, FunctionCode::SupervisorData, 0, false, 0, false, false);
		AddDisassemblyAddressInfoOffset(vectorOffset, interruptVector.GetByteSize(), true, false, 0);
	}

	PushCallStack(oldPC.GetData(), interruptVector.GetData(), oldPC.GetData(), L"Exception", true);
	SetPC(interruptVector);
	SetProcessorState(State::Normal);
	return GetExceptionProcessingTime(vector);
}

//----------------------------------------------------------------------------------------
ExecuteTime M68000::GetExceptionProcessingTime(Exceptions vector) const
{
	//These figures largely come from the M68000 Users Manual, section 8, page 11. We've
	//tweaked a couple of the numbers however. The users manual doesn't specify whether
	//the execution times listed for group 2 exceptions include the instruction execution
	//times. In the case of several instructions such as the trap instructions, it seems
	//the instruction execution times may be included. We've used several other references
	//and a few educated guesses to adjust some of the figures. There's a lot of dispute
	//between most sources about exactly how many clock cycles are required for each
	//exception, but these numbers should be pretty close.
	//##TODO## Perform hardware tests to get precise execution times for each exception.
	switch(vector)
	{
	case Exceptions::AddressError:
		return ExecuteTime(50, 4, 7);
	case Exceptions::BusError:
		return ExecuteTime(50, 4, 7);
	case Exceptions::CHKInstruction:
		return ExecuteTime(40, 4, 3);
	case Exceptions::ZeroDivide:
		return ExecuteTime(38, 4, 3);
	case Exceptions::Line1010:
	case Exceptions::Line1111:
	case Exceptions::IllegalInstruction:
		return ExecuteTime(34, 4, 3);
	case Exceptions::PrivilegeViolation:
		return ExecuteTime(34, 4, 3);
	case Exceptions::Reset:
		return ExecuteTime(40, 6, 0);
	case Exceptions::Trace:
		return ExecuteTime(34, 4, 3);
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0x0):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0x1):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0x2):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0x3):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0x4):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0x5):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0x6):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0x7):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0x8):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0x9):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0xA):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0xB):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0xC):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0xD):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0xE):
	case (Exceptions)((unsigned int)Exceptions::InterruptTrap0 + 0xF):
		return ExecuteTime(34, 4, 3);
	case Exceptions::TrapVInstruction:
		return ExecuteTime(30, 3, 3);
	default:
		//All other interrupts
		//##FIX## We have reason to believe that this interrupt execution time is too long
		//for user interrupts on the Mega Drive. Tests with horizontal interrupts on the
		//last line of the display indicate that horizontal interrupts need to be taken
		//more quickly by the M68000, so they can process far enough along before vertical
		//interrupts occur.
		return ExecuteTime(44, 5, 3); //Assuming the interrupt acknowledge cycle takes 4 clock cycles
		//##DEBUG##
		//return ExecuteTime(4, 5, 3);
	}
}

//----------------------------------------------------------------------------------------
bool M68000::ExceptionDisabled(Exceptions vector)
{
	//Check if all exceptions are disabled
	if(disableAllExceptions)
	{
		return true;
	}

	//Check if this specific exception is disabled
	if(!exceptionListEmpty)
	{
		std::unique_lock<std::mutex> lock(debugMutex);
		for(std::list<ExceptionDebuggingEntry>::const_iterator i = exceptionList.begin(); i != exceptionList.end(); ++i)
		{
			const ExceptionDebuggingEntry& entry = *i;
			if(entry.vectorNumber == vector)
			{
				return entry.disable;
			}
		}
	}

	//If no rule could be found to disable this exception, return false.
	return false;
}

//----------------------------------------------------------------------------------------
void M68000::ExceptionLogIfRequested(Exceptions vector)
{
	if(logAllExceptions)
	{
		//Log the event
		LogEntry logEntry(LogEntry::EventLevel::Debug);
		logEntry << L"Exception triggered. Exception number: 0x" << std::hex << std::uppercase << (unsigned int)vector << ", Exception address: 0x" << (unsigned int)vector * 4 << ", Exception name: " << GetExceptionName(vector) << ", Current PC: 0x" << GetPC().GetData();
		GetDeviceContext()->WriteLogEvent(logEntry);
		return;
	}

	if(!exceptionListEmpty)
	{
		std::unique_lock<std::mutex> lock(debugMutex);
		for(std::list<ExceptionDebuggingEntry>::const_iterator i = exceptionList.begin(); i != exceptionList.end(); ++i)
		{
			const ExceptionDebuggingEntry& entry = *i;
			if(entry.vectorNumber == vector)
			{
				if(entry.enableLogging)
				{
					//Log the event
					LogEntry logEntry(LogEntry::EventLevel::Debug);
					logEntry << L"Exception triggered. Exception number: 0x" << std::hex << std::uppercase << (unsigned int)vector << ", Exception address: 0x" << (unsigned int)vector * 4 << ", Exception name: " << GetExceptionName(vector) << ", Current PC: 0x" << GetPC().GetData();
					GetDeviceContext()->WriteLogEvent(logEntry);
					return;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void M68000::ExceptionBreakIfRequested(Exceptions vector)
{
	if(breakOnAllExceptions)
	{
		BreakOnCurrentOpcode();
		return;
	}

	if(!exceptionListEmpty)
	{
		std::unique_lock<std::mutex> lock(debugMutex);
		for(std::list<ExceptionDebuggingEntry>::const_iterator i = exceptionList.begin(); i != exceptionList.end(); ++i)
		{
			const ExceptionDebuggingEntry& entry = *i;
			if(entry.vectorNumber == vector)
			{
				if(entry.enableBreak)
				{
					BreakOnCurrentOpcode();
					return;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//Suspend functions
//----------------------------------------------------------------------------------------
bool M68000::UsesExecuteSuspend() const
{
	return suspendWhenBusReleased;
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

		std::unique_lock<std::mutex> lock(lineMutex);
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

			//Apply the line state change
			if(lineAccess.clockRateChange)
			{
				ApplyClockStateChange(lineAccess.clockID, lineAccess.clockRate);
			}
			else
			{
				ApplyLineStateChange(lineAccess.lineID, lineAccess.state, lock);
			}

			//If any threads were waiting for this line state change to be processed,
			//notify them that the change has now been applied.
			if(lineAccess.notifyWhenApplied)
			{
				*(lineAccess.appliedFlag) = true;
				if(lineAccess.waitingDevice != 0)
				{
					//Note that we need to release our lock in lineMutex here in order to
					//avoid a deadlock case we encountered. When we attempt to change a
					//device dependency state, there are case where we need to wait for
					//other devices to finish executing their current step. In the case
					//that one of those threads needs to obtain a lock on lineMutex, we
					//will get a deadlock if we're still holding it here.
					lock.unlock();
					GetDeviceContext()->SetDeviceDependencyEnable(lineAccess.waitingDevice, true);
					lock.lock();
				}
				advanceToTargetLineStateChanged.notify_all();
			}
		}
	}
	lastLineCheckTime = GetCurrentTimesliceProgress();

	//If no line access is pending, and we've decided to suspend until another line state
	//change is received, suspend execution waiting for another line state change to be
	//received, unless execution suspension has now been disabled.
	if(!lineAccessPending && suspendUntilLineStateChangeReceived && !manualDeviceAdvanceInProgress && !GetDeviceContext()->TimesliceSuspensionDisabled())
	{
		//Check lineAccessPending again after taking a lock on lineMutex. This will ensure
		//we never enter a suspend state when there are actually line access events
		//sitting in the buffer.
		std::unique_lock<std::mutex> lock(lineMutex);
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

	//If the reset line and halt line are both asserted, trigger a reset, and abort any
	//further processing. Note that both the halt and reset lines need to be asserted in
	//order to trigger a reset, as confirmed by the M68000 User's Manual, section 3.6.
	if(resetLineState && haltLineState)
	{
		Reset();
		return CalculateExecutionTime(cyclesExecuted) + additionalTime;
	}

	//If we don't have the bus or the HALT line is asserted, abort any further processing.
	if(bgLineState || haltLineState || !GetDeviceContext()->DeviceEnabled())
	{
		return CalculateExecutionTime(cyclesExecuted) + additionalTime;
	}

	//If an exception has been triggered from the debugger, process it.
	if(debugExceptionTriggerPending)
	{
		std::unique_lock<std::mutex> lock(debugMutex);
		debugExceptionTriggerPending = false;

		//Push the appropriate stack frame for this exception
		if(debugExceptionTriggerVector == Exceptions::Reset)
		{
			//No exception stack frame for a reset exception
		}
		else if((debugExceptionTriggerVector == Exceptions::BusError) || (debugExceptionTriggerVector == Exceptions::AddressError))
		{
			//Push a dummy group 0 exception stack frame. It's not really possible to give
			//real data here, since a real exception didn't occur.
			additionalTime += PushStackFrame(GetPC(), GetSR(), 0, 0, false, false, GetFunctionCode(true));
		}
		else
		{
			//Push a group 1/2 exception stack frame
			additionalTime += PushStackFrame(GetPC(), GetSR(), false);
		}

		//Process the exception
		cyclesExecuted = ProcessException(debugExceptionTriggerVector).cycles;
		return CalculateExecutionTime(cyclesExecuted) + additionalTime;
	}

	//Group 0 exception processing
	if(group0ExceptionPending)
	{
		group0ExceptionPending = false;
		if(!ExceptionDisabled(group0Vector))
		{
			if(group0Vector != Exceptions::Reset)
			{
				additionalTime += PushStackFrame(group0PC, group0SR, group0InstructionRegister, group0Address, group0ReadWriteFlag, group0InstructionFlag, group0FunctionCode);
			}
			cyclesExecuted = ProcessException(group0Vector).cycles;

			//Hardware tests on the Mega Drive have shown that the M68000 takes awhile to
			//initialize itself and begin executing instructions after a cold boot. We
			//have measured the approximate number of cycles for this case, and add it to
			//the exception execution time below.
			//##FIX## Mesure this to the exact cycle
			//##TODO## It's possible that the bus arbiter actually just holds the M68000
			//in a reset state for a period of time, and that's why this delay exists.
			//Perform hardware tests to determine if this is the case.
			//##FIX## We should actually add in this delay time on the very first step
			//after initialization, not on the first exception. It's quite possible for
			//the device to power up without bus ownership or in the reset state, in which
			//case, this delay time should still be added to the first execution step, not
			//the first exception that is processed.
			if(powerOnDelayPending)
			{
				static const unsigned int powerOnInitializationTime = 104167;
				cyclesExecuted += powerOnInitializationTime;
				powerOnDelayPending = false;
			}

			//If we've triggered a double bus fault, enter the halted state.
			if(group0ExceptionPending && (group0Vector != Exceptions::Reset))
			{
				group0ExceptionPending = false;
				SetProcessorState(State::Halted);
			}
			return CalculateExecutionTime(cyclesExecuted) + additionalTime;
		}
	}

	//If we're in a halted state, terminate instruction processing.
	if(processorState == State::Halted)
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
		M68000Word interruptVectorNumberAsWord = 0;
		CalculateCELineStateContext ceLineStateContext(FunctionCode::CPUSpace, true, true, true, false, false);
		IBusInterface::AccessResult accessResult = memoryBus->ReadMemory(interruptCycleAddress.GetData(), interruptVectorNumberAsWord, GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);

		//Mask the resulting interrupt vector number to a single byte. According to the
		//M68000 users manual, section 5.1.4, the M68000 does perform a 16-bit read for
		//the interrupt vector number, but only the lower byte is used: "Although a vector
		//number is one byte, both data strobes are asserted due to the microcode used for
		//exception processing. The processor does not recognize anything on data lines D8
		//through D15 at this time".
		interruptVectorNumberAsWord &= 0xFF;

		//Determine the result of the interrupt acknowledge cycle
		Exceptions interruptVectorNumber = (Exceptions)interruptVectorNumberAsWord.GetData();
		if(accessResult.busError)
		{
			//If a bus error occurred during the interrupt acknowledge cycle, a spurious
			//interrupt is triggered rather than a bus error exception.
			interruptVectorNumber = Exceptions::InterruptSpurious;
		}
		else if(autoVectorPendingInterrupt)
		{
			//If VPA was asserted during the interrupt acknowledge cycle, autovector the
			//interrupt.
			interruptVectorNumber = (Exceptions)((unsigned int)Exceptions::InterruptAutoVectorL1 + (interruptPendingLevel - 1));

			//Calculate the delay time between when we began the interrupt acknowledge
			//cycle, and when the VPA line was asserted, and use this as the execution
			//delay time.
			accessResult.executionTime = autoVectorPendingInterruptChangeTime - GetCurrentTimesliceProgress();
		}
		else if(!accessResult.deviceReplied)
		{
			//Critical error. No device responded to the interrupt acknowledge cycle. The
			//processor is now locked in the interrupt acknowledge cycle forever.
			SetProcessorState(State::Halted);
			return CalculateExecutionTime(cyclesExecuted) + additionalTime;
		}
		//##TODO## Confirm this behaviour. This is based on a best guess, that the M68000
		//probably drives the data lines low before starting the read, and considers the
		//interrupt vector number to be uninitialized if the resulting vector number is 0.
		//The official documentation doesn't clarify the exact conditions which can result
		//in this interrupt vector occurring however.
		else if((unsigned int)interruptVectorNumber == 0)
		{
			//If a device responded to the interrupt acknowledge cycle by asserting DTACK,
			//but failed to provide a vector number for the interrupt, we have an
			//uninitialized interrupt.
			interruptVectorNumber = Exceptions::InterruptUninitialized;
		}

		//Add any additional time from the memory read operation to the execution time
		additionalTime += accessResult.executionTime;

		//Process the interrupt
		if(!ExceptionDisabled(interruptVectorNumber))
		{
			additionalTime += PushStackFrame(GetPC(), GetSR(), false);
			SetSR_IPM(interruptPendingLevel);
			cyclesExecuted = ProcessException(interruptVectorNumber).cycles;
			return CalculateExecutionTime(cyclesExecuted) + additionalTime;
		}
	}

	//If the processor isn't stopped, fetch the next opcode.
	if(processorState != State::Stopped)
	{
		//Update the trace log, and test for breakpoints.
		RecordTrace(GetPC().GetData());
		CheckExecution(GetPC().GetData());

		M68000Word opcode = prefetchedWord;
		if(!wordIsPrefetched || (prefetchedWordAddress != GetPC()))
		{
			additionalTime += ReadMemory(GetPC(), opcode, GetFunctionCode(false), GetPC(), false, 0, false, false);
		}
		wordIsPrefetched = false;
		const M68000Instruction* nextOpcodeType = opcodeTable.GetInstruction(opcode.GetData());
		if(nextOpcodeType == 0)
		{
			//Generate an exception if we've encountered an unimplemented opcode
			Exceptions exception = Exceptions::IllegalInstruction;
			if((opcode & 0xF000) == 0xA000)
			{
				exception = Exceptions::Line1010;
			}
			else if((opcode & 0xF000) == 0xF000)
			{
				exception = Exceptions::Line1111;
			}
			if(!ExceptionDisabled(exception))
			{
				additionalTime += PushStackFrame(GetPC(), GetSR(), false);
				cyclesExecuted = ProcessException(exception).cycles;
			}
		}
		else
		{
			M68000Instruction* nextOpcode = 0;
//			nextOpcode = nextOpcodeType->Clone();
			nextOpcode = nextOpcodeType->ClonePlacement(opcodeBuffer);
			if(nextOpcode->Privileged() && !GetSR_S() && !ExceptionDisabled(Exceptions::PrivilegeViolation))
			{
				//Generate a privilege violation if the instruction is privileged and
				//we're not in supervisor mode.
				additionalTime += PushStackFrame(GetPC(), GetSR(), false);
				cyclesExecuted = ProcessException(Exceptions::PrivilegeViolation).cycles;
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
				additionalTime += ReadMemory(prefetchedWordAddress, prefetchedWord, GetFunctionCode(false), GetPC(), false, 0, false, false);

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
				if(trace && !group0ExceptionPending && !ExceptionDisabled(Exceptions::Trace))
				{
					additionalTime += PushStackFrame(GetPC(), GetSR(), false);
					cyclesExecuted += ProcessException(Exceptions::Trace).cycles;
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
	prefetchedWord = bprefetchedWord;
	prefetchedWordAddress = bprefetchedWordAddress;
	powerOnDelayPending = bpowerOnDelayPending;

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

	suspendUntilLineStateChangeReceived = bsuspendUntilLineStateChangeReceived;
	resetLineState = bresetLineState;
	haltLineState = bhaltLineState;
	brLineState = bbrLineState;
	bgLineState = bbgLineState;
	forceInterrupt = bforceInterrupt;
	interruptPendingLevel = binterruptPendingLevel;

	group0ExceptionPending = bgroup0ExceptionPending;
	group0InstructionRegister = bgroup0InstructionRegister;
	group0Address = bgroup0Address;
	group0PC = bgroup0PC;
	group0SR = bgroup0SR;
	group0ReadWriteFlag = bgroup0ReadWriteFlag;
	group0InstructionFlag = bgroup0InstructionFlag;
	group0Vector = bgroup0Vector;
	group0FunctionCode = bgroup0FunctionCode;

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
	bprefetchedWord = prefetchedWord;
	bprefetchedWordAddress = prefetchedWordAddress;
	bpowerOnDelayPending = powerOnDelayPending;

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

	bsuspendUntilLineStateChangeReceived = suspendUntilLineStateChangeReceived;
	bresetLineState = resetLineState;
	bhaltLineState = haltLineState;
	bbrLineState = brLineState;
	bbgLineState = bgLineState;
	bforceInterrupt = forceInterrupt;
	binterruptPendingLevel = interruptPendingLevel;

	bgroup0ExceptionPending = group0ExceptionPending;
	bgroup0InstructionRegister = group0InstructionRegister;
	bgroup0Address = group0Address;
	bgroup0PC = group0PC;
	bgroup0SR = group0SR;
	bgroup0ReadWriteFlag = group0ReadWriteFlag;
	bgroup0InstructionFlag = group0InstructionFlag;
	bgroup0Vector = group0Vector;
	bgroup0FunctionCode = group0FunctionCode;

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

	//Since a new timeslice is about to be sent, flag that we haven't yet reached the end
	//of the timeslice.
	executionReachedEndOfTimeslice = false;
}

//----------------------------------------------------------------------------------------
void M68000::NotifyAfterExecuteStepFinishedTimeslice()
{
	//If any threads were waiting for notifications about line state changes, wake them
	//now that we've reached the end of the timeslice.
	std::unique_lock<std::mutex> lock(lineMutex);
	executionReachedEndOfTimeslice = true;
	advanceToTargetLineStateChanged.notify_all();
}

//----------------------------------------------------------------------------------------
//Instruction functions
//----------------------------------------------------------------------------------------
unsigned int M68000::GetByteBitCount() const
{
	return 8;
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
unsigned int M68000::GetMinimumDataByteSize() const
{
	return 1;
}

//----------------------------------------------------------------------------------------
unsigned int M68000::GetMemorySpaceByte(unsigned int location) const
{
	M68000Byte data;
	ReadMemoryTransparent(location, data, FunctionCode::SupervisorProgram, false, false);
	return data.GetData();
}

//----------------------------------------------------------------------------------------
void M68000::SetMemorySpaceByte(unsigned int location, unsigned int data)
{
	M68000Byte byte(data);
	WriteMemoryTransparent(location, byte, FunctionCode::SupervisorProgram, false, false);
}

//----------------------------------------------------------------------------------------
bool M68000::GetOpcodeInfo(unsigned int location, IOpcodeInfo& opcodeInfo) const
{
	opcodeInfo.SetIsValidOpcode(false);

	M68000Long instructionLocation = location;
	M68000Word opcode;
	ReadMemoryTransparent(instructionLocation, opcode, FunctionCode::SupervisorProgram, false, false);

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
		LabelSubstitutionSettings labelSettings;
		labelSettings.enableSubstitution = false;
		M68000Instruction::Disassembly disassembly = targetOpcode->M68000Disassemble(labelSettings);

		opcodeInfo.SetIsValidOpcode(true);
		opcodeInfo.SetOpcodeSize(targetOpcode->GetInstructionSize());
		opcodeInfo.SetOpcodeNameDisassembly(disassembly.disassemblyOpcode);
		opcodeInfo.SetOpcodeArgumentsDisassembly(disassembly.disassemblyArguments);
		opcodeInfo.SetDisassemblyComment(disassembly.disassemblyComment);
		bool undeterminedResultantPCLocation;
		std::set<unsigned int> resultantPCLocations;
		targetOpcode->GetResultantPCLocations(resultantPCLocations, undeterminedResultantPCLocation);
		opcodeInfo.SetHasUndeterminedResultantPCLocation(undeterminedResultantPCLocation);
		opcodeInfo.SetResultantPCLocations(resultantPCLocations);
		std::set<unsigned int> labelTargetLocations;
		targetOpcode->GetLabelTargetLocations(labelTargetLocations);
		opcodeInfo.SetLabelTargetLocations(labelTargetLocations);

		delete targetOpcode;
	}

	return true;
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int M68000::GetLineID(const MarshalSupport::Marshal::In<std::wstring>& lineName) const
{
	if(lineName == L"RESET") //IO
	{
		return (unsigned int)LineID::RESET;
	}
	else if(lineName == L"BR") //I
	{
		return (unsigned int)LineID::BR;
	}
	else if(lineName == L"BG") //O
	{
		return (unsigned int)LineID::BG;
	}
	else if(lineName == L"HALT") //I
	{
		return (unsigned int)LineID::HALT;
	}
	else if(lineName == L"IPL") //I
	{
		return (unsigned int)LineID::IPL;
	}
	else if(lineName == L"VPA") //I
	{
		return (unsigned int)LineID::VPA;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> M68000::GetLineName(unsigned int lineID) const
{
	switch((LineID)lineID)
	{
	case LineID::RESET:
		return L"RESET";
	case LineID::BR:
		return L"BR";
	case LineID::BG:
		return L"BG";
	case LineID::HALT:
		return L"HALT";
	case LineID::IPL:
		return L"IPL";
	case LineID::VPA:
		return L"VPA";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int M68000::GetLineWidth(unsigned int lineID) const
{
	switch((LineID)lineID)
	{
	case LineID::RESET:
		return 1;
	case LineID::BR:
		return 1;
	case LineID::BG:
		return 1;
	case LineID::HALT:
		return 1;
	case LineID::IPL:
		return 3;
	case LineID::VPA:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void M68000::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//If this line state change is not a response to a request, we need to obtain an
	//exclusive lock. Note that if this is a line state change which we process
	//immediately, we technically don't need a lock for lineAccessBuffer, but we do need a
	//memory barrier here, to ensure that changes to any member variables of this object
	//are visible in other threads. We were getting a real-world issue where a change to
	//the bgLineState variable was not seen by the M68000 worker thread, and it executed
	//to the end of its timeslice assuming it was set, when it had actually been modified
	//by another thread.
	//##NOTE## Due to some code changes, the above comment about bgLineState no longer
	//applies.
	//##TODO## Add some memory barrier functions to our thread library. Boost doesn't have
	//any way to do just a memory barrier (yet). Wrap the concept up into a function that
	//can provide a memory barrier we can drop into our code, and use that in place of the
	//lock here, and shift the full-blown lock back below the immediate line state
	//changes.
	std::unique_lock<std::mutex> lock(lineMutex);

	//##DEBUG##
	//	std::wcout << "M68000SetLineState\t" << targetLine << '\n';

	//Check if this is a line which we need to process immediately. This would generally
	//indicate a response to a request which this device is currently asserting.
	switch((LineID)targetLine)
	{
	case LineID::VPA:
		autoVectorPendingInterrupt = true;
		autoVectorPendingInterruptChangeTime = accessTime;
		return;
	}

	//Flag that an entry exists in the buffer. This flag is used to skip the expensive
	//locking operation in the active thread for this device when no line changes are
	//pending. Note that we set this flag before we've actually written the entry into
	//the buffer so that the execution thread is aware of the line state change as soon as
	//possible, however the lock we've obtained on our line mutex will prevent the
	//execution thread from attempting to access the line access buffer until the data has
	//been written.
	lineAccessPending = true;

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(lastLineCheckTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//Insert the line access into the buffer. Note that entries in the buffer are sorted
	//by access time from lowest to highest.
	std::list<LineAccess>::reverse_iterator i = lineAccessBuffer.rbegin();
	while((i != lineAccessBuffer.rend()) && (i->accessTime > accessTime))
	{
		++i;
	}
	lineAccessBuffer.insert(i.base(), LineAccess((LineID)targetLine, lineData, accessTime));

	//Resume the main execution thread if it is currently suspended waiting for a line
	//state change to be received.
	GetDeviceContext()->ResumeTimesliceExecution();
}

//----------------------------------------------------------------------------------------
void M68000::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{
	SetLineState(targetLine, lineData, 0, lastTimesliceLength, 0);
}

//----------------------------------------------------------------------------------------
void M68000::RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(lineMutex);

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(lastLineCheckTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//Find the matching line state change entry in the line access buffer
	std::list<LineAccess>::reverse_iterator i = lineAccessBuffer.rbegin();
	bool foundTargetEntry = false;
	while(!foundTargetEntry && (i != lineAccessBuffer.rend()))
	{
		if((i->lineID == (LineID)targetLine) && (i->state == lineData) && (i->accessTime == reportedTime))
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
bool M68000::AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(lineMutex);
	bool result = false;
	if((LineID)targetLine == LineID::BG)
	{
		bool targetLineState = lineData.GetBit(0);
		if(bgLineState == targetLineState)
		{
			//If the current state of the target line matches the target state, we have
			//nothing to do.
			return true;
		}

		//In order to support non-destructive calls to this function, the only advance
		//requests we can respond to for the bus grant line are ones where a pending
		//change to the bus request line is sitting in the buffer, but has yet to be
		//processed.

		//Iterate through the line access buffer searching for a line state change which
		//will place the target line into the requested state. If we've reached the end of
		//the line access buffer, or we've managed to find a matching line state change,
		//and we've reached or passed the time at which the target line state was
		//requested, terminate the loop.
		bool foundTargetStateChange = false;
		LineAccess* matchingLineAccess = 0;
		std::list<LineAccess>::iterator i = lineAccessBuffer.begin();
		while((i != lineAccessBuffer.end()) && (!foundTargetStateChange || (i->accessTime <= accessTime)))
		{
			//If this line state change modifies the target line, latch the change if it
			//matches the requested state, otherwise clear any currently latched change.
			if(i->lineID == LineID::BR)
			{
				if(i->state == targetLineState)
				{
					foundTargetStateChange = true;
					matchingLineAccess = &(*i);
				}
				else
				{
					foundTargetStateChange = false;
					matchingLineAccess = 0;
				}
			}
			++i;
		}
		if(!foundTargetStateChange)
		{
			return false;
		}

		//Set this device as a waiting device on this line state change
		//##FIX## What if multiple devices want to wait on this change? Our implementation
		//below only allows for one waiting device.
		volatile bool targetLineStateChangeApplied = false;
		matchingLineAccess->appliedFlag = &targetLineStateChangeApplied;
		matchingLineAccess->waitingDevice = caller;
		matchingLineAccess->notifyWhenApplied = true;

		//Wait for the target line state change to be processed from the line access
		//buffer, or the end of the current timeslice to be reached. Note that we
		//suspend a dependency on the calling device if one is active. This is
		//essential in order to avoid deadlocks in the case where a dependent device of
		//this processor is the device that is triggering this advance request. In this
		//case, it's possible that we'll enter an infinite loop, since both devices could
		//end up waiting for each other to advance. We solve the issue here by temporarily
		//suspending the device dependency, until the advance request has been fulfilled.
		//The execute thread will restore the dependency itself when the state change is
		//applied. It is essential for the execute thread to restore the dependency rather
		//than us doing it here, otherwise the execute thread could possibly advance
		//further ahead without the dependency being active.
		//##NOTE## We've implemented the use of performingSingleDeviceStep here as a
		//solution for a deadlock. The problem occurs when the Z80 triggers a rollback,
		//and the operation it wants to step through at the timing point is to obtain the
		//M68000 bus for a read, but the VDP currently has it for a DMA operation. In this
		//case, this AdvanceToLineState function ends up getting called after the VDP has
		//flagged a future line state change to release the bus, and the M68000 needs to
		//advance up to the necessary point to process that change. Normally we would want
		//to suspend the dependency the M68000 has on the Z80 here and allow the M68000
		//execution thread to advance ahead, but in this case, no execution threads are
		//actually running, because the Z80 is being single stepped by the system
		//execution thread. We now determine if execution threads are running by checking
		//if a device is being single stepped here.
		if(!GetSystemInterface().PerformingSingleDeviceStep())
		{
			//Note that we need to release our lock in lineMutex here in order to avoid a
			//deadlock case we encountered. When we attempt to change a device dependency
			//state, there are case where we need to wait for other devices to finish
			//executing their current step. In the case that one of those threads needs to
			//obtain a lock on lineMutex, we will get a deadlock if we're still holding it
			//here.
			lock.unlock();
			GetDeviceContext()->SetDeviceDependencyEnable(caller, false);
			lock.lock();

			//##FIX## Using TimesliceExecutionCompleted() here is a temporary workaround for
			//our step through deadlock. Using this flag here is not thread safe, as it's
			//possible that the M68000 worker thread hasn't picked up the execute command yet.
			while(!targetLineStateChangeApplied && !executionReachedEndOfTimeslice && !GetDeviceContext()->TimesliceExecutionCompleted())
			{
				advanceToTargetLineStateChanged.wait(lock);
			}
		}

		//If the processor reached the end of the current timeslice without reaching the
		//target line state change in the buffer, advance the processor until the target
		//line state is reached.
		//##FIX## What about when a line state change occurs, but is reversed
		//within a short enough time period that it is immediately undone? We need
		//to make sure the M68000 doesn't actually change the BG line state in
		//this instance, since it only changes the BR line state when the BG line
		//state is latched, which if the line changes then changes back again
		//between bus cycles, it will not be.
		//##FIX## I don't like this execution model. What about the case where we
		//have dependent devices? This would allow us to execute out of step with
		//those devices. In our case though, it's a dependent device of the M68000
		//which is triggering the advance request indirectly, so that dependent
		//device can't advance any further. This is getting very complex.
		if(!targetLineStateChangeApplied)
		{
			//Since we're about to call ExecuteStep() manually within the context of this
			//thread, and ExecuteStep() needs to obtain the line mutex, we need to release
			//our lock here.
			lock.unlock();

			manualDeviceAdvanceInProgress = true;
			double adjustedTimesliceExecutionProgress = GetCurrentTimesliceProgress();
			while(!targetLineStateChangeApplied && !lineAccessBuffer.empty())
			{
				adjustedTimesliceExecutionProgress += ExecuteStep();
				SetCurrentTimesliceProgress(adjustedTimesliceExecutionProgress);
			}
			manualDeviceAdvanceInProgress = false;
		}

		result = true;
	}
	return result;
}

//----------------------------------------------------------------------------------------
void M68000::AssertCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(bgLineState) memoryBus->SetLineState((unsigned int)LineID::BG, Data(GetLineWidth((unsigned int)LineID::BG), 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void M68000::NegateCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(bgLineState) memoryBus->SetLineState((unsigned int)LineID::BG, Data(GetLineWidth((unsigned int)LineID::BG), 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void M68000::ApplyLineStateChange(LineID targetLine, const Data& lineData, std::unique_lock<std::mutex>& lock)
{
	//##DEBUG##
	//	std::wstringstream message;
	//	message << "M68000 line state change applied\t" << targetLine << '\t' << lineData.GetData() << "\n";
	//	std::wcout << message.str();

	switch(targetLine)
	{
	case LineID::RESET:
		resetLineState = !lineData.Zero();
		break;
	case LineID::BR:{
		bool brLineStateNew = !lineData.Zero();
		if(brLineState != brLineStateNew)
		{
			//If the BR line has been asserted, grant bus ownership to the external device
			//requesting the bus. If the BR line has been negated, reclaim bus ownership.
			brLineState = brLineStateNew;
			if(bgLineState != brLineState)
			{
				bgLineState = brLineState;

				//Release our lock on lineMutex. This is critical in order to avoid
				//deadlocks between devices if another device attempts to update the line
				//state for this device while we are updating the line state for that same
				//device, either directly or indirectly. There must never be a blocking
				//mutex held which would prevent a call to SetLineState on this device
				//succeeding when we are in tern calling SetLineState.
				lock.unlock();

				memoryBus->SetLineState((unsigned int)LineID::BG, Data(GetLineWidth((unsigned int)LineID::BG), (unsigned int)bgLineState), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);

				//Re-acquire the lock now that we've completed our external call
				lock.lock();
			}
		}
		break;}
	case LineID::HALT:
		haltLineState = !lineData.Zero();
		break;
	case LineID::IPL:{
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

	//Flag whether we want to suspend until another line state change is received, or we
	//reach the end of the current timeslice. We do this so that the M68000 doesn't
	//advance past the state of other devices in response to, for example, bus requests,
	//when we expect those events often to be brief. If the M68000 advances too far ahead,
	//when the bus is released for example, a rollback would need to be generated. This is
	//an optimization to try and avoid excessive rollbacks.
	suspendUntilLineStateChangeReceived = suspendWhenBusReleased && (haltLineState || resetLineState || bgLineState);
}

//----------------------------------------------------------------------------------------
//Clock source functions
//----------------------------------------------------------------------------------------
unsigned int M68000::GetClockSourceID(const MarshalSupport::Marshal::In<std::wstring>& clockSourceName) const
{
	if(clockSourceName == L"CLK")
	{
		return (unsigned int)ClockID::CLK;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> M68000::GetClockSourceName(unsigned int clockSourceID) const
{
	switch((ClockID)clockSourceID)
	{
	case ClockID::CLK:
		return L"CLK";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
void M68000::SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//We push clock rate changes through the normal line state change tracking system
	//here, since line state changes and clock changes are basically the same problem.
	std::unique_lock<std::mutex> lock(lineMutex);

	//Flag that an entry exists in the buffer. This flag is used to skip the expensive
	//locking operation in the active thread for this device when no line changes are
	//pending. Note that we set this flag before we've actually written the entry into
	//the buffer so that the execution thread is aware of the line state change as soon as
	//possible, however the lock we've obtained on our line mutex will prevent the
	//execution thread from attempting to access the line access buffer until the data has
	//been written.
	lineAccessPending = true;

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(lastLineCheckTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//Insert the line access into the buffer. Note that entries in the buffer are sorted
	//by access time from lowest to highest.
	std::list<LineAccess>::reverse_iterator i = lineAccessBuffer.rbegin();
	while((i != lineAccessBuffer.rend()) && (i->accessTime > accessTime))
	{
		++i;
	}
	lineAccessBuffer.insert(i.base(), LineAccess((ClockID)clockInput, clockRate, accessTime));

	//Resume the main execution thread if it is currently suspended waiting for a line
	//state change to be received.
	GetDeviceContext()->ResumeTimesliceExecution();
}

//----------------------------------------------------------------------------------------
void M68000::TransparentSetClockSourceRate(unsigned int clockInput, double clockRate)
{
	ApplyClockStateChange((ClockID)clockInput, clockRate);
}

//----------------------------------------------------------------------------------------
void M68000::ApplyClockStateChange(ClockID targetClock, double clockRate)
{
	//Apply the input clock rate change
	if(targetClock == ClockID::CLK)
	{
		SetClockSpeed(clockRate);
	}
}

//----------------------------------------------------------------------------------------
//Disassembly functions
//----------------------------------------------------------------------------------------
bool M68000::DisassemblyGetAddressRegisterLastAccessedInPostIncMode(unsigned int regNo) const
{
	if(regNo < (addressRegCount - 1))
	{
		return aDisassemblyInfo[regNo].addressRegisterLastUsedInPostIncMode;
	}
	return false;
}

//----------------------------------------------------------------------------------------
void M68000::DisassemblySetAddressRegisterLastAccessedInPostIncMode(unsigned int regNo, bool state)
{
	if(regNo < (addressRegCount - 1))
	{
		aDisassemblyInfo[regNo].addressRegisterLastUsedInPostIncMode = state;
	}
}

//----------------------------------------------------------------------------------------
unsigned int M68000::DisassemblyGetAddressRegisterCurrentArrayID(unsigned int regNo) const
{
	if(regNo < (addressRegCount - 1))
	{
		return aDisassemblyInfo[regNo].currentArrayID;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void M68000::DisassemblySetAddressRegisterCurrentArrayID(unsigned int regNo, unsigned int state)
{
	if(regNo < (addressRegCount - 1))
	{
		aDisassemblyInfo[regNo].currentArrayID = state;
	}
}

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
	memoryBus->SetLineState((unsigned int)M68000::LineID::RESET, Data(1, 1), GetDeviceContext(), GetDeviceContext(), resetTimeBegin, 0);
	memoryBus->SetLineState((unsigned int)M68000::LineID::RESET, Data(1, 0), GetDeviceContext(), GetDeviceContext(), resetTimeEnd, 0);
}

//----------------------------------------------------------------------------------------
//Memory access functions
//----------------------------------------------------------------------------------------
M68000::FunctionCode M68000::GetFunctionCode(bool programReference) const
{
	static const FunctionCode codeTable[8] = {
		FunctionCode::Undefined0,		//000
		FunctionCode::UserData,			//001
		FunctionCode::UserProgram,		//010
		FunctionCode::Undefined3,		//011
		FunctionCode::Undefined4,		//100
		FunctionCode::SupervisorData,	//101
		FunctionCode::SupervisorProgram,	//110
		FunctionCode::CPUSpace			//111
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
		group0InstructionRegister = instructionRegister;
		group0Address = location;
		group0PC = currentPC;
		group0SR = GetSR();
		group0ReadWriteFlag = true;
		group0InstructionFlag = !processingInstruction;
		group0Vector = Exceptions::AddressError;
		group0FunctionCode = code;
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
			DebugAssert(true);
			break;
		case BITCOUNT_BYTE:
			{
				M68000Word temp;
				bool odd = location.Odd();
				CalculateCELineStateContext ceLineStateContext(code, !odd, odd, true, rmwCycleInProgress, rmwCycleFirstOperation);
				result = memoryBus->ReadMemory(location.GetDataSegment(0, 24) & ~0x1, temp, GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);
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
				CalculateCELineStateContext ceLineStateContext(code, true, true, true, rmwCycleInProgress, rmwCycleFirstOperation);
				result = memoryBus->ReadMemory(location.GetDataSegment(0, 24), temp, GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);
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
				CalculateCELineStateContext ceLineStateContext(code, true, true, true, rmwCycleInProgress, rmwCycleFirstOperation);
				result = memoryBus->ReadMemory(location.GetDataSegment(0, 24), temp1, GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);
				result2 = memoryBus->ReadMemory((location + 2).GetDataSegment(0, 24), temp2, GetDeviceContext(), GetCurrentTimesliceProgress() + result.executionTime, 0, (void*)&ceLineStateContext);
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
			group0InstructionRegister = instructionRegister;
			group0Address = location;
			group0PC = currentPC;
			group0SR = GetSR();
			group0ReadWriteFlag = true;
			group0InstructionFlag = !processingInstruction;
			group0Vector = Exceptions::BusError;
			group0FunctionCode = code;
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
		DebugAssert(true);
		break;
	case BITCOUNT_BYTE:
		{
			M68000Word temp;
			if(location.Odd())
			{
				CalculateCELineStateContext ceLineStateContext(code, false, true, true, rmwCycleInProgress, rmwCycleFirstOperation);
				memoryBus->TransparentReadMemory(location.GetDataSegment(0, 24) & ~0x1, temp, GetDeviceContext(), 0, (void*)&ceLineStateContext);
				temp.GetLowerBits(data);
			}
			else
			{
				CalculateCELineStateContext ceLineStateContext(code, true, false, true, rmwCycleInProgress, rmwCycleFirstOperation);
				memoryBus->TransparentReadMemory(location.GetDataSegment(0, 24), temp, GetDeviceContext(), 0, (void*)&ceLineStateContext);
				temp.GetUpperBits(data);
			}
			break;
		}
	case BITCOUNT_WORD:
		{
			M68000Word temp;
			CalculateCELineStateContext ceLineStateContext(code, true, true, true, rmwCycleInProgress, rmwCycleFirstOperation);
			memoryBus->TransparentReadMemory(location.GetDataSegment(0, 24), temp, GetDeviceContext(), 0, (void*)&ceLineStateContext);
			data = temp;
			break;
		}
	case BITCOUNT_LONG:
		{
			M68000Word temp1;
			M68000Word temp2;
			CalculateCELineStateContext ceLineStateContext(code, true, true, true, rmwCycleInProgress, rmwCycleFirstOperation);
			memoryBus->TransparentReadMemory(location.GetDataSegment(0, 24), temp1, GetDeviceContext(), 0, (void*)&ceLineStateContext);
			memoryBus->TransparentReadMemory((location + 2).GetDataSegment(0, 24), temp2, GetDeviceContext(), 0, (void*)&ceLineStateContext);
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
		group0InstructionRegister = instructionRegister;
		group0Address = location;
		group0PC = currentPC;
		group0SR = GetSR();
		group0ReadWriteFlag = false;
		group0InstructionFlag = !processingInstruction;
		group0Vector = Exceptions::AddressError;
		group0FunctionCode = code;
	}
	else
	{
		//##DEBUG##
//		std::wcout << std::hex << std::uppercase << "M68000 Write:\t" << location.GetDataSegment(0, 24) << '\t' << data.GetData() << '\n';

		switch(data.GetBitCount())
		{
		default:
			DebugAssert(true);
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
					CalculateCELineStateContext ceLineStateContext(code, false, true, false, rmwCycleInProgress, rmwCycleFirstOperation);
					result = memoryBus->WriteMemory(location.GetDataSegment(0, 24) & ~0x1, tempData, GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);
				}
				else
				{
					CalculateCELineStateContext ceLineStateContext(code, true, false, false, rmwCycleInProgress, rmwCycleFirstOperation);
					result = memoryBus->WriteMemory(location.GetDataSegment(0, 24), tempData, GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);
				}
				break;
			}
		case BITCOUNT_WORD:
			{
				CalculateCELineStateContext ceLineStateContext(code, true, true, false, rmwCycleInProgress, rmwCycleFirstOperation);
				result = memoryBus->WriteMemory(location.GetDataSegment(0, 24), M68000Word(data), GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);
				break;
			}
		case BITCOUNT_LONG:
			{
				M68000Word word1(data.GetUpperBits(BITCOUNT_WORD));
				M68000Word word2(data.GetLowerBits(BITCOUNT_WORD));
				IBusInterface::AccessResult result2;
				CalculateCELineStateContext ceLineStateContext(code, true, true, false, rmwCycleInProgress, rmwCycleFirstOperation);
				result = memoryBus->WriteMemory(location.GetDataSegment(0, 24), word1, GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);
				result2 = memoryBus->WriteMemory((location + 2).GetDataSegment(0, 24), word2, GetDeviceContext(), GetCurrentTimesliceProgress() + result.executionTime, 0, (void*)&ceLineStateContext);
				result.busError |= result2.busError;
				result.executionTime += result2.executionTime;
				break;
			}
		}

		if(result.busError)
		{
			//Generate a bus error if communication failed
			group0ExceptionPending = true;
			group0InstructionRegister = instructionRegister;
			group0Address = location;
			group0PC = currentPC;
			group0SR = GetSR();
			group0ReadWriteFlag = false;
			group0InstructionFlag = !processingInstruction;
			group0Vector = Exceptions::BusError;
			group0FunctionCode = code;
			//##DEBUG##
			//std::wcout << "Bus error triggered on write of " << std::hex << location.GetData() << '\n';
		}
	}

	return result.executionTime;
}

//----------------------------------------------------------------------------------------
void M68000::WriteMemoryTransparent(const M68000Long& location, const Data& data, FunctionCode code, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	switch(data.GetBitCount())
	{
	default:
		DebugAssert(true);
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
				CalculateCELineStateContext ceLineStateContext(code, false, true, false, rmwCycleInProgress, rmwCycleFirstOperation);
				memoryBus->TransparentWriteMemory(location.GetDataSegment(0, 24) & ~0x1, tempData, GetDeviceContext(), 0, (void*)&ceLineStateContext);
			}
			else
			{
				CalculateCELineStateContext ceLineStateContext(code, true, false, false, rmwCycleInProgress, rmwCycleFirstOperation);
				memoryBus->TransparentWriteMemory(location.GetDataSegment(0, 24), tempData, GetDeviceContext(), 0, (void*)&ceLineStateContext);
			}
			break;
		}
	case BITCOUNT_WORD:
		{
			CalculateCELineStateContext ceLineStateContext(code, true, true, false, rmwCycleInProgress, rmwCycleFirstOperation);
			memoryBus->TransparentWriteMemory(location.GetDataSegment(0, 24), M68000Word(data), GetDeviceContext(), 0, (void*)&ceLineStateContext);
			break;
		}
	case BITCOUNT_LONG:
		{
			M68000Word word1(data.GetUpperBits(BITCOUNT_WORD));
			M68000Word word2(data.GetLowerBits(BITCOUNT_WORD));
			CalculateCELineStateContext ceLineStateContext(code, true, true, false, rmwCycleInProgress, rmwCycleFirstOperation);
			memoryBus->TransparentWriteMemory(location.GetDataSegment(0, 24), word1, GetDeviceContext(), 0, (void*)&ceLineStateContext);
			memoryBus->TransparentWriteMemory((location + 2).GetDataSegment(0, 24), word2, GetDeviceContext(), 0, (void*)&ceLineStateContext);
			break;
		}
	}
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int M68000::GetCELineID(const MarshalSupport::Marshal::In<std::wstring>& lineName, bool inputLine) const
{
	if(lineName == L"LDS")
	{
		return (unsigned int)CELineID::LDS;
	}
	else if(lineName == L"UDS")
	{
		return (unsigned int)CELineID::UDS;
	}
	else if(lineName == L"R/W")
	{
		return (unsigned int)CELineID::RW;
	}
	else if(lineName == L"AS")
	{
		return (unsigned int)CELineID::AS;
	}
	else if(lineName == L"FC")
	{
		return (unsigned int)CELineID::FC;
	}
	else if(lineName == L"FCCPUSpace")
	{
		return (unsigned int)CELineID::FCCPUSpace;
	}
	else if(lineName == L"RMWCycleInProgress")
	{
		return (unsigned int)CELineID::RMWCycleInProgress;
	}
	else if(lineName == L"RMWCycleFirstOperation")
	{
		return (unsigned int)CELineID::RMWCycleFirstOperation;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void M68000::SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch((CELineID)lineID)
	{
	case CELineID::LDS:
		ceLineMaskLowerDataStrobe = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::UDS:
		ceLineMaskUpperDataStrobe = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::RW:
		ceLineMaskReadHighWriteLow = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::AS:
		ceLineMaskAddressStrobe = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::FC:
		ceLineBitNumberFunctionCode = !lineMapped? 0: lineStartBitNumber;
		ceLineMaskFunctionCode = !lineMapped? 0: 0x7 << lineStartBitNumber;
		break;
	case CELineID::RMWCycleInProgress:
		ceLineMaskRMWCycleInProgress = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::FCCPUSpace:
		ceLineMaskFCCPUSpace = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::RMWCycleFirstOperation:
		ceLineMaskRMWCycleFirstOperation = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
unsigned int M68000::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const
{
	unsigned int ceLineState = 0;
	if((caller == GetDeviceContext()) && (calculateCELineStateContext != 0))
	{
		CalculateCELineStateContext& ceLineStateContext = *((CalculateCELineStateContext*)calculateCELineStateContext);
		ceLineState |= ceLineStateContext.upperDataStrobe? ceLineMaskUpperDataStrobe: 0x0;
		ceLineState |= ceLineStateContext.lowerDataStrobe? ceLineMaskLowerDataStrobe: 0x0;
		ceLineState |= ceLineStateContext.readHighWriteLow? ceLineMaskReadHighWriteLow: 0x0;
		ceLineState |= ceLineMaskAddressStrobe;
		ceLineState |= ((unsigned int)ceLineStateContext.functionCode << ceLineBitNumberFunctionCode) & ceLineMaskFunctionCode;
		ceLineState |= ((unsigned int)ceLineStateContext.functionCode == 0x7)? ceLineMaskFCCPUSpace: 0x0;
		ceLineState |= ceLineStateContext.rmwCycleInProgress? ceLineMaskRMWCycleInProgress: 0x0;
		ceLineState |= ceLineStateContext.rmwCycleFirstOperation? ceLineMaskRMWCycleFirstOperation: 0x0;
	}
	return ceLineState;
}

//----------------------------------------------------------------------------------------
unsigned int M68000::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const
{
	return CalculateCELineStateMemory(location, data, currentCELineState, sourceBusInterface, caller, calculateCELineStateContext, 0.0);
}

//----------------------------------------------------------------------------------------
//Active disassembly functions
//----------------------------------------------------------------------------------------
bool M68000::ActiveDisassemblySupported() const
{
	return true;
}

//----------------------------------------------------------------------------------------
//Active disassembly formatting functions
//----------------------------------------------------------------------------------------
bool M68000::GetLeadingLinesForASMFile(unsigned int analysisStartAddress, unsigned int analysisEndAddress, std::list<std::wstring>& outputLines) const
{
	if(analysisStartAddress != 0)
	{
		std::wstring baseAddressString;
		IntToStringBase16(analysisStartAddress, baseAddressString, GetPCCharWidth(), false);
		std::wstring baseAddressLine = L" org $" + baseAddressString;
		outputLines.push_back(baseAddressLine);
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool M68000::GetTrailingLinesForASMFile(unsigned int analysisStartAddress, unsigned int analysisEndAddress, std::list<std::wstring>& outputLines) const
{
	return true;
}

//----------------------------------------------------------------------------------------
bool M68000::FormatOpcodeForDisassembly(unsigned int opcodeAddress, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& opcodeArguments, std::wstring& opcodeComments) const
{
	M68000Long instructionLocation = opcodeAddress;
	M68000Word opcode;
	ReadMemoryTransparent(opcodeAddress, opcode, FunctionCode::SupervisorProgram, false, false);

	const M68000Instruction* targetOpcodeType = 0;
	targetOpcodeType = opcodeTable.GetInstruction(opcode.GetData());
	if(targetOpcodeType == 0)
	{
		return false;
	}
	M68000Instruction* targetOpcode = targetOpcodeType->Clone();

	targetOpcode->SetTransparentFlag(true);
	targetOpcode->SetInstructionSize(2);
	targetOpcode->SetInstructionLocation(instructionLocation);
	targetOpcode->SetInstructionRegister(opcode);

	targetOpcode->M68000Decode(this, targetOpcode->GetInstructionLocation(), targetOpcode->GetInstructionRegister(), targetOpcode->GetTransparentFlag());
	M68000Instruction::Disassembly disassembly = targetOpcode->M68000Disassemble(labelSettings);

	opcodePrefix = disassembly.disassemblyOpcode;
	opcodeArguments = disassembly.disassemblyArguments;
	opcodeComments = disassembly.disassemblyComment;

	delete targetOpcode;

	return true;
}

//----------------------------------------------------------------------------------------
bool M68000::FormatDataForDisassembly(const std::vector<Data>& dataElements, unsigned int dataElementByteSize, DisassemblyDataType dataType, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& formattedData) const
{
	//Ensure that at least one data element has been supplied
	if(dataElements.empty())
	{
		return false;
	}

	//Determine the size of the output data, and write the opcode prefix string.
	unsigned int outputElementBitCount = dataElementByteSize * Data::bitsPerByte;
	unsigned int displayCharWidth;
	switch(outputElementBitCount)
	{
	case BITCOUNT_BYTE:
		opcodePrefix = L"dc.b";
		displayCharWidth = 2;
		break;
	case BITCOUNT_WORD:
		opcodePrefix = L"dc.w";
		displayCharWidth = 4;
		break;
	case BITCOUNT_LONG:
		opcodePrefix = L"dc.l";
		displayCharWidth = 8;
		break;
	default:
		return false;
	}

	//Write each data value to the disassembly string
	formattedData.clear();
	if(dataType == DisassemblyDataType::Integer)
	{
		//Format these integer data elements
		bool firstValueWritten = false;
		for(unsigned int i = 0; i < (unsigned int)dataElements.size(); ++i)
		{
			std::wstring dataAsString;
			IntToStringBase16(dataElements[i].GetData(), dataAsString, displayCharWidth, false);
			formattedData += (firstValueWritten? L", $": L"$");
			formattedData += dataAsString;
			firstValueWritten = true;
		}
	}
	else if(dataType == DisassemblyDataType::Character)
	{
		//Ensure the text element size matches the required size
		if(dataElementByteSize != 1)
		{
			return false;
		}

		//Format these character data elements
		bool characterStringOpen = false;
		bool charactersWritten = false;
		for(unsigned int i = 0; i < (unsigned int)dataElements.size(); ++i)
		{
			char dataElementAsChar = (char)dataElements[i].GetData();
			bool printableChar = (isprint(dataElements[i].GetData()) != 0);
			if(printableChar)
			{
				if(!characterStringOpen)
				{
					if(charactersWritten)
					{
						formattedData += L",";
					}
					formattedData += L"\'";
					characterStringOpen = true;
				}
				if(dataElementAsChar == '\\')
				{
					formattedData += L"\\\\";
				}
				else if(dataElementAsChar == '\'')
				{
					formattedData += L"\'\'";
				}
				else
				{
					formattedData += dataElementAsChar;
				}
			}
			else
			{
				if(characterStringOpen)
				{
					formattedData += L"\'";
					characterStringOpen = false;
				}
				std::wstring nonPrintableCharAsIntString;
				IntToStringBase10(dataElements[i].GetData(), nonPrintableCharAsIntString);
				if(charactersWritten)
				{
					formattedData += L",";
				}
				formattedData += nonPrintableCharAsIntString;
			}
			charactersWritten = true;
		}
		if(characterStringOpen)
		{
			formattedData += L"\'";
		}
	}
	else
	{
		//If this is an unsupported data type, return false.
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool M68000::FormatOffsetForDisassembly(const Data& offsetData, bool relativeOffset, unsigned int relativeOffsetBaseAddress, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& formattedOffset) const
{
	//Determine the size of the output data, and write the opcode prefix string.
	unsigned int outputElementBitCount = offsetData.GetBitCount();
	unsigned int displayCharWidth;
	switch(outputElementBitCount)
	{
	case BITCOUNT_BYTE:
		opcodePrefix = L"dc.b";
		displayCharWidth = 2;
		break;
	case BITCOUNT_WORD:
		opcodePrefix = L"dc.w";
		displayCharWidth = 4;
		break;
	case BITCOUNT_LONG:
		opcodePrefix = L"dc.l";
		displayCharWidth = 8;
		break;
	default:
		return false;
	}

	//Format this offset element
	if(relativeOffset)
	{
		unsigned int offsetTarget = relativeOffsetBaseAddress + offsetData.GetData();

		bool baseAddressLabelFound = false;
		bool targetLabelFound = false;
		std::wstring offsetBaseAddressAsString;
		std::wstring offsetTargetAsString;
		if(labelSettings.enableSubstitution)
		{
			std::map<unsigned int, LabelSubstitutionEntry>::const_iterator offsetTargetLabelIterator = labelSettings.labelTargetsAddressingLocation.find(offsetTarget);
			if(offsetTargetLabelIterator != labelSettings.labelTargetsAddressingLocation.end())
			{
				offsetTargetAsString = offsetTargetLabelIterator->second.usageLabel;
				targetLabelFound = true;
			}
			std::map<unsigned int, LabelSubstitutionEntry>::const_iterator offsetBaseAddressLabelIterator = labelSettings.labelTargetsAddressingLocation.find(relativeOffsetBaseAddress);
			if(offsetBaseAddressLabelIterator != labelSettings.labelTargetsAddressingLocation.end())
			{
				offsetBaseAddressAsString = offsetBaseAddressLabelIterator->second.usageLabel;
				baseAddressLabelFound = true;
			}
		}
		if(!targetLabelFound)
		{
			std::wstring offsetTargetAsRawString;
			IntToStringBase16(offsetTarget, offsetTargetAsRawString, GetPCCharWidth(), false);
			offsetTargetAsString = L"$" + offsetTargetAsRawString;
		}
		if(!baseAddressLabelFound)
		{
			std::wstring offsetBaseAddressAsRawString;
			IntToStringBase16(relativeOffsetBaseAddress, offsetBaseAddressAsRawString, GetPCCharWidth(), false);
			offsetBaseAddressAsString = L"$" + offsetBaseAddressAsRawString;
		}

		formattedOffset = L"(";
		formattedOffset += offsetTargetAsString;
		formattedOffset += L"-";
		formattedOffset += offsetBaseAddressAsString;
		formattedOffset += L")";
	}
	else
	{
		bool targetLabelFound = false;
		std::wstring offsetTargetAsString;
		if(labelSettings.enableSubstitution)
		{
			std::map<unsigned int, LabelSubstitutionEntry>::const_iterator offsetTargetLabelIterator = labelSettings.labelTargetsAddressingLocation.find(offsetData.GetData());
			if(offsetTargetLabelIterator != labelSettings.labelTargetsAddressingLocation.end())
			{
				offsetTargetAsString = offsetTargetLabelIterator->second.usageLabel;
				targetLabelFound = true;
			}
		}
		if(!targetLabelFound)
		{
			std::wstring offsetTargetAsRawString;
			IntToStringBase16(offsetData.GetData(), offsetTargetAsRawString, GetPCCharWidth(), false);
			offsetTargetAsString = L"$" + offsetTargetAsRawString;
		}

		formattedOffset = offsetTargetAsString;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool M68000::FormatCommentForDisassembly(const std::wstring& rawComment, std::wstring& formattedComment) const
{
	formattedComment = L";" + rawComment;
	return true;
}

//----------------------------------------------------------------------------------------
bool M68000::FormatLabelPlacementForDisassembly(const std::wstring& rawLabel, std::wstring& formattedLabel) const
{
	formattedLabel = rawLabel + L":";
	return true;
}

//----------------------------------------------------------------------------------------
bool M68000::FormatLabelUsageForDisassembly(const std::wstring& rawLabel, int labelOffset, std::wstring& formattedLabel) const
{
	formattedLabel = rawLabel;
	if(labelOffset != 0)
	{
		if(labelOffset < 0)
		{
			std::wstring locationOffsetString;
			IntToString(-labelOffset, locationOffsetString);
			formattedLabel += L"-" + locationOffsetString;
		}
		else
		{
			std::wstring locationOffsetString;
			IntToString(labelOffset, locationOffsetString);
			formattedLabel += L"+" + locationOffsetString;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Exception debugging functions
//----------------------------------------------------------------------------------------
bool M68000::GetLogAllExceptions() const
{
	return logAllExceptions;
}

//----------------------------------------------------------------------------------------
void M68000::SetLogAllExceptions(bool state)
{
	logAllExceptions = state;
}

//----------------------------------------------------------------------------------------
bool M68000::GetBreakOnAllExceptions() const
{
	return breakOnAllExceptions;
}

//----------------------------------------------------------------------------------------
void M68000::SetBreakOnAllExceptions(bool state)
{
	breakOnAllExceptions = state;
}

//----------------------------------------------------------------------------------------
bool M68000::GetDisableAllExceptions() const
{
	return disableAllExceptions;
}

//----------------------------------------------------------------------------------------
void M68000::SetDisableAllExceptions(bool state)
{
	disableAllExceptions = state;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<M68000::ExceptionDebuggingEntry>> M68000::GetExceptionDebugEntries() const
{
	std::unique_lock<std::mutex> lock(debugMutex);
	return exceptionList;
}

//----------------------------------------------------------------------------------------
void M68000::SetExceptionDebugEntries(const MarshalSupport::Marshal::In<std::list<ExceptionDebuggingEntry>>& state)
{
	std::unique_lock<std::mutex> lock(debugMutex);
	exceptionList = state;
	exceptionListEmpty = exceptionList.empty();
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> M68000::GetExceptionName(Exceptions vectorNumber) const
{
	switch(vectorNumber)
	{
	case Exceptions::Reset:
		return L"Reset";
	case Exceptions::Invalid01:
		return L"Invalid";
	case Exceptions::BusError:
		return L"Bus Error";
	case Exceptions::AddressError:
		return L"Address Error";
	case Exceptions::IllegalInstruction:
		return L"Illegal Instruction";
	case Exceptions::ZeroDivide:
		return L"Zero Divide";
	case Exceptions::CHKInstruction:
		return L"CHK Instruction";
	case Exceptions::TrapVInstruction:
		return L"TRAPV Instruction";
	case Exceptions::PrivilegeViolation:
		return L"Privilege Violation";
	case Exceptions::Trace:
		return L"Trace";
	case Exceptions::Line1010:
		return L"Line 1010 Emulator";
	case Exceptions::Line1111:
		return L"Line 1111 Emulator";
	case Exceptions::InterruptUninitialized:
		return L"Uninitialized Interrupt Vector";
	case Exceptions::InterruptSpurious:
		return L"Spurious Interrupt";
	case Exceptions::InterruptAutoVectorL1:
		return L"L1 Interrupt Autovector";
	case Exceptions::InterruptAutoVectorL2:
		return L"L2 Interrupt Autovector";
	case Exceptions::InterruptAutoVectorL3:
		return L"L3 Interrupt Autovector";
	case Exceptions::InterruptAutoVectorL4:
		return L"L4 Interrupt Autovector";
	case Exceptions::InterruptAutoVectorL5:
		return L"L5 Interrupt Autovector";
	case Exceptions::InterruptAutoVectorL6:
		return L"L6 Interrupt Autovector";
	case Exceptions::InterruptAutoVectorL7:
		return L"L7 Interrupt Autovector";
	case Exceptions::InterruptTrap0:
		return L"Trap $0";
	case Exceptions::InterruptTrap1:
		return L"Trap $1";
	case Exceptions::InterruptTrap2:
		return L"Trap $2";
	case Exceptions::InterruptTrap3:
		return L"Trap $3";
	case Exceptions::InterruptTrap4:
		return L"Trap $4";
	case Exceptions::InterruptTrap5:
		return L"Trap $5";
	case Exceptions::InterruptTrap6:
		return L"Trap $6";
	case Exceptions::InterruptTrap7:
		return L"Trap $7";
	case Exceptions::InterruptTrap8:
		return L"Trap $8";
	case Exceptions::InterruptTrap9:
		return L"Trap $9";
	case Exceptions::InterruptTrapA:
		return L"Trap $A";
	case Exceptions::InterruptTrapB:
		return L"Trap $B";
	case Exceptions::InterruptTrapC:
		return L"Trap $C";
	case Exceptions::InterruptTrapD:
		return L"Trap $D";
	case Exceptions::InterruptTrapE:
		return L"Trap $E";
	case Exceptions::InterruptTrapF:
		return L"Trap $F";
	case Exceptions::Reserved0C:
	case Exceptions::Reserved0D:
	case Exceptions::Reserved0E:
	case Exceptions::Reserved10:
	case Exceptions::Reserved11:
	case Exceptions::Reserved12:
	case Exceptions::Reserved13:
	case Exceptions::Reserved14:
	case Exceptions::Reserved15:
	case Exceptions::Reserved16:
	case Exceptions::Reserved17:
	case Exceptions::Reserved30:
	case Exceptions::Reserved31:
	case Exceptions::Reserved32:
	case Exceptions::Reserved33:
	case Exceptions::Reserved34:
	case Exceptions::Reserved35:
	case Exceptions::Reserved36:
	case Exceptions::Reserved37:
	case Exceptions::Reserved38:
	case Exceptions::Reserved39:
	case Exceptions::Reserved3A:
	case Exceptions::Reserved3B:
	case Exceptions::Reserved3C:
	case Exceptions::Reserved3D:
	case Exceptions::Reserved3E:
	case Exceptions::Reserved3F:
		return L"Reserved";
	default:{
		std::wstringstream nameStream;
		nameStream << L"User Interrupt 0x" << std::hex << std::uppercase << (unsigned int)vectorNumber;
		return nameStream.str();}
	}
}

//----------------------------------------------------------------------------------------
void M68000::TriggerException(Exceptions vectorNumber)
{
	std::unique_lock<std::mutex> lock(debugMutex);
	debugExceptionTriggerVector = vectorNumber;
	debugExceptionTriggerPending = true;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void M68000::LoadState(IHierarchicalStorageNode& node)
{
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"Register")
		{
			IHierarchicalStorageAttribute* nameAttribute = (*i)->GetAttribute(L"name");
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
				else if(registerName == L"ProcessorState")	processorState = (State)(*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"WordIsPrefetched")	wordIsPrefetched = (*i)->ExtractData<bool>();
				else if(registerName == L"PrefetchedWord")	prefetchedWord = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"PrefetchedWordAddress")	prefetchedWordAddress = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"PowerOnDelayPending")	powerOnDelayPending = (*i)->ExtractData<bool>();

				else if(registerName == L"Group0ExceptionPending")	group0ExceptionPending = (*i)->ExtractData<bool>();
				else if(registerName == L"Group0InstructionRegister")	group0InstructionRegister = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"Group0Address")	group0Address = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"Group0PC")	group0PC = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"Group0SR")	group0SR = (*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"Group0ReadWriteFlag")	group0ReadWriteFlag = (*i)->ExtractData<bool>();
				else if(registerName == L"Group0InstructionFlag")	group0InstructionFlag = (*i)->ExtractData<bool>();
				else if(registerName == L"Group0Vector")	group0Vector = (Exceptions)(*i)->ExtractHexData<unsigned int>();
				else if(registerName == L"Group0FunctionCode")	group0FunctionCode = (FunctionCode)(*i)->ExtractHexData<unsigned int>();

				else if(registerName == L"LastTimesliceLength")		lastTimesliceLength = (*i)->ExtractData<double>();
				else if(registerName == L"SuspendUntilLineStateChangeReceived")		suspendUntilLineStateChangeReceived = (*i)->ExtractData<bool>();
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
			lineAccessBuffer.clear();
			IHierarchicalStorageNode& lineAccessBufferNode = *(*i);
			std::list<IHierarchicalStorageNode*> lineAccessBufferChildList = lineAccessBufferNode.GetChildList();
			for(std::list<IHierarchicalStorageNode*>::iterator lineAccessBufferEntry = lineAccessBufferChildList.begin(); lineAccessBufferEntry != lineAccessBufferChildList.end(); ++lineAccessBufferEntry)
			{
				if((*lineAccessBufferEntry)->GetName() == L"LineAccess")
				{
					IHierarchicalStorageAttribute* lineNameAttribute = (*lineAccessBufferEntry)->GetAttribute(L"LineName");
					IHierarchicalStorageAttribute* clockRateChangeAttribute = (*lineAccessBufferEntry)->GetAttribute(L"ClockRateChange");
					IHierarchicalStorageAttribute* accessTimeAttribute = (*lineAccessBufferEntry)->GetAttribute(L"AccessTime");
					if((lineNameAttribute != 0) && (clockRateChangeAttribute != 0) && (accessTimeAttribute != 0))
					{
						//Extract the entry from the XML stream
						LineAccess lineAccess;
						bool lineAccessDefined = false;
						std::wstring lineName = lineNameAttribute->ExtractValue<std::wstring>();
						bool clockRateChange = clockRateChangeAttribute->ExtractValue<bool>();
						double accessTime = accessTimeAttribute->ExtractValue<double>();
						if(clockRateChange)
						{
							IHierarchicalStorageAttribute* clockRateAttribute = (*lineAccessBufferEntry)->GetAttribute(L"ClockRate");
							if(clockRateAttribute != 0)
							{
								unsigned int lineID = GetClockSourceID(lineName.c_str());
								if(lineID != 0)
								{
									double clockRate;
									clockRateAttribute->ExtractValue(clockRate);
									lineAccess = LineAccess((ClockID)lineID, clockRate, accessTime);
									lineAccessDefined = true;
								}
							}
						}
						else
						{
							IHierarchicalStorageAttribute* lineStateAttribute = (*lineAccessBufferEntry)->GetAttribute(L"LineState");
							if(lineStateAttribute != 0)
							{
								unsigned int lineID = GetLineID(lineName.c_str());
								if(lineID != 0)
								{
									Data lineState(GetLineWidth(lineID));
									lineStateAttribute->ExtractValue(lineState);
									lineAccess.state.Resize(lineState.GetBitCount());
									lineAccess = LineAccess((LineID)lineID, lineState, accessTime);
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
void M68000::SaveState(IHierarchicalStorageNode& node) const
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
	node.CreateChildHex(L"Register", (unsigned int)processorState, 1).CreateAttribute(L"name", L"ProcessorState");
	node.CreateChild(L"Register", wordIsPrefetched).CreateAttribute(L"name", L"WordIsPrefetched");
	node.CreateChildHex(L"Register", prefetchedWord.GetData(), prefetchedWord.GetHexCharCount()).CreateAttribute(L"name", L"PrefetchedWord");
	node.CreateChildHex(L"Register", prefetchedWordAddress.GetData(), prefetchedWordAddress.GetHexCharCount()).CreateAttribute(L"name", L"PrefetchedWordAddress");
	node.CreateChild(L"Register", powerOnDelayPending).CreateAttribute(L"name", L"PowerOnDelayPending");

	node.CreateChild(L"Register", group0ExceptionPending).CreateAttribute(L"name", L"Group0ExceptionPending");
	node.CreateChildHex(L"Register", group0InstructionRegister.GetData(), group0InstructionRegister.GetHexCharCount()).CreateAttribute(L"name", L"Group0InstructionRegister");
	node.CreateChildHex(L"Register", group0Address.GetData(), group0Address.GetHexCharCount()).CreateAttribute(L"name", L"Group0Address");
	node.CreateChildHex(L"Register", group0PC.GetData(), group0PC.GetHexCharCount()).CreateAttribute(L"name", L"Group0PC");
	node.CreateChildHex(L"Register", group0SR.GetData(), group0SR.GetHexCharCount()).CreateAttribute(L"name", L"Group0SR");
	node.CreateChild(L"Register", group0ReadWriteFlag).CreateAttribute(L"name", L"Group0ReadWriteFlag");
	node.CreateChild(L"Register", group0InstructionFlag).CreateAttribute(L"name", L"Group0InstructionFlag");
	node.CreateChildHex(L"Register", (unsigned int)group0Vector, 2).CreateAttribute(L"name", L"Group0Vector");
	node.CreateChildHex(L"Register", (unsigned int)group0FunctionCode, 1).CreateAttribute(L"name", L"Group0FunctionCode");

	node.CreateChild(L"Register", lastTimesliceLength).CreateAttribute(L"name", L"LastTimesliceLength");
	node.CreateChild(L"Register", suspendUntilLineStateChangeReceived).CreateAttribute(L"name", L"SuspendUntilLineStateChangeReceived");
	node.CreateChild(L"Register", resetLineState).CreateAttribute(L"name", L"ResetLineState");
	node.CreateChild(L"Register", haltLineState).CreateAttribute(L"name", L"HaltLineState");
	node.CreateChild(L"Register", brLineState).CreateAttribute(L"name", L"BRLineState");
	node.CreateChild(L"Register", bgLineState).CreateAttribute(L"name", L"BGLineState");
	node.CreateChild(L"Register", interruptPendingLevel).CreateAttribute(L"name", L"PendingInterruptLevel");

	//Save the lineAccessBuffer state
	if(lineAccessPending)
	{
		IHierarchicalStorageNode& lineAccessState = node.CreateChild(L"LineAccessBuffer");
		for(std::list<LineAccess>::const_iterator i = lineAccessBuffer.begin(); i != lineAccessBuffer.end(); ++i)
		{
			IHierarchicalStorageNode& lineAccessEntry = lineAccessState.CreateChild(L"LineAccess");
			lineAccessEntry.CreateAttribute(L"ClockRateChange", i->clockRateChange);
			if(i->clockRateChange)
			{
				lineAccessEntry.CreateAttribute(L"LineName", GetClockSourceName((unsigned int)i->lineID));
				lineAccessEntry.CreateAttribute(L"ClockRate", i->clockRate);
			}
			else
			{
				lineAccessEntry.CreateAttribute(L"LineName", GetLineName((unsigned int)i->lineID));
				lineAccessEntry.CreateAttribute(L"LineState", i->state);
			}
			lineAccessEntry.CreateAttribute(L"AccessTime", i->accessTime);
		}
	}

	Processor::SaveState(node);
}

//----------------------------------------------------------------------------------------
void M68000::LoadDebuggerState(IHierarchicalStorageNode& node)
{
	std::unique_lock<std::mutex> lock(debugMutex);

	//Exception debugging
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		std::wstring keyName = (*i)->GetName();
		if(keyName == L"Register")
		{
			IHierarchicalStorageAttribute* nameAttribute = (*i)->GetAttribute(L"name");
			if(nameAttribute != 0)
			{
				std::wstring registerName = nameAttribute->GetValue();
				//Exception debugging
				if(registerName == L"LogAllExceptions")          logAllExceptions = (*i)->ExtractData<bool>();
				else if(registerName == L"BreakOnAllExceptions") breakOnAllExceptions = (*i)->ExtractData<bool>();
				else if(registerName == L"DisableAllExceptions") disableAllExceptions = (*i)->ExtractData<bool>();
			}
		}
		else if(keyName == L"ExceptionDebugList")
		{
			exceptionList.clear();
			std::list<IHierarchicalStorageNode*> childList = (*i)->GetChildList();
			for(std::list<IHierarchicalStorageNode*>::iterator childNodeIterator = childList.begin(); childNodeIterator != childList.end(); ++childNodeIterator)
			{
				IHierarchicalStorageNode& childNode = *(*childNodeIterator);
				if(childNode.GetName() == L"ExceptionDebugListEntry")
				{
					ExceptionDebuggingEntry entry;
					unsigned int vectorNumber;
					childNode.ExtractAttribute(L"VectorNumber", vectorNumber);
					entry.vectorNumber = (Exceptions)vectorNumber;
					childNode.ExtractAttribute(L"EnableLogging", entry.enableLogging);
					childNode.ExtractAttribute(L"EnableBreak", entry.enableBreak);
					childNode.ExtractAttribute(L"Disable", entry.disable);
					exceptionList.push_back(entry);
				}
			}
			exceptionListEmpty = exceptionList.empty();
		}
	}

	Processor::LoadDebuggerState(node);
}

//----------------------------------------------------------------------------------------
void M68000::SaveDebuggerState(IHierarchicalStorageNode& node) const
{
	std::unique_lock<std::mutex> lock(debugMutex);

	//Exception debugging
	IHierarchicalStorageNode& exceptionListNode = node.CreateChild(L"ExceptionDebugList");
	for(std::list<ExceptionDebuggingEntry>::const_iterator i = exceptionList.begin(); i != exceptionList.end(); ++i)
	{
		const ExceptionDebuggingEntry& entry = *i;
		IHierarchicalStorageNode& exceptionListEntry = exceptionListNode.CreateChild(L"ExceptionDebugListEntry");
		exceptionListEntry.CreateAttributeHex(L"VectorNumber", (unsigned int)entry.vectorNumber, 8);
		exceptionListEntry.CreateAttribute(L"EnableLogging", entry.enableLogging);
		exceptionListEntry.CreateAttribute(L"EnableBreak", entry.enableBreak);
		exceptionListEntry.CreateAttribute(L"Disable", entry.disable);
	}
	node.CreateChild(L"Register", logAllExceptions).CreateAttribute(L"name", L"LogAllExceptions");
	node.CreateChild(L"Register", breakOnAllExceptions).CreateAttribute(L"name", L"BreakOnAllExceptions");
	node.CreateChild(L"Register", disableAllExceptions).CreateAttribute(L"name", L"DisableAllExceptions");

	Processor::SaveDebuggerState(node);
}

//----------------------------------------------------------------------------------------
//Data read/write functions
//----------------------------------------------------------------------------------------
bool M68000::ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const
{
	ApplyGenericDataValueDisplaySettings(dataID, dataValue);
	switch((IM68000DataSource)dataID)
	{
	case IM68000DataSource::RegisterSRX:{
		return dataValue.SetValue(GetX());}
	case IM68000DataSource::RegisterSRN:
		return dataValue.SetValue(GetN());
	case IM68000DataSource::RegisterSRZ:
		return dataValue.SetValue(GetZ());
	case IM68000DataSource::RegisterSRV:
		return dataValue.SetValue(GetV());
	case IM68000DataSource::RegisterSRC:
		return dataValue.SetValue(GetC());
	case IM68000DataSource::RegisterSRT:
		return dataValue.SetValue(GetSR_T());
	case IM68000DataSource::RegisterSRS:
		return dataValue.SetValue(GetSR_S());
	case IM68000DataSource::RegisterSRIPM:
		return dataValue.SetValue(GetSR_IPM());
	case IM68000DataSource::RegisterPC:
		return dataValue.SetValue(GetPC().GetData());
	case IM68000DataSource::RegisterSR:
		return dataValue.SetValue(GetSR().GetData());
	case IM68000DataSource::RegisterCCR:
		return dataValue.SetValue(GetCCR().GetData());
	case IM68000DataSource::RegisterSP:
		return dataValue.SetValue(GetSP().GetData());
	case IM68000DataSource::RegisterSSP:
		return dataValue.SetValue(GetSSP().GetData());
	case IM68000DataSource::RegisterUSP:
		return dataValue.SetValue(GetUSP().GetData());
	case IM68000DataSource::RegisterA:{
		const RegisterDataContext& registerDataContext = *((RegisterDataContext*)dataContext);
		return dataValue.SetValue(GetA(registerDataContext.registerNo).GetData());}
	case IM68000DataSource::RegisterD:{
		const RegisterDataContext& registerDataContext = *((RegisterDataContext*)dataContext);
		return dataValue.SetValue(GetD(registerDataContext.registerNo).GetData());}
	}
	return Processor::ReadGenericData(dataID, dataContext, dataValue);
}

//----------------------------------------------------------------------------------------
bool M68000::WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue)
{
	ApplyGenericDataValueLimitSettings(dataID, dataValue);
	IGenericAccessDataValue::DataType dataType = dataValue.GetType();
	switch((IM68000DataSource)dataID)
	{
	case IM68000DataSource::RegisterSRX:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetX(dataValueAsBool.GetValue());
		return true;}
	case IM68000DataSource::RegisterSRN:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetN(dataValueAsBool.GetValue());
		return true;}
	case IM68000DataSource::RegisterSRZ:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetZ(dataValueAsBool.GetValue());
		return true;}
	case IM68000DataSource::RegisterSRV:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetV(dataValueAsBool.GetValue());
		return true;}
	case IM68000DataSource::RegisterSRC:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetC(dataValueAsBool.GetValue());
		return true;}
	case IM68000DataSource::RegisterSRT:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetSR_T(dataValueAsBool.GetValue());
		return true;}
	case IM68000DataSource::RegisterSRS:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetSR_S(dataValueAsBool.GetValue());
		return true;}
	case IM68000DataSource::RegisterSRIPM:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetSR_IPM(dataValueAsUInt.GetValue());
		return true;}
	case IM68000DataSource::RegisterPC:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		pc = dataValueAsUInt.GetValue();
		return true;}
	case IM68000DataSource::RegisterSR:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		sr = dataValueAsUInt.GetValue();
		return true;}
	case IM68000DataSource::RegisterCCR:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		sr.SetLowerBits(5, dataValueAsUInt.GetValue());
		return true;}
	case IM68000DataSource::RegisterSP:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetSP(M68000Long(dataValueAsUInt.GetValue()));
		return true;}
	case IM68000DataSource::RegisterSSP:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		ssp = dataValueAsUInt.GetValue();
		return true;}
	case IM68000DataSource::RegisterUSP:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		usp = dataValueAsUInt.GetValue();
		return true;}
	case IM68000DataSource::RegisterA:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		const RegisterDataContext& registerDataContext = *((RegisterDataContext*)dataContext);
		if(registerDataContext.registerNo == SP)
		{
			SetSP(M68000Long(dataValueAsUInt.GetValue()));
		}
		else
		{
			a[registerDataContext.registerNo] = dataValueAsUInt.GetValue();
		}
		return true;}
	case IM68000DataSource::RegisterD:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		const RegisterDataContext& registerDataContext = *((RegisterDataContext*)dataContext);
		d[registerDataContext.registerNo] = dataValueAsUInt.GetValue();
		return true;}
	}
	return Processor::WriteGenericData(dataID, dataContext, dataValue);
}

//----------------------------------------------------------------------------------------
//Highlight functions
//----------------------------------------------------------------------------------------
bool M68000::GetGenericDataHighlightState(unsigned int dataID, const DataContext* dataContext) const
{
	switch((IM68000DataSource)dataID)
	{
	case IM68000DataSource::RegisterSRX:
		return (regChangedX != GetX());
	case IM68000DataSource::RegisterSRN:
		return (regChangedN != GetN());
	case IM68000DataSource::RegisterSRZ:
		return (regChangedZ != GetZ());
	case IM68000DataSource::RegisterSRV:
		return (regChangedV != GetV());
	case IM68000DataSource::RegisterSRC:
		return (regChangedV != GetC());
	case IM68000DataSource::RegisterSRT:
		return (regChangedT != GetSR_T());
	case IM68000DataSource::RegisterSRS:
		return (regChangedS != GetSR_S());
	case IM68000DataSource::RegisterSRIPM:
		return (regChangedIPM != GetSR_IPM());
	case IM68000DataSource::RegisterPC:
		return (regChangedPC != GetPC().GetData());
	case IM68000DataSource::RegisterSR:
		return (regChangedSR != GetSR().GetData());
	case IM68000DataSource::RegisterCCR:
		return (regChangedCCR != GetCCR().GetData());
	case IM68000DataSource::RegisterSP:
		return (regChangedSP != GetSP().GetData());
	case IM68000DataSource::RegisterSSP:
		return (regChangedSSP != GetSSP().GetData());
	case IM68000DataSource::RegisterUSP:
		return (regChangedUSP != GetUSP().GetData());
	case IM68000DataSource::RegisterA:{
		const RegisterDataContext& registerDataContext = *((RegisterDataContext*)dataContext);
		return (regChangedA[registerDataContext.registerNo] != GetA(registerDataContext.registerNo).GetData());}
	case IM68000DataSource::RegisterD:{
		const RegisterDataContext& registerDataContext = *((RegisterDataContext*)dataContext);
		return (regChangedD[registerDataContext.registerNo] != GetD(registerDataContext.registerNo).GetData());}
	}
	return Processor::GetGenericDataHighlightState(dataID, dataContext);
}

//----------------------------------------------------------------------------------------
void M68000::PopulateChangedRegStateFromCurrentState()
{
	//Save a snapshot of the current register state for changed register tracking
	for(unsigned int i = 0; i < addressRegCount; ++i)
	{
		regChangedA[i] = GetA(i).GetData();
	}
	for(unsigned int i = 0; i < dataRegCount; ++i)
	{
		regChangedD[i] = GetD(i).GetData();
	}
	regChangedPC = GetPC().GetData();
	regChangedX = GetX();
	regChangedN = GetN();
	regChangedZ = GetZ();
	regChangedV = GetV();
	regChangedC = GetC();
	regChangedSP = GetSP().GetData();
	regChangedUSP = GetUSP().GetData();
	regChangedSSP = GetSSP().GetData();
	regChangedS = GetSR_S();
	regChangedT = GetSR_T();
	regChangedIPM = GetSR_IPM();
	regChangedSR = GetSR().GetData();
	regChangedCCR = GetCCR().GetData();
}

} //Close namespace M68000
