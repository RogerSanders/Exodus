#include "YM2612.h"
#ifndef __YM2612_LOGGINGVIEW_H__
#define __YM2612_LOGGINGVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "Stream/Stream.pkg"

class YM2612::LoggingView :public ViewBase
{
public:
	//Constructors
	LoggingView(YM2612* adevice);

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
	bool ToggleLoggingEnabledState(Stream::WAVFile& wavFile, const std::wstring& fileName, bool currentState, bool newState, unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec) const;

private:
	YM2612* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
};

#endif
