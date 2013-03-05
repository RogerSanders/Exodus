#include "SpriteListDetailsViewModel.h"
#include "SpriteListDetailsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::SpriteListDetailsViewModel::SpriteListDetailsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, S315_5313* adevice, unsigned int aspriteIndex)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{
	spriteIndex = aspriteIndex;
}

//----------------------------------------------------------------------------------------
//Sprite index functions
//----------------------------------------------------------------------------------------
void S315_5313::SpriteListDetailsViewModel::SetSpriteIndex(unsigned int aspriteIndex)
{
	spriteIndex = aspriteIndex;
	IView* viewBase = GetOpenView();
	if(viewBase != 0)
	{
		SpriteListDetailsView* view = dynamic_cast<SpriteListDetailsView*>(viewBase);
		if(view != 0)
		{
			view->SetSpriteIndex(spriteIndex);
		}
	}
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* S315_5313::SpriteListDetailsViewModel::CreateView()
{
	return new SpriteListDetailsView(device, spriteIndex);
}

//----------------------------------------------------------------------------------------
void S315_5313::SpriteListDetailsViewModel::DeleteView(IView* aview)
{
	delete aview;
}
