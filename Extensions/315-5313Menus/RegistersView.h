#ifndef __REGISTERSVIEW_H__
#define __REGISTERSVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "RegistersViewPresenter.h"
#include "315-5313/IS315_5313.h"

class RegistersView :public ViewBase
{
public:
	//Constructors
	RegistersView(IUIManager& auiManager, RegistersViewPresenter& apresenter, IS315_5313& amodel);
	~RegistersView();

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_NOTIFY(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Raw registers dialog window procedure
	static INT_PTR CALLBACK WndProcRawRegistersStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcRawRegisters(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Raw registers dialog event handlers
	INT_PTR msgRawRegistersWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgRawRegistersWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgRawRegistersWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgRawRegistersWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgRawRegistersWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgRawRegistersWM_CTLCOLOREDIT(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Mode registers dialog window procedure
	static INT_PTR CALLBACK WndProcModeRegistersStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcModeRegisters(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Mode registers dialog event handlers
	INT_PTR msgModeRegistersWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgModeRegistersWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgModeRegistersWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgModeRegistersWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgModeRegistersWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Other registers dialog window procedure
	static INT_PTR CALLBACK WndProcOtherRegistersStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcOtherRegisters(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Other registers dialog event handlers
	INT_PTR msgOtherRegistersWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgOtherRegistersWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgOtherRegistersWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgOtherRegistersWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgOtherRegistersWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgOtherRegistersWM_CTLCOLOREDIT(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Register locking functions
	bool RawRegistersControlIDToDataID(int controlID, unsigned int& genericDataID, IS315_5313::RegisterDataContext& registerDataContext, const IGenericAccess::DataContext** dataContext);
	bool ModeRegistersControlIDToDataID(int controlID, unsigned int& genericDataID);
	bool OtherRegistersControlIDToDataID(int controlID, unsigned int& genericDataID);

private:
	//Structures
	struct TabInfo
	{
		TabInfo(const std::wstring& atabName, int adialogID, DLGPROC adialogProc)
		:tabName(atabName), dialogID(adialogID), dialogProc(adialogProc), hwndDialog(NULL)
		{}

		std::wstring tabName;
		int dialogID;
		DLGPROC dialogProc;
		HWND hwndDialog;
	};

private:
	RegistersViewPresenter& presenter;
	IS315_5313& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	COLORREF lockedColor;
	HBRUSH lockedBrush;
	HWND activeTabWindow;
	std::vector<TabInfo> tabItems;
};

#endif
