#include "Z80.h"
#include "Z80Opcodes.pkg"
//##DEBUG##
#include <iostream>
namespace Z80{

//----------------------------------------------------------------------------------------
Z80::Z80(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Processor(aimplementationName, ainstanceName, amoduleID), opcodeTable(8), opcodeTableCB(8), opcodeTableED(8), opcodeBuffer(0), memoryBus(0)
{
	//Set the default state for our device preferences
	suspendWhenBusReleased = false;

	//Initialize our CE line state
	ceLineMaskRD = 0;
	ceLineMaskWR = 0;

	//Ensure we don't think a reset was triggered on the last processor step
	resetLastStep = false;
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
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int Z80::GetIZ80Version() const
{
	return ThisIZ80Version();
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool Z80::Construct(IHierarchicalStorageNode& node)
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
bool Z80::BuildDevice()
{
	bool result = Processor::BuildDevice();

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
			LogEntry logEntry(LogEntry::EventLevel::Critical);
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
			LogEntry logEntry(LogEntry::EventLevel::Critical);
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
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterA, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterF, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterB, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterC, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterD, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterE, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterH, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterL, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterAF, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterBC, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterDE, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterHL, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterA2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterF2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterB2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterC2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterD2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterE2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterH2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterL2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterAF2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterBC2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterDE2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterHL2, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterIXHigh, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterIXLow, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterIYHigh, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterIYLow, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterI, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterR, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterIX, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterIY, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterSP, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterPC, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(0xFFFF)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterIM, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(2)->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterIFF1, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterIFF2, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterFlagS, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterFlagZ, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterFlagY, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterFlagH, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterFlagX, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterFlagPV, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterFlagN, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(IZ80DataSource::RegisterFlagC, IGenericAccessDataValue::DataType::Bool))->SetHighlightUsed(true));

	//Register page layouts for generic access to this device
	GenericAccessPage* registersPage = new GenericAccessPage(L"Registers");
	registersPage->AddEntry((new GenericAccessGroup(L"Raw Registers"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterAF, L"AF"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterBC, L"BC"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterDE, L"DE"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterHL, L"HL"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterAF2, L"AF'"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterBC2, L"BC'"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterDE2, L"DE'"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterHL2, L"HL'"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterIX, L"IX"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterIY, L"IY"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterPC, L"PC"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterSP, L"SP"))
	             ->AddEntry((new GenericAccessGroup(L"Virtual Registers"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterA, L"A"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterF, L"F"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterB, L"B"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterC, L"C"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterD, L"D"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterE, L"E"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterH, L"H"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterL, L"L"))
	                 ->AddEntry((new GenericAccessGroup(L"Flags"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterFlagS, L"S"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterFlagZ, L"Z"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterFlagY, L"Y"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterFlagH, L"H"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterFlagX, L"X"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterFlagPV, L"PV"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterFlagN, L"N"))
	                     ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterFlagC, L"C"))))
	             ->AddEntry((new GenericAccessGroup(L"Private Registers"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterIFF1, L"IFF1"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterIFF2, L"IFF2"))
	                 ->AddEntry((new GenericAccessGroupSingleSelectionList(IZ80DataSource::RegisterIM, L"IM"))->SetAllowNewItemEntry(true)
	                     ->AddSelectionListEntry(new GenericAccessDataValueString(L"0 (Instruction on Bus)"), new GenericAccessDataValueUInt(0))
	                     ->AddSelectionListEntry(new GenericAccessDataValueString(L"1 (Call 38h)"), new GenericAccessDataValueUInt(1))
	                     ->AddSelectionListEntry(new GenericAccessDataValueString(L"2 (Vector on Bus)"), new GenericAccessDataValueUInt(2)))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterI, L"I"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(IZ80DataSource::RegisterR, L"R"))));
	result &= AddGenericAccessPage(registersPage);

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

	lastLineCheckTime = 0;
	lineAccessPending = false;
	resetLineState = false;
	busreqLineState = false;
	busackLineState = false;
	intLineState = false;
	nmiLineState = false;
	lastTimesliceLength = 0;
	lineAccessBuffer.clear();
	suspendUntilLineStateChangeReceived = false;

	Reset();

	//Synchronize the changed register state with the current register state
	PopulateChangedRegStateFromCurrentState();
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
void Z80::BeginExecution()
{
	//Synchronize the changed register state with the current register state
	PopulateChangedRegStateFromCurrentState();
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool Z80::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target)
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

			//##DEBUG##
			//std::wstringstream logMessage;
			//logMessage << L"Z80 Line state change: " << std::setprecision(16) << lineaccess.accessTime << '\t' << currentTimesliceProgress << '\t' << lineAccess.lineID << '\t' << i->state.GetData();
			//LogEntry logEntry(LogEntry::EventLevel::DEBUG, logMessage.str());
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

	//Apply any active effects from input lines to the processor
	bool processorNotExecuting = resetLineState || busreqLineState || !GetDeviceContext()->DeviceEnabled();
	if(processorNotExecuting)
	{
		if(resetLineState && !resetLastStep)
		{
			//Note that we remember when we reset on the last step as an optimization.
			//Since we execute a step every cycle when the reset line is held asserted,
			//we don't want to perform the reset steps every single step repeatedly when
			//no processor state has changed.
			Reset();
			resetLastStep = true;
		}
		return CalculateExecutionTime(cyclesExecuted) + additionalTime;
	}
	resetLastStep = false;

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
			sp.SetMode(EffectiveAddress::Mode::SPPreDec);
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
				GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"Z80 Attempted interrupt mode 0"));

				cyclesExecuted = 13;
			}
			else if(GetInterruptMode() == 1)
			{
				//##DEBUG##
				//std::wcout << "Z80 executed interrupt mode 1\n";

				//Write the return address to the stack
				EffectiveAddress sp;
				sp.SetMode(EffectiveAddress::Mode::SPPreDec);
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
				GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"Z80 Attempted interrupt mode 2"));

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
		EffectiveAddress::IndexState indexState = EffectiveAddress::IndexState::None;

		//Read the first byte of the instruction
		additionalTime += ReadMemory(readLocation++, opcode, false);
		++instructionSize;
		//If the first byte is a prefix byte, process it, and read the second byte.
		if((opcode == 0xDD) || (opcode == 0xFD))
		{
			AddRefresh(1);
			if(opcode == 0xDD)
			{
				indexState = EffectiveAddress::IndexState::IX;
			}
			else if(opcode == 0xFD)
			{
				indexState = EffectiveAddress::IndexState::IY;
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
			if(indexState != EffectiveAddress::IndexState::None)
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
			indexState = EffectiveAddress::IndexState::None;

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
	busackLineState = bbusackLineState;
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
	bbusackLineState = busackLineState;
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
unsigned int Z80::GetLineID(const MarshalSupport::Marshal::In<std::wstring>& lineName) const
{
	if(lineName == L"RESET")
	{
		return (unsigned int)LineID::Reset;
	}
	else if(lineName == L"BUSREQ")
	{
		return (unsigned int)LineID::BusReq;
	}
	else if(lineName == L"BUSACK")
	{
		return (unsigned int)LineID::BusAck;
	}
	else if(lineName == L"INT")
	{
		return (unsigned int)LineID::Int;
	}
	else if(lineName == L"NMI")
	{
		return (unsigned int)LineID::Nmi;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Z80::GetLineName(unsigned int lineID) const
{
	switch((LineID)lineID)
	{
	case LineID::Reset:
		return L"RESET";
	case LineID::BusReq:
		return L"BUSREQ";
	case LineID::BusAck:
		return L"BUSACK";
	case LineID::Int:
		return L"INT";
	case LineID::Nmi:
		return L"NMI";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int Z80::GetLineWidth(unsigned int lineID) const
{
	switch((LineID)lineID)
	{
	case LineID::Reset:
		return 1;
	case LineID::BusReq:
		return 1;
	case LineID::BusAck:
		return 1;
	case LineID::Int:
		return 1;
	case LineID::Nmi:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void Z80::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(lineMutex);

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
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
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
void Z80::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{
	SetLineState(targetLine, lineData, 0, lastTimesliceLength, 0);
}

//----------------------------------------------------------------------------------------
void Z80::RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext)
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
		std::wcout << "Failed to find matching line state change in RevokeSetLineState! " << GetLineName(targetLine) << '\t' << lineData.GetData() << '\t' << std::setprecision(24) << reportedTime << '\t' << std::setprecision(24) << accessTime << '\n';
	}

	//Update the lineAccessPending flag
	lineAccessPending = !lineAccessBuffer.empty();
}

//----------------------------------------------------------------------------------------
void Z80::AssertCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(busackLineState) memoryBus->SetLineState((unsigned int)LineID::BusAck, Data(GetLineWidth((unsigned int)LineID::BusAck), 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void Z80::NegateCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(busackLineState) memoryBus->SetLineState((unsigned int)LineID::BusAck, Data(GetLineWidth((unsigned int)LineID::BusAck), 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void Z80::ApplyLineStateChange(unsigned int targetLine, const Data& lineData, std::unique_lock<std::mutex>& lock)
{
	//##DEBUG##
//	std::wstringstream message;
//	message << "Z80 line state change applied\t" << targetLine << '\t' << lineData.GetData() << "\n";
//	std::wcout << message.str();

	switch(targetLine)
	{
	case LineID::Reset:
		resetLineState = lineData.NonZero();
		break;
	case LineID::BusReq:{
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
			busackLineState = busreqLineState;
			memoryBus->SetLineState((unsigned int)LineID::BusAck, Data(GetLineWidth((unsigned int)LineID::BusAck), (unsigned int)busackLineState), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);

			//Re-acquire the lock now that we've completed our external call
			lock.lock();
		}
		break;}
	case LineID::Int:
		intLineState = lineData.NonZero();
		break;
	case LineID::Nmi:
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
unsigned int Z80::GetClockSourceID(const MarshalSupport::Marshal::In<std::wstring>& clockSourceName) const
{
	if(clockSourceName == L"CLK")
	{
		return (unsigned int)ClockID::Clk;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> Z80::GetClockSourceName(unsigned int clockSourceID) const
{
	switch((ClockID)clockSourceID)
	{
	case ClockID::Clk:
		return L"CLK";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
void Z80::SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//We push clock rate changes through the normal line state change tracking system
	//here, since line state changes and clock changes are basically the same problem.
	std::unique_lock<std::mutex> lock(lineMutex);

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
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
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
	if((ClockID)targetClock == ClockID::Clk)
	{
		SetClockSpeed(clockRate);
	}
}

//----------------------------------------------------------------------------------------
//Instruction functions
//----------------------------------------------------------------------------------------
unsigned int Z80::GetByteBitCount() const
{
	return 8;
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
unsigned int Z80::GetMinimumDataByteSize() const
{
	return 1;
}

//----------------------------------------------------------------------------------------
unsigned int Z80::GetMemorySpaceByte(unsigned int location) const
{
	Z80Byte data;
	ReadMemory(location, data, true);
	return data.GetData();
}

//----------------------------------------------------------------------------------------
void Z80::SetMemorySpaceByte(unsigned int location, unsigned int data)
{
	Z80Byte byte(data);
	WriteMemory(location, byte, true);
}

//----------------------------------------------------------------------------------------
bool Z80::GetOpcodeInfo(unsigned int location, IOpcodeInfo& opcodeInfo) const
{
	opcodeInfo.SetIsValidOpcode(false);

	Z80Word instructionLocation = location;
	Z80Word readLocation = location;
	bool mandatoryIndexOffset = false;
	Z80Byte indexOffset;
	const Z80Instruction* nextOpcodeType = 0;
	unsigned int instructionSize = 0;
	Z80Byte opcode;
	EffectiveAddress::IndexState indexState = EffectiveAddress::IndexState::None;

	//Read the first byte of the instruction
	ReadMemory(readLocation++, opcode, true);
	++instructionSize;
	//If the first byte is a prefix byte, process it, and read the second byte.
	if((opcode == 0xDD) || (opcode == 0xFD))
	{
		if(opcode == 0xDD)
		{
			indexState = EffectiveAddress::IndexState::IX;
		}
		else if(opcode == 0xFD)
		{
			indexState = EffectiveAddress::IndexState::IY;
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
		if(indexState != EffectiveAddress::IndexState::None)
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
		indexState = EffectiveAddress::IndexState::None;

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
		LabelSubstitutionSettings labelSettings;
		labelSettings.enableSubstitution = false;
		Z80Instruction::Disassembly disassembly = nextOpcode->Z80Disassemble(labelSettings);

		opcodeInfo.SetIsValidOpcode(true);
		opcodeInfo.SetOpcodeSize(nextOpcode->GetInstructionSize());
		opcodeInfo.SetOpcodeNameDisassembly(disassembly.disassemblyOpcode);
		opcodeInfo.SetOpcodeArgumentsDisassembly(disassembly.disassemblyArguments);
		opcodeInfo.SetDisassemblyComment(disassembly.disassemblyComment);

		delete nextOpcode;
	}

	return true;
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
		CalculateCELineStateContext ceLineStateContext(true, false);
		if(transparent)
		{
			memoryBus->TransparentReadMemory(location.GetData(), temp, GetDeviceContext(), 0, (void*)&ceLineStateContext);
		}
		else
		{
			result = memoryBus->ReadMemory(location.GetData(), temp, GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);
		}
		data = temp;
		break;}
	case BITCOUNT_WORD:{
		Z80Byte byteLow;
		Z80Byte byteHigh;
		CalculateCELineStateContext ceLineStateContext(true, false);
		if(transparent)
		{
			memoryBus->TransparentReadMemory(location.GetData(), byteLow, GetDeviceContext(), 0, (void*)&ceLineStateContext);
			memoryBus->TransparentReadMemory((location + 1).GetData(), byteHigh, GetDeviceContext(), 0, (void*)&ceLineStateContext);
		}
		else
		{
			IBusInterface::AccessResult result2;
			result = memoryBus->ReadMemory(location.GetData(), byteLow, GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);
			result2 = memoryBus->ReadMemory((location + 1).GetData(), byteHigh, GetDeviceContext(), GetCurrentTimesliceProgress() + result.executionTime, 0, (void*)&ceLineStateContext);
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
		CalculateCELineStateContext ceLineStateContext(false, true);
		if(transparent)
		{
			memoryBus->TransparentWriteMemory(location.GetData(), data, GetDeviceContext(), 0, (void*)&ceLineStateContext);
		}
		else
		{
			result = memoryBus->WriteMemory(location.GetData(), data, GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);
		}
		break;}
	case BITCOUNT_WORD:{
		CalculateCELineStateContext ceLineStateContext(false, true);
		if(transparent)
		{
			Z80Byte byte1(data.GetLowerBits(BITCOUNT_BYTE));
			Z80Byte byte2(data.GetUpperBits(BITCOUNT_BYTE));
			memoryBus->TransparentWriteMemory(location.GetData(), byte1, GetDeviceContext(), 0, (void*)&ceLineStateContext);
			memoryBus->TransparentWriteMemory((location + 1).GetData(), byte2, GetDeviceContext(), 0, (void*)&ceLineStateContext);
		}
		else
		{
			IBusInterface::AccessResult result2;
			Z80Byte byte1(data.GetLowerBits(BITCOUNT_BYTE));
			Z80Byte byte2(data.GetUpperBits(BITCOUNT_BYTE));
			result = memoryBus->WriteMemory(location.GetData(), byte1, GetDeviceContext(), GetCurrentTimesliceProgress(), 0, (void*)&ceLineStateContext);
			result2 = memoryBus->WriteMemory((location + 1).GetData(), byte2, GetDeviceContext(), GetCurrentTimesliceProgress() + result.executionTime, 0, (void*)&ceLineStateContext);
			result.executionTime += result2.executionTime;
		}
		break;}
	}

	return result.executionTime;
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int Z80::GetCELineID(const MarshalSupport::Marshal::In<std::wstring>& lineName, bool inputLine) const
{
	if(lineName == L"RD")
	{
		return (unsigned int)CELineID::RD;
	}
	else if(lineName == L"WR")
	{
		return (unsigned int)CELineID::WR;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void Z80::SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch((CELineID)lineID)
	{
	case CELineID::RD:
		ceLineMaskRD = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::WR:
		ceLineMaskWR = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
unsigned int Z80::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const
{
	unsigned int ceLineState = 0;
	if((caller == GetDeviceContext()) && (calculateCELineStateContext != 0))
	{
		CalculateCELineStateContext& ceLineStateContext = *((CalculateCELineStateContext*)calculateCELineStateContext);
		ceLineState |= ceLineStateContext.lineRD? ceLineMaskRD: 0x0;
		ceLineState |= ceLineStateContext.lineWR? ceLineMaskWR: 0x0;
	}
	return ceLineState;
}

//----------------------------------------------------------------------------------------
unsigned int Z80::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const
{
	return CalculateCELineStateMemory(location, data, currentCELineState, sourceBusInterface, caller, calculateCELineStateContext, 0.0);
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void Z80::LoadState(IHierarchicalStorageNode& node)
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
						LineAccess lineAccess(0, 0.0, 0.0);
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
									lineAccess = LineAccess(lineID, clockRate, accessTime);
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

	//Clear the flag indicating we performed a reset on the last step. This flag is only
	//here for performance purposes. We want to reset it now so that if we loaded a
	//savestate which changed the Z80 state while a reset was being asserted, and the
	//reset line state didn't change after loading the state, but the register contents
	//did, the register contents will be correctly reset on the next cycle.
	resetLastStep = false;

	Processor::LoadState(node);
}

//----------------------------------------------------------------------------------------
void Z80::SaveState(IHierarchicalStorageNode& node) const
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
		IHierarchicalStorageNode& lineAccessState = node.CreateChild(L"LineAccessBuffer");
		for(std::list<LineAccess>::const_iterator i = lineAccessBuffer.begin(); i != lineAccessBuffer.end(); ++i)
		{
			IHierarchicalStorageNode& lineAccessEntry = lineAccessState.CreateChild(L"LineAccess");
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

//----------------------------------------------------------------------------------------
//Data read/write functions
//----------------------------------------------------------------------------------------
bool Z80::ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const
{
	ApplyGenericDataValueDisplaySettings(dataID, dataValue);
	switch((IZ80DataSource)dataID)
	{
	case IZ80DataSource::RegisterA:
		return dataValue.SetValue(GetA().GetData());
	case IZ80DataSource::RegisterF:
		return dataValue.SetValue(GetF().GetData());
	case IZ80DataSource::RegisterB:
		return dataValue.SetValue(GetB().GetData());
	case IZ80DataSource::RegisterC:
		return dataValue.SetValue(GetC().GetData());
	case IZ80DataSource::RegisterD:
		return dataValue.SetValue(GetD().GetData());
	case IZ80DataSource::RegisterE:
		return dataValue.SetValue(GetE().GetData());
	case IZ80DataSource::RegisterH:
		return dataValue.SetValue(GetH().GetData());
	case IZ80DataSource::RegisterL:
		return dataValue.SetValue(GetL().GetData());
	case IZ80DataSource::RegisterAF:
		return dataValue.SetValue(GetAF().GetData());
	case IZ80DataSource::RegisterBC:
		return dataValue.SetValue(GetBC().GetData());
	case IZ80DataSource::RegisterDE:
		return dataValue.SetValue(GetDE().GetData());
	case IZ80DataSource::RegisterHL:
		return dataValue.SetValue(GetHL().GetData());
	case IZ80DataSource::RegisterA2:
		return dataValue.SetValue(GetA2().GetData());
	case IZ80DataSource::RegisterF2:
		return dataValue.SetValue(GetF2().GetData());
	case IZ80DataSource::RegisterB2:
		return dataValue.SetValue(GetB2().GetData());
	case IZ80DataSource::RegisterC2:
		return dataValue.SetValue(GetC2().GetData());
	case IZ80DataSource::RegisterD2:
		return dataValue.SetValue(GetD2().GetData());
	case IZ80DataSource::RegisterE2:
		return dataValue.SetValue(GetE2().GetData());
	case IZ80DataSource::RegisterH2:
		return dataValue.SetValue(GetH2().GetData());
	case IZ80DataSource::RegisterL2:
		return dataValue.SetValue(GetL2().GetData());
	case IZ80DataSource::RegisterAF2:
		return dataValue.SetValue(GetAF2().GetData());
	case IZ80DataSource::RegisterBC2:
		return dataValue.SetValue(GetBC2().GetData());
	case IZ80DataSource::RegisterDE2:
		return dataValue.SetValue(GetDE2().GetData());
	case IZ80DataSource::RegisterHL2:
		return dataValue.SetValue(GetHL2().GetData());
	case IZ80DataSource::RegisterIXHigh:
		return dataValue.SetValue(GetIXHigh().GetData());
	case IZ80DataSource::RegisterIXLow:
		return dataValue.SetValue(GetIXLow().GetData());
	case IZ80DataSource::RegisterIYHigh:
		return dataValue.SetValue(GetIYHigh().GetData());
	case IZ80DataSource::RegisterIYLow:
		return dataValue.SetValue(GetIYLow().GetData());
	case IZ80DataSource::RegisterI:
		return dataValue.SetValue(GetI().GetData());
	case IZ80DataSource::RegisterR:
		return dataValue.SetValue(GetR().GetData());
	case IZ80DataSource::RegisterIX:
		return dataValue.SetValue(GetIX().GetData());
	case IZ80DataSource::RegisterIY:
		return dataValue.SetValue(GetIY().GetData());
	case IZ80DataSource::RegisterSP:
		return dataValue.SetValue(GetSP().GetData());
	case IZ80DataSource::RegisterPC:
		return dataValue.SetValue(GetPC().GetData());
	case IZ80DataSource::RegisterIM:
		return dataValue.SetValue(GetInterruptMode());
	case IZ80DataSource::RegisterIFF1:
		return dataValue.SetValue(GetIFF1());
	case IZ80DataSource::RegisterIFF2:
		return dataValue.SetValue(GetIFF2());
	case IZ80DataSource::RegisterFlagS:
		return dataValue.SetValue(GetFlagS());
	case IZ80DataSource::RegisterFlagZ:
		return dataValue.SetValue(GetFlagZ());
	case IZ80DataSource::RegisterFlagY:
		return dataValue.SetValue(GetFlagY());
	case IZ80DataSource::RegisterFlagH:
		return dataValue.SetValue(GetFlagH());
	case IZ80DataSource::RegisterFlagX:
		return dataValue.SetValue(GetFlagX());
	case IZ80DataSource::RegisterFlagPV:
		return dataValue.SetValue(GetFlagPV());
	case IZ80DataSource::RegisterFlagN:
		return dataValue.SetValue(GetFlagN());
	case IZ80DataSource::RegisterFlagC:
		return dataValue.SetValue(GetFlagC());
	}
	return Processor::ReadGenericData(dataID, dataContext, dataValue);
}

//----------------------------------------------------------------------------------------
bool Z80::WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue)
{
	ApplyGenericDataValueLimitSettings(dataID, dataValue);
	IGenericAccessDataValue::DataType dataType = dataValue.GetType();
	switch((IZ80DataSource)dataID)
	{
	case IZ80DataSource::RegisterA:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetA(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterF:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetF(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterB:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetB(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterC:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetC(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterD:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetD(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterE:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetE(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterH:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetH(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterL:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetL(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterAF:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetAF(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterBC:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetBC(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterDE:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetDE(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterHL:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetHL(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterA2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetA2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterF2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetF2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterB2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetB2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterC2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetC2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterD2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetD2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterE2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetE2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterH2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetH2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterL2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetL2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterAF2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetAF2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterBC2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetBC2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterDE2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetDE2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterHL2:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetHL2(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterIXHigh:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetIXHigh(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterIXLow:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetIXLow(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterIYHigh:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetIYHigh(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterIYLow:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetIYLow(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterI:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetI(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterR:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetR(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterIX:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetIX(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterIY:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetIY(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterSP:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetSP(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterPC:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetPC(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterIM:{
		if(dataType != IGenericAccessDataValue::DataType::UInt) return false;
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		SetInterruptMode(dataValueAsUInt.GetValue());
		return true;}
	case IZ80DataSource::RegisterIFF1:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetIFF1(dataValueAsBool.GetValue());
		return true;}
	case IZ80DataSource::RegisterIFF2:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetIFF2(dataValueAsBool.GetValue());
		return true;}
	case IZ80DataSource::RegisterFlagS:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetFlagS(dataValueAsBool.GetValue());
		return true;}
	case IZ80DataSource::RegisterFlagZ:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetFlagZ(dataValueAsBool.GetValue());
		return true;}
	case IZ80DataSource::RegisterFlagY:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetFlagY(dataValueAsBool.GetValue());
		return true;}
	case IZ80DataSource::RegisterFlagH:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetFlagH(dataValueAsBool.GetValue());
		return true;}
	case IZ80DataSource::RegisterFlagX:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetFlagX(dataValueAsBool.GetValue());
		return true;}
	case IZ80DataSource::RegisterFlagPV:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetFlagPV(dataValueAsBool.GetValue());
		return true;}
	case IZ80DataSource::RegisterFlagN:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetFlagN(dataValueAsBool.GetValue());
		return true;}
	case IZ80DataSource::RegisterFlagC:{
		if(dataType != IGenericAccessDataValue::DataType::Bool) return false;
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		SetFlagC(dataValueAsBool.GetValue());
		return true;}
	}
	return Processor::WriteGenericData(dataID, dataContext, dataValue);
}

//----------------------------------------------------------------------------------------
//Highlight functions
//----------------------------------------------------------------------------------------
bool Z80::GetGenericDataHighlightState(unsigned int dataID, const DataContext* dataContext) const
{
	switch((IZ80DataSource)dataID)
	{
	case IZ80DataSource::RegisterA:
		return (regChangedA != GetA().GetData());
	case IZ80DataSource::RegisterF:
		return (regChangedF != GetF().GetData());
	case IZ80DataSource::RegisterB:
		return (regChangedB != GetB().GetData());
	case IZ80DataSource::RegisterC:
		return (regChangedC != GetC().GetData());
	case IZ80DataSource::RegisterD:
		return (regChangedD != GetD().GetData());
	case IZ80DataSource::RegisterE:
		return (regChangedE != GetE().GetData());
	case IZ80DataSource::RegisterH:
		return (regChangedH != GetH().GetData());
	case IZ80DataSource::RegisterL:
		return (regChangedL != GetL().GetData());
	case IZ80DataSource::RegisterAF:
		return (regChangedAF != GetAF().GetData());
	case IZ80DataSource::RegisterBC:
		return (regChangedBC != GetBC().GetData());
	case IZ80DataSource::RegisterDE:
		return (regChangedDE != GetDE().GetData());
	case IZ80DataSource::RegisterHL:
		return (regChangedHL != GetHL().GetData());
	case IZ80DataSource::RegisterA2:
		return (regChangedA2 != GetA2().GetData());
	case IZ80DataSource::RegisterF2:
		return (regChangedF2 != GetF2().GetData());
	case IZ80DataSource::RegisterB2:
		return (regChangedB2 != GetB2().GetData());
	case IZ80DataSource::RegisterC2:
		return (regChangedC2 != GetC2().GetData());
	case IZ80DataSource::RegisterD2:
		return (regChangedD2 != GetD2().GetData());
	case IZ80DataSource::RegisterE2:
		return (regChangedE2 != GetE2().GetData());
	case IZ80DataSource::RegisterH2:
		return (regChangedH2 != GetH2().GetData());
	case IZ80DataSource::RegisterL2:
		return (regChangedL2 != GetL2().GetData());
	case IZ80DataSource::RegisterAF2:
		return (regChangedAF2 != GetAF2().GetData());
	case IZ80DataSource::RegisterBC2:
		return (regChangedBC2 != GetBC2().GetData());
	case IZ80DataSource::RegisterDE2:
		return (regChangedDE2 != GetDE2().GetData());
	case IZ80DataSource::RegisterHL2:
		return (regChangedHL2 != GetHL2().GetData());
	case IZ80DataSource::RegisterIXHigh:
		return (regChangedIXHigh != GetIXHigh().GetData());
	case IZ80DataSource::RegisterIXLow:
		return (regChangedIXLow != GetIXLow().GetData());
	case IZ80DataSource::RegisterIYHigh:
		return (regChangedIYHigh != GetIYHigh().GetData());
	case IZ80DataSource::RegisterIYLow:
		return (regChangedIYLow != GetIYLow().GetData());
	case IZ80DataSource::RegisterI:
		return (regChangedI != GetI().GetData());
	case IZ80DataSource::RegisterR:
		return (regChangedR != GetR().GetData());
	case IZ80DataSource::RegisterIX:
		return (regChangedIX != GetIX().GetData());
	case IZ80DataSource::RegisterIY:
		return (regChangedIY != GetIY().GetData());
	case IZ80DataSource::RegisterSP:
		return (regChangedSP != GetSP().GetData());
	case IZ80DataSource::RegisterPC:
		return (regChangedPC != GetPC().GetData());
	case IZ80DataSource::RegisterIM:
		return (regChangedIM != GetInterruptMode());
	case IZ80DataSource::RegisterIFF1:
		return (regChangedIFF1 != GetIFF1());
	case IZ80DataSource::RegisterIFF2:
		return (regChangedIFF2 != GetIFF2());
	case IZ80DataSource::RegisterFlagS:
		return (regChangedFlagS != GetFlagS());
	case IZ80DataSource::RegisterFlagZ:
		return (regChangedFlagZ != GetFlagZ());
	case IZ80DataSource::RegisterFlagY:
		return (regChangedFlagY != GetFlagY());
	case IZ80DataSource::RegisterFlagH:
		return (regChangedFlagH != GetFlagH());
	case IZ80DataSource::RegisterFlagX:
		return (regChangedFlagX != GetFlagX());
	case IZ80DataSource::RegisterFlagPV:
		return (regChangedFlagPV != GetFlagPV());
	case IZ80DataSource::RegisterFlagN:
		return (regChangedFlagN != GetFlagN());
	case IZ80DataSource::RegisterFlagC:
		return (regChangedFlagC != GetFlagC());
	}
	return Processor::GetGenericDataHighlightState(dataID, dataContext);
}

//----------------------------------------------------------------------------------------
void Z80::PopulateChangedRegStateFromCurrentState()
{
	//Save a snapshot of the current register state for changed register tracking
	regChangedAF = GetAF().GetData();
	regChangedBC = GetBC().GetData();
	regChangedDE = GetDE().GetData();
	regChangedHL = GetHL().GetData();
	regChangedAF2 = GetAF2().GetData();
	regChangedBC2 = GetBC2().GetData();
	regChangedDE2 = GetDE2().GetData();
	regChangedHL2 = GetHL2().GetData();
	regChangedA = GetA().GetData();
	regChangedF = GetF().GetData();
	regChangedB = GetB().GetData();
	regChangedC = GetC().GetData();
	regChangedD = GetD().GetData();
	regChangedE = GetE().GetData();
	regChangedH = GetH().GetData();
	regChangedL = GetL().GetData();
	regChangedA2 = GetA2().GetData();
	regChangedF2 = GetF2().GetData();
	regChangedB2 = GetB2().GetData();
	regChangedC2 = GetC2().GetData();
	regChangedD2 = GetD2().GetData();
	regChangedE2 = GetE2().GetData();
	regChangedH2 = GetH2().GetData();
	regChangedL2 = GetL2().GetData();
	regChangedI = GetI().GetData();
	regChangedR = GetR().GetData();
	regChangedIX = GetIX().GetData();
	regChangedIY = GetIY().GetData();
	regChangedIXHigh = GetIXHigh().GetData();
	regChangedIXLow = GetIXLow().GetData();
	regChangedIYHigh = GetIYHigh().GetData();
	regChangedIYLow = GetIYLow().GetData();
	regChangedSP = GetSP().GetData();
	regChangedPC = GetPC().GetData();
	regChangedIM = GetInterruptMode();
	regChangedIFF1 = GetIFF1();
	regChangedIFF2 = GetIFF2();
	regChangedFlagS = GetFlagS();
	regChangedFlagZ = GetFlagZ();
	regChangedFlagY = GetFlagY();
	regChangedFlagH = GetFlagH();
	regChangedFlagX = GetFlagX();
	regChangedFlagPV = GetFlagPV();
	regChangedFlagN = GetFlagN();
	regChangedFlagC = GetFlagC();
}

} //Close namespace Z80
