//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Extension::Extension(const std::wstring& className, const std::wstring& instanceName, unsigned int moduleID)
:_className(className), _instanceName(instanceName), _moduleID(moduleID), _assemblyHandle(0), _systemInterface(0), _guiInterface(0), _viewManager(0)
{}

//----------------------------------------------------------------------------------------
//Interface functions
//----------------------------------------------------------------------------------------
ISystemExtensionInterface& Extension::GetSystemInterface() const
{
	return *_systemInterface;
}

//----------------------------------------------------------------------------------------
IGUIExtensionInterface& Extension::GetGUIInterface() const
{
	return *_guiInterface;
}

//----------------------------------------------------------------------------------------
IViewManager& Extension::GetViewManager() const
{
	return _guiInterface->GetViewManager();
}
