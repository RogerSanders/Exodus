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
\*--------------------------------------------------------------------------------------*/
#ifndef __MDCONTROL3_H__
#define __MDCONTROL3_H__
#include "SystemInterface/SystemInterface.pkg"
#include "Device/Device.pkg"

class MDControl3 :public Device
{
public:
	//Constructors
	MDControl3(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);

	//Initialization functions
	virtual void Initialize();
	virtual bool ValidateDevice();

	//Reference functions
	using Device::AddReference;
	virtual bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Execute functions
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Line functions
	virtual unsigned int GetLineID(const std::wstring& lineName) const;
	virtual std::wstring GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;
	void ApplyLineStateChange(unsigned int targetLine, const Data& lineData);
	void UpdateLineState(IDeviceContext* caller, double accessTime, unsigned int accessContext);
	static bool GetDesiredLineState(unsigned int currentLineInputStateTH, const std::vector<bool>& currentButtonPressedState, unsigned int lineID);

	//Input functions
	virtual unsigned int GetKeyCodeID(const std::wstring& keyCodeName) const;
	virtual std::wstring GetKeyCodeName(unsigned int keyCodeID) const;
	virtual void HandleInputKeyDown(unsigned int keyCodeID);
	virtual void HandleInputKeyUp(unsigned int keyCodeID);

	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;

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
	bool lineInputStateTH;
	bool blineInputStateTH;

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
		LINE_D0 = 1, //IO
		LINE_D1,     //IO
		LINE_D2,     //IO
		LINE_D3,     //IO
		LINE_TL,     //IO
		LINE_TR,     //IO
		LINE_TH      //IO
	};
};

#endif
