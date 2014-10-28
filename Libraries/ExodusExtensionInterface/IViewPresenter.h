#ifndef __IVIEWPRESENTER_H__
#define __IVIEWPRESENTER_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include "IViewStateChangeNotifier.h"
#include "AssemblyHandle.h"
#include <string>
class IView;
class IUIManager;

class IViewPresenter
{
public:
	//Enumerations
	enum class ViewTarget;

public:
	//Constructors
	virtual ~IViewPresenter() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIViewPresenterVersion() { return 1; }
	virtual unsigned int GetIViewPresenterVersion() const = 0;

	//Assembly handle functions
	virtual AssemblyHandle GetAssemblyHandle() const = 0;

	//View management functions
	virtual bool OpenView(IUIManager& uiManager, IViewStateChangeNotifier* anotifier, IHierarchicalStorageNode* viewState) = 0;
	virtual void CloseView() = 0;
	virtual void ShowView() = 0;
	virtual void HideView() = 0;
	virtual void ActivateView() = 0;
	virtual void NotifyViewClosed(IView* aview) = 0;

	//View target functions
	virtual ViewTarget GetViewTarget() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetViewTargetDeviceInstanceName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetViewTargetExtensionInstanceName() const = 0;
	virtual bool GetViewTargetGlobalExtension() const = 0;
	virtual unsigned int GetViewTargetModuleID() const = 0;

	//State functions
	virtual int GetViewID() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetViewGroupName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetViewName() const = 0;
	virtual bool LoadViewState(IHierarchicalStorageNode& viewState) = 0;
	virtual bool SaveViewState(IHierarchicalStorageNode& viewState) const = 0;
};

#include "IViewPresenter.inl"
#endif
