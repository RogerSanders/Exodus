#ifndef __IINSTRUCTION_H__
#define __IINSTRUCTION_H__
class OpcodeTable;

class IInstruction
{
public:
	//Size functions
	virtual size_t GetOpcodeClassByteSize() const = 0;

	//Registration functions
	virtual bool RegisterOpcode(OpcodeTable& table) = 0;
};

#endif
