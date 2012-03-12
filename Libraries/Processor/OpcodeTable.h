#ifndef __OPCODETABLE_H__
#define __OPCODETABLE_H__
#include <vector>
#include <set>
#include <string>

template<class T> class OpcodeTable
{
public:
	//Constructors
	OpcodeTable(unsigned int aopcodeDecodeBits);
	~OpcodeTable();

	//Initialization functions
	void InitializeOpcodeTable();

	//Instruction functions
	bool RegisterOpcode(T* opcode);
	bool AllocateRegionToOpcode(T* opcode, const std::wstring& definition, const std::wstring& substitutions);
	T* GetInstruction(unsigned int opcode) const;

private:
	unsigned int opcodeDecodeBits;
	unsigned int opcodeDecodeMask;

	std::set<T*> opcodeSet;
	std::vector<T*> opcodeArray;
};

#include "OpcodeTable.inl"
#endif
