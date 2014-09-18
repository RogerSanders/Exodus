//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Extension::Extension(const std::wstring& aclassName, const std::wstring& ainstanceName, unsigned int amoduleID)
:className(aclassName), instanceName(ainstanceName), moduleID(amoduleID), assemblyHandle(0), systemInterface(0), guiInterface(0), viewManager(0)
{}

//----------------------------------------------------------------------------------------
//Interface functions
//----------------------------------------------------------------------------------------
ISystemExtensionInterface& Extension::GetSystemInterface() const
{
	return *systemInterface;
}

//----------------------------------------------------------------------------------------
IGUIExtensionInterface& Extension::GetGUIInterface() const
{
	return *guiInterface;
}

//----------------------------------------------------------------------------------------
IViewManager& Extension::GetViewManager() const
{
	return guiInterface->GetViewManager();
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring Extension::GetExtensionClassName() const
{
	return className;
}

//----------------------------------------------------------------------------------------
std::wstring Extension::GetExtensionInstanceName() const
{
	return instanceName;
}
