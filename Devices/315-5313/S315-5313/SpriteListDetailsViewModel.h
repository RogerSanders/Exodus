#include "S315_5313.h"
#ifndef __S315_5313_SPRITELISTDETAILSVIEWMODEL_H__
#define __S315_5313_SPRITELISTDETAILSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::SpriteListDetailsViewModel :public ViewModelBase
{
public:
	//Constructors
	SpriteListDetailsViewModel(const std::wstring& amenuHandlerName, int aviewModelID, S315_5313* adevice, unsigned int aspriteIndex = 0);

	//Sprite index functions
	void SetSpriteIndex(unsigned int aspriteIndex);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	S315_5313* device;
	unsigned int spriteIndex;
};

#endif
