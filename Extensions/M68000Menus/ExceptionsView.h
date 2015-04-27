#ifndef __EXCEPTIONSVIEW_H__
#define __EXCEPTIONSVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "ExceptionsViewPresenter.h"
#include "M68000/IM68000.h"
#include <list>

class ExceptionsView :public ViewBase
{
public:
	//Constructors
	ExceptionsView(IUIManager& auiManager, ExceptionsViewPresenter& apresenter, IM68000& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Exception functions
	void UpdateExceptionEntry(HWND hwnd, const IM68000::ExceptionDebuggingEntry& targetExceptionEntry);
	std::wstring BuildExceptionName(IM68000::Exceptions vectorNumber) const;

private:
	ExceptionsViewPresenter& presenter;
	IM68000& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	unsigned int addressBusCharWidth;
	int exceptionListIndex;
	IM68000::ExceptionDebuggingEntry exceptionEntry;
	std::list<IM68000::ExceptionDebuggingEntry> cachedExceptionList;
};

#endif
