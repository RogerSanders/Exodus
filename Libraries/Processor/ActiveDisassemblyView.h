#include "Processor.h"
#ifndef __PROCESSOR_ACTIVEDISASSEMBLYVIEW_H__
#define __PROCESSOR_ACTIVEDISASSEMBLYVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class Processor::ActiveDisassemblyView :public ViewBase
{
public:
	//Constructors
	ActiveDisassemblyView(Processor* adevice);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	void OutputActiveDisassemblyToTextFile(const std::wstring& filePath);
	void OutputActiveDisassemblyToIDCFile(const std::wstring& filePath);

private:
	Processor* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
};

#endif
