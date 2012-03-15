#ifndef __OPCODETABLE_H__
#define __OPCODETABLE_H__
#include <vector>
#include <set>
#include <string>
class IInstruction;

class OpcodeTable
{
public:
	//Constructors
	OpcodeTable(unsigned int aopcodeDecodeBits);
	~OpcodeTable();

	//Initialization functions
	void InitializeOpcodeTable();

	//Instruction functions
	bool RegisterOpcode(IInstruction* opcode);
	bool AllocateRegionToOpcode(IInstruction* opcode, const std::wstring& definition, const std::wstring& substitutions);
	inline IInstruction* GetInstruction(unsigned int opcode) const;
	size_t GetLargestOpcodeObjectSize() const;

private:
	unsigned int opcodeDecodeBits;
	unsigned int opcodeDecodeMask;
	std::set<IInstruction*> opcodeSet;
	std::vector<IInstruction*> opcodeArray;
};

#include "OpcodeTable.inl"
#endif
