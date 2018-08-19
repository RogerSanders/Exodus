#ifndef __INPUTMAPPINGDETAILSVIEW_H__
#define __INPUTMAPPINGDETAILSVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "InputMappingDetailsViewPresenter.h"
#include <mutex>
#include <condition_variable>

class InputMappingDetailsView :public ViewBase
{
public:
	//Constructors
	InputMappingDetailsView(IUIManager& uiManager, InputMappingDetailsViewPresenter& presenter, ISystemGUIInterface& model, IDevice* targetDevice);

	//Target device functions
	void SetTargetDevice(IDevice* targetDevice);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_USER(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Target device update functions
	void UpdateTargetDeviceInputMappingsDisplay(HWND hwnd);
	void UpdateSelectedInputRegistration(HWND hwnd);

	//Joystick functions
	void JoystickKeyMappingWorkerThread();

private:
	InputMappingDetailsViewPresenter& _presenter;
	ISystemGUIInterface& _model;
	bool _initializedDialog;
	bool _systemKeyInputFieldInFocus;
	HWND _hwndInternal;
	IDevice* _targetDevice;
	std::vector<unsigned int> _deviceKeyCodeMap;
	int _selectedInputRegistration;
	volatile bool _autoKeyMappingActive;
	mutable std::mutex _joystickWorkerThreadMutex;
	volatile bool _joystickWorkerThreadActive;
	std::condition_variable _joystickWorkerThreadStopped;
};

#endif
