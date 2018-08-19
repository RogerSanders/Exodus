#include "A10000.h"
//##DEBUG##
#include <iostream>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
A10000::A10000(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Device(implementationName, instanceName, moduleID), _versionRegister(8), _bversionRegister(8)
{
	_memoryBus = 0;
	_controlPortBus = 0;

	//Initialize the control port registers
	_dataRegisters.resize(ControlPortCount, Data(8));
	_bdataRegisters.resize(ControlPortCount, Data(8));
	_controlRegisters.resize(ControlPortCount, Data(8));
	_bcontrolRegisters.resize(ControlPortCount, Data(8));
	_serialControlRegisters.resize(ControlPortCount, Data(8));
	_bserialControlRegisters.resize(ControlPortCount, Data(8));
	_txDataRegisters.resize(ControlPortCount, Data(8));
	_btxDataRegisters.resize(ControlPortCount, Data(8));
	_rxDataRegisters.resize(ControlPortCount, Data(8));
	_brxDataRegisters.resize(ControlPortCount, Data(8));

	_inputLineState.resize(ControlPortCount);
	_binputLineState.resize(ControlPortCount);
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool A10000::ValidateDevice()
{
	return (_memoryBus != 0) && (_controlPortBus != 0);
}

//----------------------------------------------------------------------------------------
void A10000::Initialize()
{
	//Initialize the version register
	_inputHardwareVersion = 0;
	_versionRegister = 0;
	SetOverseasFlag(false);
	SetPALFlag(true);
	SetNoDiskFlag(true);
	SetHardwareVersion(_inputHardwareVersion);

	//Initialize the control port registers to their correct default values. These
	//defaults were obtained from Charles MacDonald.
	//##TODO## Verify these defaults on the hardware. We have seen other documents which
	//state that the default value for TxData port 3 is initialized to 0xFF, not 0xFB.
	_dataRegisters[0] = 0x7F;
	_dataRegisters[1] = 0x7F;
	_dataRegisters[2] = 0x7F;
	_controlRegisters[0] = 0x00;
	_controlRegisters[1] = 0x00;
	_controlRegisters[2] = 0x00;
	_txDataRegisters[0] = 0xFF;
	_rxDataRegisters[0] = 0x00;
	_serialControlRegisters[0] = 0x00;
	_txDataRegisters[1] = 0xFF;
	_rxDataRegisters[1] = 0x00;
	_serialControlRegisters[1] = 0x00;
	_txDataRegisters[2] = 0xFF; //0xFB
	_rxDataRegisters[2] = 0x00;
	_serialControlRegisters[2] = 0x00;

	_lastLineCheckTime = 0;
	_lineAccessPending = false;
	_lastTimesliceLength = 0;
	_lineAccessBuffer.clear();
	_currentHLLineState = false;

	//Note that we initialize these lines to false, but on the real system, these lines
	//read as true when no controller is connected. This occurs because there are pull-up
	//resistors attached to these input lines. Pull-up resistors are mapped through the
	//system XML mappings in our emulator, so we shouldn't initialize these values here
	//assuming that these pull-up resistors exist, since they could be removed.
	for(unsigned int i = 0; i < ControlPortCount; ++i)
	{
		_inputLineState[i].lineAssertedD0 = false;
		_inputLineState[i].lineAssertedD1 = false;
		_inputLineState[i].lineAssertedD2 = false;
		_inputLineState[i].lineAssertedD3 = false;
		_inputLineState[i].lineAssertedTL = false;
		_inputLineState[i].lineAssertedTR = false;
		_inputLineState[i].lineAssertedTH = false;
	}
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool A10000::AddReference(const Marshal::In<std::wstring>& referenceName, IBusInterface* target)
{
	if(referenceName == L"BusInterface")
	{
		_memoryBus = target;
	}
	else if(referenceName == L"ControlPortBus")
	{
		_controlPortBus = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
void A10000::RemoveReference(IBusInterface* target)
{
	if(_memoryBus == target)
	{
		_memoryBus = 0;
	}
	else if(_controlPortBus == target)
	{
		_controlPortBus = 0;
	}
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
bool A10000::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void A10000::NotifyUpcomingTimeslice(double nanoseconds)
{
	//##DEBUG##
//	std::wcout << "A10000Notify: " << nanoseconds << '\n';

	//Reset lastLineCheckTime for the beginning of the new timeslice, and force any
	//remaining line state changes to be evaluated at the start of the new timeslice.
	_lastLineCheckTime = 0;
	for(std::list<LineAccess>::iterator i = _lineAccessBuffer.begin(); i != _lineAccessBuffer.end(); ++i)
	{
		//We rebase accessTime here to the start of the new time block, in order to allow
		//line state changes to be flagged ahead of the time they actually take effect.
		//This rebasing allows changes flagged ahead of time to safely cross timeslice
		//boundaries.
		i->accessTime -= _lastTimesliceLength;
	}
	_lastTimesliceLength = nanoseconds;
}

//----------------------------------------------------------------------------------------
bool A10000::SendNotifyAfterExecuteCalled() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void A10000::NotifyAfterExecuteCalled()
{
	//Ensure that any pending line state changes which we have passed in this timeslice
	//are applied
	ApplyPendingLineStateChanges(_lastTimesliceLength);
}

//----------------------------------------------------------------------------------------
void A10000::ExecuteRollback()
{
	_versionRegister = _bversionRegister;
	_inputHardwareVersion = _binputHardwareVersion;
	_dataRegisters = _bdataRegisters;
	_controlRegisters = _bcontrolRegisters;
	_serialControlRegisters = _bserialControlRegisters;
	_txDataRegisters = _btxDataRegisters;
	_rxDataRegisters = _brxDataRegisters;
	_inputLineState = _binputLineState;
	_currentHLLineState = _bcurrentHLLineState;

	_lastTimesliceLength = _blastTimesliceLength;
	_lineAccessBuffer = _blineAccessBuffer;
	_lineAccessPending = !_lineAccessBuffer.empty();
}

//----------------------------------------------------------------------------------------
void A10000::ExecuteCommit()
{
	_bversionRegister = _versionRegister;
	_binputHardwareVersion = _inputHardwareVersion;
	_bdataRegisters = _dataRegisters;
	_bcontrolRegisters = _controlRegisters;
	_bserialControlRegisters = _serialControlRegisters;
	_btxDataRegisters = _txDataRegisters;
	_brxDataRegisters = _rxDataRegisters;
	_binputLineState = _inputLineState;
	_bcurrentHLLineState = _currentHLLineState;

	_blastTimesliceLength = _lastTimesliceLength;
	if(_lineAccessPending)
	{
		_blineAccessBuffer = _lineAccessBuffer;
	}
	else
	{
		_blineAccessBuffer.clear();
	}
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int A10000::GetLineID(const Marshal::In<std::wstring>& lineName) const
{
	if(lineName == L"PORT1_D0")
	{
		return (unsigned int)LineID::LINE_PORT1_D0;
	}
	else if(lineName == L"PORT1_D1")
	{
		return (unsigned int)LineID::LINE_PORT1_D1;
	}
	else if(lineName == L"PORT1_D2")
	{
		return (unsigned int)LineID::LINE_PORT1_D2;
	}
	else if(lineName == L"PORT1_D3")
	{
		return (unsigned int)LineID::LINE_PORT1_D3;
	}
	else if(lineName == L"PORT1_TL")
	{
		return (unsigned int)LineID::LINE_PORT1_TL;
	}
	else if(lineName == L"PORT1_TR")
	{
		return (unsigned int)LineID::LINE_PORT1_TR;
	}
	else if(lineName == L"PORT1_TH")
	{
		return (unsigned int)LineID::LINE_PORT1_TH;
	}

	else if(lineName == L"PORT2_D0")
	{
		return (unsigned int)LineID::LINE_PORT2_D0;
	}
	else if(lineName == L"PORT2_D1")
	{
		return (unsigned int)LineID::LINE_PORT2_D1;
	}
	else if(lineName == L"PORT2_D2")
	{
		return (unsigned int)LineID::LINE_PORT2_D2;
	}
	else if(lineName == L"PORT2_D3")
	{
		return (unsigned int)LineID::LINE_PORT2_D3;
	}
	else if(lineName == L"PORT2_TL")
	{
		return (unsigned int)LineID::LINE_PORT2_TL;
	}
	else if(lineName == L"PORT2_TR")
	{
		return (unsigned int)LineID::LINE_PORT2_TR;
	}
	else if(lineName == L"PORT2_TH")
	{
		return (unsigned int)LineID::LINE_PORT2_TH;
	}

	else if(lineName == L"PORT3_D0")
	{
		return (unsigned int)LineID::LINE_PORT3_D0;
	}
	else if(lineName == L"PORT3_D1")
	{
		return (unsigned int)LineID::LINE_PORT3_D1;
	}
	else if(lineName == L"PORT3_D2")
	{
		return (unsigned int)LineID::LINE_PORT3_D2;
	}
	else if(lineName == L"PORT3_D3")
	{
		return (unsigned int)LineID::LINE_PORT3_D3;
	}
	else if(lineName == L"PORT3_TL")
	{
		return (unsigned int)LineID::LINE_PORT3_TL;
	}
	else if(lineName == L"PORT3_TR")
	{
		return (unsigned int)LineID::LINE_PORT3_TR;
	}
	else if(lineName == L"PORT3_TH")
	{
		return (unsigned int)LineID::LINE_PORT3_TH;
	}

	else if(lineName == L"JAP")
	{
		return (unsigned int)LineID::LINE_JAP;
	}
	else if(lineName == L"NTSC")
	{
		return (unsigned int)LineID::LINE_NTSC;
	}
	else if(lineName == L"DISK")
	{
		return (unsigned int)LineID::LINE_DISK;
	}
	else if(lineName == L"HL")
	{
		return (unsigned int)LineID::LINE_HL;
	}
	else if(lineName == L"HWVersion")
	{
		return (unsigned int)LineID::LINE_HWVERSION;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> A10000::GetLineName(unsigned int lineID) const
{
	switch((LineID)lineID)
	{
	case LineID::LINE_PORT1_D0:
		return L"PORT1_D0";
	case LineID::LINE_PORT1_D1:
		return L"PORT1_D1";
	case LineID::LINE_PORT1_D2:
		return L"PORT1_D2";
	case LineID::LINE_PORT1_D3:
		return L"PORT1_D3";
	case LineID::LINE_PORT1_TL:
		return L"PORT1_TL";
	case LineID::LINE_PORT1_TR:
		return L"PORT1_TR";
	case LineID::LINE_PORT1_TH:
		return L"PORT1_TH";

	case LineID::LINE_PORT2_D0:
		return L"PORT2_D0";
	case LineID::LINE_PORT2_D1:
		return L"PORT2_D1";
	case LineID::LINE_PORT2_D2:
		return L"PORT2_D2";
	case LineID::LINE_PORT2_D3:
		return L"PORT2_D3";
	case LineID::LINE_PORT2_TL:
		return L"PORT2_TL";
	case LineID::LINE_PORT2_TR:
		return L"PORT2_TR";
	case LineID::LINE_PORT2_TH:
		return L"PORT2_TH";

	case LineID::LINE_PORT3_D0:
		return L"PORT3_D0";
	case LineID::LINE_PORT3_D1:
		return L"PORT3_D1";
	case LineID::LINE_PORT3_D2:
		return L"PORT3_D2";
	case LineID::LINE_PORT3_D3:
		return L"PORT3_D3";
	case LineID::LINE_PORT3_TL:
		return L"PORT3_TL";
	case LineID::LINE_PORT3_TR:
		return L"PORT3_TR";
	case LineID::LINE_PORT3_TH:
		return L"PORT3_TH";

	case LineID::LINE_JAP:
		return L"JAP";
	case LineID::LINE_NTSC:
		return L"NTSC";
	case LineID::LINE_DISK:
		return L"DISK";
	case LineID::LINE_HL:
		return L"HL";
	case LineID::LINE_HWVERSION:
		return L"HWVersion";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int A10000::GetLineWidth(unsigned int lineID) const
{
	switch((LineID)lineID)
	{
	case LineID::LINE_PORT1_D0:
	case LineID::LINE_PORT1_D1:
	case LineID::LINE_PORT1_D2:
	case LineID::LINE_PORT1_D3:
	case LineID::LINE_PORT1_TL:
	case LineID::LINE_PORT1_TR:
	case LineID::LINE_PORT1_TH:
		return 1;

	case LineID::LINE_PORT2_D0:
	case LineID::LINE_PORT2_D1:
	case LineID::LINE_PORT2_D2:
	case LineID::LINE_PORT2_D3:
	case LineID::LINE_PORT2_TL:
	case LineID::LINE_PORT2_TR:
	case LineID::LINE_PORT2_TH:
		return 1;

	case LineID::LINE_PORT3_D0:
	case LineID::LINE_PORT3_D1:
	case LineID::LINE_PORT3_D2:
	case LineID::LINE_PORT3_D3:
	case LineID::LINE_PORT3_TL:
	case LineID::LINE_PORT3_TR:
	case LineID::LINE_PORT3_TH:
		return 1;

	case LineID::LINE_JAP:
		return 1;
	case LineID::LINE_NTSC:
		return 1;
	case LineID::LINE_DISK:
		return 1;
	case LineID::LINE_HL:
		return 1;

	case LineID::LINE_HWVERSION:
		return 4;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void A10000::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(_lineMutex);

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(_lastLineCheckTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//If this is a line state change which needs to be processed immediately, apply it now
	//and return.
	switch((LineID)targetLine)
	{
	case LineID::LINE_HWVERSION:
		_inputHardwareVersion = lineData.GetData();
		SetHardwareVersion(_inputHardwareVersion);
		return;
	}

	//Flag that an entry exists in the buffer. This flag is used to skip the expensive
	//locking operation in the active thread for this device when no line changes are
	//pending. Note that we set this flag before we've actually written the entry into
	//the buffer so that the execution thread is aware of the line state change as soon as
	//possible, however the lock we've obtained on our line mutex will prevent the
	//execution thread from attempting to access the line access buffer until the data has
	//been written.
	_lineAccessPending = true;

	//Insert the line access into the buffer. Note that entries in the buffer are sorted
	//by access time from lowest to highest.
	std::list<LineAccess>::reverse_iterator i = _lineAccessBuffer.rbegin();
	while((i != _lineAccessBuffer.rend()) && (i->accessTime > accessTime))
	{
		++i;
	}
	_lineAccessBuffer.insert(i.base(), LineAccess((LineID)targetLine, lineData, accessTime));

	//We explicitly release our lock on lineMutex here so that we're not blocking access
	//to SetLineState() on this class before we modify the line state for other devices in
	//the code that follows. Adhering to this pattern helps to avoid deadlock cases that
	//could otherwise arise from valid line mappings.
	lock.unlock();

	//##TODO## Clean this up
	//##TODO## Handle changes to the HL interrupt line state. It's possible for an
	//interrupt line to be flagged here as a result of the TH line changing. What we
	//probably need to do is actually call SetLineState for this line if appropriate here,
	//but revoke the line state change later on if the TH line state change is revoked, or
	//if the control register settings that affect TH line generation are changed. This
	//means we need to look for changes to the TH line here, and conditionally trigger a
	//line state change.
	//UpdateHLInterruptState(caller, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------
void A10000::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{
	SetLineState(targetLine, lineData, 0, _lastTimesliceLength, 0);
}

//----------------------------------------------------------------------------------------
void A10000::RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(_lineMutex);

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(_lastLineCheckTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//Find the matching line state change entry in the line access buffer
	std::list<LineAccess>::reverse_iterator i = _lineAccessBuffer.rbegin();
	bool foundTargetEntry = false;
	while(!foundTargetEntry && (i != _lineAccessBuffer.rend()))
	{
		if((i->lineID == (LineID)targetLine) && (i->state == lineData) && (i->accessTime == reportedTime))
		{
			foundTargetEntry = true;
			continue;
		}
		++i;
	}

	//Erase the target line state change entry from the line access buffer
	if(foundTargetEntry)
	{
		_lineAccessBuffer.erase((++i).base());
	}
	else
	{
		//##DEBUG##
		std::wcout << "Failed to find matching line state change in RevokeSetLineState! " << GetLineName(targetLine) << '\t' << lineData.GetData() << '\t' << reportedTime << '\t' << accessTime << '\n';
		for(std::list<LineAccess>::iterator i = _lineAccessBuffer.begin(); i != _lineAccessBuffer.end(); ++i)
		{
			std::wcout << "-" << GetLineName((unsigned int)i->lineID) << '\t' << i->state.GetData() << '\t' << i->accessTime << '\n';
		}
	}

	//Update the lineAccessPending flag
	_lineAccessPending = !_lineAccessBuffer.empty();
}

//----------------------------------------------------------------------------------------
void A10000::AssertCurrentOutputLineState() const
{
	if(_memoryBus != 0)
	{
		AssertCurrentOutputLineStateForPort(Ports::Port1);
		AssertCurrentOutputLineStateForPort(Ports::Port2);
		AssertCurrentOutputLineStateForPort(Ports::Port3);
		if(_currentHLLineState) _memoryBus->SetLineState((unsigned int)LineID::LINE_HL, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void A10000::NegateCurrentOutputLineState() const
{
	if(_memoryBus != 0)
	{
		NegateCurrentOutputLineStateForPort(Ports::Port1);
		NegateCurrentOutputLineStateForPort(Ports::Port2);
		NegateCurrentOutputLineStateForPort(Ports::Port3);
		if(_currentHLLineState) _memoryBus->SetLineState((unsigned int)LineID::LINE_HL, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void A10000::AssertCurrentOutputLineStateForPort(Ports portNo) const
{
	if(GetControlRegisterD0(portNo) && GetDataRegisterD0(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D0), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD1(portNo) && GetDataRegisterD1(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D1), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD2(portNo) && GetDataRegisterD2(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D2), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD3(portNo) && GetDataRegisterD3(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D3), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTL(portNo) && GetDataRegisterTL(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TL), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTR(portNo) && GetDataRegisterTR(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TR), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTH(portNo) && GetDataRegisterTH(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TH), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void A10000::NegateCurrentOutputLineStateForPort(Ports portNo) const
{
	if(GetControlRegisterD0(portNo) && GetDataRegisterD0(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D0), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD1(portNo) && GetDataRegisterD1(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D1), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD2(portNo) && GetDataRegisterD2(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D2), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD3(portNo) && GetDataRegisterD3(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D3), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTL(portNo) && GetDataRegisterTL(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TL), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTR(portNo) && GetDataRegisterTR(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TR), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTH(portNo) && GetDataRegisterTH(portNo))
	{
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TH), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void A10000::ApplyLineStateChange(LineID targetLine, const Data& lineData)
{
	switch(targetLine)
	{
	//Control port 1 interface
	case LineID::LINE_PORT1_TH:
		_inputLineState[GetPortIndexForPort(Ports::Port1)].lineAssertedTH = lineData.GetBit(0);
		if(!GetControlRegisterTH(Ports::Port1))
		{
			SetDataRegisterTH(Ports::Port1, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT1_TR:
		_inputLineState[GetPortIndexForPort(Ports::Port1)].lineAssertedTR = lineData.GetBit(0);
		if(!GetControlRegisterTR(Ports::Port1))
		{
			SetDataRegisterTR(Ports::Port1, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT1_TL:
		_inputLineState[GetPortIndexForPort(Ports::Port1)].lineAssertedTL = lineData.GetBit(0);
		if(!GetControlRegisterTL(Ports::Port1))
		{
			SetDataRegisterTL(Ports::Port1, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT1_D3:
		_inputLineState[GetPortIndexForPort(Ports::Port1)].lineAssertedD3 = lineData.GetBit(0);
		if(!GetControlRegisterD3(Ports::Port1))
		{
			SetDataRegisterD3(Ports::Port1, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT1_D2:
		_inputLineState[GetPortIndexForPort(Ports::Port1)].lineAssertedD2 = lineData.GetBit(0);
		if(!GetControlRegisterD2(Ports::Port1))
		{
			SetDataRegisterD2(Ports::Port1, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT1_D1:
		_inputLineState[GetPortIndexForPort(Ports::Port1)].lineAssertedD1 = lineData.GetBit(0);
		if(!GetControlRegisterD1(Ports::Port1))
		{
			SetDataRegisterD1(Ports::Port1, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT1_D0:
		_inputLineState[GetPortIndexForPort(Ports::Port1)].lineAssertedD0 = lineData.GetBit(0);
		if(!GetControlRegisterD0(Ports::Port1))
		{
			SetDataRegisterD0(Ports::Port1, lineData.GetBit(0));
		}
		break;

	//Control port 2 interface
	case LineID::LINE_PORT2_TH:
		_inputLineState[GetPortIndexForPort(Ports::Port2)].lineAssertedTH = lineData.GetBit(0);
		if(!GetControlRegisterTH(Ports::Port2))
		{
			SetDataRegisterTH(Ports::Port2, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT2_TR:
		_inputLineState[GetPortIndexForPort(Ports::Port2)].lineAssertedTR = lineData.GetBit(0);
		if(!GetControlRegisterTR(Ports::Port2))
		{
			SetDataRegisterTR(Ports::Port2, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT2_TL:
		_inputLineState[GetPortIndexForPort(Ports::Port2)].lineAssertedTL = lineData.GetBit(0);
		if(!GetControlRegisterTL(Ports::Port2))
		{
			SetDataRegisterTL(Ports::Port2, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT2_D3:
		_inputLineState[GetPortIndexForPort(Ports::Port2)].lineAssertedD3 = lineData.GetBit(0);
		if(!GetControlRegisterD3(Ports::Port2))
		{
			SetDataRegisterD3(Ports::Port2, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT2_D2:
		_inputLineState[GetPortIndexForPort(Ports::Port2)].lineAssertedD2 = lineData.GetBit(0);
		if(!GetControlRegisterD2(Ports::Port2))
		{
			SetDataRegisterD2(Ports::Port2, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT2_D1:
		_inputLineState[GetPortIndexForPort(Ports::Port2)].lineAssertedD1 = lineData.GetBit(0);
		if(!GetControlRegisterD1(Ports::Port2))
		{
			SetDataRegisterD1(Ports::Port2, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT2_D0:
		_inputLineState[GetPortIndexForPort(Ports::Port2)].lineAssertedD0 = lineData.GetBit(0);
		if(!GetControlRegisterD0(Ports::Port2))
		{
			SetDataRegisterD0(Ports::Port2, lineData.GetBit(0));
		}
		break;

	//Control port 3 interface
	case LineID::LINE_PORT3_TH:
		_inputLineState[GetPortIndexForPort(Ports::Port3)].lineAssertedTH = lineData.GetBit(0);
		if(!GetControlRegisterTH(Ports::Port3))
		{
			SetDataRegisterTH(Ports::Port3, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT3_TR:
		_inputLineState[GetPortIndexForPort(Ports::Port3)].lineAssertedTR = lineData.GetBit(0);
		if(!GetControlRegisterTR(Ports::Port3))
		{
			SetDataRegisterTR(Ports::Port3, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT3_TL:
		_inputLineState[GetPortIndexForPort(Ports::Port3)].lineAssertedTL = lineData.GetBit(0);
		if(!GetControlRegisterTL(Ports::Port3))
		{
			SetDataRegisterTL(Ports::Port3, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT3_D3:
		_inputLineState[GetPortIndexForPort(Ports::Port3)].lineAssertedD3 = lineData.GetBit(0);
		if(!GetControlRegisterD3(Ports::Port3))
		{
			SetDataRegisterD3(Ports::Port3, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT3_D2:
		_inputLineState[GetPortIndexForPort(Ports::Port3)].lineAssertedD2 = lineData.GetBit(0);
		if(!GetControlRegisterD2(Ports::Port3))
		{
			SetDataRegisterD2(Ports::Port3, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT3_D1:
		_inputLineState[GetPortIndexForPort(Ports::Port3)].lineAssertedD1 = lineData.GetBit(0);
		if(!GetControlRegisterD1(Ports::Port3))
		{
			SetDataRegisterD1(Ports::Port3, lineData.GetBit(0));
		}
		break;
	case LineID::LINE_PORT3_D0:
		_inputLineState[GetPortIndexForPort(Ports::Port3)].lineAssertedD0 = lineData.GetBit(0);
		if(!GetControlRegisterD0(Ports::Port3))
		{
			SetDataRegisterD0(Ports::Port3, lineData.GetBit(0));
		}
		break;

	case LineID::LINE_JAP:
		SetOverseasFlag(!lineData.GetBit(0));
		break;
	case LineID::LINE_NTSC:
		SetPALFlag(!lineData.GetBit(0));
		break;
	case LineID::LINE_DISK:
		SetNoDiskFlag(!lineData.GetBit(0));
		break;
	}
}

//----------------------------------------------------------------------------------------
void A10000::ApplyPendingLineStateChanges(double currentTimesliceProgress)
{
	//If we have any pending line state changes waiting, apply any which we have now
	//reached.
	if(_lineAccessPending)
	{
		bool done = false;
		while(!done)
		{
			//Remove the next line access that we've reached from the front of the line
			//access buffer
			std::list<LineAccess>::iterator i = _lineAccessBuffer.begin();
			if((i == _lineAccessBuffer.end()) || (i->accessTime > currentTimesliceProgress))
			{
				done = true;
				continue;
			}
			LineAccess lineAccess = *i;
			_lineAccessBuffer.pop_front();
			_lineAccessPending = !_lineAccessBuffer.empty();

			//Apply the line state change
			ApplyLineStateChange(lineAccess.lineID, lineAccess.state);
		}
	}
}

//----------------------------------------------------------------------------------------
A10000::LineID A10000::GetLineIDForPort(Ports portNo, PortLine portLine)
{
	LineID basePortLineID;
	switch(portNo)
	{
	default:
	case Ports::Port1:
		basePortLineID = LineID::LINE_PORT1_D0;
		break;
	case Ports::Port2:
		basePortLineID = LineID::LINE_PORT2_D0;
		break;
	case Ports::Port3:
		basePortLineID = LineID::LINE_PORT3_D0;
		break;
	}
	return (LineID)((unsigned int)basePortLineID + ((unsigned int)portLine - (unsigned int)PortLine::D0));
}

//----------------------------------------------------------------------------------------
//Interrupt functions
//----------------------------------------------------------------------------------------
void A10000::UpdateHLInterruptState(IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Update the HL interrupt line state
	bool newHLLineState = (GetControlRegisterHL(Ports::Port1) && !GetControlRegisterTH(Ports::Port1) && GetDataRegisterTH(Ports::Port1))
	                   || (GetControlRegisterHL(Ports::Port2) && !GetControlRegisterTH(Ports::Port2) && GetDataRegisterTH(Ports::Port2))
	                   || (GetControlRegisterHL(Ports::Port3) && !GetControlRegisterTH(Ports::Port3) && GetDataRegisterTH(Ports::Port3));
	if(newHLLineState != _currentHLLineState)
	{
		_currentHLLineState = newHLLineState;
		_memoryBus->SetLineState((unsigned int)LineID::LINE_HL, Data(1, _currentHLLineState), GetDeviceContext(), caller, accessTime, accessContext);
	}
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult A10000::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(_accessMutex);

	//Trigger a system rollback if the device has been accessed out of order
	std::unique_lock<std::mutex> lineLock(_lineMutex);
	if(_lastLineCheckTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}
	_lastLineCheckTime = accessTime;

	//Apply any pending line state changes
	ApplyPendingLineStateChanges(accessTime);

	//Release the lock on lineMutex
	lineLock.unlock();

	//Process the read
	switch(location)
	{
	case 0x00:
		data = GetVersionRegister();
		break;
	case 0x01:
		data = ReadDataRegister(caller, accessTime, accessContext, Ports::Port1);
		break;
	case 0x02:
		data = ReadDataRegister(caller, accessTime, accessContext, Ports::Port2);
		break;
	case 0x03:
		data = ReadDataRegister(caller, accessTime, accessContext, Ports::Port3);
		break;
	case 0x04:
		data = ReadControlRegister(caller, accessTime, accessContext, Ports::Port1);
		break;
	case 0x05:
		data = ReadControlRegister(caller, accessTime, accessContext, Ports::Port2);
		break;
	case 0x06:
		data = ReadControlRegister(caller, accessTime, accessContext, Ports::Port3);
		break;
	case 0x07:
		data = ReadTxDataRegister(caller, accessTime, accessContext, Ports::Port1);
		break;
	case 0x08:
		data = ReadRxDataRegister(caller, accessTime, accessContext, Ports::Port1);
		break;
	case 0x09:
		data = ReadSerialControlRegister(caller, accessTime, accessContext, Ports::Port1);
		break;
	case 0x0A:
		data = ReadTxDataRegister(caller, accessTime, accessContext, Ports::Port2);
		break;
	case 0x0B:
		data = ReadRxDataRegister(caller, accessTime, accessContext, Ports::Port2);
		break;
	case 0x0C:
		data = ReadSerialControlRegister(caller, accessTime, accessContext, Ports::Port2);
		break;
	case 0x0D:
		data = ReadTxDataRegister(caller, accessTime, accessContext, Ports::Port3);
		break;
	case 0x0E:
		data = ReadRxDataRegister(caller, accessTime, accessContext, Ports::Port3);
		break;
	case 0x0F:
		data = ReadSerialControlRegister(caller, accessTime, accessContext, Ports::Port3);
		break;
	}

	//##DEBUG##
	//if((location == 1) || (location == 4) || (location == 7) || (location == 8) || (location == 9))
	//{
	//	std::wcout << "A10000Read:  " << std::hex << std::uppercase << location << '\t' << data.GetData() << '\t' << accessTime << '\t' << lastLineCheckTime << '\n';
	//}

	//The 8-bit result is mirrored in both the upper and lower bytes for word-wide reads.
	//This appears to be done internally, not as part of the bus mapping logic. We emulate
	//that behaviour here.
	data = (data.GetData() | (data.GetData() << 8));

	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult A10000::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(_accessMutex);

	//##DEBUG##
	//InputLineState* inputLineStatePointer = &inputLineState[0];
	//if((location == 1) || (location == 4) || (location == 7) || (location == 8) || (location == 9))
	//{
	//	std::wcout << "A10000Write: " << std::hex << std::uppercase << location << '\t' << data.GetData() << '\t' << accessTime << '\t' << lastLineCheckTime << '\n';
	//}

	//Trigger a system rollback if the device has been accessed out of order
	std::unique_lock<std::mutex> lineLock(_lineMutex);
	if(_lastLineCheckTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}
	_lastLineCheckTime = accessTime;

	//Apply any pending line state changes
	ApplyPendingLineStateChanges(accessTime);

	//Release the lock on lineMutex
	lineLock.unlock();

	//Process the write
	switch(location)
	{
	case 0x00:
		//Writes to the version register are ignored
		break;
	case 0x01:
		WriteDataRegister(caller, accessTime, accessContext, Ports::Port1, data);
		break;
	case 0x02:
		WriteDataRegister(caller, accessTime, accessContext, Ports::Port2, data);
		break;
	case 0x03:
		WriteDataRegister(caller, accessTime, accessContext, Ports::Port3, data);
		break;
	case 0x04:
		WriteControlRegister(caller, accessTime, accessContext, Ports::Port1, data);
		break;
	case 0x05:
		WriteControlRegister(caller, accessTime, accessContext, Ports::Port2, data);
		break;
	case 0x06:
		WriteControlRegister(caller, accessTime, accessContext, Ports::Port3, data);
		break;
	case 0x07:
		WriteTxDataRegister(caller, accessTime, accessContext, Ports::Port1, data);
		break;
	case 0x08:
		WriteRxDataRegister(caller, accessTime, accessContext, Ports::Port1, data);
		break;
	case 0x09:
		WriteSerialControlRegister(caller, accessTime, accessContext, Ports::Port1, data);
		break;
	case 0x0A:
		WriteTxDataRegister(caller, accessTime, accessContext, Ports::Port2, data);
		break;
	case 0x0B:
		WriteRxDataRegister(caller, accessTime, accessContext, Ports::Port2, data);
		break;
	case 0x0C:
		WriteSerialControlRegister(caller, accessTime, accessContext, Ports::Port2, data);
		break;
	case 0x0D:
		WriteTxDataRegister(caller, accessTime, accessContext, Ports::Port3, data);
		break;
	case 0x0E:
		WriteRxDataRegister(caller, accessTime, accessContext, Ports::Port3, data);
		break;
	case 0x0F:
		WriteSerialControlRegister(caller, accessTime, accessContext, Ports::Port3, data);
		break;
	}

	//##TODO## Re-implement support for the HL interrupt line
	//UpdateHLInterruptState(caller, accessTime, accessContext);
	return true;
}

//----------------------------------------------------------------------------------------
//Data register access
//----------------------------------------------------------------------------------------
Data A10000::ReadDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo) const
{
	//Return the current buffered data for each line
	return GetDataRegister(portNo);
}

//----------------------------------------------------------------------------------------
void A10000::WriteDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo, const Data& data)
{
	//---------------------------------
	//| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|-------------------------------|
	//|*U |TH |TR |TL |D3 |D2 |D1 |D0 |
	//---------------------------------
	bool newDataRegisterStateD0 = data.GetBit(0);
	bool newDataRegisterStateD1 = data.GetBit(1);
	bool newDataRegisterStateD2 = data.GetBit(2);
	bool newDataRegisterStateD3 = data.GetBit(3);
	bool newDataRegisterStateTL = data.GetBit(4);
	bool newDataRegisterStateTR = data.GetBit(5);
	bool newDataRegisterStateTH = data.GetBit(6);
	bool newDataRegisterStateHL = data.GetBit(7);
	if(GetControlRegisterD0(portNo) && (newDataRegisterStateD0 != GetDataRegisterD0(portNo)))
	{
		SetDataRegisterD0(portNo, newDataRegisterStateD0);
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D0), Data(1, (unsigned int)newDataRegisterStateD0), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterD1(portNo) && (newDataRegisterStateD1 != GetDataRegisterD1(portNo)))
	{
		SetDataRegisterD1(portNo, newDataRegisterStateD1);
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D1), Data(1, (unsigned int)newDataRegisterStateD1), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterD2(portNo) && (newDataRegisterStateD2 != GetDataRegisterD2(portNo)))
	{
		SetDataRegisterD2(portNo, newDataRegisterStateD2);
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D2), Data(1, (unsigned int)newDataRegisterStateD2), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterD3(portNo) && (newDataRegisterStateD3 != GetDataRegisterD3(portNo)))
	{
		SetDataRegisterD3(portNo, newDataRegisterStateD3);
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D3), Data(1, (unsigned int)newDataRegisterStateD3), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterTL(portNo) && (newDataRegisterStateTL != GetDataRegisterTL(portNo)))
	{
		SetDataRegisterTL(portNo, newDataRegisterStateTL);
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TL), Data(1, (unsigned int)newDataRegisterStateTL), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterTR(portNo) && (newDataRegisterStateTR != GetDataRegisterTR(portNo)))
	{
		SetDataRegisterTR(portNo, newDataRegisterStateTR);
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TR), Data(1, (unsigned int)newDataRegisterStateTR), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterTH(portNo) && (newDataRegisterStateTH != GetDataRegisterTH(portNo)))
	{
		SetDataRegisterTH(portNo, newDataRegisterStateTH);
		_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TH), Data(1, (unsigned int)newDataRegisterStateTH), GetDeviceContext(), caller, accessTime, accessContext);
	}

	SetDataRegisterHL(portNo, newDataRegisterStateHL);
}

//----------------------------------------------------------------------------------------
//Control register access
//----------------------------------------------------------------------------------------
Data A10000::ReadControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo) const
{
	//Reads from the control register always return 0
	//##TODO## Verify on hardware
	return Data(8, 0);
}

//----------------------------------------------------------------------------------------
void A10000::WriteControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo, const Data& data)
{
	//---------------------------------
	//| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|-------------------------------|
	//|HL |TH |TR |TL |D3 |D2 |D1 |D0 |
	//---------------------------------
	bool newControlRegisterStateD0 = data.GetBit(0);
	bool newControlRegisterStateD1 = data.GetBit(1);
	bool newControlRegisterStateD2 = data.GetBit(2);
	bool newControlRegisterStateD3 = data.GetBit(3);
	bool newControlRegisterStateTL = data.GetBit(4);
	bool newControlRegisterStateTR = data.GetBit(5);
	bool newControlRegisterStateTH = data.GetBit(6);
	bool newControlRegisterStateHL = data.GetBit(7);
	if(newControlRegisterStateD0 != GetControlRegisterD0(portNo))
	{
		SetControlRegisterD0(portNo, newControlRegisterStateD0);
		if(!newControlRegisterStateD0)
		{
			//If we were previously asserting this output line, negate it.
			if(GetDataRegisterD0(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D0), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterD0(portNo, _inputLineState[GetPortIndexForPort(portNo)].lineAssertedD0);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterD0(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D0), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
			}
		}
	}
	if(newControlRegisterStateD1 != GetControlRegisterD1(portNo))
	{
		SetControlRegisterD1(portNo, newControlRegisterStateD1);
		if(!newControlRegisterStateD1)
		{
			//If we were previously asserting this output line, negate it.
			if(GetDataRegisterD1(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D1), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterD1(portNo, _inputLineState[GetPortIndexForPort(portNo)].lineAssertedD1);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterD1(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D1), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
			}
		}
	}
	if(newControlRegisterStateD2 != GetControlRegisterD2(portNo))
	{
		SetControlRegisterD2(portNo, newControlRegisterStateD2);
		if(!newControlRegisterStateD2)
		{
			//If we were previously asserting this output line, negate it.
			if(GetDataRegisterD2(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D2), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterD2(portNo, _inputLineState[GetPortIndexForPort(portNo)].lineAssertedD2);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterD2(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D2), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
			}
		}
	}
	if(newControlRegisterStateD3 != GetControlRegisterD3(portNo))
	{
		SetControlRegisterD3(portNo, newControlRegisterStateD3);
		if(!newControlRegisterStateD3)
		{
			//If we were previously asserting this output line, negate it.
			if(GetDataRegisterD3(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D3), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterD3(portNo, _inputLineState[GetPortIndexForPort(portNo)].lineAssertedD3);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterD3(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::D3), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
			}
		}
	}
	if(newControlRegisterStateTL != GetControlRegisterTL(portNo))
	{
		SetControlRegisterTL(portNo, newControlRegisterStateTL);
		if(!newControlRegisterStateTL)
		{
			//If we were previously asserting this output line, negate it.
			if(GetDataRegisterTL(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TL), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterTL(portNo, _inputLineState[GetPortIndexForPort(portNo)].lineAssertedTL);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterTL(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TL), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
			}
		}
	}
	if(newControlRegisterStateTR != GetControlRegisterTR(portNo))
	{
		SetControlRegisterTR(portNo, newControlRegisterStateTR);
		if(!newControlRegisterStateTR)
		{
			//If we were previously asserting this output line, negate it.
			if(GetDataRegisterTR(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TR), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterTR(portNo, _inputLineState[GetPortIndexForPort(portNo)].lineAssertedTR);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterTR(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TR), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
			}
		}
	}
	if(newControlRegisterStateTH != GetControlRegisterTH(portNo))
	{
		SetControlRegisterTH(portNo, newControlRegisterStateTH);
		if(!newControlRegisterStateTH)
		{
			//If we were previously asserting this output line, negate it.
			if(GetDataRegisterTH(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TH), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterTH(portNo, _inputLineState[GetPortIndexForPort(portNo)].lineAssertedTH);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterTH(portNo))
			{
				_controlPortBus->SetLineState((unsigned int)GetLineIDForPort(portNo, PortLine::TH), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
			}
		}
	}

	//Update the HL output line enable state
	SetControlRegisterHL(portNo, newControlRegisterStateHL);
}

//----------------------------------------------------------------------------------------
//Serial control register access
//----------------------------------------------------------------------------------------
Data A10000::ReadSerialControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo) const
{
	return GetSerialControlRegister(portNo);
}

//----------------------------------------------------------------------------------------
void A10000::WriteSerialControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo, const Data& data)
{
	//Write to the upper 5 bits of the serial control register. The lower 3 bits are
	//read-only status flags.
	SetSerialControlRegister(portNo, GetSerialControlRegister(portNo).SetDataSegment(3, 5, data.GetDataSegment(3, 5)));
}

//----------------------------------------------------------------------------------------
//TxData register access
//
//A write to the TxData register will send the written byte over a serial transfer if
//serial output is enabled.
//----------------------------------------------------------------------------------------
Data A10000::ReadTxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo) const
{
	//According to gen-hw.txt, it would seem that this port returns the last value
	//written when serial output is not enabled. We have not yet confirmed the correct
	//behaviour when serial output is enabled.
	//##TODO## Find out what to return when reading this port
	return GetTxDataRegister(portNo);
}

//----------------------------------------------------------------------------------------
void A10000::WriteTxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo, const Data& data)
{
	//##TODO## According to Charles MacDonald, "Writing a byte to this register will make
	//the I/O chip output the data serially through the TL pin, providing it was
	//configured for serial mode". This raises the following questions:
	//-What does reading this register do?
	//-What happens when serial output is not enabled?
	SetTxDataRegister(portNo, data);
}

//----------------------------------------------------------------------------------------
//RxData register access
//
//The RxData register can be read to retrieve the last byte received from a serial
//transfer.
//----------------------------------------------------------------------------------------
Data A10000::ReadRxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo) const
{
	return _rxDataRegisters[GetPortIndexForPort(portNo)];
}

//----------------------------------------------------------------------------------------
void A10000::WriteRxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo, const Data& data)
{
	//Writes to the RxData register are ignored
	//##TODO## Verify on hardware
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void A10000::LoadState(IHierarchicalStorageNode& node)
{
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		IHierarchicalStorageAttribute* portNumberAttribute = (*i)->GetAttribute(L"PortNumber");
		if(portNumberAttribute != 0)
		{
			unsigned int portNoAsInteger;
			portNumberAttribute->ExtractHexValue(portNoAsInteger);
			if(portNoAsInteger < ControlPortCount)
			{
				Ports portNo = (Ports)portNoAsInteger;
				if((*i)->GetName() == L"DataRegister")
				{
					SetDataRegister(portNo, Data(8, (*i)->ExtractHexData<unsigned int>()));
				}
				else if((*i)->GetName() == L"ControlRegister")
				{
					SetControlRegister(portNo, Data(8, (*i)->ExtractHexData<unsigned int>()));
				}
				else if((*i)->GetName() == L"SerialControlRegister")
				{
					SetSerialControlRegister(portNo, Data(8, (*i)->ExtractHexData<unsigned int>()));
				}
				else if((*i)->GetName() == L"TxDataRegister")
				{
					SetTxDataRegister(portNo, Data(8, (*i)->ExtractHexData<unsigned int>()));
				}
				else if((*i)->GetName() == L"TxDataRegister")
				{
					SetRxDataRegister(portNo, Data(8, (*i)->ExtractHexData<unsigned int>()));
				}
				else if((*i)->GetName() == L"LineAsserted")
				{
					IHierarchicalStorageAttribute* lineNameAttribute = (*i)->GetAttribute(L"LineName");
					if(lineNameAttribute != 0)
					{
						std::wstring lineName = lineNameAttribute->GetValue();
						if(lineName == L"D0")
						{
							(*i)->ExtractData(_inputLineState[GetPortIndexForPort(portNo)].lineAssertedD0);
						}
						else if(lineName == L"D1")
						{
							(*i)->ExtractData(_inputLineState[GetPortIndexForPort(portNo)].lineAssertedD1);
						}
						else if(lineName == L"D2")
						{
							(*i)->ExtractData(_inputLineState[GetPortIndexForPort(portNo)].lineAssertedD2);
						}
						else if(lineName == L"D3")
						{
							(*i)->ExtractData(_inputLineState[GetPortIndexForPort(portNo)].lineAssertedD3);
						}
						else if(lineName == L"TL")
						{
							(*i)->ExtractData(_inputLineState[GetPortIndexForPort(portNo)].lineAssertedTL);
						}
						else if(lineName == L"TR")
						{
							(*i)->ExtractData(_inputLineState[GetPortIndexForPort(portNo)].lineAssertedTR);
						}
						else if(lineName == L"TH")
						{
							(*i)->ExtractData(_inputLineState[GetPortIndexForPort(portNo)].lineAssertedTH);
						}
					}
				}
			}
		}
		else if((*i)->GetName() == L"VersionRegister")
		{
			_versionRegister = (*i)->ExtractHexData<unsigned int>();
		}
		else if((*i)->GetName() == L"InputHardwareVersion")
		{
			_inputHardwareVersion = (*i)->ExtractHexData<unsigned int>();
		}
		else if((*i)->GetName() == L"HLLineState")
		{
			_currentHLLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"LastTimesliceLength")
		{
			_lastTimesliceLength = (*i)->ExtractData<bool>();
		}
		//Restore the lineAccessBuffer state
		else if((*i)->GetName() == L"LineAccessBuffer")
		{
			_lineAccessBuffer.clear();
			IHierarchicalStorageNode& lineAccessBufferNode = *(*i);
			std::list<IHierarchicalStorageNode*> lineAccessBufferChildList = lineAccessBufferNode.GetChildList();
			for(std::list<IHierarchicalStorageNode*>::iterator lineAccessBufferEntry = lineAccessBufferChildList.begin(); lineAccessBufferEntry != lineAccessBufferChildList.end(); ++lineAccessBufferEntry)
			{
				if((*lineAccessBufferEntry)->GetName() == L"LineAccess")
				{
					IHierarchicalStorageAttribute* lineNameAttribute = (*lineAccessBufferEntry)->GetAttribute(L"LineName");
					IHierarchicalStorageAttribute* lineStateAttribute = (*lineAccessBufferEntry)->GetAttribute(L"LineState");
					IHierarchicalStorageAttribute* accessTimeAttribute = (*lineAccessBufferEntry)->GetAttribute(L"AccessTime");
					if((lineNameAttribute != 0) && (lineStateAttribute != 0) && (accessTimeAttribute != 0))
					{
						//Extract the entry from the XML stream
						std::wstring lineName = lineNameAttribute->ExtractValue<std::wstring>();
						double accessTime = accessTimeAttribute->ExtractValue<double>();
						unsigned int lineIDAsInteger = GetLineID(lineName.c_str());
						if(lineIDAsInteger != 0)
						{
							LineID lineID = (LineID)lineIDAsInteger;
							Data lineState(GetLineWidth((unsigned int)lineID));
							lineStateAttribute->ExtractValue(lineState);
							LineAccess lineAccess(lineID, lineState, accessTime);

							//Find the correct location in the list to insert the entry. The
							//list must be sorted from earliest to latest.
							std::list<LineAccess>::reverse_iterator j = _lineAccessBuffer.rbegin();
							while((j != _lineAccessBuffer.rend()) && (j->accessTime > lineAccess.accessTime))
							{
								++j;
							}
							_lineAccessBuffer.insert(j.base(), lineAccess);
						}
					}
				}
			}
			_lineAccessPending = !_lineAccessBuffer.empty();
		}
	}
}

//----------------------------------------------------------------------------------------
void A10000::SaveState(IHierarchicalStorageNode& node) const
{
	for(unsigned int i = 0; i < ControlPortCount; ++i)
	{
		Ports portNo = (Ports)i;
		node.CreateChildHex(L"DataRegister", GetDataRegister(portNo).GetData(), GetDataRegister(portNo).GetHexCharCount()).CreateAttribute(L"PortNumber", i);
		node.CreateChildHex(L"ControlRegister", GetControlRegister(portNo).GetData(), GetControlRegister(portNo).GetHexCharCount()).CreateAttribute(L"PortNumber", i);
		node.CreateChildHex(L"SerialControlRegister", GetSerialControlRegister(portNo).GetData(), GetSerialControlRegister(portNo).GetHexCharCount()).CreateAttribute(L"PortNumber", i);
		node.CreateChildHex(L"TxDataRegister", GetTxDataRegister(portNo).GetData(), GetTxDataRegister(portNo).GetHexCharCount()).CreateAttribute(L"PortNumber", i);
		node.CreateChildHex(L"RxDataRegister", GetRxDataRegister(portNo).GetData(), GetRxDataRegister(portNo).GetHexCharCount()).CreateAttribute(L"PortNumber", i);

		node.CreateChild(L"LineAsserted", _inputLineState[i].lineAssertedD0).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "D0");
		node.CreateChild(L"LineAsserted", _inputLineState[i].lineAssertedD1).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "D1");
		node.CreateChild(L"LineAsserted", _inputLineState[i].lineAssertedD2).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "D2");
		node.CreateChild(L"LineAsserted", _inputLineState[i].lineAssertedD3).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "D3");
		node.CreateChild(L"LineAsserted", _inputLineState[i].lineAssertedTL).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "TL");
		node.CreateChild(L"LineAsserted", _inputLineState[i].lineAssertedTR).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "TR");
		node.CreateChild(L"LineAsserted", _inputLineState[i].lineAssertedTH).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "TH");
	}
	node.CreateChildHex(L"VersionRegister", _versionRegister.GetData(), _versionRegister.GetHexCharCount());
	node.CreateChildHex(L"InputHardwareVersion", _inputHardwareVersion, 1);
	node.CreateChild(L"HLLineState", _currentHLLineState);
	node.CreateChild(L"LastTimesliceLength", _lastTimesliceLength);

	//Save the lineAccessBuffer state
	if(_lineAccessPending)
	{
		IHierarchicalStorageNode& lineAccessState = node.CreateChild(L"LineAccessBuffer");
		for(std::list<LineAccess>::const_iterator i = _lineAccessBuffer.begin(); i != _lineAccessBuffer.end(); ++i)
		{
			IHierarchicalStorageNode& lineAccessEntry = lineAccessState.CreateChild(L"LineAccess");
			lineAccessEntry.CreateAttribute(L"LineName", GetLineName((unsigned int)i->lineID));
			lineAccessEntry.CreateAttribute(L"LineState", i->state);
			lineAccessEntry.CreateAttribute(L"AccessTime", i->accessTime);
		}
	}
}
