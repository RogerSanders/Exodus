#include "Z80Instruction.h"
#ifndef __Z80_DAA_H__
#define __Z80_DAA_H__
namespace Z80 {

class DAA :public Z80Instruction
{
public:
	virtual DAA* Clone() const {return new DAA();}
	virtual DAA* ClonePlacement(void* buffer) const {return new(buffer) DAA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00100111", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"DAA";
	}

	virtual Disassembly Z80Disassemble() const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	Z80Byte GetDiffValue(bool cflag, unsigned int upperData, bool hflag, unsigned int lowerData) const
	{
		//	----------------------------
		//	|CF | High |HF | Low  |Diff|
		//	|   |nibble|   |nibble|    |
		//	|---|------|---|------|----|
		//	| 0 | 0-9  | 0 | 0-9  | 00 |
		//	| 0 | 0-9  | 1 | 0-9  | 06 |
		//	| 0 | 0-8  | * | A-F  | 06 |
		//	| 0 | A-F  | 0 | 0-9  | 60 |
		//	| 1 |  *   | 0 | 0-9  | 60 |
		//	| 1 |  *   | 1 | 0-9  | 66 |
		//	| 1 |  *   | * | A-F  | 66 |
		//	| 0 | 9-F  | * | A-F  | 66 |
		//	| 0 | A-F  | 1 | 0-9  | 66 |
		//	----------------------------
		if(!cflag && (upperData <= 0x9) && !hflag && (lowerData <= 0x9))
		{
			return 0x00;
		}
		else if(!cflag && (upperData <= 0x9) && hflag && (lowerData <= 0x9))
		{
			return 0x06;
		}
		else if(!cflag && (upperData <= 0x8) && (lowerData >= 0xA))
		{
			return 0x06;
		}
		else if(!cflag && (upperData >= 0xA) && !hflag && (lowerData <= 0x9))
		{
			return 0x60;
		}
		else if(cflag && !hflag && (lowerData <= 0x9))
		{
			return 0x60;
		}
		else if(cflag && hflag && (lowerData <= 0x9))
		{
			return 0x66;
		}
		else if(cflag && (lowerData >= 0xA))
		{
			return 0x66;
		}
		else if(!cflag && (upperData >= 0x9) && (lowerData >= 0xA))
		{
			return 0x66;
		}
		else if(!cflag && (upperData >= 0xA) && hflag && (lowerData <= 0x9))
		{
			return 0x66;
		}
		else
		{
			//##DEBUG##
			std::wcout << "Z80 Error! DAA diff table failed to find match for data!\n";
			std::wcout << cflag << '\t' << upperData << '\t' << hflag << '\t' << lowerData << '\n';
			return 0;
		}
	}

	bool GetNewCF(bool cflag, unsigned int upperData, unsigned int lowerData) const
	{
		//	-----------------------
		//	|CF | High | Low  |CF'|
		//	|   |nibble|nibble|   |
		//	|---|------|------|---|
		//	| 0 | 0-9  | 0-9  | 0 |
		//	| 0 | 0-8  | A-F  | 0 |
		//	| 0 | 9-F  | A-F  | 1 |
		//	| 0 | A-F  | 0-9  | 1 |
		//	| 1 |  *   |  *   | 1 |
		//	-----------------------
		if(!cflag && (upperData <= 0x9) && (lowerData <= 0x9))
		{
			return false;
		}
		else if(!cflag && (upperData <= 0x8) && (lowerData >= 0xA))
		{
			return false;
		}
		else if(!cflag && (upperData >= 0x9) && (lowerData >= 0xA))
		{
			return true;
		}
		else if(!cflag && (upperData >= 0xA) && (lowerData <= 0x9))
		{
			return true;
		}
		else if(cflag)
		{
			return true;
		}
		else
		{
			//##DEBUG##
			std::wcout << "Z80 Error! DAA CF table failed to find match for data!\n";
			std::wcout << cflag << '\t' << upperData << '\t' << lowerData << '\n';
			return false;
		}
	}

	bool GetNewHF(bool nflag, bool hflag, unsigned int lowerData) const
	{
		//	--------------------
		//	|NF |HF | Low  |HF'|
		//	|   |   |nibble|   |
		//	|---|---|------|---|
		//	| 0 | * | 0-9  | 0 |
		//	| 0 | * | A-F  | 1 |
		//	| 1 | 0 |  *   | 0 |
		//	| 1 | 1 | 6-F  | 0 |
		//	| 1 | 1 | 0-5  | 1 |
		//	-----------------------
		if(!nflag && (lowerData <= 0x9))
		{
			return false;
		}
		else if(!nflag && (lowerData >= 0xA))
		{
			return true;
		}
		else if(nflag && !hflag)
		{
			return false;
		}
		else if(nflag && hflag && (lowerData >= 0x6))
		{
			return false;
		}
		else if(nflag && hflag && (lowerData <= 0x5))
		{
			return true;
		}
		else
		{
			//##DEBUG##
			std::wcout << "Z80 Error! DAA HF table failed to find match for data!\n";
			std::wcout << nflag << '\t' << hflag << '\t' << lowerData << '\n';
			return false;
		}
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetMode(EffectiveAddress::MODE_A);

		AddInstructionSize(GetIndexOffsetSize(target.UsesIndexOffset()));
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte result;
		Z80Byte diff;

		//Perform the operation
		additionalTime += target.Read(cpu, location, op1);
		diff = GetDiffValue(cpu->GetFlagC(), op1.GetUpperHalf(), cpu->GetFlagH(), op1.GetLowerHalf());
		if(!cpu->GetFlagN())
		{
			result = op1 + diff;
		}
		else
		{
			result = op1 - diff;
		}
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagS(result.Negative());
		cpu->SetFlagZ(result.Zero());
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH(GetNewHF(cpu->GetFlagN(), cpu->GetFlagH(), op1.GetLowerHalf()));
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagPV(result.ParityEven());
		cpu->SetFlagC(GetNewCF(cpu->GetFlagC(), op1.GetUpperHalf(), op1.GetLowerHalf()));

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
};

} //Close namespace Z80
#endif
