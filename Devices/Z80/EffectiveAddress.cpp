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
	case Mode::A:
		cpu->GetA(target);
		break;
	case Mode::B:
		cpu->GetB(target);
		break;
	case Mode::C:
		cpu->GetC(target);
		break;
	case Mode::D:
		cpu->GetD(target);
		break;
	case Mode::E:
		cpu->GetE(target);
		break;

	case Mode::AF:
		cpu->GetAF(target);
		break;
	case Mode::BC:
		cpu->GetBC(target);
		break;
	case Mode::DE:
		cpu->GetDE(target);
		break;

	case Mode::AF2:
		cpu->GetAF2(target);
		break;
	case Mode::BC2:
		cpu->GetBC2(target);
		break;
	case Mode::DE2:
		cpu->GetDE2(target);
		break;

	case Mode::I:
		cpu->GetI(target);
		break;
	case Mode::R:
		cpu->GetR(target);
		break;
	case Mode::SP:
		cpu->GetSP(target);
		break;
	case Mode::PC:
		cpu->GetPC(target);
		break;
	case Mode::BCIndirect:
		additionalTime = cpu->ReadMemory(cpu->GetBC(), target, false);
		break;
	case Mode::DEIndirect:
		additionalTime = cpu->ReadMemory(cpu->GetDE(), target, false);
		break;
	case Mode::SPIndirect:
		additionalTime = cpu->ReadMemory(cpu->GetSP(), target, false);
		break;
	case Mode::SPPreDec:
		cpu->SetSP(cpu->GetSP() - target.GetByteSize());
		additionalTime = cpu->ReadMemory(cpu->GetSP(), target, false);
		break;
	case Mode::DEPostInc:
		additionalTime = cpu->ReadMemory(cpu->GetDE(), target, false);
		cpu->SetDE(cpu->GetDE() + target.GetByteSize());
		break;
	case Mode::SPPostInc:
		additionalTime = cpu->ReadMemory(cpu->GetSP(), target, false);
		cpu->SetSP(cpu->GetSP() + target.GetByteSize());
		break;
	case Mode::DEPostDec:
		additionalTime = cpu->ReadMemory(cpu->GetDE(), target, false);
		cpu->SetDE(cpu->GetDE() - target.GetByteSize());
		break;
	case Mode::AddressIndirect:
		additionalTime = cpu->ReadMemory(address, target, false);
		break;
	case Mode::Immediate:
		target = data;
		break;

	case Mode::H:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			cpu->GetH(target);
			break;
		case IndexState::IX:
			cpu->GetIXHigh(target);
			break;
		case IndexState::IY:
			cpu->GetIYHigh(target);
			break;
		}
		break;
	case Mode::L:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			cpu->GetL(target);
			break;
		case IndexState::IX:
			cpu->GetIXLow(target);
			break;
		case IndexState::IY:
			cpu->GetIYLow(target);
			break;
		}
		break;
	case Mode::HL:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			cpu->GetHL(target);
			break;
		case IndexState::IX:
			cpu->GetIX(target);
			break;
		case IndexState::IY:
			cpu->GetIY(target);
			break;
		}
		break;
	case Mode::HL2:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			cpu->GetHL2(target);
			break;
		}
		break;
	case Mode::HLIndirect:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			additionalTime = cpu->ReadMemory(cpu->GetHL(), target, false);
			break;
		case IndexState::IX:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIX();
			additionalTime = cpu->ReadMemory(offset, target, false);
			break;}
		case IndexState::IY:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIY();
			additionalTime = cpu->ReadMemory(offset, target, false);
			break;}
		}
		break;
	case Mode::HLPostInc:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			additionalTime = cpu->ReadMemory(cpu->GetHL(), target, false);
			cpu->SetHL(cpu->GetHL() + target.GetByteSize());
			break;
		case IndexState::IX:
			additionalTime = cpu->ReadMemory(cpu->GetIX(), target, false);
			cpu->SetIX(cpu->GetIX() + target.GetByteSize());
			break;
		case IndexState::IY:
			additionalTime = cpu->ReadMemory(cpu->GetIY(), target, false);
			cpu->SetIY(cpu->GetIY() + target.GetByteSize());
			break;
		}
		break;
	case Mode::HLPostDec:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			additionalTime = cpu->ReadMemory(cpu->GetHL(), target, false);
			cpu->SetHL(cpu->GetHL() - target.GetByteSize());
			break;
		case IndexState::IX:
			additionalTime = cpu->ReadMemory(cpu->GetIX(), target, false);
			cpu->SetIX(cpu->GetIX() - target.GetByteSize());
			break;
		case IndexState::IY:
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
	case Mode::A:
		cpu->SetA(Z80Byte(target));
		break;
	case Mode::B:
		cpu->SetB(Z80Byte(target));
		break;
	case Mode::C:
		cpu->SetC(Z80Byte(target));
		break;
	case Mode::D:
		cpu->SetD(Z80Byte(target));
		break;
	case Mode::E:
		cpu->SetE(Z80Byte(target));
		break;

	case Mode::AF:
		cpu->SetAF(Z80Word(target));
		break;
	case Mode::BC:
		cpu->SetBC(Z80Word(target));
		break;
	case Mode::DE:
		cpu->SetDE(Z80Word(target));
		break;

	case Mode::AF2:
		cpu->SetAF2(Z80Word(target));
		break;
	case Mode::BC2:
		cpu->SetBC2(Z80Word(target));
		break;
	case Mode::DE2:
		cpu->SetDE2(Z80Word(target));
		break;

	case Mode::I:
		cpu->SetI(Z80Byte(target));
		break;
	case Mode::R:
		cpu->SetR(Z80Byte(target));
		break;
	case Mode::SP:
		cpu->SetSP(Z80Word(target));
		break;
	case Mode::PC:
		cpu->SetPC(Z80Word(target));
		break;
	case Mode::BCIndirect:
		additionalTime = cpu->WriteMemory(cpu->GetBC(), target, false);
		break;
	case Mode::DEIndirect:
		additionalTime = cpu->WriteMemory(cpu->GetDE(), target, false);
		break;
	case Mode::SPIndirect:
		additionalTime = cpu->WriteMemory(cpu->GetSP(), target, false);
		break;
	case Mode::SPPreDec:
		cpu->SetSP(cpu->GetSP() - target.GetByteSize());
		additionalTime = cpu->WriteMemory(cpu->GetSP(), target, false);
		break;
	case Mode::DEPostInc:
		additionalTime = cpu->WriteMemory(cpu->GetDE(), target, false);
		cpu->SetDE(cpu->GetDE() + target.GetByteSize());
		break;
	case Mode::SPPostInc:
		additionalTime = cpu->WriteMemory(cpu->GetSP(), target, false);
		cpu->SetSP(cpu->GetSP() + target.GetByteSize());
		break;
	case Mode::DEPostDec:
		additionalTime = cpu->WriteMemory(cpu->GetDE(), target, false);
		cpu->SetDE(cpu->GetDE() - target.GetByteSize());
		break;
	case Mode::AddressIndirect:
		additionalTime = cpu->WriteMemory(address, target, false);
		break;
	case Mode::Immediate:
		DebugAssert(false);
		break;

	case Mode::H:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			cpu->SetH(Z80Byte(target));
			break;
		case IndexState::IX:
			cpu->SetIXHigh(Z80Byte(target));
			break;
		case IndexState::IY:
			cpu->SetIYHigh(Z80Byte(target));
			break;
		}
		break;
	case Mode::L:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			cpu->SetL(Z80Byte(target));
			break;
		case IndexState::IX:
			cpu->SetIXLow(Z80Byte(target));
			break;
		case IndexState::IY:
			cpu->SetIYLow(Z80Byte(target));
			break;
		}
		break;
	case Mode::HL:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			cpu->SetHL(Z80Word(target));
			break;
		case IndexState::IX:
			cpu->SetIX(Z80Word(target));
			break;
		case IndexState::IY:
			cpu->SetIY(Z80Word(target));
			break;
		}
		break;
	case Mode::HL2:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			cpu->SetHL2(Z80Word(target));
			break;
		}
		break;
	case Mode::HLIndirect:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			additionalTime = cpu->WriteMemory(cpu->GetHL(), target, false);
			break;
		case IndexState::IX:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIX();
			additionalTime = cpu->WriteMemory(offset, target, false);
			break;}
		case IndexState::IY:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIY();
			additionalTime = cpu->WriteMemory(offset, target, false);
			break;}
		}
		break;
	case Mode::HLPostInc:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			additionalTime = cpu->WriteMemory(cpu->GetHL(), target, false);
			cpu->SetHL(cpu->GetHL() + target.GetByteSize());
			break;
		case IndexState::IX:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIX();
			additionalTime = cpu->WriteMemory(offset, target, false);
			cpu->SetIX(cpu->GetIX() + target.GetByteSize());
			break;}
		case IndexState::IY:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIY();
			additionalTime = cpu->WriteMemory(offset, target, false);
			cpu->SetIY(cpu->GetIY() + target.GetByteSize());
			break;}
		}
		break;
	case Mode::HLPostDec:
		switch(indexState)
		{
		default:
			DebugAssert(false);
			break;
		case IndexState::None:
			additionalTime = cpu->WriteMemory(cpu->GetHL(), target, false);
			cpu->SetHL(cpu->GetHL() - target.GetByteSize());
			break;
		case IndexState::IX:{
			Z80Word offset(GetIndexOffset().SignExtend(BITCOUNT_WORD));
			offset += cpu->GetIX();
			additionalTime = cpu->WriteMemory(offset, target, false);
			cpu->SetIX(cpu->GetIX() - target.GetByteSize());
			break;}
		case IndexState::IY:{
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
	case Mode::A:	return L"A";
	case Mode::B:	return L"B";
	case Mode::C:	return L"C";
	case Mode::D:	return L"D";
	case Mode::E:	return L"E";

	case Mode::AF:	return L"AF";
	case Mode::BC:	return L"BC";
	case Mode::DE:	return L"DE";

	case Mode::AF2:	return L"AF'";
	case Mode::BC2:	return L"BC'";
	case Mode::DE2:	return L"DE'";
	case Mode::HL2:	return L"HL'";

	case Mode::I:	return L"I";
	case Mode::R:	return L"R";
	case Mode::SP:	return L"SP";
	case Mode::PC:	return L"PC";
	case Mode::BCIndirect:	return L"(BC)";
	case Mode::DEIndirect:	return L"(DE)";
	case Mode::SPIndirect:	return L"(SP)";
	case Mode::AddressIndirect:{
		std::wstringstream stream;
		stream << L'(' << std::setw(address.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase << address.GetData() << L"h)";
		return stream.str();}
	case Mode::Immediate:{
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

	if(indexState == IndexState::None)
	{
		switch(mode)
		{
		case Mode::H:	return L"H";
		case Mode::L:	return L"L";
		case Mode::HL:	return L"HL";
		case Mode::HLIndirect:	return L"(HL)";
		}
	}
	else if(indexState == IndexState::IX)
	{
		switch(mode)
		{
		case Mode::H:	return L"IXH";
		case Mode::L:	return L"IXL";
		case Mode::HL:	return L"IX";
		case Mode::HLIndirect:{
			std::wstringstream stream;
			stream << L"(IX+" << std::setw(2) << std::setfill(L'0') << std::hex << std::uppercase << GetIndexOffset().GetData() << L"h)";
			return stream.str();}
		}
	}
	else if(indexState == IndexState::IY)
	{
		switch(mode)
		{
		case Mode::H:	return L"IYH";
		case Mode::L:	return L"IYL";
		case Mode::HL:	return L"IY";
		case Mode::HLIndirect:{
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
