#ifndef __EMBEDDEDROMVIEWPRESENTER_H__
#define __EMBEDDEDROMVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ExodusSystemMenus.h"
#include "SystemInterface/SystemInterface.pkg"

class EmbeddedROMViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	EmbeddedROMViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusSystemMenus& owner, ISystemGUIInterface& model);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

	//Interface functions
	IGUIExtensionInterface& GetGUIInterface() const;

private:
	ExodusSystemMenus& _owner;
	ISystemGUIInterface& _model;
};

#endif
