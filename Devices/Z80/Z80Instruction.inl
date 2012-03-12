namespace Z80 {

//----------------------------------------------------------------------------------------
enum Z80Instruction::ConditionCode
{
	CONDITIONCODE_NZ   = 0x0, //Non Zero
	CONDITIONCODE_Z    = 0x1, //Zero
	CONDITIONCODE_NC   = 0x2, //No Carry
	CONDITIONCODE_C    = 0x3, //Carry
	CONDITIONCODE_PO   = 0x4, //Parity Odd
	CONDITIONCODE_PE   = 0x5, //Parity Even
	CONDITIONCODE_P    = 0x6, //Sign Positive
	CONDITIONCODE_M    = 0x7, //Sign Negative
	CONDITIONCODE_NONE = 0xFF //No condition. Always returns true.
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct Z80Instruction::Disassembly
{
	Disassembly()
	{}
	Disassembly(const std::wstring& aopcode)
	:disassemblyOpcode(aopcode)
	{}
	Disassembly(const std::wstring& aopcode, const std::wstring& aarguments)
	:disassemblyOpcode(aopcode), disassemblyArguments(aarguments)
	{}
	Disassembly(const std::wstring& aopcode, const std::wstring& aarguments, const std::wstring& acomment)
	:disassemblyOpcode(aopcode), disassemblyArguments(aarguments), disassemblyComment(acomment)
	{}

	std::wstring disassemblyOpcode;
	std::wstring disassemblyArguments;
	std::wstring disassemblyComment;
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Z80Instruction::Z80Instruction()
:instructionSize(0), mandatoryIndexOffset(false), transparentOpcode(false), indexState(EffectiveAddress::INDEX_NONE), executeCycleCount(0)
{}

//----------------------------------------------------------------------------------------
Z80Instruction::Z80Instruction(const Z80Instruction& object)
:instructionSize(object.instructionSize), indexState(object.indexState), instructionLocation(object.instructionLocation), instructionRegister(object.instructionRegister), indexOffset(object.indexOffset), transparentOpcode(object.transparentOpcode), mandatoryIndexOffset(object.mandatoryIndexOffset), executeCycleCount(0)
{}

//----------------------------------------------------------------------------------------
//Size functions
//----------------------------------------------------------------------------------------
unsigned int Z80Instruction::GetInstructionSize() const
{
	return instructionSize;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetInstructionSize(unsigned int ainstructionSize)
{
	instructionSize = ainstructionSize;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::AddInstructionSize(unsigned int ainstructionSize)
{
	instructionSize += ainstructionSize;
}

//----------------------------------------------------------------------------------------
//Index functions
//----------------------------------------------------------------------------------------
EffectiveAddress::IndexState Z80Instruction::GetIndexState() const
{
	return indexState;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetIndexState(EffectiveAddress::IndexState aindexState)
{
	indexState = aindexState;
}

//----------------------------------------------------------------------------------------
Z80Byte Z80Instruction::GetIndexOffset() const
{
	return indexOffset;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetIndexOffset(const Z80Byte& aindexOffset, bool amandatoryIndexOffset)
{
	indexOffset = aindexOffset;
	mandatoryIndexOffset = amandatoryIndexOffset;
}

//----------------------------------------------------------------------------------------
unsigned int Z80Instruction::GetIndexOffsetSize(bool add)
{
	if(add && !mandatoryIndexOffset)
	{
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//Execute time functions
//----------------------------------------------------------------------------------------
ExecuteTime Z80Instruction::GetExecuteCycleCount() const
{
	return executeCycleCount;
}

//----------------------------------------------------------------------------------------
ExecuteTime Z80Instruction::GetExecuteCycleCount(double additionalTime) const
{
	ExecuteTime executeTime = executeCycleCount;
	executeTime.additionalTime += additionalTime;
	return executeTime;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::AddExecuteCycleCount(const ExecuteTime& aexecuteCycleCount)
{
	executeCycleCount += aexecuteCycleCount;
}

//----------------------------------------------------------------------------------------
//Instruction info functions
//----------------------------------------------------------------------------------------
const Z80Byte& Z80Instruction::GetInstructionRegister() const
{
	return instructionRegister;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetInstructionRegister(const Z80Byte& ainstructionData)
{
	instructionRegister = ainstructionData;
}

//----------------------------------------------------------------------------------------
const Z80Word& Z80Instruction::GetInstructionLocation() const
{
	return instructionLocation;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetInstructionLocation(const Z80Word& ainstructionLocation)
{
	instructionLocation = ainstructionLocation;
}

//----------------------------------------------------------------------------------------
//Transparent decode functions
//----------------------------------------------------------------------------------------
bool Z80Instruction::GetTransparentFlag() const
{
	return transparentOpcode;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetTransparentFlag(bool astate)
{
	transparentOpcode = astate;
}

} //Close namespace Z80
