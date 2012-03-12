//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Device::Device(const std::wstring& aclassName, const std::wstring& ainstanceName, unsigned int amoduleID)
:className(aclassName), instanceName(ainstanceName), moduleID(amoduleID), deviceContext(0), assemblyHandle(0)
{}
