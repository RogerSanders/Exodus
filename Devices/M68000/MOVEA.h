#include "M68000Instruction.h"
#ifndef __M68000_MOVEA_H__
#define __M68000_MOVEA_H__
namespace M68000 {

class MOVEA :public M68000Instruction
{
public:
	ExecuteTime GetExecuteTime(EffectiveAddress::Mode sourceMode, EffectiveAddress::Mode targetMode, Bitcount size)
	{
		const ExecuteTime executeTimeArray[12][9] = {
			//Dn					An						(An)					(An)+					-(An)					d(An)					d(An,ix)				xxx.W					xxx.L
/*Dn*/		{ExecuteTime(4, 1, 0),	ExecuteTime(4, 1, 0),	ExecuteTime(8, 1, 1),	ExecuteTime(8, 1, 1),	ExecuteTime(8, 1, 1),	ExecuteTime(12, 2, 1),	ExecuteTime(14, 2, 1),	ExecuteTime(12, 2, 1),	ExecuteTime(16, 3, 1)},
/*An*/		{ExecuteTime(4, 1, 0),	ExecuteTime(4, 1, 0),	ExecuteTime(8, 1, 1),	ExecuteTime(8, 1, 1),	ExecuteTime(8, 1, 1),	ExecuteTime(12, 2, 1),	ExecuteTime(14, 2, 1),	ExecuteTime(12, 2, 1),	ExecuteTime(16, 3, 1)},
/*(An)*/	{ExecuteTime(8, 2, 0),	ExecuteTime(8, 2, 0),	ExecuteTime(12, 2, 1),	ExecuteTime(12, 2, 1),	ExecuteTime(12, 2, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(18, 3, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(20, 4, 1)},
/*(An)+*/	{ExecuteTime(8, 2, 0),	ExecuteTime(8, 2, 0),	ExecuteTime(12, 2, 1),	ExecuteTime(12, 2, 1),	ExecuteTime(12, 2, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(18, 3, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(20, 4, 1)},
/*-(An)*/	{ExecuteTime(10, 2, 0),	ExecuteTime(10, 2, 0),	ExecuteTime(14, 2, 1),	ExecuteTime(14, 2, 1),	ExecuteTime(14, 2, 1),	ExecuteTime(18, 3, 1),	ExecuteTime(20, 4, 1),	ExecuteTime(18, 3, 1),	ExecuteTime(22, 4, 1)},
/*d(An)*/	{ExecuteTime(12, 3, 0),	ExecuteTime(12, 3, 0),	ExecuteTime(16, 3, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(20, 4, 1),	ExecuteTime(22, 4, 1),	ExecuteTime(20, 4, 1),	ExecuteTime(24, 5, 1)},
/*d(An,ix)*/{ExecuteTime(14, 3, 0),	ExecuteTime(14, 3, 0),	ExecuteTime(18, 3, 1),	ExecuteTime(18, 3, 1),	ExecuteTime(18, 3, 1),	ExecuteTime(22, 4, 1),	ExecuteTime(24, 4, 1),	ExecuteTime(22, 4, 1),	ExecuteTime(26, 5, 1)},
/*xxx.W*/	{ExecuteTime(12, 3, 0),	ExecuteTime(12, 3, 0),	ExecuteTime(16, 3, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(20, 4, 1),	ExecuteTime(22, 4, 1),	ExecuteTime(20, 4, 1),	ExecuteTime(24, 5, 1)},
/*xxx.L*/	{ExecuteTime(16, 4, 0),	ExecuteTime(16, 4, 0),	ExecuteTime(20, 4, 1),	ExecuteTime(20, 4, 1),	ExecuteTime(20, 4, 1),	ExecuteTime(24, 5, 1),	ExecuteTime(26, 5, 1),	ExecuteTime(24, 5, 1),	ExecuteTime(28, 6, 1)},
/*d(PC)*/	{ExecuteTime(12, 3, 0),	ExecuteTime(12, 3, 0),	ExecuteTime(16, 3, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(20, 4, 1),	ExecuteTime(22, 4, 1),	ExecuteTime(20, 4, 1),	ExecuteTime(24, 5, 1)},
/*d(PC,ix)*/{ExecuteTime(14, 3, 0),	ExecuteTime(14, 3, 0),	ExecuteTime(18, 3, 1),	ExecuteTime(18, 3, 1),	ExecuteTime(18, 3, 1),	ExecuteTime(22, 4, 1),	ExecuteTime(24, 4, 1),	ExecuteTime(22, 4, 1),	ExecuteTime(26, 5, 1)},
/*#xxx*/	{ExecuteTime(8, 2, 0),	ExecuteTime(8, 2, 0),	ExecuteTime(12, 2, 1),	ExecuteTime(12, 2, 1),	ExecuteTime(12, 2, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(18, 3, 1),	ExecuteTime(16, 3, 1),	ExecuteTime(20, 4, 1)}};
		const ExecuteTime executeTimeArrayLong[12][9] = {
			//Dn					An						(An)					(An)+					-(An)					d(An)					d(An,ix)				xxx.W					xxx.L
/*Dn*/		{ExecuteTime(4, 1, 0),	ExecuteTime(4, 1, 0),	ExecuteTime(12, 1, 2),	ExecuteTime(12, 1, 2),	ExecuteTime(12, 1, 2),	ExecuteTime(16, 2, 2),	ExecuteTime(18, 2, 2),	ExecuteTime(16, 2, 2),	ExecuteTime(20, 3, 2)},
/*An*/		{ExecuteTime(4, 1, 0),	ExecuteTime(4, 1, 0),	ExecuteTime(12, 1, 2),	ExecuteTime(12, 1, 2),	ExecuteTime(12, 1, 2),	ExecuteTime(16, 2, 2),	ExecuteTime(18, 2, 2),	ExecuteTime(16, 2, 2),	ExecuteTime(20, 3, 2)},
/*(An)*/	{ExecuteTime(12, 3, 0),	ExecuteTime(12, 3, 0),	ExecuteTime(20, 3, 2),	ExecuteTime(20, 3, 2),	ExecuteTime(20, 3, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(26, 4, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(28, 5, 2)},
/*(An)+*/	{ExecuteTime(12, 3, 0),	ExecuteTime(12, 3, 0),	ExecuteTime(20, 3, 2),	ExecuteTime(20, 3, 2),	ExecuteTime(20, 3, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(26, 4, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(28, 5, 2)},
/*-(An)*/	{ExecuteTime(14, 3, 0),	ExecuteTime(14, 3, 0),	ExecuteTime(22, 3, 2),	ExecuteTime(22, 3, 2),	ExecuteTime(22, 3, 2),	ExecuteTime(26, 4, 2),	ExecuteTime(28, 4, 2),	ExecuteTime(26, 4, 2),	ExecuteTime(30, 5, 2)},
/*d(An)*/	{ExecuteTime(16, 4, 0),	ExecuteTime(16, 4, 0),	ExecuteTime(24, 4, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(28, 5, 2),	ExecuteTime(30, 5, 2),	ExecuteTime(28, 5, 2),	ExecuteTime(32, 6, 2)},
/*d(An,ix)*/{ExecuteTime(18, 4, 0),	ExecuteTime(18, 4, 0),	ExecuteTime(26, 4, 2),	ExecuteTime(26, 4, 2),	ExecuteTime(26, 4, 2),	ExecuteTime(30, 5, 2),	ExecuteTime(32, 5, 2),	ExecuteTime(30, 5, 2),	ExecuteTime(34, 6, 2)},
/*xxx.W*/	{ExecuteTime(16, 4, 0),	ExecuteTime(16, 4, 0),	ExecuteTime(24, 4, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(28, 5, 2),	ExecuteTime(30, 5, 2),	ExecuteTime(28, 5, 2),	ExecuteTime(32, 6, 2)},
/*xxx.L*/	{ExecuteTime(20, 5, 0),	ExecuteTime(20, 5, 0),	ExecuteTime(28, 5, 2),	ExecuteTime(28, 5, 2),	ExecuteTime(28, 5, 2),	ExecuteTime(32, 6, 2),	ExecuteTime(34, 6, 2),	ExecuteTime(32, 6, 2),	ExecuteTime(36, 7, 2)},
/*d(PC)*/	{ExecuteTime(16, 4, 0),	ExecuteTime(16, 4, 0),	ExecuteTime(24, 4, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(28, 5, 2),	ExecuteTime(30, 5, 2),	ExecuteTime(28, 5, 2),	ExecuteTime(32, 5, 2)},
/*d(PC,ix)*/{ExecuteTime(18, 4, 0),	ExecuteTime(18, 4, 0),	ExecuteTime(26, 4, 2),	ExecuteTime(26, 4, 2),	ExecuteTime(26, 4, 2),	ExecuteTime(30, 5, 2),	ExecuteTime(32, 5, 2),	ExecuteTime(30, 5, 2),	ExecuteTime(34, 6, 2)},
/*#xxx*/	{ExecuteTime(12, 3, 0),	ExecuteTime(12, 3, 0),	ExecuteTime(20, 3, 2),	ExecuteTime(20, 3, 2),	ExecuteTime(20, 3, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(26, 4, 2),	ExecuteTime(24, 4, 2),	ExecuteTime(28, 5, 2)}};
		unsigned int sourceIndex = 0;
		unsigned int targetIndex = 0;
		switch(sourceMode)
		{
		case EffectiveAddress::DATAREG_DIRECT:
			sourceIndex = 0;
			break;
		case EffectiveAddress::ADDREG_DIRECT:
			sourceIndex = 1;
			break;
		case EffectiveAddress::ADDREG_INDIRECT:
			sourceIndex = 2;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_POSTINC:
			sourceIndex = 3;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_PREDEC:
			sourceIndex = 4;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_DISPLACE:
			sourceIndex = 5;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_INDEX_8BIT:
			sourceIndex = 6;
			break;
		case EffectiveAddress::ABS_WORD:
			sourceIndex = 7;
			break;
		case EffectiveAddress::ABS_LONG:
			sourceIndex = 8;
			break;
		case EffectiveAddress::PC_INDIRECT_DISPLACE:
			sourceIndex = 9;
			break;
		case EffectiveAddress::PC_INDIRECT_INDEX_8BIT:
			sourceIndex = 10;
			break;
		case EffectiveAddress::IMMEDIATE:
			sourceIndex = 11;
			break;
		}
		switch(targetMode)
		{
		case EffectiveAddress::DATAREG_DIRECT:
			sourceIndex = 0;
			break;
		case EffectiveAddress::ADDREG_DIRECT:
			sourceIndex = 1;
			break;
		case EffectiveAddress::ADDREG_INDIRECT:
			sourceIndex = 2;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_POSTINC:
			sourceIndex = 3;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_PREDEC:
			sourceIndex = 4;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_DISPLACE:
			sourceIndex = 5;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_INDEX_8BIT:
			sourceIndex = 6;
			break;
		case EffectiveAddress::ABS_WORD:
			sourceIndex = 7;
			break;
		case EffectiveAddress::ABS_LONG:
			sourceIndex = 8;
			break;
		}
		if(size != BITCOUNT_LONG)
		{
			return executeTimeArray[sourceIndex][targetIndex];
		}
		else
		{
			return executeTimeArrayLong[sourceIndex][targetIndex];
		}
	}

	virtual MOVEA* Clone() {return new MOVEA();}
	virtual MOVEA* ClonePlacement(void* buffer) {return new(buffer) MOVEA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"00CC***001DDDDDD", L"CC=11,10 DDDDDD=000000-110111,111000,111001,111010,111011,111100");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"MOVEA." + DisassembleSize(size), source.Disassemble() + L", " + target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|-------|-----------|---|---|---|-----------|-----------|
//	| 0 | 0 |  SIZE |  ADDRESS  | 0 | 0 | 1 |    MODE   | REGISTER  |
//	|   |   |       |  REGISTER |   |   |   |           |           |
//	----------------------------------------=========================
//	                                        |------source <ea>------|
		switch(data.GetDataSegment(12, 2))
		{
		case 3:	//11
			size = BITCOUNT_WORD;
			break;
		case 2:	//10
			size = BITCOUNT_LONG;
			break;
		}

		source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());
		target.BuildAddressDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
		AddExecuteCycleCount(GetExecuteTime(source.GetAddressMode(), target.GetAddressMode(), size));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data result(size);

		//Read information about the source for use in active disassembly
		unsigned int sourceReadFromAddress;
		bool sourceIsUnmodifiedFromAddress = source.IsTargetUnmodifiedFromMemoryRead(cpu, size, sourceReadFromAddress);
		unsigned int sourceReadFromAddressSize = source.GetTargetOriginalMemoryReadSize(cpu, size);

		//Perform the operation
		additionalTime += source.Read(cpu, result, GetInstructionRegister());
		additionalTime += target.Write(cpu, result.SignExtend(BITCOUNT_LONG), GetInstructionRegister(), false, false, sourceIsUnmodifiedFromAddress, sourceReadFromAddress, sourceReadFromAddressSize);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	Bitcount size;
};

} //Close namespace M68000
#endif
