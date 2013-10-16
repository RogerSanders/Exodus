//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IExtension::ThisIExtensionVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IDevice* target)
{
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IExtension* target)
{
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IBusInterface* target)
{
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IClockSource* target)
{
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
std::wstring IExtension::GetExtensionClassName() const
{
	std::wstring result;
	GetExtensionClassNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IExtension::GetExtensionInstanceName() const
{
	std::wstring result;
	GetExtensionInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void IExtension::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHierarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	RestoreViewModelStateInternal(InteropSupport::STLObjectSource<std::wstring>(viewModelGroupName), InteropSupport::STLObjectSource<std::wstring>(viewModelName), node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void IExtension::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	OpenViewModelInternal(InteropSupport::STLObjectSource<std::wstring>(viewModelGroupName), InteropSupport::STLObjectSource<std::wstring>(viewModelName), viewModelLauncher);
}
