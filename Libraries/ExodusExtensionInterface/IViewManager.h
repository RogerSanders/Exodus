#ifndef __IVIEWMANAGER_H__
#define __IVIEWMANAGER_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"
#include "IViewPresenter.h"

class IViewManager
{
public:
	//Constructors
	virtual ~IViewManager() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIViewManagerVersion();
	virtual unsigned int GetIViewManagerVersion() const = 0;

	//View management functions
	virtual bool OpenView(IViewPresenter& aviewPresenter, bool waitToClose = true) = 0;
	virtual bool OpenView(IViewPresenter& aviewPresenter, IHierarchicalStorageNode& viewState, bool waitToClose = true) = 0;
	virtual void CloseView(IViewPresenter& aviewPresenter, bool waitToClose = true) = 0;
	virtual void ShowView(IViewPresenter& aviewPresenter) = 0;
	virtual void HideView(IViewPresenter& aviewPresenter) = 0;
	virtual void ActivateView(IViewPresenter& aviewPresenter) = 0;
	virtual bool WaitUntilViewOpened(IViewPresenter& aviewPresenter) = 0;
	virtual void WaitUntilViewClosed(IViewPresenter& aviewPresenter) = 0;
};

#include "IViewManager.inl"
#endif
