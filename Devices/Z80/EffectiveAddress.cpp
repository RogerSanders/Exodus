#include "EffectiveAddress.h"
#include "Z80.h"
#include "Debug/Debug.pkg"
namespace Z80 {

//----------------------------------------------------------------------------------------
//Data transfer
//----------------------------------------------------------------------------------------
double EffectiveAddress::Read(Z80* cpu, const Z80Word& pc, Data& target) const
{
	double additionalTime = 0;

	switch(mode)
	{
	default:
		DebugAssert(false);
		break;
	case MODE_A:
		cpu->GetA(target);
		break;
	case MODE_B:
		cpu->GetB(target);
		break;
	case MODE_C:
		cpu->GetC(target);
		break;
	case MODE_D:
		cpu->GetD(target);
		break;
	case MODE_E:
		cpu->GetE(target);
		break;

	case MODE_AF:
		cpu->GetAF(target);
		break;
	case MODE_BC:
		cpu->GetBC(target);
		break;
	case MODE_DE:
		cpu->GetDE(target);
		break;

	case MODE_AF2:
		cpu->GetAF2(target);
		break;
	case MODE_BC2:
		cpu->GetBC2(target);
		break;
	case MODE_DE2:
		cpu->GetDE2(target);
		break;

	case MODE_I:
		cpu->GetI(target);
		break;
	case MODE_R:
		cpu->GetR(target);
		break;
	case MODE_SP:
		cpu->GetSP(target);
		break;
	case MODE_PC:
		cpu->GetPC(target);
		break;
	case MODE_BC_INDIRECT:
		additionalTime = cpu->ReadMemory(cpu->GetBC(), target, false);
		break;
	case MODE_DE_INDIRECT:
		additionalTime = cpu->ReadMemory(cpu->GetDE(), target, false);
		break;
	case MODE_SP_INDIRECT:
		additionalTime = cpu->ReadMemory(cpu->GetSP(), target, false);
		break;
	case MODE_SP_PREDEC:
		cpu->SetSP(cpu->GetSP() - target.GetByteSize());
		additionalTime = cpu->ReadMemory(cpu->GetSP(), target, false);
		break;
	case MODE_DE_POSTINC:
		additionalTime = cpu->ReadMemory(cpu->GetDE(), target, false);
		cpu->SetDE(cpu->GetDE() + target.GetByteSize());
		break;
	case MODE_SP_POSTINC:
		additionalTime = cpu->ReadMemory(cpu->GetSP(), target, false);
		cpu->SetSP(cpu->GetSP() + target.GetByteSize());
		break;
	case MODE_DE_POSTDEC:
		additionalTime = cpu->ReadMemory(cpu->GetDE(), target, false);
		cpu->SetDE(cpu->GetDE() - target.GetByteSize());
		break;
	case MODE_ADDRESS_INDIRECT:
		additionalTime = cpu->ReadMemory(address, target, false);
		break;
	case MODE_IMMEDIATE:
		target = data;
		break;

	case MODE_H:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			cpu->GetH(target);
			break;
		case INDEX_IX:
			cpu->GetIXHigh(target);
			break;
		case INDEX_IY:
			cpu->GetIYHigh(target);
			break;
		}
		break;
	case MODE_L:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			cpu->GetL(target);
			break;
		case INDEX_IX:
			cpu->GetIXLow(target);
			break;
		case INDEX_IY:
			cpu->GetIYLow(target);
			break;
		}
		break;
	case MODE_HL:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			cpu->GetHL(target);
			break;
		case INDEX_IX:
			cpu->GetIX(target);
			break;
		case INDEX_IY:
			cpu->GetIY(target);
			break;
		}
		break;
	case MODE_HL2:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			cpu->GetHL2(target);
			break;
		}
		break;
	case MODE_HL_INDIRECT:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			additionalTime = cpu->ReadMemory(cpu->GetHL(), target, false);
			break;
		case INDEX_IX:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIX();
			additionalTime = cpu->ReadMemory(offset, target, false);
			break;}
		case INDEX_IY:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIY();
			additionalTime = cpu->ReadMemory(offset, target, false);
			break;}
		}
		break;
	case MODE_HL_POSTINC:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			additionalTime = cpu->ReadMemory(cpu->GetHL(), target, false);
			cpu->SetHL(cpu->GetHL() + target.GetByteSize());
			break;
		case INDEX_IX:
			additionalTime = cpu->ReadMemory(cpu->GetIX(), target, false);
			cpu->SetIX(cpu->GetIX() + target.GetByteSize());
			break;
		case INDEX_IY:
			additionalTime = cpu->ReadMemory(cpu->GetIY(), target, false);
			cpu->SetIY(cpu->GetIY() + target.GetByteSize());
			break;
		}
		break;
	case MODE_HL_POSTDEC:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			additionalTime = cpu->ReadMemory(cpu->GetHL(), target, false);
			cpu->SetHL(cpu->GetHL() - target.GetByteSize());
			break;
		case INDEX_IX:
			additionalTime = cpu->ReadMemory(cpu->GetIX(), target, false);
			cpu->SetIX(cpu->GetIX() - target.GetByteSize());
			break;
		case INDEX_IY:
			additionalTime = cpu->ReadMemory(cpu->GetIY(), target, false);
			cpu->SetIY(cpu->GetIY() - target.GetByteSize());
			break;
		}
		break;
	}

	return additionalTime;
}

//----------------------------------------------------------------------------------------
double EffectiveAddress::Write(Z80* cpu, const Z80Word& pc, const Data& target) const
{
	double additionalTime = 0;

	switch(mode)
	{
	default:
		DebugAssert(false);
		break;
	case MODE_A:
		cpu->SetA(Z80Byte(target));
		break;
	case MODE_B:
		cpu->SetB(Z80Byte(target));
		break;
	case MODE_C:
		cpu->SetC(Z80Byte(target));
		break;
	case MODE_D:
		cpu->SetD(Z80Byte(target));
		break;
	case MODE_E:
		cpu->SetE(Z80Byte(target));
		break;

	case MODE_AF:
		cpu->SetAF(Z80Word(target));
		break;
	case MODE_BC:
		cpu->SetBC(Z80Word(target));
		break;
	case MODE_DE:
		cpu->SetDE(Z80Word(target));
		break;

	case MODE_AF2:
		cpu->SetAF2(Z80Word(target));
		break;
	case MODE_BC2:
		cpu->SetBC2(Z80Word(target));
		break;
	case MODE_DE2:
		cpu->SetDE2(Z80Word(target));
		break;

	case MODE_I:
		cpu->SetI(Z80Byte(target));
		break;
	case MODE_R:
		cpu->SetR(Z80Byte(target));
		break;
	case MODE_SP:
		cpu->SetSP(Z80Word(target));
		break;
	case MODE_PC:
		cpu->SetPC(Z80Word(target));
		break;
	case MODE_BC_INDIRECT:
		additionalTime = cpu->WriteMemory(cpu->GetBC(), target, false);
		break;
	case MODE_DE_INDIRECT:
		additionalTime = cpu->WriteMemory(cpu->GetDE(), target, false);
		break;
	case MODE_SP_INDIRECT:
		additionalTime = cpu->WriteMemory(cpu->GetSP(), target, false);
		break;
	case MODE_SP_PREDEC:
		cpu->SetSP(cpu->GetSP() - target.GetByteSize());
		additionalTime = cpu->WriteMemory(cpu->GetSP(), target, false);
		break;
	case MODE_DE_POSTINC:
		additionalTime = cpu->WriteMemory(cpu->GetDE(), target, false);
		cpu->SetDE(cpu->GetDE() + target.GetByteSize());
		break;
	case MODE_SP_POSTINC:
		additionalTime = cpu->WriteMemory(cpu->GetSP(), target, false);
		cpu->SetSP(cpu->GetSP() + target.GetByteSize());
		break;
	case MODE_DE_POSTDEC:
		additionalTime = cpu->WriteMemory(cpu->GetDE(), target, false);
		cpu->SetDE(cpu->GetDE() - target.GetByteSize());
		break;
	case MODE_ADDRESS_INDIRECT:
		additionalTime = cpu->WriteMemory(address, target, false);
		break;
	case MODE_IMMEDIATE:
		DebugAssert(false);
		break;

	case MODE_H:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			cpu->SetH(Z80Byte(target));
			break;
		case INDEX_IX:
			cpu->SetIXHigh(Z80Byte(target));
			break;
		case INDEX_IY:
			cpu->SetIYHigh(Z80Byte(target));
			break;
		}
		break;
	case MODE_L:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			cpu->SetL(Z80Byte(target));
			break;
		case INDEX_IX:
			cpu->SetIXLow(Z80Byte(target));
			break;
		case INDEX_IY:
			cpu->SetIYLow(Z80Byte(target));
			break;
		}
		break;
	case MODE_HL:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			cpu->SetHL(Z80Word(target));
			break;
		case INDEX_IX:
			cpu->SetIX(Z80Word(target));
			break;
		case INDEX_IY:
			cpu->SetIY(Z80Word(target));
			break;
		}
		break;
	case MODE_HL2:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			cpu->SetHL2(Z80Word(target));
			break;
		}
		break;
	case MODE_HL_INDIRECT:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			additionalTime = cpu->WriteMemory(cpu->GetHL(), target, false);
			break;
		case INDEX_IX:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIX();
			additionalTime = cpu->WriteMemory(offset, target, false);
			break;}
		case INDEX_IY:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIY();
			additionalTime = cpu->WriteMemory(offset, target, false);
			break;}
		}
		break;
	case MODE_HL_POSTINC:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			additionalTime = cpu->WriteMemory(cpu->GetHL(), target, false);
			cpu->SetHL(cpu->GetHL() + target.GetByteSize());
			break;
		case INDEX_IX:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIX();
			additionalTime = cpu->WriteMemory(offset, target, false);
			cpu->SetIX(cpu->GetIX() + target.GetByteSize());
			break;}
		case INDEX_IY:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIY();
			additionalTime = cpu->WriteMemory(offset, target, false);
			cpu->SetIY(cpu->GetIY() + target.GetByteSize());
			break;}
		}
		break;
	case MODE_HL_POSTDEC:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case INDEX_NONE:
			additionalTime = cpu->WriteMemory(cpu->GetHL(), target, false);
			cpu->SetHL(cpu->GetHL() - target.GetByteSize());
			break;
		case INDEX_IX:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIX();
			additionalTime = cpu->WriteMemory(offset, target, false);
			cpu->SetIX(cpu->GetIX() - target.GetByteSize());
			break;}
		case INDEX_IY:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIY();
			additionalTime = cpu->WriteMemory(offset, target, false);
			cpu->SetIY(cpu->GetIY() - target.GetByteSize());
			break;}
		}
		break;
	}

	return additionalTime;
}

//----------------------------------------------------------------------------------------
//Disassembly functions
//----------------------------------------------------------------------------------------
std::wstring EffectiveAddress::Disassemble() const
{
	switch(mode)
	{
	case MODE_A:	return L"A";
	case MODE_B:	return L"B";
	case MODE_C:	return L"C";
	case MODE_D:	return L"D";
	case MODE_E:	return L"E";

	case MODE_AF:	return L"AF";
	case MODE_BC:	return L"BC";
	case MODE_DE:	return L"DE";

	case MODE_AF2:	return L"AF'";
	case MODE_BC2:	return L"BC'";
	case MODE_DE2:	return L"DE'";
	case MODE_HL2:	return L"HL'";

	case MODE_I:	return L"I";
	case MODE_R:	return L"R";
	case MODE_SP:	return L"SP";
	case MODE_PC:	return L"PC";
	case MODE_BC_INDIRECT:	return L"(BC)";
	case MODE_DE_INDIRECT:	return L"(DE)";
	case MODE_SP_INDIRECT:	return L"(SP)";
	case MODE_ADDRESS_INDIRECT:{
		std::wstringstream stream;
		stream << L'(' << std::setw(address.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase << address.GetData() << L"h)";
		return stream.str();}
	case MODE_IMMEDIATE:{
		std::wstringstream stream;
		if(data.GetData() > 9)
		{
			stream << std::setw(data.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase << data.GetData() << L'h';
		}
		else
		{
			stream << data.GetData();
		}
		return stream.str();}
	}

	if(indexState == INDEX_NONE)
	{
		switch(mode)
		{
		case MODE_H:	return L"H";
		case MODE_L:	return L"L";
		case MODE_HL:	return L"HL";
		case MODE_HL_INDIRECT:	return L"(HL)";
		}
	}
	else if(indexState == INDEX_IX)
	{
		switch(mode)
		{
		case MODE_H:	return L"IXH";
		case MODE_L:	return L"IXL";
		case MODE_HL:	return L"IX";
		case MODE_HL_INDIRECT:{
			std::wstringstream stream;
			stream << L"(IX+" << std::setw(2) << std::setfill(L'0') << std::hex << std::uppercase << GetIndexOffset().GetData() << L"h)";
			return stream.str();}
		}
	}
	else if(indexState == INDEX_IY)
	{
		switch(mode)
		{
		case MODE_H:	return L"IYH";
		case MODE_L:	return L"IYL";
		case MODE_HL:	return L"IY";
		case MODE_HL_INDIRECT:{
			std::wstringstream stream;
			stream << L"(IY+" << std::setw(2) << std::setfill(L'0') << std::hex << std::uppercase << GetIndexOffset().GetData() << L"h)";
			return stream.str();}
		}
	}

	return L"INVALID";
}

//----------------------------------------------------------------------------------------
std::wstring EffectiveAddress::DisassembleImmediateAsPCDisplacement(const Z80Word& location) const
{
	Z80Word newPC = location + Z80Word(data.SignExtend(BITCOUNT_WORD));
	std::wstringstream stream;
	stream << std::setw(newPC.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase << newPC.GetData();
	return stream.str();
}

} //Close namespace Z80
