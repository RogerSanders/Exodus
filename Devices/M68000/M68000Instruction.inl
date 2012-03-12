namespace M68000 {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum M68000Instruction::ConditionCode
{
	CONDITIONCODE_T  = 0x0, //True             1
	CONDITIONCODE_F  = 0x1, //False            0
	CONDITIONCODE_HI = 0x2, //HIgh             C + Z = 0
	CONDITIONCODE_LS = 0x3, //Low or Same      C + Z = 1
	CONDITIONCODE_CC = 0x4, //Carry Clear      C = 0
	CONDITIONCODE_CS = 0x5, //Carry Set        C = 1
	CONDITIONCODE_NE = 0x6, //Not Equal        Z = 0
	CONDITIONCODE_EQ = 0x7, //EQual            Z = 1
	CONDITIONCODE_VC = 0x8, //oVerflow Clear   V = 0
	CONDITIONCODE_VS = 0x9, //oVerflow Set     V = 1
	CONDITIONCODE_PL = 0xA, //PLus             N = 0
	CONDITIONCODE_MI = 0xB, //MInus            N = 1
	CONDITIONCODE_GE = 0xC, //Greater or Equal N = V
	CONDITIONCODE_LT = 0xD, //Less Than        N != V
	CONDITIONCODE_GT = 0xE, //Greater Than     !Z & (N = V)
	CONDITIONCODE_LE = 0xF  //Less or Equal    Z | (N != V)
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct M68000Instruction::Disassembly
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
M68000Instruction::M68000Instruction()
:instructionSize(0), transparentOpcode(false)
{}

//----------------------------------------------------------------------------------------
M68000Instruction::M68000Instruction(const M68000Instruction& object)
:instructionSize(object.instructionSize), executeCycleCount(object.executeCycleCount), instructionRegister(object.instructionRegister), instructionLocation(object.instructionLocation), transparentOpcode(object.transparentOpcode)
{}

//----------------------------------------------------------------------------------------
//Size functions
//----------------------------------------------------------------------------------------
unsigned int M68000Instruction::GetInstructionSize() const
{
	return instructionSize;
}

//----------------------------------------------------------------------------------------
void M68000Instruction::SetInstructionSize(unsigned int ainstructionSize)
{
	instructionSize = ainstructionSize;
}

//----------------------------------------------------------------------------------------
void M68000Instruction::AddInstructionSize(unsigned int ainstructionSize)
{
	instructionSize += ainstructionSize;
}

//----------------------------------------------------------------------------------------
//Execute time functions
//----------------------------------------------------------------------------------------
ExecuteTime M68000Instruction::GetExecuteCycleCount() const
{
	return executeCycleCount;
}

//----------------------------------------------------------------------------------------
ExecuteTime M68000Instruction::GetExecuteCycleCount(double additionalTime) const
{
	ExecuteTime executeTime = executeCycleCount;
	executeTime.additionalTime += additionalTime;
	return executeTime;
}

//----------------------------------------------------------------------------------------
void M68000Instruction::AddExecuteCycleCount(const ExecuteTime& aexecuteCycleCount)
{
	executeCycleCount = executeCycleCount + aexecuteCycleCount;
}

//----------------------------------------------------------------------------------------
//Instruction info functions
//----------------------------------------------------------------------------------------
const M68000Word& M68000Instruction::GetInstructionRegister() const
{
	return instructionRegister;
}

//----------------------------------------------------------------------------------------
void M68000Instruction::SetInstructionRegister(const M68000Word& ainstructionData)
{
	instructionRegister = ainstructionData;
}

//----------------------------------------------------------------------------------------
const M68000Long& M68000Instruction::GetInstructionLocation() const
{
	return instructionLocation;
}

//----------------------------------------------------------------------------------------
void M68000Instruction::SetInstructionLocation(const M68000Long& ainstructionLocation)
{
	instructionLocation = ainstructionLocation;
}

//----------------------------------------------------------------------------------------
//Transparent decode functions
//----------------------------------------------------------------------------------------
bool M68000Instruction::GetTransparentFlag() const
{
	return transparentOpcode;
}

//----------------------------------------------------------------------------------------
void M68000Instruction::SetTransparentFlag(bool astate)
{
	transparentOpcode = astate;
}

} //Close namespace M68000
