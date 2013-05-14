#include "S315_5313.h"
#ifndef __S315_5313_REGISTERSVIEW_H__
#define __S315_5313_REGISTERSVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::RegistersView :public ViewBase
{
public:
	//Constructors
	RegistersView(S315_5313* adevice);
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

	//Other registers dialog window procedure
	static INT_PTR CALLBACK WndProcOtherRegistersStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcOtherRegisters(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Other registers dialog event handlers
	INT_PTR msgOtherRegistersWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgOtherRegistersWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgOtherRegistersWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgOtherRegistersWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Register locking functions
	unsigned int ControlIDToLockedRegKey(int controlID);
	void ToggleRegisterLock(int controlID);

private:
	//Structures
	struct TabInfo
	{
		TabInfo(const std::wstring& atabName, int adialogID, DLGPROC adialogProc)
		:tabName(atabName), dialogID(adialogID), dialogProc(adialogProc)
		{}

		std::wstring tabName;
		int dialogID;
		DLGPROC dialogProc;
	};

private:
	S315_5313* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	COLORREF lockedColor;
	HBRUSH lockedBrush;
	HWND activeTabDialog;
	std::vector<TabInfo> tabItems;
};

#endif
