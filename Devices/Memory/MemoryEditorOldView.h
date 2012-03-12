#include "MemoryRead.h"
#ifndef __MEMORYREAD_MEMORYEDITOROLDVIEW_H__
#define __MEMORYREAD_MEMORYEDITOROLDVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class MemoryRead::MemoryEditorOldView :public ViewBase
{
public:
	//Constructors
	MemoryEditorOldView(MemoryRead* adevice, unsigned int atargetMemoryAddress);

	//Memory address functions
	void SetTargetMemoryAddress(unsigned int atargetMemoryAddress);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Update functions
	void UpdateMemoryLocation(HWND hwnd, unsigned int targetMemoryLocation);

private:
	MemoryRead* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;

	unsigned int addressWidth;
	unsigned int memoryLocation;
	bool memoryLocationModified;
};

#endif
