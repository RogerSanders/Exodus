#include "Processor.h"
#ifndef __PROCESSOR_DISASSEMBLYOLDVIEW_H__
#define __PROCESSOR_DISASSEMBLYOLDVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class Processor::DisassemblyOldView :public ViewBase
{
public:
	//Constructors
	DisassemblyOldView(Processor* adevice);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Disassembly functions
	void UpdateDisassembly(unsigned int location, unsigned int size, HWND hwnd, std::map<unsigned int, unsigned int>& pcToListIndex);

private:
	Processor* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	bool track;
	bool forcePCSync;
	unsigned int currentPC;
	unsigned int location;
	unsigned int size;
	std::map<unsigned int, unsigned int> pcToListIndex;
};

#endif
