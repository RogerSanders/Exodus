#include "System.h"
#ifndef __SYSTEM_INPUTMAPPINGDETAILSVIEWMODEL_H__
#define __SYSTEM_INPUTMAPPINGDETAILSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class System::InputMappingDetailsViewModel :public ViewModelBase
{
public:
	//Constructors
	InputMappingDetailsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, System* adevice, IDevice* atargetDevice);

	//Target device functions
	void SetTargetDevice(IDevice* atargetDevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	System* device;
	IDevice* targetDevice;
};

#endif
