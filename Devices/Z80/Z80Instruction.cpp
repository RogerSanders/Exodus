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
	case ConditionCode::NZ: //Non Zero
		return !cpu->GetFlagZ();
	case ConditionCode::Z:  //Zero
		return cpu->GetFlagZ();
	case ConditionCode::NC: //No Carry
		return !cpu->GetFlagC();
	case ConditionCode::C:  //Carry
		return cpu->GetFlagC();
	case ConditionCode::PO: //Parity Odd
		return !cpu->GetFlagPV();
	case ConditionCode::PE: //Parity Even
		return cpu->GetFlagPV();
	case ConditionCode::P:  //Sign Positive
		return !cpu->GetFlagS();
	case ConditionCode::M:  //Sign Negative
		return cpu->GetFlagS();
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Disassembly functions
//----------------------------------------------------------------------------------------
std::wstring Z80Instruction::GetOpcodeName() const
{
	return L"<Unknown>";
}

//----------------------------------------------------------------------------------------
Z80Instruction::Disassembly Z80Instruction::Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
{
	return Disassembly();
}

//----------------------------------------------------------------------------------------
std::wstring Z80Instruction::DisassembleConditionCode(ConditionCode conditionCode)
{
	switch(conditionCode)
	{
	case ConditionCode::NZ: //Non Zero
		return L"NZ";
	case ConditionCode::Z:  //Zero
		return L"Z";
	case ConditionCode::NC: //No Carry
		return L"NC";
	case ConditionCode::C:  //Carry
		return L"C";
	case ConditionCode::PO: //Parity Odd
		return L"PO";
	case ConditionCode::PE: //Parity Even
		return L"PE";
	case ConditionCode::P: //Sign Positive 
		return L"P";
	case ConditionCode::M:  //Sign Negative
		return L"M";
	}
	return L"BAD";
}

} //Close namespace Z80
