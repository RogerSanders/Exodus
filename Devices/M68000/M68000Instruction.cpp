#include "M68000Instruction.h"
#include "M68000.h"
namespace M68000 {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
M68000Instruction::~M68000Instruction()
{}

//----------------------------------------------------------------------------------------
//Condition code functions
//----------------------------------------------------------------------------------------
bool M68000Instruction::ConditionCodeTrue(M68000* cpu, ConditionCode code)
{
	switch(code)
	{
	case ConditionCode::T:  //True             1
		return true;
	case ConditionCode::F:  //False            0
		return false;
	case ConditionCode::HI: //HIgh             C + Z = 0
		return !(cpu->GetC() || cpu->GetZ());
	case ConditionCode::LS: //Low or Same      C + Z = 1
		return (cpu->GetC() || cpu->GetZ());
	case ConditionCode::CC: //Carry Clear      C = 0
		return (cpu->GetC() == 0);
	case ConditionCode::CS: //Carry Set        C = 1
		return (cpu->GetC() == 1);
	case ConditionCode::NE: //Not Equal        Z = 0
		return (cpu->GetZ() == 0);
	case ConditionCode::EQ: //EQual            Z = 1
		return (cpu->GetZ() == 1);
	case ConditionCode::VC: //oVerflow Clear   V = 0
		return (cpu->GetV() == 0);
	case ConditionCode::VS: //oVerflow Set     V = 1
		return (cpu->GetV() == 1);
	case ConditionCode::PL: //PLus             N = 0
		return (cpu->GetN() == 0);
	case ConditionCode::MI: //MInus            N = 1
		return (cpu->GetN() == 1);
	case ConditionCode::GE: //Greater or Equal N = V
		return (cpu->GetN() == cpu->GetV());
	case ConditionCode::LT: //Less Than        N != V
		return (cpu->GetN() != cpu->GetV());
	case ConditionCode::GT: //Greater Than     !Z & (N = V)
		return (!cpu->GetZ() && ((cpu->GetN() == cpu->GetV())));
	case ConditionCode::LE: //Less or Equal    Z | (N != V)
		return (cpu->GetZ() || (cpu->GetN() != cpu->GetV()));
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Instruction info functions
//----------------------------------------------------------------------------------------
bool M68000Instruction::Privileged() const
{
	return false;
}

//----------------------------------------------------------------------------------------
//Disassembly functions
//----------------------------------------------------------------------------------------
std::wstring M68000Instruction::GetOpcodeName() const
{
	return L"<Unknown>";
}

//----------------------------------------------------------------------------------------
M68000Instruction::Disassembly M68000Instruction::M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
{
	return Disassembly();
}

//----------------------------------------------------------------------------------------
void M68000Instruction::GetResultantPCLocations(std::set<unsigned int>& resultantPCLocations, bool& undeterminedResultantPCLocation) const
{
	//Return the address directly after this opcode as the only resultant PC location from
	//executing this opcode. Note that when overriding this function for opcodes that do
	//branching we ignore exception locations, so opcodes which explicitly or implicitly
	//invoke exception handlers don't return the location of those exception handlers as
	//resultant locations, and any branching that can also return, such as jump opcodes or
	//calls to exception handlers, must also flag the following instruction as a resultant
	//PC location, even though other opcodes may always be executed before the following
	//opcode.
	undeterminedResultantPCLocation = false;
	unsigned int nextOpcodeAddress = GetInstructionLocation().GetData() + GetInstructionSize();
	resultantPCLocations.insert(nextOpcodeAddress);
}

//----------------------------------------------------------------------------------------
std::wstring M68000Instruction::DisassembleConditionCode(ConditionCode code)
{
	switch(code)
	{
	case ConditionCode::T:  //True             1
		return L"T";
	case ConditionCode::F:  //False            0
		return L"F";
	case ConditionCode::HI: //HIgh             C + Z = 0
		return L"HI";
	case ConditionCode::LS: //Low or Same      C + Z = 1
		return L"LS";
	case ConditionCode::CC: //Carry Clear      C = 0
		return L"CC";
	case ConditionCode::CS: //Carry Set        C = 1
		return L"CS";
	case ConditionCode::NE: //Not Equal        Z = 0
		return L"NE";
	case ConditionCode::EQ: //EQual            Z = 1
		return L"EQ";
	case ConditionCode::VC: //oVerflow Clear   V = 0
		return L"VC";
	case ConditionCode::VS: //oVerflow Set     V = 1
		return L"VS";
	case ConditionCode::PL: //PLus             N = 0
		return L"PL";
	case ConditionCode::MI: //MInus            N = 1
		return L"MI";
	case ConditionCode::GE: //Greater or Equal N = V
		return L"GE";
	case ConditionCode::LT: //Less Than        N != V
		return L"LT";
	case ConditionCode::GT: //Greater Than     !Z & (N = V)
		return L"GT";
	case ConditionCode::LE: //Less or Equal    Z | (N != V)
		return L"LE";
	default:
		return L"BAD";
	}
}

//----------------------------------------------------------------------------------------
std::wstring M68000Instruction::DisassembleSize(Bitcount asize)
{
	switch(asize)
	{
	case BITCOUNT_BYTE:
		return L"b";
	case BITCOUNT_WORD:
		return L"w";
	case BITCOUNT_LONG:
		return L"l";
	default:
		return L"INVALID";
	}
}

} //Close namespace M68000
