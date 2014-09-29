#include "EffectiveAddress.h"
#include "M68000.h"
#include "M68000Instruction.h"
#include <sstream>
namespace M68000 {

//----------------------------------------------------------------------------------------
//Extension word size tables
//----------------------------------------------------------------------------------------
const unsigned int EffectiveAddress::extensionSize8[] = {
	0,	//Mode::DATAREG_DIRECT
	0,	//Mode::ADDREG_DIRECT
	0,	//ADDREG_INDIRECT
	0,	//Mode::ADDREG_INDIRECT_POSTINC
	0,	//Mode::ADDREG_INDIRECT_PREDEC
	2,	//Mode::ADDREG_INDIRECT_DISPLACE
	2, 	//Mode::ADDREG_INDIRECT_INDEX_8BIT
	2, 	//Mode::PC_INDIRECT_DISPLACE
	2, 	//Mode::PC_INDIRECT_INDEX_8BIT
	2, 	//Mode::ABS_WORD
	4, 	//Mode::ABS_LONG
	2};	//IMMEDIATE

//----------------------------------------------------------------------------------------
const unsigned int EffectiveAddress::extensionSize32[] = {
	0,	//Mode::DATAREG_DIRECT
	0,	//Mode::ADDREG_DIRECT
	0,	//ADDREG_INDIRECT
	0,	//Mode::ADDREG_INDIRECT_POSTINC
	0,	//Mode::ADDREG_INDIRECT_PREDEC
	2,	//Mode::ADDREG_INDIRECT_DISPLACE
	2, 	//Mode::ADDREG_INDIRECT_INDEX_8BIT
	2, 	//Mode::PC_INDIRECT_DISPLACE
	2, 	//Mode::PC_INDIRECT_INDEX_8BIT
	2, 	//Mode::ABS_WORD
	4, 	//Mode::ABS_LONG
	4};	//IMMEDIATE

//----------------------------------------------------------------------------------------
//Decode time tables
//----------------------------------------------------------------------------------------
const ExecuteTime EffectiveAddress::executeTime8[] = {
	ExecuteTime(0,  0, 0),	//Mode::DATAREG_DIRECT
	ExecuteTime(0,  0, 0),	//Mode::ADDREG_DIRECT
	ExecuteTime(4,  1, 0),	//ADDREG_INDIRECT
	ExecuteTime(4,  1, 0),	//Mode::ADDREG_INDIRECT_POSTINC
	ExecuteTime(6,  1, 0),	//Mode::ADDREG_INDIRECT_PREDEC
	ExecuteTime(8,  2, 0),	//Mode::ADDREG_INDIRECT_DISPLACE
	ExecuteTime(10, 2, 0), 	//Mode::ADDREG_INDIRECT_INDEX_8BIT
	ExecuteTime(8,  2, 0), 	//Mode::PC_INDIRECT_DISPLACE
	ExecuteTime(10, 2, 0), 	//Mode::PC_INDIRECT_INDEX_8BIT
	ExecuteTime(8,  2, 0), 	//Mode::ABS_WORD
	ExecuteTime(12, 3, 0), 	//Mode::ABS_LONG
	ExecuteTime(4,  1, 0)};	//IMMEDIATE

//----------------------------------------------------------------------------------------
const ExecuteTime EffectiveAddress::executeTime32[] = {
	ExecuteTime(0,  0, 0),	//Mode::DATAREG_DIRECT
	ExecuteTime(0,  0, 0),	//Mode::ADDREG_DIRECT
	ExecuteTime(8,  2, 0),	//ADDREG_INDIRECT
	ExecuteTime(8,  2, 0),	//Mode::ADDREG_INDIRECT_POSTINC
	ExecuteTime(10, 2, 0),	//Mode::ADDREG_INDIRECT_PREDEC
	ExecuteTime(12, 3, 0),	//Mode::ADDREG_INDIRECT_DISPLACE
	ExecuteTime(14, 3, 0), 	//Mode::ADDREG_INDIRECT_INDEX_8BIT
	ExecuteTime(12, 3, 0), 	//Mode::PC_INDIRECT_DISPLACE
	ExecuteTime(14, 3, 0), 	//Mode::PC_INDIRECT_INDEX_8BIT
	ExecuteTime(12, 3, 0), 	//Mode::ABS_WORD
	ExecuteTime(16, 4, 0), 	//Mode::ABS_LONG
	ExecuteTime(8,  2, 0)};	//IMMEDIATE

//----------------------------------------------------------------------------------------
//Decode functions
//----------------------------------------------------------------------------------------
void EffectiveAddress::Decode(unsigned int areg, unsigned int amode, Bitcount asize, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	savedPC = location;
	switch(amode)
	{
	default:
		DebugAssert(false);
		break;
	case 0:	//000
		BuildDataDirect(asize, location, areg);
		break;
	case 1:	//001
		BuildAddressDirect(asize, location, areg);
		break;
	case 2: //010
		BuildAddressIndirect(asize, location, areg);
		break;
	case 3:	//011
		BuildAddressPostinc(asize, location, areg);
		break;
	case 4:	//100
		BuildAddressPredec(asize, location, areg);
		break;
	case 5:	//101
		BuildAddressIndirectDisplace(asize, location, areg, cpu, transparent, instructionRegister);
		break;
	case 6:	//110
		BuildAddressIndirectIndex(asize, location, areg, cpu, transparent, instructionRegister);
		break;
	case 7:	//111
		switch(areg)
		{
		case 0:	//000
			BuildAbsoluteAddressWord(asize, location, cpu, transparent, instructionRegister);
			break;
		case 1:	//001
			BuildAbsoluteAddressLong(asize, location, cpu, transparent, instructionRegister);
			break;
		case 2:	//010
			BuildPCIndirectDisplace(asize, location, cpu, transparent, instructionRegister);
			break;
		case 3:	//011
			BuildPCIndirectIndex(asize, location, cpu, transparent, instructionRegister);
			break;
		case 4:	//100
			BuildImmediateData(asize, location, cpu, transparent, instructionRegister);
			break;
		}
	}
}

//----------------------------------------------------------------------------------------
//Address functions
//----------------------------------------------------------------------------------------
bool EffectiveAddress::GetAddress(const M68000* cpu, Data& aaddress) const
{
	switch(mode)
	{
	default:
		DebugAssert(false);
		return false;
	case Mode::AddRegIndirect:
		cpu->GetA(reg, aaddress);
		break;
	case Mode::AddRegIndirectDisplace:
		cpu->GetA(reg, aaddress);
		aaddress += displacement.SignExtend(BITCOUNT_LONG);
		break;
	case Mode::AddRegIndirectIndex8Bit:{
		cpu->GetA(reg, aaddress);

		//Get index value
		Data index(indexSize);
		if(!useAddressRegister)
		{
			cpu->GetD(indexReg, index);
		}
		else
		{
			cpu->GetA(indexReg, index);
		}

		aaddress += displacement.SignExtend(BITCOUNT_LONG);
		aaddress += index.SignExtend(BITCOUNT_LONG);
		break;}
	case Mode::PCIndirectDisplace:
		aaddress = savedPC;
		aaddress += displacement.SignExtend(BITCOUNT_LONG);
		break;
	case Mode::PCIndirectIndex8Bit:{
		aaddress = savedPC;

		//Get index value
		Data index(indexSize);
		if(!useAddressRegister)
		{
			cpu->GetD(indexReg, index);
		}
		else
		{
			cpu->GetA(indexReg, index);
		}

		aaddress += displacement.SignExtend(BITCOUNT_LONG);
		aaddress += index.SignExtend(BITCOUNT_LONG);
		break;}
	case Mode::ABSWord:
	case Mode::ABSLong:
		aaddress = address.SignExtend(BITCOUNT_LONG);
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool EffectiveAddress::GetAddressTransparent(Data& aaddress) const
{
	switch(mode)
	{
	default:
		return false;
	case Mode::PCIndirectDisplace:
		aaddress = savedPC;
		aaddress += displacement.SignExtend(BITCOUNT_LONG);
		break;
	case Mode::ABSWord:
	case Mode::ABSLong:
		aaddress = address.SignExtend(BITCOUNT_LONG);
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool EffectiveAddress::GetAddressDisplacementTargetNoIndex(M68000* cpu, Data& aaddress) const
{
	switch(mode)
	{
	default:
		return false;
	case Mode::AddRegIndirectDisplace:
	case Mode::AddRegIndirectIndex8Bit:
		cpu->GetA(reg, aaddress);
		aaddress += displacement.SignExtend(BITCOUNT_LONG);
		break;
	case Mode::PCIndirectDisplace:
	case Mode::PCIndirectIndex8Bit:
		aaddress = savedPC;
		aaddress += displacement.SignExtend(BITCOUNT_LONG);
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Data transfer
//----------------------------------------------------------------------------------------
double EffectiveAddress::Read(M68000* cpu, Data& target, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	double additionalTime = 0;
	switch(mode)
	{
	default:
		DebugAssert(false);
		break;
	case Mode::DataRegDirect:
		cpu->GetD(reg, target);
		break;
	case Mode::AddRegDirect:
		cpu->GetA(reg, target);
		break;
	case Mode::AddRegIndirect:{
		unsigned int originalAddressSourceLocation;
		if(cpu->DisassemblyGetAddressRegisterUnmodified(reg, originalAddressSourceLocation))
		{
			cpu->AddDisassemblyAddressInfoOffset(originalAddressSourceLocation, cpu->DisassemblyGetAddressRegisterUnmodifiedSize(reg), false, false, 0);
		}

		M68000Long address;
		cpu->GetA(reg, address);
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;}
	case Mode::AddRegIndirectPostInc:{
		unsigned int originalAddressSourceLocation;
		if(cpu->DisassemblyGetAddressRegisterUnmodified(reg, originalAddressSourceLocation))
		{
			cpu->AddDisassemblyAddressInfoOffset(originalAddressSourceLocation, cpu->DisassemblyGetAddressRegisterUnmodifiedSize(reg), false, false, 0);
		}

		M68000Long address;
		cpu->GetA(reg, address);
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		unsigned int arrayID = 0;
		if(!cpu->DisassemblyGetAddressRegisterLastAccessedInPostIncMode(reg))
		{
			arrayID = cpu->MakeDataArrayAtLocation(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
			cpu->DisassemblySetAddressRegisterCurrentArrayID(reg, arrayID);
		}
		else
		{
			arrayID = cpu->DisassemblyGetAddressRegisterCurrentArrayID(reg);
		}
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer, arrayID);
		//Adjust the address register. We've got a little bit of code here to prevent
		//the stack pointer from ending up on an odd address when using byte access.
		//See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		//documentation says this alignment happens to maintain "maximum efficiency".
		//Of course, for the M68000 the real reason is that an address error would be
		//generated when attempting word or long access on an unaligned address, so
		//we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((reg == M68000::SP) && (target.GetBitCount() == BITCOUNT_BYTE))? 2: target.GetByteSize();
		address += offset;
		cpu->SetA(reg, address);
		cpu->DisassemblySetAddressRegisterLastAccessedInPostIncMode(reg, true);
		cpu->DisassemblySetAddressRegisterUnmodified(reg, false);
		break;}
	case Mode::AddRegIndirectPreDec:{
		M68000Long address;
		cpu->GetA(reg, address);
		//Adjust the address register. We've got a little bit of code here to prevent
		//the stack pointer from ending up on an odd address when using byte access.
		//See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		//documentation says this alignment happens to maintain "maximum efficiency".
		//Of course, for the M68000 the real reason is that an address error would be
		//generated when attempting word or long access on an unaligned address, so
		//we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((reg == M68000::SP) && (target.GetBitCount() == BITCOUNT_BYTE))? 2: target.GetByteSize();
		address -= offset;
		cpu->SetA(reg, address);

		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		cpu->DisassemblySetAddressRegisterUnmodified(reg, false);
		break;}
	case Mode::AddRegIndirectDisplace:{
		unsigned int originalAddressSourceLocation;
		if(cpu->DisassemblyGetAddressRegisterUnmodified(reg, originalAddressSourceLocation))
		{
			cpu->AddDisassemblyAddressInfoOffset(originalAddressSourceLocation, cpu->DisassemblyGetAddressRegisterUnmodifiedSize(reg), false, false, 0);
		}

		M68000Long address;
		cpu->GetA(reg, address);
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;}
	case Mode::AddRegIndirectIndex8Bit:{
		unsigned int originalAddressSourceLocation;
		if(cpu->DisassemblyGetAddressRegisterUnmodified(reg, originalAddressSourceLocation))
		{
			cpu->AddDisassemblyAddressInfoOffset(originalAddressSourceLocation, cpu->DisassemblyGetAddressRegisterUnmodifiedSize(reg), false, false, 0);
		}

		M68000Long address;
		cpu->GetA(reg, address);

		//Get index value
		Data index(indexSize);
		if(!useAddressRegister)
		{
			cpu->GetD(indexReg, index);
		}
		else
		{
			cpu->GetA(indexReg, index);
		}

		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		address += M68000Long(index.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;}
	case Mode::PCIndirectDisplace:{
		M68000Long address;
		address = savedPC;
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(true), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;}
	case Mode::PCIndirectIndex8Bit:{
		M68000Long address;
		address = savedPC;

		//Get index value
		Data index(indexSize);
		if(!useAddressRegister)
		{
			cpu->GetD(indexReg, index);
		}
		else
		{
			cpu->GetA(indexReg, index);
		}

		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		address += M68000Long(index.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(true), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;}

	case Mode::ABSWord:
	case Mode::ABSLong:
		additionalTime = cpu->ReadMemory(M68000Long(address.SignExtend(BITCOUNT_LONG)), target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DisassemblyDataType::Integer);
		break;

	case Mode::Immediate:
		//##TODO## I have a feeling this sign extension may not be correct in all cases.
		//Check which opcodes might be relying on this behaviour, and verify it on the
		//hardware.
		target = data.SignExtend(target.GetBitCount());
		break;
	}
	return additionalTime;
}

//----------------------------------------------------------------------------------------
double EffectiveAddress::Write(M68000* cpu, const Data& target, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation, bool dataUnmodifiedFromMemoryRead, unsigned int sourceMemoryAddress, unsigned int sourceMemorySize) const
{
	double additionalTime = 0;
	switch(mode)
	{
	default:
		DebugAssert(false);
		break;
	case Mode::DataRegDirect:{
		M68000Long newReg;
		cpu->GetD(reg, newReg);
		newReg.SetLowerBits(target);
		cpu->SetD(reg, newReg);
		cpu->DisassemblySetDataRegisterUnmodified(reg, dataUnmodifiedFromMemoryRead, sourceMemorySize, sourceMemoryAddress);
		break;}
	case Mode::AddRegDirect:
		cpu->SetA(reg, M68000Long(target.SignExtend(BITCOUNT_LONG)));
		cpu->DisassemblySetAddressRegisterUnmodified(reg, dataUnmodifiedFromMemoryRead, sourceMemorySize, sourceMemoryAddress);
		break;
	case Mode::AddRegIndirect:{
		M68000Long address;
		cpu->GetA(reg, address);
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}
	case Mode::AddRegIndirectPostInc:{
		M68000Long address;
		cpu->GetA(reg, address);
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		//Adjust the address register. We've got a little bit of code here to prevent
		//the stack pointer from ending up on an odd address when using byte access.
		//See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		//documentation says this alignment happens to maintain "maximum efficiency".
		//Of course, for the M68000 the real reason is that an address error would be
		//generated when attempting word or long access on an unaligned address, so
		//we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((reg == M68000::SP) && (target.GetBitCount() == BITCOUNT_BYTE))? 2: target.GetByteSize();
		address += offset;
		cpu->SetA(reg, address);
		cpu->DisassemblySetAddressRegisterUnmodified(reg, false);
		break;}
	case Mode::AddRegIndirectPreDec:{
		M68000Long address;
		cpu->GetA(reg, address);
		//Adjust the address register. We've got a little bit of code here to prevent
		//the stack pointer from ending up on an odd address when using byte access.
		//See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		//documentation says this alignment happens to maintain "maximum efficiency".
		//Of course, for the M68000 the real reason is that an address error would be
		//generated when attempting word or long access on an unaligned address, so
		//we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((reg == M68000::SP) && (target.GetBitCount() == BITCOUNT_BYTE))? 2: target.GetByteSize();
		address -= offset;
		cpu->SetA(reg, address);

		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->DisassemblySetAddressRegisterUnmodified(reg, false);
		break;}
	case Mode::AddRegIndirectDisplace:{
		M68000Long address;
		cpu->GetA(reg, address);
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}
	case Mode::AddRegIndirectIndex8Bit:{
		M68000Long address;
		cpu->GetA(reg, address);

		//Get index value
		Data index(indexSize);
		if(!useAddressRegister)
		{
			cpu->GetD(indexReg, index);
		}
		else
		{
			cpu->GetA(indexReg, index);
		}

		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		address += M68000Long(index.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}
	case Mode::PCIndirectDisplace:{
		M68000Long address;
		address = savedPC;
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(true), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}
	case Mode::PCIndirectIndex8Bit:{
		M68000Long address;
		address = savedPC;

		//Get index value
		Data index(indexSize);
		if(!useAddressRegister)
		{
			cpu->GetD(indexReg, index);
		}
		else
		{
			cpu->GetA(indexReg, index);
		}

		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		address += M68000Long(index.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(true), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}

	case Mode::ABSWord:
	case Mode::ABSLong:
		additionalTime = cpu->WriteMemory(M68000Long(address.SignExtend(BITCOUNT_LONG)), target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;

	case Mode::Immediate:
		DebugAssert(false);
		break;
	}
	return additionalTime;
}

//----------------------------------------------------------------------------------------
double EffectiveAddress::ReadWithoutAdjustingAddress(M68000* cpu, Data& target, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	double additionalTime = 0;
	if(mode == Mode::AddRegIndirectPostInc)
	{
		M68000Long address;
		cpu->GetA(reg, address);
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
	}
	else if(mode == Mode::AddRegIndirectPreDec)
	{
		M68000Long address;
		cpu->GetA(reg, address);
		//Adjust the address register. We've got a little bit of code here to prevent
		//the stack pointer from ending up on an odd address when using byte access.
		//See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		//documentation says this alignment happens to maintain "maximum efficiency".
		//Of course, for the M68000 the real reason is that an address error would be
		//generated when attempting word or long access on an unaligned address, so
		//we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((reg == M68000::SP) && (target.GetBitCount() == BITCOUNT_BYTE))? 2: target.GetByteSize();
		address -= offset;
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
	}
	else
	{
		additionalTime = Read(cpu, target, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
	}
	return additionalTime;
}

//----------------------------------------------------------------------------------------
bool EffectiveAddress::IsTargetUnmodifiedFromMemoryRead(M68000* cpu, Bitcount readSize, unsigned int& sourceMemoryAddress) const
{
	bool targetUnmodified = false;
	switch(mode)
	{
	default:
		DebugAssert(false);
		break;
	case Mode::DataRegDirect:
		targetUnmodified = cpu->DisassemblyGetDataRegisterUnmodified(reg, sourceMemoryAddress);
		break;
	case Mode::AddRegDirect:
		targetUnmodified = cpu->DisassemblyGetAddressRegisterUnmodified(reg, sourceMemoryAddress);
		break;
	case Mode::AddRegIndirect:
	case Mode::AddRegIndirectPostInc:{
		M68000Long address;
		cpu->GetA(reg, address);
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case Mode::AddRegIndirectPreDec:{
		M68000Long address;
		cpu->GetA(reg, address);
		//Adjust the address register. We've got a little bit of code here to prevent
		//the stack pointer from ending up on an odd address when using byte access.
		//See the M68000 Programmer's Manual, section 2.6.1 for more info. The
		//documentation says this alignment happens to maintain "maximum efficiency".
		//Of course, for the M68000 the real reason is that an address error would be
		//generated when attempting word or long access on an unaligned address, so
		//we need to prevent byte access to the stack pointer causing this to occur.
		unsigned int offset;
		offset = ((reg == M68000::SP) && (readSize == BITCOUNT_BYTE))? 2: readSize / 8;
		address -= offset;
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case Mode::AddRegIndirectDisplace:{
		M68000Long address;
		cpu->GetA(reg, address);
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case Mode::AddRegIndirectIndex8Bit:{
		M68000Long address;
		cpu->GetA(reg, address);

		//Get index value
		Data index(indexSize);
		if(!useAddressRegister)
		{
			cpu->GetD(indexReg, index);
		}
		else
		{
			cpu->GetA(indexReg, index);
		}

		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		address += M68000Long(index.SignExtend(BITCOUNT_LONG));
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case Mode::PCIndirectDisplace:{
		M68000Long address;
		address = savedPC;
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case Mode::PCIndirectIndex8Bit:{
		M68000Long address;
		address = savedPC;

		//Get index value
		Data index(indexSize);
		if(!useAddressRegister)
		{
			cpu->GetD(indexReg, index);
		}
		else
		{
			cpu->GetA(indexReg, index);
		}

		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
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

//----------------------------------------------------------------------------------------
unsigned int EffectiveAddress::GetTargetOriginalMemoryReadSize(M68000* cpu, Bitcount readBitcount) const
{
	unsigned int memorySize = ((unsigned int)readBitcount) / 8;
	switch(mode)
	{
	case Mode::DataRegDirect:{
		memorySize = cpu->DisassemblyGetDataRegisterUnmodifiedSize(reg);
		break;}
	case Mode::AddRegDirect:
		memorySize = cpu->DisassemblyGetAddressRegisterUnmodifiedSize(reg);
		break;
	}
	return memorySize;
}

//----------------------------------------------------------------------------------------
bool EffectiveAddress::IsTargetUnmodifiedFromMemoryReadV2(M68000* cpu, unsigned int& sourceMemoryAddress, bool& dataIsOffset, unsigned int& offsetBaseAddress, unsigned int& dataSize) const
{
	bool targetUnmodified = false;
	switch(mode)
	{
	default:
		DebugAssert(false);
		break;
	case Mode::AddRegIndirect:
		targetUnmodified = cpu->DisassemblyGetAddressRegisterUnmodified(reg, sourceMemoryAddress);
		dataSize = cpu->DisassemblyGetAddressRegisterUnmodifiedSize(reg);
		dataIsOffset = false;
		break;
	case Mode::AddRegIndirectIndex8Bit:{
		if(!useAddressRegister)
		{
			targetUnmodified = cpu->DisassemblyGetDataRegisterUnmodified(indexReg, sourceMemoryAddress);
		}
		else
		{
			targetUnmodified = cpu->DisassemblyGetAddressRegisterUnmodified(indexReg, sourceMemoryAddress);
		}

		//##TODO## If the address register itself is also unmodified from a memory read,
		//it needs to be setup as a pointer too.
		M68000Long address;
		cpu->GetA(reg, address);
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		offsetBaseAddress = address.GetData();
		dataSize = indexSize / 8;
		dataIsOffset = true;
		break;}
	case Mode::PCIndirectIndex8Bit:{
		if(!useAddressRegister)
		{
			targetUnmodified = cpu->DisassemblyGetDataRegisterUnmodified(indexReg, sourceMemoryAddress);
		}
		else
		{
			targetUnmodified = cpu->DisassemblyGetAddressRegisterUnmodified(indexReg, sourceMemoryAddress);
		}

		M68000Long address;
		address = savedPC;
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		offsetBaseAddress = address.GetData();
		dataSize = indexSize / 8;
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

//----------------------------------------------------------------------------------------
//Disassembly functions
//----------------------------------------------------------------------------------------
std::wstring EffectiveAddress::Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
{
	std::wstringstream stream;

	switch(mode)
	{
	default:
		stream << L"<INVALID>";
		break;
	case Mode::DataRegDirect:
		stream << L'D' << reg;
		break;
	case Mode::AddRegDirect:
		stream << L'A' << reg;
		break;
	case Mode::AddRegIndirect:
		stream << L"(A" << reg << L")";
		break;
	case Mode::AddRegIndirectPostInc:
		stream << L"(A" << reg << L")+";
		break;
	case Mode::AddRegIndirectPreDec:
		stream << L"-(A" << reg << L")";
		break;
	case Mode::AddRegIndirectDisplace:{
		Data displacementExtended = displacement.SignExtend(BITCOUNT_LONG);
		if(displacementExtended.Negative())
		{
			stream << L"-$" << std::hex << std::uppercase << 0 - displacementExtended.GetData();
		}
		else
		{
			stream << L"$" << std::hex << std::uppercase << displacementExtended.GetData();
		}
		stream << L"(A" << reg << L")";
		break;}
	case Mode::AddRegIndirectIndex8Bit:
		if(displacement.NonZero())
		{
			Data displacementExtended = displacement.SignExtend(BITCOUNT_LONG);
			if(displacementExtended.Negative())
			{
				stream << L"-$" << std::hex << std::uppercase << 0 - displacementExtended.GetData();
			}
			else
			{
				stream << L"$" << std::hex << std::uppercase << displacementExtended.GetData();
			}
		}
		stream << L"(A" << reg << L",";
		if(!useAddressRegister)
		{
			stream << L"D";
		}
		else
		{
			stream << L"A";
		}
		stream << indexReg << "." << M68000Instruction::DisassembleSize(indexSize) << L")";
		break;
	case Mode::PCIndirectDisplace:{
		Data targetAddress(BITCOUNT_LONG);
		targetAddress = savedPC.GetData() + displacement.SignExtend(BITCOUNT_LONG).GetData();
		bool labelWritten = false;
		if(labelSettings.enableSubstitution)
		{
			std::map<unsigned int, M68000::LabelSubstitutionEntry>::const_iterator labelIterator = labelSettings.labelTargetsAddressingLocation.find(targetAddress.GetData());
			if(labelIterator != labelSettings.labelTargetsAddressingLocation.end())
			{
				stream << labelIterator->second.usageLabel << L"(PC)";
				labelWritten = true;
			}
		}
		if(!labelWritten)
		{
			if(targetAddress.Negative())
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
		targetAddress = savedPC.GetData() + displacement.SignExtend(BITCOUNT_LONG).GetData();
		bool labelWritten = false;
		if(labelSettings.enableSubstitution)
		{
			std::map<unsigned int, M68000::LabelSubstitutionEntry>::const_iterator labelIterator = labelSettings.labelTargetsAddressingLocation.find(targetAddress.GetData());
			if(labelIterator != labelSettings.labelTargetsAddressingLocation.end())
			{
				stream << labelIterator->second.usageLabel;
				labelWritten = true;
			}
		}
		if(!labelWritten)
		{
			if(targetAddress.Negative())
			{
				stream << L"*-$" << std::hex << std::uppercase << 0 - targetAddress.GetData();
			}
			else
			{
				stream << L"*+$" << std::hex << std::uppercase << targetAddress.GetData();
			}
		}
		stream << L"(PC,";
		if(!useAddressRegister)
		{
			stream << L"D";
		}
		else
		{
			stream << L"A";
		}
		stream << indexReg << "." << M68000Instruction::DisassembleSize(indexSize) << L")";
		break;}
	case Mode::ABSWord:
	case Mode::ABSLong:{
		Data targetAddress(BITCOUNT_LONG);
		targetAddress = address.SignExtend(BITCOUNT_LONG);
		bool labelWritten = false;
		if(labelSettings.enableSubstitution)
		{
			std::map<unsigned int, M68000::LabelSubstitutionEntry>::const_iterator labelIterator = labelSettings.labelTargetsAddressingLocation.find(targetAddress.GetData());
			if(labelIterator != labelSettings.labelTargetsAddressingLocation.end())
			{
				stream << labelIterator->second.usageLabel;
				labelWritten = true;
			}
		}
		if(!labelWritten)
		{
			stream << L'$' << std::setw(targetAddress.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase << targetAddress.GetData();
			if(address.GetBitCount() < BITCOUNT_LONG)
			{
				stream << L"." << M68000Instruction::DisassembleSize((Bitcount)address.GetBitCount());
			}
		}
		break;}
	case Mode::Immediate:
		stream << L'#';
		if(dataSignExtended)
		{
			Data dataModified = data.SignExtend(BITCOUNT_LONG);
			if(dataModified.Negative())
			{
				stream << L"-";
				if((0 - dataModified.GetData()) > 9)
				{
					stream << L'$' << std::setw(dataModified.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase;
				}
				stream << 0 - dataModified.GetData();
			}
			else
			{
				if(dataModified.GetData() > 9)
				{
					stream << L'$' << std::setw(dataModified.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase;
				}
				stream << dataModified.GetData();
			}
		}
		else
		{
			if(data.GetData() > 9)
			{
				stream << L'$' << std::setw(data.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase;
			}
			stream << data.GetData();
		}
		break;
	}

	return stream.str();
}

//----------------------------------------------------------------------------------------
std::wstring EffectiveAddress::DisassembleImmediateAsPCDisplacement(const M68000::LabelSubstitutionSettings& labelSettings) const
{
	M68000Long targetAddress = savedPC + M68000Long(data.SignExtend(BITCOUNT_LONG));
	std::wstringstream stream;
	bool labelWritten = false;
	if(labelSettings.enableSubstitution)
	{
		std::map<unsigned int, M68000::LabelSubstitutionEntry>::const_iterator labelIterator = labelSettings.labelTargetsAddressingLocation.find(targetAddress.GetData());
		if(labelIterator != labelSettings.labelTargetsAddressingLocation.end())
		{
			stream << labelIterator->second.usageLabel;
			labelWritten = true;
		}
	}
	if(!labelWritten)
	{
		if(targetAddress.Negative())
		{
			stream << L"*-$" << std::hex << std::uppercase << 0 - data.GetData();
		}
		else
		{
			stream << L"*+$" << std::hex << std::uppercase << data.GetData();
		}
	}
	return stream.str();
}

//----------------------------------------------------------------------------------------
std::wstring EffectiveAddress::DisassembleImmediateAsPCDisplacementTargetAddressString() const
{
	M68000Long newPC = savedPC + M68000Long(data.SignExtend(BITCOUNT_LONG));
	std::wstringstream stream;
	stream << std::setw(newPC.GetByteSize() * 2) << std::setfill(L'0') << std::hex << std::uppercase << newPC.GetData();
	return stream.str();
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::AddLabelTargetsToSet(std::set<unsigned int>& labelTargetLocations) const
{
	switch(mode)
	{
	case Mode::PCIndirectDisplace:
	case Mode::PCIndirectIndex8Bit:{
		Data targetAddress(BITCOUNT_LONG);
		targetAddress = savedPC.GetData() + displacement.SignExtend(BITCOUNT_LONG).GetData();
		labelTargetLocations.insert(targetAddress.GetData());
		break;}
	case Mode::ABSWord:
	case Mode::ABSLong:{
		M68000Long targetAddress = address.SignExtend(BITCOUNT_LONG).GetData();
		labelTargetLocations.insert(targetAddress.GetData());
		break;}
	}
}

} //Close namespace M68000
