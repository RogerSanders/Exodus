#include "OpcodeInfo.h"

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
void OpcodeInfo::GetOpcodeNameDisassemblyInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetOpcodeNameDisassembly());
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetOpcodeNameDisassemblyInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetOpcodeNameDisassembly(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::GetOpcodeArgumentsDisassemblyInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetOpcodeArgumentsDisassembly());
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetOpcodeArgumentsDisassemblyInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetOpcodeArgumentsDisassembly(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::GetDisassemblyCommentInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetDisassemblyComment());
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetDisassemblyCommentInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetDisassemblyComment(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::GetLabelTargetLocationsInternal(const InteropSupport::ISTLObjectTarget<std::set<unsigned int>>& marshaller) const
{
	marshaller.MarshalFrom(GetLabelTargetLocations());
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetLabelTargetLocationsInternal(const InteropSupport::ISTLObjectSource<std::set<unsigned int>>& marshaller)
{
	SetLabelTargetLocations(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::GetResultantPCLocationsInternal(const InteropSupport::ISTLObjectTarget<std::set<unsigned int>>& marshaller) const
{
	marshaller.MarshalFrom(GetResultantPCLocations());
}

//----------------------------------------------------------------------------------------
void OpcodeInfo::SetResultantPCLocationsInternal(const InteropSupport::ISTLObjectSource<std::set<unsigned int>>& marshaller)
{
	SetResultantPCLocations(marshaller.MarshalTo());
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
