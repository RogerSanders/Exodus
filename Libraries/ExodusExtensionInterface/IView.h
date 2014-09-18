#ifndef __IVIEW_H__
#define __IVIEW_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"
#include "InteropSupport/InteropSupport.pkg"
#include <string>
class IViewPresenter;
class IUIManager;

class IView
{
public:
	//Enumerations
	enum InitialDockPos;
	enum InitialDialogPos;
	enum ViewType;
	enum DialogMode;

public:
	//Constructors
	virtual ~IView() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIViewVersion();
	virtual unsigned int GetIViewVersion() const = 0;

	//View management functions
	virtual bool OpenView(IHierarchicalStorageNode* viewState) = 0;
	virtual void CloseView() = 0;
	virtual void ShowView() = 0;
	virtual void HideView() = 0;
	virtual void ActivateView() = 0;

	//State functions
	virtual bool LoadViewState(IHierarchicalStorageNode& viewState) = 0;
	virtual bool SaveViewState(IHierarchicalStorageNode& viewState) const = 0;

	//New window state
	inline std::wstring GetViewDockingGroup() const;
	virtual bool IsViewInitiallyDocked() const = 0;
	virtual bool IsViewInitiallyCollapsed() const = 0;
	virtual InitialDockPos GetViewInitialDockPosition() const = 0;
	virtual ViewType GetViewType() const = 0;
	virtual DialogMode GetViewDialogMode() const = 0;
	virtual InitialDialogPos GetViewInitialDialogPosition() const = 0;
	virtual bool CanResizeDialog() const = 0;

protected:
	//New window state
	virtual void GetViewDockingGroupInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IView.inl"
#endif
