/*--------------------------------------------------------------------------------------*\
Things to do:
-Implement buffering of input, so that rollbacks don't cause lost input events
-Consider adding an optional mode to simulate analog effects on button press events, where
a button will always flicker between the pressed and not pressed state very rapidly while
the user is in the process of pressing the button. This will help ensure robust input
routines when writing user code.
-Implement the bank switching delay reported by Charles MacDonald

References:
-Sega Genesis I/O Chip and Peripherals, Charles MacDonald
-Sega Six Button Controller Hardware Info, Charles Rosenberg, v1.0 1996-09-09
-http://segaretro.org/Six_Button_Control_Pad_(Mega_Drive), retrieved 2012-08-14
\*--------------------------------------------------------------------------------------*/
#ifndef __MDCONTROL6_H__
#define __MDCONTROL6_H__
#include "SystemInterface/SystemInterface.pkg"
#include "Device/Device.pkg"

class MDControl6 :public Device
{
public:
	//Constructors
	MDControl6(const std::wstring& ainstanceName, unsigned int amoduleID);

	//Initialization functions
	virtual void Initialize();
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
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;
	void ApplyLineStateChange(unsigned int targetLine, const Data& lineData);
	void UpdateLineState(bool timeoutSettingsChanged, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	void UpdateOutputLineStateForLine(unsigned int lineID, bool revokeAllTimeoutStateChanges, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	static bool GetDesiredLineState(unsigned int currentBankswitchCounter, unsigned int currentLineInputStateTH, const std::vector<bool>& currentButtonPressedState, unsigned int lineID);

	//Input functions
	virtual unsigned int GetKeyCodeID(const wchar_t* keyCodeName) const;
	virtual const wchar_t* GetKeyCodeName(unsigned int keyCodeID) const;
	virtual void HandleInputKeyDown(unsigned int keyCodeID);
	virtual void HandleInputKeyUp(unsigned int keyCodeID);

private:
	//Enumerations
	enum LineID
	{
		LINE_D0 = 1, //IO
		LINE_D1,     //IO
		LINE_D2,     //IO
		LINE_D3,     //IO
		LINE_TL,     //IO
		LINE_TR,     //IO
		LINE_TH      //IO
	};
	enum Button
	{
		BUTTON_UP     = 0,
		BUTTON_DOWN   = 1,
		BUTTON_LEFT   = 2,
		BUTTON_RIGHT  = 3,
		BUTTON_A      = 4,
		BUTTON_B      = 5,
		BUTTON_C      = 6,
		BUTTON_START  = 7,
		BUTTON_X      = 8,
		BUTTON_Y      = 9,
		BUTTON_Z      = 10,
		BUTTON_MODE   = 11
	};

	//Structures
	struct OutputLineState
	{
		bool asserted;
		bool timeoutFlagged;
		bool timeoutAssertedState;
		double timeoutTime;
	};

private:
	//Constants
	static const unsigned int buttonCount = 12;
	static const unsigned int bankswitchCounterResetPoint = 4;
	static const unsigned int outputLineCount = 7;

	//Bus interface
	IBusInterface* memoryBus;

	//Button state
	std::vector<bool> buttonPressed;
	std::vector<bool> bbuttonPressed;

	//Device state
	double bankswitchTimeoutInterval;
	double currentTimesliceLength;
	double bcurrentTimesliceLength;
	bool lineInputStateTH;
	bool blineInputStateTH;
	bool bankswitchingDisabled;
	bool bbankswitchingDisabled;
	unsigned int bankswitchCounter;
	unsigned int bbankswitchCounter;
	double bankswitchCounterToggleLastRisingEdge;
	double bbankswitchCounterToggleLastRisingEdge;

	//Line state
	std::vector<OutputLineState> outputLineState;
	std::vector<OutputLineState> boutputLineState;

	//Line access
	boost::mutex lineMutex;
	double lastLineAccessTime;
};

#endif
