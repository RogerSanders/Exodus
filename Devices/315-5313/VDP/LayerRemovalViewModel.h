#include "VDP.h"
#ifndef __VDP_LAYERREMOVALVIEWMODEL_H__
#define __VDP_LAYERREMOVALVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class VDP::LayerRemovalViewModel :public ViewModelBase
{
public:
	//Constructors
	LayerRemovalViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, VDP* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	VDP* device;
};

#endif
