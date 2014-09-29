//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IViewPresenter::ViewTarget
{
	System,
	Module,
	Device,
	Extension
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IViewPresenter::ThisIViewPresenterVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//View target functions
//----------------------------------------------------------------------------------------
std::wstring IViewPresenter::GetViewTargetDeviceInstanceName() const
{
	std::wstring result;
	GetViewTargetDeviceInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IViewPresenter::GetViewTargetExtensionInstanceName() const
{
	std::wstring result;
	GetViewTargetExtensionInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
std::wstring IViewPresenter::GetViewGroupName() const
{
	std::wstring result;
	GetViewGroupNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IViewPresenter::GetViewName() const
{
	std::wstring result;
	GetViewNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
