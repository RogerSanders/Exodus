#ifndef __IVIEWMODELMANAGER_H__
#define __IVIEWMODELMANAGER_H__
#include "SystemInterface/SystemInterface.pkg"

class IViewModelManager :public IViewModelLauncher
{
public:
	//View management functions
	virtual void NotifyModelViewClosed(IViewModel* aviewModel) = 0;
};

#endif
