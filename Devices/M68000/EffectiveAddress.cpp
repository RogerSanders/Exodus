#include "EffectiveAddress.h"
#include "M68000.h"
#include "M68000Instruction.h"
#include <sstream>
namespace M68000 {

//----------------------------------------------------------------------------------------
//Extension word size tables
//----------------------------------------------------------------------------------------
const unsigned int EffectiveAddress::extensionSize8[] = {
	0,	//DATAREG_DIRECT
	0,	//ADDREG_DIRECT
	0,	//ADDREG_INDIRECT
	0,	//ADDREG_INDIRECT_POSTINC
	0,	//ADDREG_INDIRECT_PREDEC
	2,	//ADDREG_INDIRECT_DISPLACE
	2, 	//ADDREG_INDIRECT_INDEX_8BIT
	2, 	//PC_INDIRECT_DISPLACE
	2, 	//PC_INDIRECT_INDEX_8BIT
	2, 	//ABS_WORD
	4, 	//ABS_LONG
	2};	//IMMEDIATE

//----------------------------------------------------------------------------------------
const unsigned int EffectiveAddress::extensionSize32[] = {
	0,	//DATAREG_DIRECT
	0,	//ADDREG_DIRECT
	0,	//ADDREG_INDIRECT
	0,	//ADDREG_INDIRECT_POSTINC
	0,	//ADDREG_INDIRECT_PREDEC
	2,	//ADDREG_INDIRECT_DISPLACE
	2, 	//ADDREG_INDIRECT_INDEX_8BIT
	2, 	//PC_INDIRECT_DISPLACE
	2, 	//PC_INDIRECT_INDEX_8BIT
	2, 	//ABS_WORD
	4, 	//ABS_LONG
	4};	//IMMEDIATE

//----------------------------------------------------------------------------------------
//Decode time tables
//----------------------------------------------------------------------------------------
const ExecuteTime EffectiveAddress::executeTime8[] = {
	ExecuteTime(0,  0, 0),	//DATAREG_DIRECT
	ExecuteTime(0,  0, 0),	//ADDREG_DIRECT
	ExecuteTime(4,  1, 0),	//ADDREG_INDIRECT
	ExecuteTime(4,  1, 0),	//ADDREG_INDIRECT_POSTINC
	ExecuteTime(6,  1, 0),	//ADDREG_INDIRECT_PREDEC
	ExecuteTime(8,  2, 0),	//ADDREG_INDIRECT_DISPLACE
	ExecuteTime(10, 2, 0), 	//ADDREG_INDIRECT_INDEX_8BIT
	ExecuteTime(8,  2, 0), 	//PC_INDIRECT_DISPLACE
	ExecuteTime(10, 2, 0), 	//PC_INDIRECT_INDEX_8BIT
	ExecuteTime(8,  2, 0), 	//ABS_WORD
	ExecuteTime(12, 3, 0), 	//ABS_LONG
	ExecuteTime(4,  1, 0)};	//IMMEDIATE

//----------------------------------------------------------------------------------------
const ExecuteTime EffectiveAddress::executeTime32[] = {
	ExecuteTime(0,  0, 0),	//DATAREG_DIRECT
	ExecuteTime(0,  0, 0),	//ADDREG_DIRECT
	ExecuteTime(8,  2, 0),	//ADDREG_INDIRECT
	ExecuteTime(8,  2, 0),	//ADDREG_INDIRECT_POSTINC
	ExecuteTime(10, 2, 0),	//ADDREG_INDIRECT_PREDEC
	ExecuteTime(12, 3, 0),	//ADDREG_INDIRECT_DISPLACE
	ExecuteTime(14, 3, 0), 	//ADDREG_INDIRECT_INDEX_8BIT
	ExecuteTime(12, 3, 0), 	//PC_INDIRECT_DISPLACE
	ExecuteTime(14, 3, 0), 	//PC_INDIRECT_INDEX_8BIT
	ExecuteTime(12, 3, 0), 	//ABS_WORD
	ExecuteTime(16, 4, 0), 	//ABS_LONG
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
		ReleaseAssert(false);
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
		ReleaseAssert(false);
		return false;
	case ADDREG_INDIRECT:
		cpu->GetA(reg, aaddress);
		break;
	case ADDREG_INDIRECT_DISPLACE:
		cpu->GetA(reg, aaddress);
		aaddress += displacement.SignExtend(BITCOUNT_LONG);
		break;
	case ADDREG_INDIRECT_INDEX_8BIT:{
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
	case PC_INDIRECT_DISPLACE:
		aaddress = savedPC;
		aaddress += displacement.SignExtend(BITCOUNT_LONG);
		break;
	case PC_INDIRECT_INDEX_8BIT:{
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
	case ABS_WORD:
	case ABS_LONG:
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
	case PC_INDIRECT_DISPLACE:
		aaddress = savedPC;
		aaddress += displacement.SignExtend(BITCOUNT_LONG);
		break;
	case ABS_WORD:
	case ABS_LONG:
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
	case ADDREG_INDIRECT_DISPLACE:
	case ADDREG_INDIRECT_INDEX_8BIT:
		cpu->GetA(reg, aaddress);
		aaddress += displacement.SignExtend(BITCOUNT_LONG);
		break;
	case PC_INDIRECT_DISPLACE:
	case PC_INDIRECT_INDEX_8BIT:
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
		ReleaseAssert(false);
		break;
	case DATAREG_DIRECT:
		cpu->GetD(reg, target);
		break;
	case ADDREG_DIRECT:
		cpu->GetA(reg, target);
		break;
	case ADDREG_INDIRECT:{
		unsigned int originalAddressSourceLocation;
		if(cpu->DisassemblyGetAddressRegisterUnmodified(reg, originalAddressSourceLocation))
		{
			cpu->AddDisassemblyAddressInfoOffset(originalAddressSourceLocation, cpu->DisassemblyGetAddressRegisterUnmodifiedSize(reg), false, false, 0);
		}

		M68000Long address;
		cpu->GetA(reg, address);
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DISASSEMBLYDATATYPE_INTEGER);
		break;}
	case ADDREG_INDIRECT_POSTINC:{
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
			arrayID = cpu->MakeDataArrayAtLocation(address.GetData(), target.GetByteSize(), M68000::DISASSEMBLYDATATYPE_INTEGER);
			cpu->DisassemblySetAddressRegisterCurrentArrayID(reg, arrayID);
		}
		else
		{
			arrayID = cpu->DisassemblyGetAddressRegisterCurrentArrayID(reg);
		}
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DISASSEMBLYDATATYPE_INTEGER, arrayID);
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
	case ADDREG_INDIRECT_PREDEC:{
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
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DISASSEMBLYDATATYPE_INTEGER);
		cpu->DisassemblySetAddressRegisterUnmodified(reg, false);
		break;}
	case ADDREG_INDIRECT_DISPLACE:{
		unsigned int originalAddressSourceLocation;
		if(cpu->DisassemblyGetAddressRegisterUnmodified(reg, originalAddressSourceLocation))
		{
			cpu->AddDisassemblyAddressInfoOffset(originalAddressSourceLocation, cpu->DisassemblyGetAddressRegisterUnmodifiedSize(reg), false, false, 0);
		}

		M68000Long address;
		cpu->GetA(reg, address);
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DISASSEMBLYDATATYPE_INTEGER);
		break;}
	case ADDREG_INDIRECT_INDEX_8BIT:{
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
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DISASSEMBLYDATATYPE_INTEGER);
		break;}
	case PC_INDIRECT_DISPLACE:{
		M68000Long address;
		address = savedPC;
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(true), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DISASSEMBLYDATATYPE_INTEGER);
		break;}
	case PC_INDIRECT_INDEX_8BIT:{
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
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DISASSEMBLYDATATYPE_INTEGER);
		break;}

	case ABS_WORD:
	case ABS_LONG:
		additionalTime = cpu->ReadMemory(M68000Long(address.SignExtend(BITCOUNT_LONG)), target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		cpu->AddDisassemblyAddressInfoData(address.GetData(), target.GetByteSize(), M68000::DISASSEMBLYDATATYPE_INTEGER);
		break;

	case IMMEDIATE:
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
		ReleaseAssert(false);
		break;
	case DATAREG_DIRECT:{
		M68000Long newReg;
		cpu->GetD(reg, newReg);
		newReg.SetLowerBits(target);
		cpu->SetD(reg, newReg);
		cpu->DisassemblySetDataRegisterUnmodified(reg, dataUnmodifiedFromMemoryRead, sourceMemorySize, sourceMemoryAddress);
		break;}
	case ADDREG_DIRECT:
		cpu->SetA(reg, M68000Long(target.SignExtend(BITCOUNT_LONG)));
		cpu->DisassemblySetAddressRegisterUnmodified(reg, dataUnmodifiedFromMemoryRead, sourceMemorySize, sourceMemoryAddress);
		break;
	case ADDREG_INDIRECT:{
		M68000Long address;
		cpu->GetA(reg, address);
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}
	case ADDREG_INDIRECT_POSTINC:{
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
	case ADDREG_INDIRECT_PREDEC:{
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
	case ADDREG_INDIRECT_DISPLACE:{
		M68000Long address;
		cpu->GetA(reg, address);
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}
	case ADDREG_INDIRECT_INDEX_8BIT:{
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
	case PC_INDIRECT_DISPLACE:{
		M68000Long address;
		address = savedPC;
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		additionalTime = cpu->WriteMemory(address, target, cpu->GetFunctionCode(true), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;}
	case PC_INDIRECT_INDEX_8BIT:{
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

	case ABS_WORD:
	case ABS_LONG:
		additionalTime = cpu->WriteMemory(M68000Long(address.SignExtend(BITCOUNT_LONG)), target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
		break;

	case IMMEDIATE:
		ReleaseAssert(false);
		break;
	}
	return additionalTime;
}

//----------------------------------------------------------------------------------------
double EffectiveAddress::ReadWithoutAdjustingAddress(M68000* cpu, Data& target, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	double additionalTime = 0;
	if(mode == ADDREG_INDIRECT_POSTINC)
	{
		M68000Long address;
		cpu->GetA(reg, address);
		additionalTime = cpu->ReadMemory(address, target, cpu->GetFunctionCode(false), savedPC, true, instructionRegister, rmwCycleInProgress, rmwCycleFirstOperation);
	}
	else if(mode == ADDREG_INDIRECT_PREDEC)
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
		ReleaseAssert(false);
		break;
	case DATAREG_DIRECT:
		targetUnmodified = cpu->DisassemblyGetDataRegisterUnmodified(reg, sourceMemoryAddress);
		break;
	case ADDREG_DIRECT:
		targetUnmodified = cpu->DisassemblyGetAddressRegisterUnmodified(reg, sourceMemoryAddress);
		break;
	case ADDREG_INDIRECT:
	case ADDREG_INDIRECT_POSTINC:{
		M68000Long address;
		cpu->GetA(reg, address);
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case ADDREG_INDIRECT_PREDEC:{
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
	case ADDREG_INDIRECT_DISPLACE:{
		M68000Long address;
		cpu->GetA(reg, address);
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case ADDREG_INDIRECT_INDEX_8BIT:{
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
	case PC_INDIRECT_DISPLACE:{
		M68000Long address;
		address = savedPC;
		address += M68000Long(displacement.SignExtend(BITCOUNT_LONG));
		targetUnmodified = true;
		sourceMemoryAddress = address.GetData();
		break;}
	case PC_INDIRECT_INDEX_8BIT:{
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

	case ABS_WORD:
	case ABS_LONG:
	case IMMEDIATE:
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
	case DATAREG_DIRECT:{
		memorySize = cpu->DisassemblyGetDataRegisterUnmodifiedSize(reg);
		break;}
	case ADDREG_DIRECT:
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
		ReleaseAssert(false);
		break;
	case ADDREG_INDIRECT:
		targetUnmodified = cpu->DisassemblyGetAddressRegisterUnmodified(reg, sourceMemoryAddress);
		dataSize = cpu->DisassemblyGetAddressRegisterUnmodifiedSize(reg);
		dataIsOffset = false;
		break;
	case ADDREG_INDIRECT_INDEX_8BIT:{
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
	case PC_INDIRECT_INDEX_8BIT:{
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

	case DATAREG_DIRECT:
	case ADDREG_DIRECT:
	case ADDREG_INDIRECT_DISPLACE:
	case PC_INDIRECT_DISPLACE:
	case ABS_WORD:
	case ABS_LONG:
	case IMMEDIATE:
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
	case DATAREG_DIRECT:
		stream << L'D' << reg;
		break;
	case ADDREG_DIRECT:
		stream << L'A' << reg;
		break;
	case ADDREG_INDIRECT:
		stream << L"(A" << reg << L")";
		break;
	case ADDREG_INDIRECT_POSTINC:
		stream << L"(A" << reg << L")+";
		break;
	case ADDREG_INDIRECT_PREDEC:
		stream << L"-(A" << reg << L")";
		break;
	case ADDREG_INDIRECT_DISPLACE:{
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
	case ADDREG_INDIRECT_INDEX_8BIT:
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
	case PC_INDIRECT_DISPLACE:{
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
	case PC_INDIRECT_INDEX_8BIT:{
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
	case ABS_WORD:
	case ABS_LONG:{
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
	case IMMEDIATE:
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
	case PC_INDIRECT_DISPLACE:
	case PC_INDIRECT_INDEX_8BIT:{
		Data targetAddress(BITCOUNT_LONG);
		targetAddress = savedPC.GetData() + displacement.SignExtend(BITCOUNT_LONG).GetData();
		labelTargetLocations.insert(targetAddress.GetData());
		break;}
	case ABS_WORD:
	case ABS_LONG:{
		M68000Long targetAddress = address.SignExtend(BITCOUNT_LONG).GetData();
		labelTargetLocations.insert(targetAddress.GetData());
		break;}
	}
}

} //Close namespace M68000
