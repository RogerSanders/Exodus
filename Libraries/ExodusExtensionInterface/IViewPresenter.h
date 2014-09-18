#ifndef __IVIEWPRESENTER_H__
#define __IVIEWPRESENTER_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"
#include "InteropSupport/InteropSupport.pkg"
#include "IViewStateChangeNotifier.h"
#include "AssemblyHandle.h"
#include <string>
class IView;
class IUIManager;

class IViewPresenter
{
public:
	//Enumerations
	enum ViewTarget;

public:
	//Constructors
	virtual ~IViewPresenter() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIViewPresenterVersion();
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
	inline std::wstring GetViewTargetDeviceInstanceName() const;
	inline std::wstring GetViewTargetExtensionInstanceName() const;
	virtual bool GetViewTargetGlobalExtension() const = 0;
	virtual unsigned int GetViewTargetModuleID() const = 0;

	//State functions
	virtual int GetViewID() const = 0;
	inline std::wstring GetViewGroupName() const;
	inline std::wstring GetViewName() const;
	virtual bool LoadViewState(IHierarchicalStorageNode& viewState) = 0;
	virtual bool SaveViewState(IHierarchicalStorageNode& viewState) const = 0;

protected:
	//View target functions
	virtual void GetViewTargetDeviceInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetViewTargetExtensionInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//State functions
	virtual void GetViewGroupNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetViewNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IViewPresenter.inl"
#endif
