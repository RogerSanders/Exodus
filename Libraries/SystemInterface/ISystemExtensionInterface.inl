//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ISystemExtensionInterface::ThisISystemExtensionInterfaceVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Loaded module info functions
//----------------------------------------------------------------------------------------
std::list<unsigned int> ISystemExtensionInterface::GetLoadedModuleIDs() const
{
	std::list<unsigned int> result;
	GetLoadedModuleIDsInternal(InteropSupport::STLObjectTarget<std::list<unsigned int>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::list<unsigned int> ISystemExtensionInterface::GetConnectorIDs() const
{
	std::list<unsigned int> result;
	GetConnectorIDsInternal(InteropSupport::STLObjectTarget<std::list<unsigned int>>(result));
	return result;
}
