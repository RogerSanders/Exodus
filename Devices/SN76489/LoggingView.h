#include "SN76489.h"
#ifndef __SN76489_LOGGINGVIEW_H__
#define __SN76489_LOGGINGVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "WAVFile/WAVFile.pkg"

class SN76489::LoggingView :public ViewBase
{
public:
	//Constructors
	LoggingView(SN76489* adevice);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Logging functions
	void SelectLoggingPath(HWND hwnd, std::wstring& fileName) const;
	bool ToggleLoggingEnabledState(WAVFile& wavFile, const std::wstring& fileName, bool currentState, bool newState, unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec) const;

private:
	SN76489* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
};

#endif
