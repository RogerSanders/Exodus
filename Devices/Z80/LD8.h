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
		return Disassembly(GetOpcodeName(), _target.Disassemble() + L", " + _source.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_source.SetIndexState(GetIndexState(), GetIndexOffset());
		_target.SetIndexState(GetIndexState(), GetIndexOffset());

		if(data.GetBit(6))
		{
			if(data.GetDataSegment(0, 3) == 6)
			{
				//LD r,(HL)		01rrr110
				//LD r,(IX+d)	11011101 01rrr110 dddddddd
				//LD r,(IY+d)	11111101 01rrr110 dddddddd
				_source.SetMode(EffectiveAddress::Mode::HLIndirect);
				//##NOTE## We override the index state for the _target in this form of the
				//opcode. Just to be difficult, this form doesn't apply the index mode to
				//the _target field, but it does apply it to the _source field. The same is
				//true of the reverse form below. The only other opcode where there are
				//exceptions to the indexing rule is EX.
				_target.SetIndexState(EffectiveAddress::IndexState::None, 0);
				_target.Decode8BitRegister(data.GetDataSegment(3, 3));
				if(GetIndexState() == EffectiveAddress::IndexState::None)
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
				_source.SetIndexState(EffectiveAddress::IndexState::None, 0);
				_source.Decode8BitRegister(data.GetDataSegment(0, 3));
				_target.SetMode(EffectiveAddress::Mode::HLIndirect);
				if(GetIndexState() == EffectiveAddress::IndexState::None)
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
				_source.Decode8BitRegister(data.GetDataSegment(0, 3));
				_target.Decode8BitRegister(data.GetDataSegment(3, 3));
				AddExecuteCycleCount(4);
			}
		}
		else
		{
			if(data == 0x0A)
			{
				//LD A,(BC)		00001010
				_source.SetMode(EffectiveAddress::Mode::BCIndirect);
				_target.SetMode(EffectiveAddress::Mode::A);
				AddExecuteCycleCount(7);
			}
			else if(data == 0x1A)
			{
				//LD A,(DE)		00011010
				_source.SetMode(EffectiveAddress::Mode::DEIndirect);
				_target.SetMode(EffectiveAddress::Mode::A);
				AddExecuteCycleCount(7);
			}
			else if(data == 0x3A)
			{
				//LD A,(nn)		00111010 nnnnnnnn nnnnnnnn
				_source.BuildAbsoluteAddress(location + GetInstructionSize(), cpu, transparent);
				_target.SetMode(EffectiveAddress::Mode::A);
				AddExecuteCycleCount(13);
			}
			else if(data == 0x02)
			{
				//LD (BC),A		00000010
				_source.SetMode(EffectiveAddress::Mode::A);
				_target.SetMode(EffectiveAddress::Mode::BCIndirect);
				AddExecuteCycleCount(7);
			}
			else if(data == 0x12)
			{
				//LD (DE),A		00010010
				_source.SetMode(EffectiveAddress::Mode::A);
				_target.SetMode(EffectiveAddress::Mode::DEIndirect);
				AddExecuteCycleCount(7);
			}
			else if(data == 0x32)
			{
				//LD (nn),A		00110010 nnnnnnnn nnnnnnnn
				_source.SetMode(EffectiveAddress::Mode::A);
				_target.BuildAbsoluteAddress(location + GetInstructionSize(), cpu, transparent);
				AddExecuteCycleCount(13);
			}
			else if(data == 0x36)
			{
				//LD (HL),n		00110110 nnnnnnnn
				//LD (IX+d),n	11011101 00110110 nnnnnnnn
				//LD (IY+d),n	11111101 00110110 nnnnnnnn
				_target.SetMode(EffectiveAddress::Mode::HLIndirect);
				_source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize() + GetIndexOffsetSize(_target.UsesIndexOffset()), cpu, transparent);
				if(GetIndexState() == EffectiveAddress::IndexState::None)
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
				_source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent);
				_target.Decode8BitRegister(data.GetDataSegment(3, 3));
				AddExecuteCycleCount(7);
			}
		}

		AddInstructionSize(GetIndexOffsetSize(_source.UsesIndexOffset() || _target.UsesIndexOffset()));
		AddInstructionSize(_source.ExtensionSize());
		AddInstructionSize(_target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte result;

		//Perform the operation
		additionalTime += _source.Read(cpu, location, result);
		additionalTime += _target.Write(cpu, location, result);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress _source;
	EffectiveAddress _target;
};

} //Close namespace Z80
#endif
