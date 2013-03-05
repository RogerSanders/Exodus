#include "SpriteListDetailsViewModel.h"
#include "SpriteListDetailsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::SpriteListDetailsViewModel::SpriteListDetailsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, VDP* adevice, unsigned int aspriteMappingAddress, unsigned int aspriteIndex)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{
	spriteMappingAddress = aspriteMappingAddress;
	spriteIndex = aspriteIndex;
}

//----------------------------------------------------------------------------------------
//Sprite address functions
//----------------------------------------------------------------------------------------
void VDP::SpriteListDetailsViewModel::SetSpriteAddress(unsigned int aspriteMappingAddress, unsigned int aspriteIndex)
{
	spriteMappingAddress = aspriteMappingAddress;
	spriteIndex = aspriteIndex;
	IView* viewBase = GetOpenView();
	if(viewBase != 0)
	{
		SpriteListDetailsView* view = dynamic_cast<SpriteListDetailsView*>(viewBase);
		if(view != 0)
		{
			view->SetSpriteAddress(spriteMappingAddress, spriteIndex);
		}
	}
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* VDP::SpriteListDetailsViewModel::CreateView()
{
	return new SpriteListDetailsView(device, spriteMappingAddress, spriteIndex);
}

//----------------------------------------------------------------------------------------
void VDP::SpriteListDetailsViewModel::DeleteView(IView* aview)
{
	delete aview;
}
