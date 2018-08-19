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
	enum class ConditionCode;

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
	inline void SetInstructionSize(unsigned int instructionSize);
	inline void AddInstructionSize(unsigned int instructionSize);

	//Condition code functions
	static bool ConditionCodeTrue(Z80* cpu, ConditionCode conditionCode);

	//Index functions
	inline EffectiveAddress::IndexState GetIndexState() const;
	inline void SetIndexState(EffectiveAddress::IndexState indexState);
	inline Z80Byte GetIndexOffset() const;
	inline void SetIndexOffset(const Z80Byte& indexOffset, bool mandatoryIndexOffset);
	inline unsigned int GetIndexOffsetSize(bool add) const;

	//Execute functions
	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent) = 0;
	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const = 0;

	//Execute time functions
	inline ExecuteTime GetExecuteCycleCount() const;
	inline ExecuteTime GetExecuteCycleCount(double additionalTime) const;
	inline void AddExecuteCycleCount(const ExecuteTime& executeCycleCount);

	//Instruction info functions
	inline const Z80Byte& GetInstructionRegister() const;
	inline void SetInstructionRegister(const Z80Byte& instructionData);
	inline const Z80Word& GetInstructionLocation() const;
	inline void SetInstructionLocation(const Z80Word& instructionLocation);

	//Transparent decode functions
	inline bool GetTransparentFlag() const;
	inline void SetTransparentFlag(bool state);

	//Disassembly functions
	virtual std::wstring GetOpcodeName() const;
	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const;
	static std::wstring DisassembleConditionCode(ConditionCode conditionCode);

private:
	unsigned int _instructionSize;
	ExecuteTime _executeCycleCount;
	EffectiveAddress::IndexState _indexState;
	Z80Word _instructionLocation;
	Z80Byte _instructionRegister;
	Z80Byte _indexOffset;
	bool _transparentOpcode;
	bool _mandatoryIndexOffset;
};

} //Close namespace Z80
#include "Z80Instruction.inl"
#endif
