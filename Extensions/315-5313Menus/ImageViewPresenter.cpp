#include "ImageViewPresenter.h"
#include "ImageView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ImageViewPresenter::ImageViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring ImageViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Image Window";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* ImageViewPresenter::CreateView(IUIManager& uiManager)
{
	return new ImageView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void ImageViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//System interface functions
//----------------------------------------------------------------------------------------
ISystemExtensionInterface& ImageViewPresenter::GetSystemInterface() const
{
	return owner.GetSystemInterface();
}
