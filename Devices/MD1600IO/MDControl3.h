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
#include "DeviceInterface/DeviceInterface.pkg"
#include "Device/Device.pkg"
#include <mutex>

class MDControl3 :public Device
{
public:
	//Constructors
	MDControl3(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);

	//Initialization functions
	virtual void Initialize();
	virtual bool ValidateDevice();

	//Reference functions
	virtual bool AddReference(const Marshal::In<std::wstring>& referenceName, IBusInterface* target);
	virtual void RemoveReference(IBusInterface* target);

	//Execute functions
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Line functions
	virtual unsigned int GetLineID(const Marshal::In<std::wstring>& lineName) const;
	virtual Marshal::Ret<std::wstring> GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	//Input functions
	virtual unsigned int GetKeyCodeID(const Marshal::In<std::wstring>& keyCodeName) const;
	virtual Marshal::Ret<std::wstring> GetKeyCodeName(unsigned int keyCodeID) const;
	virtual void HandleInputKeyDown(unsigned int keyCodeID);
	virtual void HandleInputKeyUp(unsigned int keyCodeID);

	//Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;

private:
	//Enumerations
	enum LineID
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
		BUTTONINDEX_START  = 7
	};

	//Constants
	static const unsigned int buttonCount = 8;

private:
	//Line functions
	void UpdateLineState(IDeviceContext* caller, double accessTime, unsigned int accessContext);
	static bool GetDesiredLineState(unsigned int currentLineInputStateTH, const std::vector<bool>& currentButtonPressedState, LineID lineID);

private:
	//Bus interface
	IBusInterface* _memoryBus;

	//Button state
	std::vector<bool> _buttonPressed;
	std::vector<bool> _bbuttonPressed;

	//Device state
	bool _lineInputStateTH;
	bool _blineInputStateTH;

	//Line state
	bool _lineAssertedD0;
	bool _blineAssertedD0;
	bool _lineAssertedD1;
	bool _blineAssertedD1;
	bool _lineAssertedD2;
	bool _blineAssertedD2;
	bool _lineAssertedD3;
	bool _blineAssertedD3;
	bool _lineAssertedTL;
	bool _blineAssertedTL;
	bool _lineAssertedTR;
	bool _blineAssertedTR;
	bool _lineAssertedTH;
	bool _blineAssertedTH;

	//Line access
	std::mutex _lineMutex;
	double _lastLineAccessTime;
	double _currentTimesliceLength;
	double _bcurrentTimesliceLength;
};

#endif
