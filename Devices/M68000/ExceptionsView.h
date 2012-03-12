#include "M68000.h"
#ifndef __M68000_EXCEPTIONSVIEW_H__
#define __M68000_EXCEPTIONSVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"
namespace M68000{

class M68000::ExceptionsView :public ViewBase
{
public:
	//Constructors
	ExceptionsView(M68000* adevice);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Exception functions
	void UpdateExceptionEntry(HWND hwnd, const ExceptionDebuggingEntry& targetExceptionEntry, unsigned int addressCharWidth);
	std::wstring BuildExceptionName(unsigned int vectorNumber) const;

private:
	M68000* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;

	int exceptionListIndex;
	ExceptionDebuggingEntry exceptionEntry;
};

} //Close namespace M68000
#endif
