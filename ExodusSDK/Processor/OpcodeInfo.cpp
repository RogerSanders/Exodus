#include "OpcodeInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
OpcodeInfo::OpcodeInfo()
:_valid(false), _opcodeSize(0)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int OpcodeInfo::GetIOpcodeInfoVersion() const
{
	return ThisIOpcodeInfoVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Opcode info functions
//----------------------------------------------------------------------------------------------------------------------
bool OpcodeInfo::GetIsValidOpcode() const
{
	return _valid;
}

//----------------------------------------------------------------------------------------------------------------------
void OpcodeInfo::SetIsValidOpcode(bool state)
{
	_valid = state;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int OpcodeInfo::GetOpcodeSize() const
{
	return _opcodeSize;
}

//----------------------------------------------------------------------------------------------------------------------
void OpcodeInfo::SetOpcodeSize(unsigned int state)
{
	_opcodeSize = state;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> OpcodeInfo::GetOpcodeNameDisassembly() const
{
	return _disassemblyOpcode;
}

//----------------------------------------------------------------------------------------------------------------------
void OpcodeInfo::SetOpcodeNameDisassembly(const Marshal::In<std::wstring>& state)
{
	_disassemblyOpcode = state;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> OpcodeInfo::GetOpcodeArgumentsDisassembly() const
{
	return _disassemblyArguments;
}

//----------------------------------------------------------------------------------------------------------------------
void OpcodeInfo::SetOpcodeArgumentsDisassembly(const Marshal::In<std::wstring>& state)
{
	_disassemblyArguments = state;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> OpcodeInfo::GetDisassemblyComment() const
{
	return _disassemblyComment;
}

//----------------------------------------------------------------------------------------------------------------------
void OpcodeInfo::SetDisassemblyComment(const Marshal::In<std::wstring>& state)
{
	_disassemblyComment = state;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::set<unsigned int>> OpcodeInfo::GetLabelTargetLocations() const
{
	return _labelTargetLocations;
}

//----------------------------------------------------------------------------------------------------------------------
void OpcodeInfo::SetLabelTargetLocations(const Marshal::In<std::set<unsigned int>>& state)
{
	_labelTargetLocations = state;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::set<unsigned int>> OpcodeInfo::GetResultantPCLocations() const
{
	return _resultantPCLocations;
}

//----------------------------------------------------------------------------------------------------------------------
void OpcodeInfo::SetResultantPCLocations(const Marshal::In<std::set<unsigned int>>& state)
{
	_resultantPCLocations = state;
}

//----------------------------------------------------------------------------------------------------------------------
bool OpcodeInfo::GetHasUndeterminedResultantPCLocation() const
{
	return _undeterminedResultantPCLocation;
}

//----------------------------------------------------------------------------------------------------------------------
void OpcodeInfo::SetHasUndeterminedResultantPCLocation(bool state)
{
	_undeterminedResultantPCLocation = state;
}
