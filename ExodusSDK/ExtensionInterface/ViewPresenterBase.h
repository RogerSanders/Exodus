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
	ViewPresenterBase(AssemblyHandle assemblyHandle, const std::wstring& viewGroupName, const std::wstring& viewName, int viewID);
	ViewPresenterBase(AssemblyHandle assemblyHandle, const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, unsigned int moduleID, const std::wstring& moduleDisplayName);
	ViewPresenterBase(AssemblyHandle assemblyHandle, const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, const std::wstring& deviceInstanceName, unsigned int moduleID, const std::wstring& moduleDisplayName);
	ViewPresenterBase(AssemblyHandle assemblyHandle, const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, const std::wstring& extensionInstanceName, bool globalExtension, unsigned int moduleID = 0, const std::wstring& moduleDisplayName = L"");

	//Interface version functions
	virtual unsigned int GetIViewPresenterVersion() const;

	//Assembly handle functions
	virtual AssemblyHandle GetAssemblyHandle() const;

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager) = 0;
	virtual void DeleteView(IView* view) = 0;

	//View management functions
	virtual bool OpenView(IUIManager& uiManager, IViewStateChangeNotifier* notifier, IHierarchicalStorageNode* viewState);
	virtual void CloseView();
	virtual void ShowView();
	virtual void HideView();
	virtual void ActivateView();
	virtual void NotifyViewClosed(IView* view);

	//View target functions
	virtual ViewTarget GetViewTarget() const;
	Marshal::Ret<std::wstring> GetViewTargetDeviceInstanceName() const;
	Marshal::Ret<std::wstring> GetViewTargetExtensionInstanceName() const;
	virtual bool GetViewTargetGlobalExtension() const;
	virtual unsigned int GetViewTargetModuleID() const;

	//Notifiers
	virtual void NotifyViewOpened();
	virtual void NotifyViewClosed();

	//State functions
	virtual int GetViewID() const;
	Marshal::Ret<std::wstring> GetViewGroupName() const;
	Marshal::Ret<std::wstring> GetViewName() const;
	virtual bool LoadViewState(IHierarchicalStorageNode& viewState);
	virtual bool SaveViewState(IHierarchicalStorageNode& viewState) const;

protected:
	//View management functions
	bool IsViewOpen() const;
	IView* GetOpenView() const;

private:
	bool _viewOpen;
	IViewStateChangeNotifier* _notifier;
	IView* _view;
	int _viewID;
	AssemblyHandle _assemblyHandle;
	ViewTarget _viewTarget;
	std::wstring _viewTargetDeviceInstanceName;
	std::wstring _viewTargetExtensionInstanceName;
	bool _viewTargetGlobalExtension;
	std::wstring _viewTargetModuleDisplayName;
	unsigned int _viewTargetModuleID;
	std::wstring _viewGroupName;
	std::wstring _viewName;
};

#endif
