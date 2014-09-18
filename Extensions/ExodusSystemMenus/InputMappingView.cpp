#include "InputMappingView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
InputMappingView::InputMappingView(IUIManager& auiManager, InputMappingViewPresenter& apresenter, ISystemGUIInterface& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	inputDeviceListLastModifiedToken = 0;
	selectedInputDevice = 0;
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_INPUTMAPPING));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR InputMappingView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR InputMappingView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR InputMappingView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR InputMappingView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//If the input device list hasn't been modified, abort any further processing.
	unsigned int newInputDeviceListLastModifiedToken = model.GetInputDeviceListLastModifiedToken();
	if(inputDeviceListLastModifiedToken == newInputDeviceListLastModifiedToken)
	{
		return TRUE;
	}

	//Obtain the current set of input devices
	std::list<IDevice*> inputDeviceList = model.GetInputDeviceList();

	//Disable visual updates to the input device list
	SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), WM_SETREDRAW, FALSE, 0);

	//Update the embedded ROM list
	bool foundCurrentlySelectedInputDevice = false;
	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_GETTOPINDEX, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_RESETCONTENT, 0, NULL);
	for(std::list<IDevice*>::const_iterator i = inputDeviceList.begin(); i != inputDeviceList.end(); ++i)
	{
		//Attempt to retrieve the display name of the next input device
		IDevice* inputDevice = *i;
		std::wstring deviceDisplayName;
		if(!model.GetFullyQualifiedDeviceDisplayName(inputDevice, deviceDisplayName))
		{
			continue;
		}

		//Add an entry for this input device to the list
		LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_ADDSTRING, 0, (LPARAM)deviceDisplayName.c_str());
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)inputDevice);

		//If this input device was the previously selected input device, record its index
		//in the list.
		if(inputDevice == selectedInputDevice)
		{
			SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_SETCURSEL, newItemIndex, 0);
			foundCurrentlySelectedInputDevice = true;
		}
	}
	SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_SETTOPINDEX, top, 0);

	//If the previously selected input device was not found, clear the current selection
	//in the list.
	if(!foundCurrentlySelectedInputDevice)
	{
		selectedInputDevice = 0;
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_SETCURSEL, (WPARAM)-1, 0);
	}

	//Enable visual updates to the input device list, and trigger a refresh.
	SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), NULL, FALSE);

	//Save the new last modified token as the current last modified token
	inputDeviceListLastModifiedToken = newInputDeviceListLastModifiedToken;

	//If we currently have an open details view looking at input mappings for a particular
	//device, ensure that device still exists, and close the details view if it does not,
	//since it will now have a pointer to a device object that has been destroyed.
	//##FIX## This isn't entirely safe. System windows such as this one need a hook into
	//the module unload process, where they can be notified before a device is unloaded,
	//and have a chance to fully process the event before the device is removed from the
	//model. Windows such as our input mapping details view must subscribe to this
	//message, and ensure they clean up any references to the device which is being
	//removed when the message is received.
	if(!foundCurrentlySelectedInputDevice)
	{
		presenter.CloseInputMappingDetailsView();
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR InputMappingView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == LBN_DBLCLK)
	{
		//Display the input mapping details dialog for the selected input device
		switch(LOWORD(wparam))
		{
		case IDC_INPUTMAPPING_LIST:{
			LRESULT getCurrentSelectionResult = SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_GETCURSEL, 0, 0);
			if(getCurrentSelectionResult != LB_ERR)
			{
				int currentItemListIndex = (int)getCurrentSelectionResult;
				selectedInputDevice = (IDevice*)SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_GETITEMDATA, currentItemListIndex, NULL);
				presenter.OpenInputMappingDetailsView(selectedInputDevice);
			}
			break;}
		}
	}

	return TRUE;
}
