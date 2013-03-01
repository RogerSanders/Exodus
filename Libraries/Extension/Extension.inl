//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Extension::Extension(const std::wstring& aclassName, const std::wstring& ainstanceName, unsigned int amoduleID)
:className(aclassName), instanceName(ainstanceName), moduleID(amoduleID), assemblyHandle(0)
{}

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
