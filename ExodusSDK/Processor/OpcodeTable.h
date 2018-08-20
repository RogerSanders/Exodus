#ifndef __OPCODETABLE_H__
#define __OPCODETABLE_H__
#include <vector>
#include <set>
#include <string>

template<class T>
class OpcodeTable
{
public:
	// Constructors
	OpcodeTable(unsigned int opcodeDecodeBits);

	// Initialization functions
	void InitializeOpcodeTable();

	// Instruction functions
	bool AllocateRegionToOpcode(const T* opcode, const std::wstring& definition, const std::wstring& substitutions);
	inline const T* GetInstruction(unsigned int opcode) const;

private:
	unsigned int _opcodeDecodeBits;
	unsigned int _opcodeDecodeMask;
	std::vector<const T*> _opcodeArray;
};

#include "OpcodeTable.inl"
#endif
