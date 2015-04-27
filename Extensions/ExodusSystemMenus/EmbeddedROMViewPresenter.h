#ifndef __EMBEDDEDROMVIEWPRESENTER_H__
#define __EMBEDDEDROMVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ExodusSystemMenus.h"
#include "SystemInterface/SystemInterface.pkg"

class EmbeddedROMViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	EmbeddedROMViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

	//Interface functions
	IGUIExtensionInterface& GetGUIInterface() const;

private:
	ExodusSystemMenus& owner;
	ISystemGUIInterface& model;
};

#endif
