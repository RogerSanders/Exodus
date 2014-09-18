#ifndef __VIEWSTATECHANGENOTIFIER_H__
#define __VIEWSTATECHANGENOTIFIER_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include "IViewManagerNotifierInterface.h"

class ViewStateChangeNotifier :public IViewStateChangeNotifier
{
public:
	//Constructors
	ViewStateChangeNotifier(IViewManagerNotifierInterface& aviewManager, IViewPresenter& aviewPresenter);

	//Interface version functions
	virtual unsigned int GetIViewStateChangeNotifierVersion() const;

	//Notification functions
	virtual void NotifyViewClosed();

private:
	IViewPresenter& viewPresenter;
	IViewManagerNotifierInterface& viewManager;
};

#endif
