/*--------------------------------------------------------------------------------------*\
Things to do:
-Implement buffering of input, so that rollbacks don't cause lost input events
-Consider adding an optional mode to simulate analog effects on button press events, where
a button will always flicker between the pressed and not pressed state very rapidly while
the user is in the process of pressing the button. This will help ensure robust input
routines when writing user code.

References:
-Sega Genesis I/O Chip and Peripherals, Charles MacDonald
-Sega Six Button Controller Hardware Info, Charles Rosenberg, v1.0 1996-09-09
\*--------------------------------------------------------------------------------------*/
#ifndef __MDCONTROL3_H__
#define __MDCONTROL3_H__
#include "SystemInterface/SystemInterface.pkg"
#include "Device/Device.pkg"

class MDControl3 :public Device
{
public:
	//Constructors
	MDControl3(const std::wstring& ainstanceName, unsigned int amoduleID);

	//Initialization functions
	virtual bool ValidateDevice();

	//Reference functions
	virtual bool AddReference(const wchar_t* referenceName, IBusInterface* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Execute functions
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Line functions
	virtual unsigned int GetLineID(const wchar_t* lineName) const;
	virtual const wchar_t* GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	void ApplyLineStateChange(unsigned int targetLine, const Data& lineData);

	//Initialization functions
	virtual void Initialize();

	//Input functions
	virtual unsigned int GetKeyCodeID(const wchar_t* keyCodeName) const;
	virtual void HandleInputKeyDown(unsigned int keyCode);
	virtual void HandleInputKeyUp(unsigned int keyCode);
	void UpdateLineState(IDeviceContext* caller, double accessTime, unsigned int accessContext);

private:
	enum Button
	{
		BUTTON_UP     = 0,
		BUTTON_DOWN   = 1,
		BUTTON_LEFT   = 2,
		BUTTON_RIGHT  = 3,
		BUTTON_A      = 4,
		BUTTON_B      = 5,
		BUTTON_C      = 6,
		BUTTON_START  = 7
	};
	static const unsigned int buttonCount = 8;

	//Bus interface
	IBusInterface* memoryBus;

	//Button state
	std::vector<bool> buttonPressed;
	std::vector<bool> bbuttonPressed;

	//Device state
	bool secondBankEnabled;
	bool bsecondBankEnabled;

	//Line state
	bool lineAssertedD0;
	bool blineAssertedD0;
	bool lineAssertedD1;
	bool blineAssertedD1;
	bool lineAssertedD2;
	bool blineAssertedD2;
	bool lineAssertedD3;
	bool blineAssertedD3;
	bool lineAssertedTL;
	bool blineAssertedTL;
	bool lineAssertedTR;
	bool blineAssertedTR;
	bool lineAssertedTH;
	bool blineAssertedTH;

	//Line access
	boost::mutex lineMutex;
	double lastLineAccessTime;
	enum LineID
	{
		LINE_D0 = 1,	//IO
		LINE_D1,		//IO
		LINE_D2,		//IO
		LINE_D3,		//IO
		LINE_TL,		//IO
		LINE_TR,		//IO
		LINE_TH			//IO
	};
};

#endif
