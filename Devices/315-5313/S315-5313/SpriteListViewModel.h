#include "S315_5313.h"
#ifndef __S315_5313_SPRITELISTVIEWMODEL_H__
#define __S315_5313_SPRITELISTVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::SpriteListViewModel :public ViewModelBase
{
public:
	//Constructors
	SpriteListViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, S315_5313* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	S315_5313* device;
};

#endif
