#include "Z80Instruction.h"
#include "Z80.h"
namespace Z80 {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Z80Instruction::~Z80Instruction()
{}

//----------------------------------------------------------------------------------------
//Condition code functions
//----------------------------------------------------------------------------------------
bool Z80Instruction::ConditionCodeTrue(Z80* cpu, ConditionCode conditionCode)
{
	switch(conditionCode)
	{
	case CONDITIONCODE_NZ: //Non Zero
		return !cpu->GetFlagZ();
	case CONDITIONCODE_Z:  //Zero
		return cpu->GetFlagZ();
	case CONDITIONCODE_NC: //No Carry
		return !cpu->GetFlagC();
	case CONDITIONCODE_C:  //Carry
		return cpu->GetFlagC();
	case CONDITIONCODE_PO: //Parity Odd
		return !cpu->GetFlagPV();
	case CONDITIONCODE_PE: //Parity Even
		return cpu->GetFlagPV();
	case CONDITIONCODE_P:  //Sign Positive
		return !cpu->GetFlagS();
	case CONDITIONCODE_M:  //Sign Negative
		return cpu->GetFlagS();
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Disassembly functions
//----------------------------------------------------------------------------------------
Z80Instruction::Disassembly Z80Instruction::Z80Disassemble()
{
	return Disassembly();
}

//----------------------------------------------------------------------------------------
std::wstring Z80Instruction::DisassembleConditionCode(ConditionCode conditionCode)
{
	switch(conditionCode)
	{
	case CONDITIONCODE_NZ: //Non Zero
		return L"NZ";
	case CONDITIONCODE_Z:  //Zero
		return L"Z";
	case CONDITIONCODE_NC: //No Carry
		return L"NC";
	case CONDITIONCODE_C:  //Carry
		return L"C";
	case CONDITIONCODE_PO: //Parity Odd
		return L"PO";
	case CONDITIONCODE_PE: //Parity Even
		return L"PE";
	case CONDITIONCODE_P: //Sign Positive 
		return L"P";
	case CONDITIONCODE_M:  //Sign Negative
		return L"M";
	}
	return L"BAD";
}

} //Close namespace Z80
