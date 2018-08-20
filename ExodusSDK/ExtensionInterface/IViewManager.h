#ifndef __IVIEWMANAGER_H__
#define __IVIEWMANAGER_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"
#include "IViewPresenter.h"

class IViewManager
{
public:
	// Constructors
	inline virtual ~IViewManager() = 0;

	// Interface version functions
	static inline unsigned int ThisIViewManagerVersion() { return 1; }
	virtual unsigned int GetIViewManagerVersion() const = 0;

	// View management functions
	virtual bool OpenView(IViewPresenter& viewPresenter, bool waitToClose = true) = 0;
	virtual bool OpenView(IViewPresenter& viewPresenter, IHierarchicalStorageNode& viewState, bool waitToClose = true) = 0;
	virtual void CloseView(IViewPresenter& viewPresenter, bool waitToClose = true) = 0;
	virtual void ShowView(IViewPresenter& viewPresenter) = 0;
	virtual void HideView(IViewPresenter& viewPresenter) = 0;
	virtual void ActivateView(IViewPresenter& viewPresenter) = 0;
	virtual bool WaitUntilViewOpened(IViewPresenter& viewPresenter) = 0;
	virtual void WaitUntilViewClosed(IViewPresenter& viewPresenter) = 0;
};
IViewManager::~IViewManager() { }

#endif
