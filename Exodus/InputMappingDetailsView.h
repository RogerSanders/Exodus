#include "System.h"
#ifndef __SYSTEM_INPUTMAPPINGDETAILSVIEW_H__
#define __SYSTEM_INPUTMAPPINGDETAILSVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class System::InputMappingDetailsView :public ViewBase
{
public:
	//Constructors
	InputMappingDetailsView(System* asystem, IDevice* atargetDevice);

	//Target device functions
	void SetTargetDevice(IDevice* atargetDevice);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Target device update functions
	void UpdateTargetDeviceInputMappingsDisplay(HWND hwnd);
	void UpdateSelectedInputRegistration(HWND hwnd);

	//Input selection functions
	void SetSystemKeySelectionForDeviceKey(unsigned int inputRegistrationNo, IDeviceContext::KeyCode systemKeyCode);

private:
	System* system;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	HWND hwndInternal;
	IDevice* targetDevice;
	std::vector<InputRegistration> inputRegistrationsForDevice;
	int selectedInputRegistration;
	bool autoKeyMappingActive;
};

#endif
