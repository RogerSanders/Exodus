namespace Z80 {

//----------------------------------------------------------------------------------------
enum class Z80Instruction::ConditionCode
{
	NZ   = 0x0, //Non Zero
	Z    = 0x1, //Zero
	NC   = 0x2, //No Carry
	C    = 0x3, //Carry
	PO   = 0x4, //Parity Odd
	PE   = 0x5, //Parity Even
	P    = 0x6, //Sign Positive
	M    = 0x7, //Sign Negative
	None = 0xFF //No condition. Always returns true.
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
:_instructionSize(0), _mandatoryIndexOffset(false), _transparentOpcode(false), _indexState(EffectiveAddress::IndexState::None), _executeCycleCount(0)
{}

//----------------------------------------------------------------------------------------
Z80Instruction::Z80Instruction(const Z80Instruction& object)
:_instructionSize(object._instructionSize), _indexState(object._indexState), _instructionLocation(object._instructionLocation), _instructionRegister(object._instructionRegister), _indexOffset(object._indexOffset), _transparentOpcode(object._transparentOpcode), _mandatoryIndexOffset(object._mandatoryIndexOffset), _executeCycleCount(0)
{}

//----------------------------------------------------------------------------------------
//Size functions
//----------------------------------------------------------------------------------------
unsigned int Z80Instruction::GetInstructionSize() const
{
	return _instructionSize;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetInstructionSize(unsigned int instructionSize)
{
	_instructionSize = instructionSize;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::AddInstructionSize(unsigned int instructionSize)
{
	_instructionSize += instructionSize;
}

//----------------------------------------------------------------------------------------
//Index functions
//----------------------------------------------------------------------------------------
EffectiveAddress::IndexState Z80Instruction::GetIndexState() const
{
	return _indexState;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetIndexState(EffectiveAddress::IndexState indexState)
{
	_indexState = indexState;
}

//----------------------------------------------------------------------------------------
Z80Byte Z80Instruction::GetIndexOffset() const
{
	return _indexOffset;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetIndexOffset(const Z80Byte& indexOffset, bool mandatoryIndexOffset)
{
	_indexOffset = indexOffset;
	_mandatoryIndexOffset = mandatoryIndexOffset;
}

//----------------------------------------------------------------------------------------
unsigned int Z80Instruction::GetIndexOffsetSize(bool add) const
{
	if(add && !_mandatoryIndexOffset)
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
	return _executeCycleCount;
}

//----------------------------------------------------------------------------------------
ExecuteTime Z80Instruction::GetExecuteCycleCount(double additionalTime) const
{
	ExecuteTime executeTime = _executeCycleCount;
	executeTime.additionalTime += additionalTime;
	return executeTime;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::AddExecuteCycleCount(const ExecuteTime& executeCycleCount)
{
	_executeCycleCount += executeCycleCount;
}

//----------------------------------------------------------------------------------------
//Instruction info functions
//----------------------------------------------------------------------------------------
const Z80Byte& Z80Instruction::GetInstructionRegister() const
{
	return _instructionRegister;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetInstructionRegister(const Z80Byte& instructionData)
{
	_instructionRegister = instructionData;
}

//----------------------------------------------------------------------------------------
const Z80Word& Z80Instruction::GetInstructionLocation() const
{
	return _instructionLocation;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetInstructionLocation(const Z80Word& instructionLocation)
{
	_instructionLocation = instructionLocation;
}

//----------------------------------------------------------------------------------------
//Transparent decode functions
//----------------------------------------------------------------------------------------
bool Z80Instruction::GetTransparentFlag() const
{
	return _transparentOpcode;
}

//----------------------------------------------------------------------------------------
void Z80Instruction::SetTransparentFlag(bool state)
{
	_transparentOpcode = state;
}

} //Close namespace Z80
