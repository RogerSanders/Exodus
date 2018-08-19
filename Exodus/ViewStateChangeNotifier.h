#ifndef __VIEWSTATECHANGENOTIFIER_H__
#define __VIEWSTATECHANGENOTIFIER_H__
#include "ExtensionInterface/ExtensionInterface.pkg"
#include "IViewManagerNotifierInterface.h"

class ViewStateChangeNotifier :public IViewStateChangeNotifier
{
public:
	// Constructors
	ViewStateChangeNotifier(IViewManagerNotifierInterface& viewManager, IViewPresenter& viewPresenter);

	// Interface version functions
	virtual unsigned int GetIViewStateChangeNotifierVersion() const;

	// Notification functions
	virtual void NotifyViewClosed();

private:
	IViewPresenter& _viewPresenter;
	IViewManagerNotifierInterface& _viewManager;
};

#endif
