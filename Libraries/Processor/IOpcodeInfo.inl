//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IOpcodeInfo::ThisIOpcodeInfoVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Opcode info functions
//----------------------------------------------------------------------------------------
std::wstring IOpcodeInfo::GetOpcodeNameDisassembly() const
{
	std::wstring result;
	GetOpcodeNameDisassemblyInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IOpcodeInfo::SetOpcodeNameDisassembly(const std::wstring& state)
{
	return SetOpcodeNameDisassemblyInternal(InteropSupport::STLObjectSource<std::wstring>(state));
}

//----------------------------------------------------------------------------------------
std::wstring IOpcodeInfo::GetOpcodeArgumentsDisassembly() const
{
	std::wstring result;
	GetOpcodeArgumentsDisassemblyInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IOpcodeInfo::SetOpcodeArgumentsDisassembly(const std::wstring& state)
{
	return SetOpcodeArgumentsDisassemblyInternal(InteropSupport::STLObjectSource<std::wstring>(state));
}

//----------------------------------------------------------------------------------------
std::wstring IOpcodeInfo::GetDisassemblyComment() const
{
	std::wstring result;
	GetDisassemblyCommentInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IOpcodeInfo::SetDisassemblyComment(const std::wstring& state)
{
	return SetDisassemblyCommentInternal(InteropSupport::STLObjectSource<std::wstring>(state));
}

//----------------------------------------------------------------------------------------
std::set<unsigned int> IOpcodeInfo::GetLabelTargetLocations() const
{
	std::set<unsigned int> result;
	GetLabelTargetLocationsInternal(InteropSupport::STLObjectTarget<std::set<unsigned int>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IOpcodeInfo::SetLabelTargetLocations(const std::set<unsigned int>& state)
{
	return SetLabelTargetLocationsInternal(InteropSupport::STLObjectSource<std::set<unsigned int>>(state));
}

//----------------------------------------------------------------------------------------
std::set<unsigned int> IOpcodeInfo::GetResultantPCLocations() const
{
	std::set<unsigned int> result;
	GetResultantPCLocationsInternal(InteropSupport::STLObjectTarget<std::set<unsigned int>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IOpcodeInfo::SetResultantPCLocations(const std::set<unsigned int>& state)
{
	return SetResultantPCLocationsInternal(InteropSupport::STLObjectSource<std::set<unsigned int>>(state));
}
