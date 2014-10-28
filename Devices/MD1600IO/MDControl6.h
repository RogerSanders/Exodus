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
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Device/Device.pkg"
#include <mutex>

class MDControl6 :public Device
{
public:
	//Constructors
	MDControl6(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);

	//Initialization functions
	virtual void Initialize();
	virtual bool ValidateDevice();

	//Reference functions
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Execute functions
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Line functions
	virtual unsigned int GetLineID(const MarshalSupport::Marshal::In<std::wstring>& lineName) const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	//Input functions
	virtual unsigned int GetKeyCodeID(const MarshalSupport::Marshal::In<std::wstring>& keyCodeName) const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetKeyCodeName(unsigned int keyCodeID) const;
	virtual void HandleInputKeyDown(unsigned int keyCodeID);
	virtual void HandleInputKeyUp(unsigned int keyCodeID);

	//Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;

private:
	//Constants
	static const unsigned int buttonCount = 12;
	static const unsigned int bankswitchCounterResetPoint = 4;
	static const unsigned int outputLineCount = 7;

	//Enumerations
	enum class LineID
	{
		D0 = 1, //IO
		D1,     //IO
		D2,     //IO
		D3,     //IO
		TL,     //IO
		TR,     //IO
		TH      //IO
	};
	enum ButtonIndex
	{
		BUTTONINDEX_UP     = 0,
		BUTTONINDEX_DOWN   = 1,
		BUTTONINDEX_LEFT   = 2,
		BUTTONINDEX_RIGHT  = 3,
		BUTTONINDEX_A      = 4,
		BUTTONINDEX_B      = 5,
		BUTTONINDEX_C      = 6,
		BUTTONINDEX_START  = 7,
		BUTTONINDEX_X      = 8,
		BUTTONINDEX_Y      = 9,
		BUTTONINDEX_Z      = 10,
		BUTTONINDEX_MODE   = 11
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
	//Line functions
	void ApplyLineStateChange(LineID targetLine, const Data& lineData);
	void UpdateLineState(bool timeoutSettingsChanged, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	void UpdateOutputLineStateForLine(LineID lineID, bool revokeAllTimeoutStateChanges, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	static bool GetDesiredLineState(unsigned int currentBankswitchCounter, unsigned int currentLineInputStateTH, const std::vector<bool>& currentButtonPressedState, LineID lineID);
	static unsigned int GetLineIndex(LineID lineID);

private:
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
	std::mutex lineMutex;
	double lastLineAccessTime;
};

#endif
