//----------------------------------------------------------------------------------------
//Instruction functions
//----------------------------------------------------------------------------------------
IInstruction* OpcodeTable::GetInstruction(unsigned int opcode) const
{
	return opcodeArray[opcode & opcodeDecodeMask];
}
