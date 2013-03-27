#include "Z80Instruction.h"
#ifndef __Z80_LD8_H__
#define __Z80_LD8_H__
namespace Z80 {

class LD8 :public Z80Instruction
{
public:
	virtual LD8* Clone() const {return new LD8();}
	virtual LD8* ClonePlacement(void* buffer) const {return new(buffer) LD8();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"01AAABBB", L"AAA=111,000-101 BBB=111,000-101");
		result &= table.AllocateRegionToOpcode(this, L"01110AAA", L"AAA=111,000-101");
		result &= table.AllocateRegionToOpcode(this, L"01AAA110", L"AAA=111,000-101");
		result &= table.AllocateRegionToOpcode(this, L"000**010", L"");
		result &= table.AllocateRegionToOpcode(this, L"0011*010", L"");
		result &= table.AllocateRegionToOpcode(this, L"00110110", L"");
		result &= table.AllocateRegionToOpcode(this, L"00AAA110", L"AAA=111,000-101");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"LD";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		if(data.GetBit(6))
		{
			if(data.GetDataSegment(0, 3) == 6)
			{
				//LD r,(HL)		01rrr110
				//LD r,(IX+d)	11011101 01rrr110 dddddddd
				//LD r,(IY+d)	11111101 01rrr110 dddddddd
				source.SetMode(EffectiveAddress::MODE_HL_INDIRECT);
				//##NOTE## We override the index state for the target in this form of the
				//opcode. Just to be difficult, this form doesn't apply the index mode to
				//the target field, but it does apply it to the source field. The same is
				//true of the reverse form below. The only other opcode where there are
				//exceptions to the indexing rule is EX.
				target.SetIndexState(EffectiveAddress::INDEX_NONE, 0);
				target.Decode8BitRegister(data.GetDataSegment(3, 3));
				if(GetIndexState() == EffectiveAddress::INDEX_NONE)
				{
					AddExecuteCycleCount(7);
				}
				else
				{
					AddExecuteCycleCount(15);
				}
			}
			else if(data.GetDataSegment(3, 3) == 6)
			{
				//LD (HL),r		01110rrr
				//LD (IX+d),r	11011101 01110rrr dddddddd
				//LD (IY+d),r	11111101 01110rrr dddddddd
				//##NOTE## See notes on reverse form above.
				source.SetIndexState(EffectiveAddress::INDEX_NONE, 0);
				source.Decode8BitRegister(data.GetDataSegment(0, 3));
				target.SetMode(EffectiveAddress::MODE_HL_INDIRECT);
				if(GetIndexState() == EffectiveAddress::INDEX_NONE)
				{
					AddExecuteCycleCount(7);
				}
				else
				{
					AddExecuteCycleCount(15);
				}
			}
			else
			{
				//##NOTE## The notation r' doesn't refer to a shadow register, just a
				//second normal register
				//LD r,r'		01rrrddd
				source.Decode8BitRegister(data.GetDataSegment(0, 3));
				target.Decode8BitRegister(data.GetDataSegment(3, 3));
				AddExecuteCycleCount(4);
			}
		}
		else
		{
			if(data == 0x0A)
			{
				//LD A,(BC)		00001010
				source.SetMode(EffectiveAddress::MODE_BC_INDIRECT);
				target.SetMode(EffectiveAddress::MODE_A);
				AddExecuteCycleCount(7);
			}
			else if(data == 0x1A)
			{
				//LD A,(DE)		00011010
				source.SetMode(EffectiveAddress::MODE_DE_INDIRECT);
				target.SetMode(EffectiveAddress::MODE_A);
				AddExecuteCycleCount(7);
			}
			else if(data == 0x3A)
			{
				//LD A,(nn)		00111010 nnnnnnnn nnnnnnnn
				source.BuildAbsoluteAddress(location + GetInstructionSize(), cpu, transparent);
				target.SetMode(EffectiveAddress::MODE_A);
				AddExecuteCycleCount(13);
			}
			else if(data == 0x02)
			{
				//LD (BC),A		00000010
				source.SetMode(EffectiveAddress::MODE_A);
				target.SetMode(EffectiveAddress::MODE_BC_INDIRECT);
				AddExecuteCycleCount(7);
			}
			else if(data == 0x12)
			{
				//LD (DE),A		00010010
				source.SetMode(EffectiveAddress::MODE_A);
				target.SetMode(EffectiveAddress::MODE_DE_INDIRECT);
				AddExecuteCycleCount(7);
			}
			else if(data == 0x32)
			{
				//LD (nn),A		00110010 nnnnnnnn nnnnnnnn
				source.SetMode(EffectiveAddress::MODE_A);
				target.BuildAbsoluteAddress(location + GetInstructionSize(), cpu, transparent);
				AddExecuteCycleCount(13);
			}
			else if(data == 0x36)
			{
				//LD (HL),n		00110110 nnnnnnnn
				//LD (IX+d),n	11011101 00110110 nnnnnnnn
				//LD (IY+d),n	11111101 00110110 nnnnnnnn
				target.SetMode(EffectiveAddress::MODE_HL_INDIRECT);
				source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize() + GetIndexOffsetSize(target.UsesIndexOffset()), cpu, transparent);
				if(GetIndexState() == EffectiveAddress::INDEX_NONE)
				{
					AddExecuteCycleCount(10);
				}
				else
				{
					AddExecuteCycleCount(15);
				}
			}
			else
			{
				//LD r,n		00rrr110 nnnnnnnn
				source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent);
				target.Decode8BitRegister(data.GetDataSegment(3, 3));
				AddExecuteCycleCount(7);
			}
		}

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset() || target.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte result;

		//Perform the operation
		additionalTime += source.Read(cpu, location, result);
		additionalTime += target.Write(cpu, location, result);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace Z80
#endif
