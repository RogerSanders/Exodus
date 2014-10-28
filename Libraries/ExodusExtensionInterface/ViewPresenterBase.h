#ifndef __VIEWPRESENTERBASE_H__
#define __VIEWPRESENTERBASE_H__
#include "IViewPresenter.h"
#include "IView.h"
#include "AssemblyHandle.h"
#include "IUIManager.h"

class ViewPresenterBase :public IViewPresenter
{
public:
	//Constructors
	ViewPresenterBase(AssemblyHandle aassemblyHandle, const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID);
	ViewPresenterBase(AssemblyHandle aassemblyHandle, const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, unsigned int amoduleID, const std::wstring& amoduleDisplayName);
	ViewPresenterBase(AssemblyHandle aassemblyHandle, const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, const std::wstring& adeviceInstanceName, unsigned int amoduleID, const std::wstring& amoduleDisplayName);
	ViewPresenterBase(AssemblyHandle aassemblyHandle, const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, const std::wstring& aextensionInstanceName, bool aglobalExtension, unsigned int amoduleID = 0, const std::wstring& amoduleDisplayName = L"");

	//Interface version functions
	virtual unsigned int GetIViewPresenterVersion() const;

	//Assembly handle functions
	virtual AssemblyHandle GetAssemblyHandle() const;

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager) = 0;
	virtual void DeleteView(IView* aview) = 0;

	//View management functions
	virtual bool OpenView(IUIManager& uiManager, IViewStateChangeNotifier* anotifier, IHierarchicalStorageNode* viewState);
	virtual void CloseView();
	virtual void ShowView();
	virtual void HideView();
	virtual void ActivateView();
	virtual void NotifyViewClosed(IView* aview);

	//View target functions
	virtual ViewTarget GetViewTarget() const;
	MarshalSupport::Marshal::Ret<std::wstring> GetViewTargetDeviceInstanceName() const;
	MarshalSupport::Marshal::Ret<std::wstring> GetViewTargetExtensionInstanceName() const;
	virtual bool GetViewTargetGlobalExtension() const;
	virtual unsigned int GetViewTargetModuleID() const;

	//Notifiers
	virtual void NotifyViewOpened();
	virtual void NotifyViewClosed();

	//State functions
	virtual int GetViewID() const;
	MarshalSupport::Marshal::Ret<std::wstring> GetViewGroupName() const;
	MarshalSupport::Marshal::Ret<std::wstring> GetViewName() const;
	virtual bool LoadViewState(IHierarchicalStorageNode& viewState);
	virtual bool SaveViewState(IHierarchicalStorageNode& viewState) const;

protected:
	//View management functions
	bool IsViewOpen() const;
	IView* GetOpenView() const;

private:
	bool viewOpen;
	IViewStateChangeNotifier* notifier;
	IView* view;
	int viewID;
	AssemblyHandle assemblyHandle;
	ViewTarget viewTarget;
	std::wstring viewTargetDeviceInstanceName;
	std::wstring viewTargetExtensionInstanceName;
	bool viewTargetGlobalExtension;
	std::wstring viewTargetModuleDisplayName;
	unsigned int viewTargetModuleID;
	std::wstring viewGroupName;
	std::wstring viewName;
};

#endif
