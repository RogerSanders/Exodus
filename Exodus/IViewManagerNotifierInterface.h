#ifndef __IVIEWMANAGERNOTIFIERINTERFACE_H__
#define __IVIEWMANAGERNOTIFIERINTERFACE_H__
#include "ExtensionInterface/ExtensionInterface.pkg"

class IViewManagerNotifierInterface :public IViewManager
{
public:
	// View management functions
	virtual void NotifyViewClosed(IViewPresenter& viewPresenter) = 0;
};

#endif
