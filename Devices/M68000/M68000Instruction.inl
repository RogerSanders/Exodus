namespace M68000 {

//----------------------------------------------------------------------------------------------------------------------
// Enumerations
//----------------------------------------------------------------------------------------------------------------------
enum class M68000Instruction::ConditionCode
{
	T  = 0x0, // True             1
	F  = 0x1, // False            0
	HI = 0x2, // HIgh             C + Z = 0
	LS = 0x3, // Low or Same      C + Z = 1
	CC = 0x4, // Carry Clear      C = 0
	CS = 0x5, // Carry Set        C = 1
	NE = 0x6, // Not Equal        Z = 0
	EQ = 0x7, // EQual            Z = 1
	VC = 0x8, // oVerflow Clear   V = 0
	VS = 0x9, // oVerflow Set     V = 1
	PL = 0xA, // PLus             N = 0
	MI = 0xB, // MInus            N = 1
	GE = 0xC, // Greater or Equal N = V
	LT = 0xD, // Less Than        N != V
	GT = 0xE, // Greater Than     !Z & (N = V)
	LE = 0xF  // Less or Equal    Z | (N != V)
};

//----------------------------------------------------------------------------------------------------------------------
// Structures
//----------------------------------------------------------------------------------------------------------------------
struct M68000Instruction::Disassembly
{
	Disassembly()
	{}
	Disassembly(const std::wstring& opcode)
		:disassemblyOpcode(opcode)
	{}
	Disassembly(const std::wstring& opcode, const std::wstring& arguments)
		:disassemblyOpcode(opcode), disassemblyArguments(arguments)
	{}
	Disassembly(const std::wstring& opcode, const std::wstring& arguments, const std::wstring& comment)
		:disassemblyOpcode(opcode), disassemblyArguments(arguments), disassemblyComment(comment)
	{}

	std::wstring disassemblyOpcode;
	std::wstring disassemblyArguments;
	std::wstring disassemblyComment;
};

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
M68000Instruction::M68000Instruction()
:_instructionSize(0), _transparentOpcode(false)
{}

//----------------------------------------------------------------------------------------------------------------------
M68000Instruction::M68000Instruction(const M68000Instruction& object)
:_instructionSize(object._instructionSize), _executeCycleCount(object._executeCycleCount), _instructionRegister(object._instructionRegister), _instructionLocation(object._instructionLocation), _transparentOpcode(object._transparentOpcode)
{}

//----------------------------------------------------------------------------------------------------------------------
// Size functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int M68000Instruction::GetInstructionSize() const
{
	return _instructionSize;
}

//----------------------------------------------------------------------------------------------------------------------
void M68000Instruction::SetInstructionSize(unsigned int instructionSize)
{
	_instructionSize = instructionSize;
}

//----------------------------------------------------------------------------------------------------------------------
void M68000Instruction::AddInstructionSize(unsigned int instructionSize)
{
	_instructionSize += instructionSize;
}

//----------------------------------------------------------------------------------------------------------------------
// Execute time functions
//----------------------------------------------------------------------------------------------------------------------
ExecuteTime M68000Instruction::GetExecuteCycleCount() const
{
	return _executeCycleCount;
}

//----------------------------------------------------------------------------------------------------------------------
ExecuteTime M68000Instruction::GetExecuteCycleCount(double additionalTime) const
{
	ExecuteTime executeTime = _executeCycleCount;
	executeTime.additionalTime += additionalTime;
	return executeTime;
}

//----------------------------------------------------------------------------------------------------------------------
void M68000Instruction::AddExecuteCycleCount(const ExecuteTime& executeCycleCount)
{
	_executeCycleCount = _executeCycleCount + executeCycleCount;
}

//----------------------------------------------------------------------------------------------------------------------
// Instruction info functions
//----------------------------------------------------------------------------------------------------------------------
const M68000Word& M68000Instruction::GetInstructionRegister() const
{
	return _instructionRegister;
}

//----------------------------------------------------------------------------------------------------------------------
void M68000Instruction::SetInstructionRegister(const M68000Word& instructionData)
{
	_instructionRegister = instructionData;
}

//----------------------------------------------------------------------------------------------------------------------
const M68000Long& M68000Instruction::GetInstructionLocation() const
{
	return _instructionLocation;
}

//----------------------------------------------------------------------------------------------------------------------
void M68000Instruction::SetInstructionLocation(const M68000Long& instructionLocation)
{
	_instructionLocation = instructionLocation;
}

//----------------------------------------------------------------------------------------------------------------------
// Transparent decode functions
//----------------------------------------------------------------------------------------------------------------------
bool M68000Instruction::GetTransparentFlag() const
{
	return _transparentOpcode;
}

//----------------------------------------------------------------------------------------------------------------------
void M68000Instruction::SetTransparentFlag(bool state)
{
	_transparentOpcode = state;
}

} // Close namespace M68000
