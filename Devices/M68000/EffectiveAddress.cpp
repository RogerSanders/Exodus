#include "EffectiveAddress.h"
#include "M68000.h"
#include "M68000Instruction.h"
#include <sstream>
namespace M68000 {

//----------------------------------------------------------------------------------------------------------------------
// Extension word size tables
//----------------------------------------------------------------------------------------------------------------------
const unsigned int EffectiveAddress::ExtensionSize8[] = {
	0,	// Mode::DATAREG_DIRECT
	0,	// Mode::ADDREG_DIRECT
	0,	// ADDREG_INDIRECT
	0,	// Mode::ADDREG_INDIRECT_POSTINC
	0,	// Mode::ADDREG_INDIRECT_PREDEC
	2,	// Mode::ADDREG_INDIRECT_DISPLACE
	2, 	// Mode::ADDREG_INDIRECT_INDEX_8BIT
	2, 	// Mode::PC_INDIRECT_DISPLACE
	2, 	// Mode::PC_INDIRECT_INDEX_8BIT
	2, 	// Mode::ABS_WORD
	4, 	// Mode::ABS_LONG
	2};	// IMMEDIATE

//----------------------------------------------------------------------------------------------------------------------
const unsigned int EffectiveAddress::ExtensionSize32[] = {
	0,	// Mode::DATAREG_DIRECT
	0,	// Mode::ADDREG_DIRECT
	0,	// ADDREG_INDIRECT
	0,	// Mode::ADDREG_INDIRECT_POSTINC
	0,	// Mode::ADDREG_INDIRECT_PREDEC
	2,	// Mode::ADDREG_INDIRECT_DISPLACE
	2, 	// Mode::ADDREG_INDIRECT_INDEX_8BIT
	2, 	// Mode::PC_INDIRECT_DISPLACE
	2, 	// Mode::PC_INDIRECT_INDEX_8BIT
	2, 	// Mode::ABS_WORD
	4, 	// Mode::ABS_LONG
	4};	// IMMEDIATE

//----------------------------------------------------------------------------------------------------------------------
// Decode time tables
//----------------------------------------------------------------------------------------------------------------------
const ExecuteTime EffectiveAddress::ExecuteTime8[] = {
	ExecuteTime(0,  0, 0),	// Mode::DATAREG_DIRECT
	ExecuteTime(0,  0, 0),	// Mode::ADDREG_DIRECT
	ExecuteTime(4,  1, 0),	// ADDREG_INDIRECT
	ExecuteTime(4,  1, 0),	// Mode::ADDREG_INDIRECT_POSTINC
	ExecuteTime(6,  1, 0),	// Mode::ADDREG_INDIRECT_PREDEC
	ExecuteTime(8,  2, 0),	// Mode::ADDREG_INDIRECT_DISPLACE
	ExecuteTime(10, 2, 0), 	// Mode::ADDREG_INDIRECT_INDEX_8BIT
	ExecuteTime(8,  2, 0), 	// Mode::PC_INDIRECT_DISPLACE
	ExecuteTime(10, 2, 0), 	// Mode::PC_INDIRECT_INDEX_8BIT
	ExecuteTime(8,  2, 0), 	// Mode::ABS_WORD
	ExecuteTime(12, 3, 0), 	// Mode::ABS_LONG
	ExecuteTime(4,  1, 0)};	// IMMEDIATE

//----------------------------------------------------------------------------------------------------------------------
const ExecuteTime EffectiveAddress::ExecuteTime32[] = {
	ExecuteTime(0,  0, 0),	// Mode::DATAREG_DIRECT
	ExecuteTime(0,  0, 0),	// Mode::ADDREG_DIRECT
	ExecuteTime(8,  2, 0),	// ADDREG_INDIRECT
	ExecuteTime(8,  2, 0),	// Mode::ADDREG_INDIRECT_POSTINC
	ExecuteTime(10, 2, 0),	// Mode::ADDREG_INDIRECT_PREDEC
	ExecuteTime(12, 3, 0),	// Mode::ADDREG_INDIRECT_DISPLACE
	ExecuteTime(14, 3, 0), 	// Mode::ADDREG_INDIRECT_INDEX_8BIT
	ExecuteTime(12, 3, 0), 	// Mode::PC_INDIRECT_DISPLACE
	ExecuteTime(14, 3, 0), 	// Mode::PC_INDIRECT_INDEX_8BIT
	ExecuteTime(12, 3, 0), 	// Mode::ABS_WORD
	ExecuteTime(16, 4, 0), 	// Mode::ABS_LONG
	ExecuteTime(8,  2, 0)};	// IMMEDIATE

//----------------------------------------------------------------------------------------------------------------------
// Decode functions
//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::Decode(unsigned int reg, unsigned int mode, Bitcount size, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	_savedPC = location;
	switch (mode)
	{
	default:
		DebugAssert(false);
		break;
	case 0:	// 000
		BuildDataDirect(size, location, reg);
		break;
	case 1:	// 001
		BuildAddressDirect(size, location, reg);
		break;
	case 2: // 010
		BuildAddressIndirect(size, location, reg);
		break;
	case 3:	// 011
		BuildAddressPostinc(size, location, reg);
		break;
	case 4:	// 100
		BuildAddressPredec(size, location, reg);
		break;
	case 5:	// 101
		BuildAddressIndirectDisplace(size, location, reg, cpu, transparent, instructionRegister);
		break;
	case 6:	// 110
		BuildAddressIndirectIndex(size, location, reg, cpu, transparent, instructionRegister);
		break;
	case 7:	// 111
		switch (reg)
		{
		case 0:	// 000
			BuildAbsoluteAddressWord(size, location, cpu, transparent, instructionRegister);
			break;
		case 1:	// 001
			BuildAbsoluteAddressLong(size, location, cpu, transparent, instructionRegister);
			break;
		case 2:	// 010
			BuildPCIndirectDisplace(size, location, cpu, transparent, instructionRegister);
			break;
		case 3:	// 011
			BuildPCIndirectIndex(size, location, cpu, transparent, instructionRegister);
			break;
		case 4:	// 100
			BuildImmediateData(size, location, cpu, transparent, instructionRegister);
			break;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Address functions
//----------------------------------------------------------------------------------------------------------------------
bool EffectiveAddress::GetAddress(const M68000* cpu, Data& address) const
{
	switch (_mode)
	{
	default:
		DebugAssert(false);
		return false;
	case Mode::AddRegIndirect:
		cpu->GetA(_reg, address);
		break;
	case Mode::AddRegIndirectDisplace:
		cpu->GetA(_reg, address);
		address += _displacement.SignExtend(BITCOUNT_LONG);
		break;
	case Mode::AddRegIndirectIndex8Bit:{
		cpu->GetA(_reg, address);

		// Get index value
		Data index(_indexSize);
		if (!_useAddressRegister)
		{
			cpu->GetD(_indexReg, index);
		}
		else
		{
			cpu->GetA(_indexReg, index);
		}

		address += _displacement.SignExtend(BITCOUNT_LONG);
		address += index.SignExtend(BITCOUNT_LONG);
		break;}
	case Mode::PCIndirectDisplace:
		address = _savedPC;
		address += _displacement.SignExtend(BITCOUNT_LONG);
		break;
	case Mode::PCIndirectIndex8Bit:{
		address = _savedPC;

		// Get index value
		Data index(_indexSize);
		if (!_useAddressRegister)
		{
			cpu->GetD(_indexReg, index);
		}
		else
		{
			cpu->GetA(_indexReg, index);
		}

		address += _displacement.SignExtend(BITCOUNT_LONG);
		address += index.SignExtend(BITCOUNT_LONG);
		break;}
	case Mode::ABSWord:
	case Mode::ABSLong:
		address = _address.SignExtend(BITCOUNT_LONG);
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool EffectiveAddress::GetAddressTransparent(Data& address) const
{
	switch (_mode)
	{
	default:
		return false;
	case Mode::PCIndirectDisplace:
		address = _savedPC;
		address += _displacement.SignExtend(BITCOUNT_LONG);
		break;
	case Mode::ABSWord:
	case Mode::ABSLong:
		address = _address.SignExtend(BITCOUNT_LONG);
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool EffectiveAddress::GetAddressDisplacementTargetNoIndex(M68000* cpu, Data& address) const
{
	switch (_mode)
	{
	default:
		return false;
	case Mode::AddRegIndirectDisplace:
	case Mode::AddRegIndirectIndex8Bit:
		cpu->GetA(_reg, address);
		address += _displacement.SignExtend(BITCOUNT_LONG);
		break;
	case Mode::PCIndirectDisplace:
	case Mode::PCIndirectIndex8Bit:
		address = _savedPC;
		address += _displacement.SignExtend(BITCOUNT_LONG);
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Data transfer
//----------------------------------------------------------------------------------------------------------------------
double EffectiveAddress::Read(M68000* cpu, Data& target, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	double additionalTime = 0;
	switch (_mode)
	{
	default:
		DebugAssert(false);
		break;
	case Mode::DataRegDirect:
		cpu->GetD(_reg, target);
		break;
	case Mode::AddRegDirect:
		cpu->GetA(_reg, target);
		break;
	case Mode::AddRegIndirect:{
		unsigned int originalAddressSourceLocation;
		if (cpu->DisassemblyGetAddressRegisterUnmodified(_reg, originalAddressSourceLocation))
		{
			cpu->AddDisassemblyAddressInfoOffset(originalAddressSourceLocation, cpu->DisassemblyGetAddressRegisterUnmodifiedSize(_reg), false, false, 0);
		}

		M68000Long address;
		cpu->GetA(_reg, address);
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;}
	case Mode::AddRegIndirectPostInc:{
		unsigned int originalAddressSourceLocation;
		if (cpu->DisassemblyGetAddressRegisterUnmodified(_reg, originalAddressSourceLocation))
		{
			cpu->AddDisassemblyAddressInfoOffset(originalAddressSourceLocation, cpu->DisassemblyGetAddressRegisterUnmodifiedSize(_reg), false, false, 0);
		}

		M68000Long address;
		cpu->GetA(_reg, address);
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		unsigned int arrayID = 0;
		if (!cpu->DisassemblyGetAddressRegisterLastAccessedInPostIncMode(_reg))
		{
			arrayID = cpu->MakeDataArrayAtLocation(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
			cpu->DisassemblySetAddressRegisterCurrentArrayID(_reg, arrayID);
		}
		else
		{
			arrayID = cpu->DisassemblyGetAddressRegisterCurrentArrayID(_reg);
		}
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer, arrayID);
		// Adjust the address register. We've got a little bit of code here to prevent
		// the stack pointer from ending up on an odd address when using byte access.
		// See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		// documentation says this alignment happens to maintain "maximum efficiency".
		// Of course, for the M68000 the real reason is that an address error would be
		// generated when attempting word or long access on an unaligned address, so
		// we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((_reg == M68000::SP) && (target.GetBitCount() == BITCOUNT_BYTE))? 2: target.GetByteSize();
		address += offset;
		cpu->SetA(_reg, address);
		cpu->DisassemblySetAddressRegisterLastAccessedInPostIncMode(_reg, true);
		cpu->DisassemblySetAddressRegisterUnmodified(_reg, false);
		break;}
	case Mode::AddRegIndirectPreDec:{
		M68000Long address;
		cpu->GetA(_reg, address);
		// Adjust the address register. We've got a little bit of code here to prevent
		// the stack pointer from ending up on an odd address when using byte access.
		// See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		// documentation says this alignment happens to maintain "maximum efficiency".
		// Of course, for the M68000 the real reason is that an address error would be
		// generated when attempting word or long access on an unaligned address, so
		// we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((_reg == M68000::SP) && (target.GetBitCount() == BITCOUNT_BYTE))? 2: target.GetByteSize();
		address -= offset;
		cpu->SetA(_reg, address);

		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		cpu->DisassemblySetAddressRegisterUnmodified(_reg, false);
		break;}
	case Mode::AddRegIndirectDisplace:{
		unsigned int originalAddressSourceLocation;
		if (cpu->DisassemblyGetAddressRegisterUnmodified(_reg, originalAddressSourceLocation))
		{
			cpu->AddDisassemblyAddressInfoOffset(originalAddressSourceLocation, cpu->DisassemblyGetAddressRegisterUnmodifiedSize(_reg), false, false, 0);
		}

		M68000Long address;
		cpu->GetA(_reg, address);
		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;}
	case Mode::AddRegIndirectIndex8Bit:{
		unsigned int originalAddressSourceLocation;
		if (cpu->DisassemblyGetAddressRegisterUnmodified(_reg, originalAddressSourceLocation))
		{
			cpu->AddDisassemblyAddressInfoOffset(originalAddressSourceLocation, cpu->DisassemblyGetAddressRegisterUnmodifiedSize(_reg), false, false, 0);
		}

		M68000Long address;
		cpu->GetA(_reg, address);

		// Get index value
		Data index(_indexSize);
		if (!_useAddressRegister)
		{
			cpu->GetD(_indexReg, index);
		}
		else
		{
			cpu->GetA(_indexReg, index);
		}

		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		address += M68000Long(index.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;}
	case Mode::PCIndirectDisplace:{
		M68000Long address;
		address = _savedPC;
		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(true), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;}
	case Mode::PCIndirectIndex8Bit:{
		M68000Long address;
		address = _savedPC;

		// Get index value
		Data index(_indexSize);
		if (!_useAddressRegister)
		{
			cpu->GetD(_indexReg, index);
		}
		else
		{
			cpu->GetA(_indexReg, index);
		}

		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		address += M68000Long(index.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(true), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;}

	case Mode::ABSWord:
	case Mode::ABSLong:
		additionalTime = cpu->ReadMemory(M68000Long(_address.SignExtend(BITCOUNT_LONG)), target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(_address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;

	case Mode::Immediate:
		//##TODO## I have a feeling this sign extension may not be correct in all cases. Check which opcodes might be
		//relying on this behaviour, and verify it on the hardware.
		//##NOTE## The LINK opcode is known to rely on this behaviour currently, which is correct in this case.
		target = _data.SignExtend(target.GetBitCount());
		break;
	}
	return additionalTime;
}

//----------------------------------------------------------------------------------------------------------------------
double EffectiveAddress::Write(M68000* cpu, const Data& target, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation, bool dataUnmodifiedFromMemoryRead, unsigned int sourceMemoryAddress, unsigned int sourceMemorySize) const
{
	double additionalTime = 0;
	switch (_mode)
	{
	default:
		DebugAssert(false);
		break;
	case Mode::DataRegDirect:{
		M68000Long newReg;
		cpu->GetD(_reg, newReg);
		newReg.SetLowerBits(target);
		cpu->SetD(_reg, newReg);
		cpu->DisassemblySetDataRegisterUnmodified(_reg, dataUnmodifiedFromMemoryRead, sourceMemorySize, sourceMemoryAddress);
		break;}
	case Mode::AddRegDirect:
		cpu->SetA(_reg, M68000Long(target.SignExtend(BITCOUNT_LONG)));
		cpu->DisassemblySetAddressRegisterUnmodified(_reg, dataUnmodifiedFromMemoryRead, sourceMemorySize, sourceMemoryAddress);
		break;
	case Mode::AddRegIndirect:{
		M68000Long address;
		cpu->GetA(_reg, address);
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}
	case Mode::AddRegIndirectPostInc:{
		M68000Long address;
		cpu->GetA(_reg, address);
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		// Adjust the address register. We've got a little bit of code here to prevent
		// the stack pointer from ending up on an odd address when using byte access.
		// See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		// documentation says this alignment happens to maintain "maximum efficiency".
		// Of course, for the M68000 the real reason is that an address error would be
		// generated when attempting word or long access on an unaligned address, so
		// we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((_reg == M68000::SP) && (target.GetBitCount() == BITCOUNT_BYTE))? 2: target.GetByteSize();
		address += offset;
		cpu->SetA(_reg, address);
		cpu->DisassemblySetAddressRegisterUnmodified(_reg, false);
		break;}
	case Mode::AddRegIndirectPreDec:{
		M68000Long address;
		cpu->GetA(_reg, address);
		// Adjust the address register. We've got a little bit of code here to prevent
		// the stack pointer from ending up on an odd address when using byte access.
		// See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		// documentation says this alignment happens to maintain "maximum efficiency".
		// Of course, for the M68000 the real reason is that an address error would be
		// generated when attempting word or long access on an unaligned address, so
		// we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((_reg == M68000::SP) && (target.GetBitCount() == BITCOUNT_BYTE))? 2: target.GetByteSize();
		address -= offset;
		cpu->SetA(_reg, address);

		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->DisassemblySetAddressRegisterUnmodified(_reg, false);
		break;}
	case Mode::AddRegIndirectDisplace:{
		M68000Long address;
		cpu->GetA(_reg, address);
		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}
	case Mode::AddRegIndirectIndex8Bit:{
		M68000Long address;
		cpu->GetA(_reg, address);

		// Get index value
		Data index(_indexSize);
		if (!_useAddressRegister)
		{
			cpu->GetD(_indexReg, index);
		}
		else
		{
			cpu->GetA(_indexReg, index);
		}

		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		address += M68000Long(index.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}
	case Mode::PCIndirectDisplace:{
		M68000Long address;
		address = _savedPC;
		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(true), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}
	case Mode::PCIndirectIndex8Bit:{
		M68000Long address;
		address = _savedPC;

		// Get index value
		Data index(_indexSize);
		if (!_useAddressRegister)
		{
			cpu->GetD(_indexReg, index);
		}
		else
		{
			cpu->GetA(_indexReg, index);
		}

		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		address += M68000Long(index.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(true), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}

	case Mode::ABSWord:
	case Mode::ABSLong:
		additionalTime = cpu->WriteMemory(M68000Long(_address.SignExtend(BITCOUNT_LONG)), target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;

	case Mode::Immediate:
		DebugAssert(false);
		break;
	}
	return additionalTime;
}

//----------------------------------------------------------------------------------------------------------------------
double EffectiveAddress::ReadWithoutAdjustingAddress(M68000* cpu, Data& target, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	double additionalTime = 0;
	if (_mode == Mode::AddRegIndirectPostInc)
	{
		M68000Long address;
		cpu->GetA(_reg, address);
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
	}
	else if (_mode == Mode::AddRegIndirectPreDec)
	{
		M68000Long address;
		cpu->GetA(_reg, address);
		// Adjust the address register. We've got a little bit of code here to prevent
		// the stack pointer from ending up on an odd address when using byte access.
		// See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		// documentation says this alignment happens to maintain "maximum efficiency".
		// Of course, for the M68000 the real reason is that an address error would be
		// generated when attempting word or long access on an unaligned address, so
		// we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((_reg == M68000::SP) && (target.GetBitCount() == BITCOUNT_BYTE))? 2: target.GetByteSize();
		address -= offset;
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), _savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
	}
	else
	{
		additionalTime = Read(cpu, target, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
	}
	return additionalTime;
}

//----------------------------------------------------------------------------------------------------------------------
bool EffectiveAddress::IsTargetUnmodifiedFromMemoryRead(M68000* cpu, Bitcount readSize, unsigned int& sourceMemoryAddress) const
{
	bool targetUnmodified = false;
	switch (_mode)
	{
	default:
		DebugAssert(false);
		break;
	case Mode::DataRegDirect:
		targetUnmodified = cpu->DisassemblyGetDataRegisterUnmodified(_reg, sourceMemoryAddress);
		break;
	case Mode::AddRegDirect:
		targetUnmodified = cpu->DisassemblyGetAddressRegisterUnmodified(_reg, sourceMemoryAddress);
		break;
	case Mode::AddRegIndirect:
	case Mode::AddRegIndirectPostInc:{
		M68000Long address;
		cpu->GetA(_reg, address);
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case Mode::AddRegIndirectPreDec:{
		M68000Long address;
		cpu->GetA(_reg, address);
		// Adjust the address register. We've got a little bit of code here to prevent
		// the stack pointer from ending up on an odd address when using byte access.
		// See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		// documentation says this alignment happens to maintain "maximum efficiency".
		// Of course, for the M68000 the real reason is that an address error would be
		// generated when attempting word or long access on an unaligned address, so
		// we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((_reg == M68000::SP) && (readSize == BITCOUNT_BYTE))? 2: readSize / 8;
		address -= offset;
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case Mode::AddRegIndirectDisplace:{
		M68000Long address;
		cpu->GetA(_reg, address);
		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case Mode::AddRegIndirectIndex8Bit:{
		M68000Long address;
		cpu->GetA(_reg, address);

		// Get index value
		Data index(_indexSize);
		if (!_useAddressRegister)
		{
			cpu->GetD(_indexReg, index);
		}
		else
		{
			cpu->GetA(_indexReg, index);
		}

		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		address += M68000Long(index.SignExtend(BITCOUNT_LONG));
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case Mode::PCIndirectDisplace:{
		M68000Long address;
		address = _savedPC;
		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case Mode::PCIndirectIndex8Bit:{
		M68000Long address;
		address = _savedPC;

		// Get index value
		Data index(_indexSize);
		if (!_useAddressRegister)
		{
			cpu->GetD(_indexReg, index);
		}
		else
		{
			cpu->GetA(_indexReg, index);
		}

		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		address += M68000Long(index.SignExtend(BITCOUNT_LONG));
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}

	case Mode::ABSWord:
	case Mode::ABSLong:
	case Mode::Immediate:
		targetUnmodified = false;
		break;
	}
	return targetUnmodified;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int EffectiveAddress::GetTargetOriginalMemoryReadSize(M68000* cpu, Bitcount readBitcount) const
{
	unsigned int memorySize = ((unsigned int)readBitcount) / 8;
	switch (_mode)
	{
	case Mode::DataRegDirect:{
		memorySize = cpu->DisassemblyGetDataRegisterUnmodifiedSize(_reg);
		break;}
	case Mode::AddRegDirect:
		memorySize = cpu->DisassemblyGetAddressRegisterUnmodifiedSize(_reg);
		break;
	}
	return memorySize;
}

//----------------------------------------------------------------------------------------------------------------------
bool EffectiveAddress::IsTargetUnmodifiedFromMemoryReadV2(M68000* cpu, unsigned int& sourceMemoryAddress, bool& dataIsOffset, unsigned int& offsetBaseAddress, unsigned int& dataSize) const
{
	bool targetUnmodified = false;
	switch (_mode)
	{
	default:
		DebugAssert(false);
		break;
	case Mode::AddRegIndirect:
		targetUnmodified = cpu->DisassemblyGetAddressRegisterUnmodified(_reg, sourceMemoryAddress);
		dataSize = cpu->DisassemblyGetAddressRegisterUnmodifiedSize(_reg);
		dataIsOffset = false;
		break;
	case Mode::AddRegIndirectIndex8Bit:{
		if (!_useAddressRegister)
		{
			targetUnmodified = cpu->DisassemblyGetDataRegisterUnmodified(_indexReg, sourceMemoryAddress);
		}
		else
		{
			targetUnmodified = cpu->DisassemblyGetAddressRegisterUnmodified(_indexReg, sourceMemoryAddress);
		}

		//##TODO## If the address register itself is also unmodified from a memory read,
		// it needs to be setup as a pointer too.
		M68000Long address;
		cpu->GetA(_reg, address);
		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		offsetBaseAddress = address.GetData();
		dataSize = _indexSize / 8;
		dataIsOffset = true;
		break;}
	case Mode::PCIndirectIndex8Bit:{
		if (!_useAddressRegister)
		{
			targetUnmodified = cpu->DisassemblyGetDataRegisterUnmodified(_indexReg, sourceMemoryAddress);
		}
		else
		{
			targetUnmodified = cpu->DisassemblyGetAddressRegisterUnmodified(_indexReg, sourceMemoryAddress);
		}

		M68000Long address;
		address = _savedPC;
		address += M68000Long(_displacement.SignExtend(BITCOUNT_LONG));
		offsetBaseAddress = address.GetData();
		dataSize = _indexSize / 8;
		dataIsOffset = true;
		break;}

	case Mode::DataRegDirect:
	case Mode::AddRegDirect:
	case Mode::AddRegIndirectDisplace:
	case Mode::PCIndirectDisplace:
	case Mode::ABSWord:
	case Mode::ABSLong:
	case Mode::Immediate:
		targetUnmodified = false;
		break;
	}
	return targetUnmodified;
}

//----------------------------------------------------------------------------------------------------------------------
// Disassembly functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring EffectiveAddress::Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
{
	std::wstringstream stream;

	switch (_mode)
	{
	default:
		stream << L"<INVALID>";
		break;
	case Mode::DataRegDirect:
		stream << L'D' << _reg;
		break;
	case Mode::AddRegDirect:
		stream << L'A' << _reg;
		break;
	case Mode::AddRegIndirect:
		stream << L"(A" << _reg << L")";
		break;
	case Mode::AddRegIndirectPostInc:
		stream << L"(A" << _reg << L")+";
		break;
	case Mode::AddRegIndirectPreDec:
		stream << L"-(A" << _reg << L")";
		break;
	case Mode::AddRegIndirectDisplace:{
		Data displacementExtended = _displacement.SignExtend(BITCOUNT_LONG);
		if (displacementExtended.Negative())
		{
			stream << L"-$" << std::hex << std::uppercase << 0 - displacementExtended.GetData();
		}
		else
		{
			stream << L"$" << std::hex << std::uppercase << displacementExtended.GetData();
		}
		stream << L"(A" << _reg << L")";
		break;}
	case Mode::AddRegIndirectIndex8Bit:
		if (_displacement.NonZero())
		{
			Data displacementExtended = _displacement.SignExtend(BITCOUNT_LONG);
			if (displacementExtended.Negative())
			{
				stream << L"-$" << std::hex << std::uppercase << 0 - displacementExtended.GetData();
			}
			else
			{
				stream << L"$" << std::hex << std::uppercase << displacementExtended.GetData();
			}
		}
		stream << L"(A" << _reg << L",";
		if (!_useAddressRegister)
		{
			stream << L"D";
		}
		else
		{
			stream << L"A";
		}
		stream << _indexReg << "." << M68000Instruction::DisassembleSize(_indexSize) << L")";
		break;
	case Mode::PCIndirectDisplace:{
		Data targetAddress(BITCOUNT_LONG);
		targetAddress = _savedPC.GetData() + _displacement.SignExtend(BITCOUNT_LONG).GetData();
		bool labelWritten = false;
		if (labelSettings.enableSubstitution)
		{
			std::map<unsigned int, M68000::LabelSubstitutionEntry>::const_iterator labelIterator = labelSettings.labelTargetsAddressingLocation.find(targetAddress.GetData());
			if (labelIterator != labelSettings.labelTargetsAddressingLocation.end())
			{
				stream << labelIterator->second.usageLabel << L"(PC)";
				labelWritten = true;
			}
		}
		if (!labelWritten)
		{
			if (targetAddress.Negative())
			{
				stream << L"*-$" << std::hex << std::uppercase << 0 - targetAddress.GetData();
			}
			else
			{
				stream << L"*+$" << std::hex << std::uppercase << targetAddress.GetData();
			}
		}
		break;}
	case Mode::PCIndirectIndex8Bit:{
		Data targetAddress(BITCOUNT_LONG);
		targetAddress = _savedPC.GetData() + _displacement.SignExtend(BITCOUNT_LONG).GetData();
		bool labelWritten = false;
		if (labelSettings.enableSubstitution)
		{
			std::map<unsigned int, M68000::LabelSubstitutionEntry>::const_iterator labelIterator = labelSettings.labelTargetsAddressingLocation.find(targetAddress.GetData());
			if (labelIterator != labelSettings.labelTargetsAddressingLocation.end())
			{
				stream << labelIterator->second.usageLabel;
				labelWritten = true;
			}
		}
		if (!labelWritten)
		{
			if (targetAddress.Negative())
			{
				stream << L"*-$" << std::hex << std::uppercase << 0 - targetAddress.GetData();
			}
			else
			{
				stream << L"*+$" << std::hex << std::uppercase << targetAddress.GetData();
			}
		}
		stream << L"(PC,";
		if (!_useAddressRegister)
		{
			stream << L"D";
		}
		else
		{
			stream << L"A";
		}
		stream << _indexReg << "." << M68000Instruction::DisassembleSize(_indexSize) << L")";
		break;}
	case Mode::ABSWord:
	case Mode::ABSLong:{
		Data targetAddress(BITCOUNT_LONG);
		targetAddress = _address.SignExtend(BITCOUNT_LONG);
		bool labelWritten = false;
		if (labelSettings.enableSubstitution)
		{
			std::map<unsigned int, M68000::LabelSubstitutionEntry>::const_iterator labelIterator = labelSettings.labelTargetsAddressingLocation.find(targetAddress.GetData());
			if (labelIterator != labelSettings.labelTargetsAddressingLocation.end())
			{
				stream << labelIterator->second.usageLabel;
				labelWritten = true;
			}
		}
		if (!labelWritten)
		{
			stream << L'$' << std::setw(targetAddress.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase << targetAddress.GetData();
			if (_address.GetBitCount() < BITCOUNT_LONG)
			{
				stream << L"." << M68000Instruction::DisassembleSize((Bitcount)_address.GetBitCount());
			}
		}
		break;}
	case Mode::Immediate:
		stream << L'#';
		if (_dataSignExtended)
		{
			Data dataModified = _data.SignExtend(BITCOUNT_LONG);
			if (dataModified.Negative())
			{
				stream << L"-";
				if ((0 - dataModified.GetData()) > 9)
				{
					stream << L'$' << std::setw(dataModified.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase;
				}
				stream << 0 - dataModified.GetData();
			}
			else
			{
				if (dataModified.GetData() > 9)
				{
					stream << L'$' << std::setw(dataModified.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase;
				}
				stream << dataModified.GetData();
			}
		}
		else
		{
			if (_data.GetData() > 9)
			{
				stream << L'$' << std::setw(_data.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase;
			}
			stream << _data.GetData();
		}
		break;
	}

	return stream.str();
}

//----------------------------------------------------------------------------------------------------------------------
std::wstring EffectiveAddress::DisassembleImmediateAsPCDisplacement(const M68000::LabelSubstitutionSettings& labelSettings) const
{
	M68000Long targetAddress = _savedPC + M68000Long(_data.SignExtend(BITCOUNT_LONG));
	std::wstringstream stream;
	bool labelWritten = false;
	if (labelSettings.enableSubstitution)
	{
		std::map<unsigned int, M68000::LabelSubstitutionEntry>::const_iterator labelIterator = labelSettings.labelTargetsAddressingLocation.find(targetAddress.GetData());
		if (labelIterator != labelSettings.labelTargetsAddressingLocation.end())
		{
			stream << labelIterator->second.usageLabel;
			labelWritten = true;
		}
	}
	if (!labelWritten)
	{
		if (targetAddress.Negative())
		{
			stream << L"*-$" << std::hex << std::uppercase << 0 - _data.GetData();
		}
		else
		{
			stream << L"*+$" << std::hex << std::uppercase << _data.GetData();
		}
	}
	return stream.str();
}

//----------------------------------------------------------------------------------------------------------------------
std::wstring EffectiveAddress::DisassembleImmediateAsPCDisplacementTargetAddressString() const
{
	M68000Long newPC = _savedPC + M68000Long(_data.SignExtend(BITCOUNT_LONG));
	std::wstringstream stream;
	stream << std::setw(newPC.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase << newPC.GetData();
	return stream.str();
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::AddLabelTargetsToSet(std::set<unsigned int>& labelTargetLocations) const
{
	switch (_mode)
	{
	case Mode::PCIndirectDisplace:
	case Mode::PCIndirectIndex8Bit:{
		Data targetAddress(BITCOUNT_LONG);
		targetAddress = _savedPC.GetData() + _displacement.SignExtend(BITCOUNT_LONG).GetData();
		labelTargetLocations.insert(targetAddress.GetData());
		break;}
	case Mode::ABSWord:
	case Mode::ABSLong:{
		M68000Long targetAddress = _address.SignExtend(BITCOUNT_LONG).GetData();
		labelTargetLocations.insert(targetAddress.GetData());
		break;}
	}
}

} // Close namespace M68000
