#ifndef __SPRITELISTDETAILSVIEWPRESENTER_H__
#define __SPRITELISTDETAILSVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "S315_5313Menus.h"
#include "315-5313/IS315_5313.h"

class SpriteListDetailsViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	SpriteListDetailsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel, unsigned int aspriteIndex = 0);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

	//Sprite index functions
	void SetSpriteIndex(unsigned int aspriteIndex);

private:
	S315_5313Menus& owner;
	const IDevice& modelInstanceKey;
	IS315_5313& model;
	unsigned int spriteIndex;
};

#endif
