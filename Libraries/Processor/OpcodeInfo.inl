//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
OpcodeInfo::OpcodeInfo()
:valid(false), opcodeSize(0)
{}

//----------------------------------------------------------------------------------------
//Opcode info functions
//----------------------------------------------------------------------------------------
std::wstring OpcodeInfo::GetOpcodeNameDisassembly() const
{
	return disassemblyOpcode;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetOpcodeNameDisassembly(const std::wstring& state)
{
	disassemblyOpcode = state;
}

//----------------------------------------------------------------------------------------
std::wstring OpcodeInfo::GetOpcodeArgumentsDisassembly() const
{
	return disassemblyArguments;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetOpcodeArgumentsDisassembly(const std::wstring& state)
{
	disassemblyArguments = state;
}

//----------------------------------------------------------------------------------------
std::wstring OpcodeInfo::GetDisassemblyComment() const
{
	return disassemblyComment;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetDisassemblyComment(const std::wstring& state)
{
	disassemblyComment = state;
}

//----------------------------------------------------------------------------------------
std::set<unsigned int> OpcodeInfo::GetLabelTargetLocations() const
{
	return labelTargetLocations;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetLabelTargetLocations(const std::set<unsigned int>& state)
{
	labelTargetLocations = state;
}

//----------------------------------------------------------------------------------------
std::set<unsigned int> OpcodeInfo::GetResultantPCLocations() const
{
	return resultantPCLocations;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetResultantPCLocations(const std::set<unsigned int>& state)
{
	resultantPCLocations = state;
}
