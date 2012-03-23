#ifndef __Z80INSTRUCTION_H__
#define __Z80INSTRUCTION_H__
#include "Z80.h"
#include "ExecuteTime.h"
#include "EffectiveAddress.h"
namespace Z80 {

class Z80Instruction
{
public:
	//Enumerations
	enum ConditionCode;

	//Structures
	struct Disassembly;

	//Constructors
	inline Z80Instruction();
	inline Z80Instruction(const Z80Instruction& object);
	virtual ~Z80Instruction();

	//Clone functions
	virtual Z80Instruction* Clone() const = 0;
	virtual Z80Instruction* ClonePlacement(void* buffer) const = 0;
	virtual size_t GetOpcodeClassByteSize() const = 0;

	//Registration functions
	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const = 0;

	//Size functions
	inline unsigned int GetInstructionSize() const;
	inline void SetInstructionSize(unsigned int ainstructionSize);
	inline void AddInstructionSize(unsigned int ainstructionSize);

	//Condition code functions
	static bool ConditionCodeTrue(Z80* cpu, ConditionCode conditionCode);

	//Index functions
	inline EffectiveAddress::IndexState GetIndexState() const;
	inline void SetIndexState(EffectiveAddress::IndexState aindexState);
	inline Z80Byte GetIndexOffset() const;
	inline void SetIndexOffset(const Z80Byte& aindexOffset, bool amandatoryIndexOffset);
	inline unsigned int GetIndexOffsetSize(bool add) const;

	//Execute functions
	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent) = 0;
	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const = 0;

	//Execute time functions
	inline ExecuteTime GetExecuteCycleCount() const;
	inline ExecuteTime GetExecuteCycleCount(double additionalTime) const;
	inline void AddExecuteCycleCount(const ExecuteTime& aexecuteCycleCount);

	//Instruction info functions
	inline const Z80Byte& GetInstructionRegister() const;
	inline void SetInstructionRegister(const Z80Byte& ainstructionData);
	inline const Z80Word& GetInstructionLocation() const;
	inline void SetInstructionLocation(const Z80Word& ainstructionLocation);

	//Transparent decode functions
	inline bool GetTransparentFlag() const;
	inline void SetTransparentFlag(bool astate);

	//Disassembly functions
	virtual std::wstring GetOpcodeName() const;
	virtual Disassembly Z80Disassemble() const;
	static std::wstring DisassembleConditionCode(ConditionCode conditionCode);

private:
	unsigned int instructionSize;
	ExecuteTime executeCycleCount;
	EffectiveAddress::IndexState indexState;
	Z80Word instructionLocation;
	Z80Byte instructionRegister;
	Z80Byte indexOffset;
	bool transparentOpcode;
	bool mandatoryIndexOffset;
};

} //Close namespace Z80
#include "Z80Instruction.inl"
#endif
