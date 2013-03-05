#include "VDP.h"
#ifndef __VDP_SPRITELISTDETAILSVIEWMODEL_H__
#define __VDP_SPRITELISTDETAILSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class VDP::SpriteListDetailsViewModel :public ViewModelBase
{
public:
	//Constructors
	SpriteListDetailsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, VDP* adevice, unsigned int aspriteMappingAddress = 0, unsigned int aspriteIndex = 0);

	//Sprite address functions
	void SetSpriteAddress(unsigned int aspriteMappingAddress, unsigned int aspriteIndex);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	VDP* device;
	unsigned int spriteMappingAddress;
	unsigned int spriteIndex;
};

#endif
