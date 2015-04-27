#include "OpcodeInfo.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
OpcodeInfo::OpcodeInfo()
:valid(false), opcodeSize(0)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int OpcodeInfo::GetIOpcodeInfoVersion() const
{
	return ThisIOpcodeInfoVersion();
}

//----------------------------------------------------------------------------------------
//Opcode info functions
//----------------------------------------------------------------------------------------
bool OpcodeInfo::GetIsValidOpcode() const
{
	return valid;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetIsValidOpcode(bool state)
{
	valid = state;
}

//----------------------------------------------------------------------------------------
unsigned int OpcodeInfo::GetOpcodeSize() const
{
	return opcodeSize;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetOpcodeSize(unsigned int state)
{
	opcodeSize = state;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> OpcodeInfo::GetOpcodeNameDisassembly() const
{
	return disassemblyOpcode;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetOpcodeNameDisassembly(const MarshalSupport::Marshal::In<std::wstring>& state)
{
	disassemblyOpcode = state;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> OpcodeInfo::GetOpcodeArgumentsDisassembly() const
{
	return disassemblyArguments;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetOpcodeArgumentsDisassembly(const MarshalSupport::Marshal::In<std::wstring>& state)
{
	disassemblyArguments = state;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> OpcodeInfo::GetDisassemblyComment() const
{
	return disassemblyComment;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetDisassemblyComment(const MarshalSupport::Marshal::In<std::wstring>& state)
{
	disassemblyComment = state;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::set<unsigned int>> OpcodeInfo::GetLabelTargetLocations() const
{
	return labelTargetLocations;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetLabelTargetLocations(const MarshalSupport::Marshal::In<std::set<unsigned int>>& state)
{
	labelTargetLocations = state;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::set<unsigned int>> OpcodeInfo::GetResultantPCLocations() const
{
	return resultantPCLocations;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetResultantPCLocations(const MarshalSupport::Marshal::In<std::set<unsigned int>>& state)
{
	resultantPCLocations = state;
}

//----------------------------------------------------------------------------------------
bool OpcodeInfo::GetHasUndeterminedResultantPCLocation() const
{
	return undeterminedResultantPCLocation;
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetHasUndeterminedResultantPCLocation(bool state)
{
	undeterminedResultantPCLocation = state;
}
