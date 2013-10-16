#include "A10000.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
A10000::A10000(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(aimplementationName, ainstanceName, amoduleID), versionRegister(8), bversionRegister(8)
{
	memoryBus = 0;
	controlPortBus = 0;

	//Initialize the control port registers
	dataRegisters.resize(controlPortCount, Data(8));
	bdataRegisters.resize(controlPortCount, Data(8));
	controlRegisters.resize(controlPortCount, Data(8));
	bcontrolRegisters.resize(controlPortCount, Data(8));
	serialControlRegisters.resize(controlPortCount, Data(8));
	bserialControlRegisters.resize(controlPortCount, Data(8));
	txDataRegisters.resize(controlPortCount, Data(8));
	btxDataRegisters.resize(controlPortCount, Data(8));
	rxDataRegisters.resize(controlPortCount, Data(8));
	brxDataRegisters.resize(controlPortCount, Data(8));

	inputLineState.resize(controlPortCount);
	binputLineState.resize(controlPortCount);
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool A10000::ValidateDevice()
{
	return (memoryBus != 0) && (controlPortBus != 0);
}

//----------------------------------------------------------------------------------------
void A10000::Initialize()
{
	//Initialize the version register
	inputHardwareVersion = 0;
	versionRegister = 0;
	SetOverseasFlag(false);
	SetPALFlag(true);
	SetNoDiskFlag(true);
	SetHardwareVersion(inputHardwareVersion);

	//Initialize the control port registers to their correct default values. These
	//defaults were obtained from Charles MacDonald.
	//##TODO## Verify these defaults on the hardware. We have seen other documents which
	//state that the default value for TxData port 3 is initialized to 0xFF, not 0xFB.
	dataRegisters[0] = 0x7F;
	dataRegisters[1] = 0x7F;
	dataRegisters[2] = 0x7F;
	controlRegisters[0] = 0x00;
	controlRegisters[1] = 0x00;
	controlRegisters[2] = 0x00;
	txDataRegisters[0] = 0xFF;
	rxDataRegisters[0] = 0x00;
	serialControlRegisters[0] = 0x00;
	txDataRegisters[1] = 0xFF;
	rxDataRegisters[1] = 0x00;
	serialControlRegisters[1] = 0x00;
	txDataRegisters[2] = 0xFF; //0xFB
	rxDataRegisters[2] = 0x00;
	serialControlRegisters[2] = 0x00;

	lastLineCheckTime = 0;
	lineAccessPending = false;
	lastTimesliceLength = 0;
	lineAccessBuffer.clear();
	currentHLLineState = false;

	//Note that we initialize these lines to false, but on the real system, these lines
	//read as true when no controller is connected. This occurs because there are pull-up
	//resistors attached to these input lines. Pull-up resistors are mapped through the
	//system XML mappings in our emulator, so we shouldn't initialize these values here
	//assuming that these pull-up resistors exist, since they could be removed.
	for(unsigned int i = 0; i < controlPortCount; ++i)
	{
		inputLineState[i].lineAssertedD0 = false;
		inputLineState[i].lineAssertedD1 = false;
		inputLineState[i].lineAssertedD2 = false;
		inputLineState[i].lineAssertedD3 = false;
		inputLineState[i].lineAssertedTL = false;
		inputLineState[i].lineAssertedTR = false;
		inputLineState[i].lineAssertedTH = false;
	}
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool A10000::AddReference(const std::wstring& referenceName, IBusInterface* target)
{
	if(referenceName == L"BusInterface")
	{
		memoryBus = target;
	}
	else if(referenceName == L"ControlPortBus")
	{
		controlPortBus = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool A10000::RemoveReference(IBusInterface* target)
{
	if(memoryBus == target)
	{
		memoryBus = 0;
	}
	else if(controlPortBus == target)
	{
		controlPortBus = 0;
	}
	else
	{
		return false;
	}
	return true;
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
	lastLineCheckTime = 0;
	for(std::list<LineAccess>::iterator i = lineAccessBuffer.begin(); i != lineAccessBuffer.end(); ++i)
	{
		//We rebase accessTime here to the start of the new time block, in order to allow
		//line state changes to be flagged ahead of the time they actually take effect.
		//This rebasing allows changes flagged ahead of time to safely cross timeslice
		//boundaries.
		i->accessTime -= lastTimesliceLength;
	}
	lastTimesliceLength = nanoseconds;
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
	ApplyPendingLineStateChanges(lastTimesliceLength);
}

//----------------------------------------------------------------------------------------
void A10000::ExecuteRollback()
{
	versionRegister = bversionRegister;
	inputHardwareVersion = binputHardwareVersion;
	dataRegisters = bdataRegisters;
	controlRegisters = bcontrolRegisters;
	serialControlRegisters = bserialControlRegisters;
	txDataRegisters = btxDataRegisters;
	rxDataRegisters = brxDataRegisters;
	inputLineState = binputLineState;
	currentHLLineState = bcurrentHLLineState;

	lastTimesliceLength = blastTimesliceLength;
	lineAccessBuffer = blineAccessBuffer;
	lineAccessPending = !lineAccessBuffer.empty();
}

//----------------------------------------------------------------------------------------
void A10000::ExecuteCommit()
{
	bversionRegister = versionRegister;
	binputHardwareVersion = inputHardwareVersion;
	bdataRegisters = dataRegisters;
	bcontrolRegisters = controlRegisters;
	bserialControlRegisters = serialControlRegisters;
	btxDataRegisters = txDataRegisters;
	brxDataRegisters = rxDataRegisters;
	binputLineState = inputLineState;
	bcurrentHLLineState = currentHLLineState;

	blastTimesliceLength = lastTimesliceLength;
	if(lineAccessPending)
	{
		blineAccessBuffer = lineAccessBuffer;
	}
	else
	{
		blineAccessBuffer.clear();
	}
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int A10000::GetLineID(const std::wstring& lineName) const
{
	if(lineName == L"PORT1_D0")
	{
		return LINE_PORT1_D0;
	}
	else if(lineName == L"PORT1_D1")
	{
		return LINE_PORT1_D1;
	}
	else if(lineName == L"PORT1_D2")
	{
		return LINE_PORT1_D2;
	}
	else if(lineName == L"PORT1_D3")
	{
		return LINE_PORT1_D3;
	}
	else if(lineName == L"PORT1_TL")
	{
		return LINE_PORT1_TL;
	}
	else if(lineName == L"PORT1_TR")
	{
		return LINE_PORT1_TR;
	}
	else if(lineName == L"PORT1_TH")
	{
		return LINE_PORT1_TH;
	}

	else if(lineName == L"PORT2_D0")
	{
		return LINE_PORT2_D0;
	}
	else if(lineName == L"PORT2_D1")
	{
		return LINE_PORT2_D1;
	}
	else if(lineName == L"PORT2_D2")
	{
		return LINE_PORT2_D2;
	}
	else if(lineName == L"PORT2_D3")
	{
		return LINE_PORT2_D3;
	}
	else if(lineName == L"PORT2_TL")
	{
		return LINE_PORT2_TL;
	}
	else if(lineName == L"PORT2_TR")
	{
		return LINE_PORT2_TR;
	}
	else if(lineName == L"PORT2_TH")
	{
		return LINE_PORT2_TH;
	}

	else if(lineName == L"PORT3_D0")
	{
		return LINE_PORT3_D0;
	}
	else if(lineName == L"PORT3_D1")
	{
		return LINE_PORT3_D1;
	}
	else if(lineName == L"PORT3_D2")
	{
		return LINE_PORT3_D2;
	}
	else if(lineName == L"PORT3_D3")
	{
		return LINE_PORT3_D3;
	}
	else if(lineName == L"PORT3_TL")
	{
		return LINE_PORT3_TL;
	}
	else if(lineName == L"PORT3_TR")
	{
		return LINE_PORT3_TR;
	}
	else if(lineName == L"PORT3_TH")
	{
		return LINE_PORT3_TH;
	}

	else if(lineName == L"JAP")
	{
		return LINE_JAP;
	}
	else if(lineName == L"NTSC")
	{
		return LINE_NTSC;
	}
	else if(lineName == L"DISK")
	{
		return LINE_DISK;
	}
	else if(lineName == L"HL")
	{
		return LINE_HL;
	}
	else if(lineName == L"HWVersion")
	{
		return LINE_HWVERSION;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
std::wstring A10000::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_PORT1_D0:
		return L"PORT1_D0";
	case LINE_PORT1_D1:
		return L"PORT1_D1";
	case LINE_PORT1_D2:
		return L"PORT1_D2";
	case LINE_PORT1_D3:
		return L"PORT1_D3";
	case LINE_PORT1_TL:
		return L"PORT1_TL";
	case LINE_PORT1_TR:
		return L"PORT1_TR";
	case LINE_PORT1_TH:
		return L"PORT1_TH";

	case LINE_PORT2_D0:
		return L"PORT2_D0";
	case LINE_PORT2_D1:
		return L"PORT2_D1";
	case LINE_PORT2_D2:
		return L"PORT2_D2";
	case LINE_PORT2_D3:
		return L"PORT2_D3";
	case LINE_PORT2_TL:
		return L"PORT2_TL";
	case LINE_PORT2_TR:
		return L"PORT2_TR";
	case LINE_PORT2_TH:
		return L"PORT2_TH";

	case LINE_PORT3_D0:
		return L"PORT3_D0";
	case LINE_PORT3_D1:
		return L"PORT3_D1";
	case LINE_PORT3_D2:
		return L"PORT3_D2";
	case LINE_PORT3_D3:
		return L"PORT3_D3";
	case LINE_PORT3_TL:
		return L"PORT3_TL";
	case LINE_PORT3_TR:
		return L"PORT3_TR";
	case LINE_PORT3_TH:
		return L"PORT3_TH";

	case LINE_JAP:
		return L"JAP";
	case LINE_NTSC:
		return L"NTSC";
	case LINE_DISK:
		return L"DISK";
	case LINE_HL:
		return L"HL";
	case LINE_HWVERSION:
		return L"HWVersion";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int A10000::GetLineWidth(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_PORT1_D0:
	case LINE_PORT1_D1:
	case LINE_PORT1_D2:
	case LINE_PORT1_D3:
	case LINE_PORT1_TL:
	case LINE_PORT1_TR:
	case LINE_PORT1_TH:
		return 1;

	case LINE_PORT2_D0:
	case LINE_PORT2_D1:
	case LINE_PORT2_D2:
	case LINE_PORT2_D3:
	case LINE_PORT2_TL:
	case LINE_PORT2_TR:
	case LINE_PORT2_TH:
		return 1;

	case LINE_PORT3_D0:
	case LINE_PORT3_D1:
	case LINE_PORT3_D2:
	case LINE_PORT3_D3:
	case LINE_PORT3_TL:
	case LINE_PORT3_TR:
	case LINE_PORT3_TH:
		return 1;

	case LINE_JAP:
		return 1;
	case LINE_NTSC:
		return 1;
	case LINE_DISK:
		return 1;
	case LINE_HL:
		return 1;

	case LINE_HWVERSION:
		return 4;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void A10000::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(lineMutex);

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(lastLineCheckTime > accessTime)
	{
		GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//If this is a line state change which needs to be processed immediately, apply it now
	//and return.
	switch(targetLine)
	{
	case LINE_HWVERSION:
		inputHardwareVersion = lineData.GetData();
		SetHardwareVersion(inputHardwareVersion);
		return;
	}

	//Flag that an entry exists in the buffer. This flag is used to skip the expensive
	//locking operation in the active thread for this device when no line changes are
	//pending. Note that we set this flag before we've actually written the entry into
	//the buffer so that the execution thread is aware of the line state change as soon as
	//possible, however the lock we've obtained on our line mutex will prevent the
	//execution thread from attempting to access the line access buffer until the data has
	//been written.
	lineAccessPending = true;

	//Insert the line access into the buffer. Note that entries in the buffer are sorted
	//by access time from lowest to highest.
	std::list<LineAccess>::reverse_iterator i = lineAccessBuffer.rbegin();
	while((i != lineAccessBuffer.rend()) && (i->accessTime > accessTime))
	{
		++i;
	}
	lineAccessBuffer.insert(i.base(), LineAccess(targetLine, lineData, accessTime));

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
	SetLineState(targetLine, lineData, 0, lastTimesliceLength, 0);
}

//----------------------------------------------------------------------------------------
void A10000::RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(lineMutex);

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(lastLineCheckTime > accessTime)
	{
		GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//Find the matching line state change entry in the line access buffer
	std::list<LineAccess>::reverse_iterator i = lineAccessBuffer.rbegin();
	bool foundTargetEntry = false;
	while(!foundTargetEntry && (i != lineAccessBuffer.rend()))
	{
		if((i->lineID == targetLine) && (i->state == lineData) && (i->accessTime == reportedTime))
		{
			foundTargetEntry = true;
			continue;
		}
		++i;
	}

	//Erase the target line state change entry from the line access buffer
	if(foundTargetEntry)
	{
		lineAccessBuffer.erase((++i).base());
	}
	else
	{
		//##DEBUG##
		std::wcout << "Failed to find matching line state change in RevokeSetLineState! " << GetLineName(targetLine) << '\t' << lineData.GetData() << '\t' << reportedTime << '\t' << accessTime << '\n';
		for(std::list<LineAccess>::iterator i = lineAccessBuffer.begin(); i != lineAccessBuffer.end(); ++i)
		{
			std::wcout << "-" << GetLineName(i->lineID) << '\t' << i->state.GetData() << '\t' << i->accessTime << '\n';
		}
	}

	//Update the lineAccessPending flag
	lineAccessPending = !lineAccessBuffer.empty();
}

//----------------------------------------------------------------------------------------
void A10000::AssertCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		AssertCurrentOutputLineStateForPort(PORT1);
		AssertCurrentOutputLineStateForPort(PORT2);
		AssertCurrentOutputLineStateForPort(PORT3);
		if(currentHLLineState) memoryBus->SetLineState(LINE_HL, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void A10000::NegateCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		NegateCurrentOutputLineStateForPort(PORT1);
		NegateCurrentOutputLineStateForPort(PORT2);
		NegateCurrentOutputLineStateForPort(PORT3);
		if(currentHLLineState) memoryBus->SetLineState(LINE_HL, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void A10000::AssertCurrentOutputLineStateForPort(unsigned int portNo) const
{
	if(GetControlRegisterD0(portNo) && GetDataRegisterD0(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D0), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD1(portNo) && GetDataRegisterD1(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D1), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD2(portNo) && GetDataRegisterD2(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D2), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD3(portNo) && GetDataRegisterD3(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D3), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTL(portNo) && GetDataRegisterTL(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TL), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTR(portNo) && GetDataRegisterTR(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TR), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTH(portNo) && GetDataRegisterTH(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TH), Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void A10000::NegateCurrentOutputLineStateForPort(unsigned int portNo) const
{
	if(GetControlRegisterD0(portNo) && GetDataRegisterD0(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D0), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD1(portNo) && GetDataRegisterD1(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D1), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD2(portNo) && GetDataRegisterD2(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D2), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterD3(portNo) && GetDataRegisterD3(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D3), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTL(portNo) && GetDataRegisterTL(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TL), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTR(portNo) && GetDataRegisterTR(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TR), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(GetControlRegisterTH(portNo) && GetDataRegisterTH(portNo))
	{
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TH), Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void A10000::ApplyLineStateChange(unsigned int targetLine, const Data& lineData)
{
	switch(targetLine)
	{
	//Control port 1 interface
	case LINE_PORT1_TH:
		inputLineState[PORT1-PORT1].lineAssertedTH = lineData.GetBit(0);
		if(!GetControlRegisterTH(PORT1))
		{
			SetDataRegisterTH(PORT1, lineData.GetBit(0));
		}
		break;
	case LINE_PORT1_TR:
		inputLineState[PORT1-PORT1].lineAssertedTR = lineData.GetBit(0);
		if(!GetControlRegisterTR(PORT1))
		{
			SetDataRegisterTR(PORT1, lineData.GetBit(0));
		}
		break;
	case LINE_PORT1_TL:
		inputLineState[PORT1-PORT1].lineAssertedTL = lineData.GetBit(0);
		if(!GetControlRegisterTL(PORT1))
		{
			SetDataRegisterTL(PORT1, lineData.GetBit(0));
		}
		break;
	case LINE_PORT1_D3:
		inputLineState[PORT1-PORT1].lineAssertedD3 = lineData.GetBit(0);
		if(!GetControlRegisterD3(PORT1))
		{
			SetDataRegisterD3(PORT1, lineData.GetBit(0));
		}
		break;
	case LINE_PORT1_D2:
		inputLineState[PORT1-PORT1].lineAssertedD2 = lineData.GetBit(0);
		if(!GetControlRegisterD2(PORT1))
		{
			SetDataRegisterD2(PORT1, lineData.GetBit(0));
		}
		break;
	case LINE_PORT1_D1:
		inputLineState[PORT1-PORT1].lineAssertedD1 = lineData.GetBit(0);
		if(!GetControlRegisterD1(PORT1))
		{
			SetDataRegisterD1(PORT1, lineData.GetBit(0));
		}
		break;
	case LINE_PORT1_D0:
		inputLineState[PORT1-PORT1].lineAssertedD0 = lineData.GetBit(0);
		if(!GetControlRegisterD0(PORT1))
		{
			SetDataRegisterD0(PORT1, lineData.GetBit(0));
		}
		break;

	//Control port 2 interface
	case LINE_PORT2_TH:
		inputLineState[PORT2-PORT1].lineAssertedTH = lineData.GetBit(0);
		if(!GetControlRegisterTH(PORT2))
		{
			SetDataRegisterTH(PORT2, lineData.GetBit(0));
		}
		break;
	case LINE_PORT2_TR:
		inputLineState[PORT2-PORT1].lineAssertedTR = lineData.GetBit(0);
		if(!GetControlRegisterTR(PORT2))
		{
			SetDataRegisterTR(PORT2, lineData.GetBit(0));
		}
		break;
	case LINE_PORT2_TL:
		inputLineState[PORT2-PORT1].lineAssertedTL = lineData.GetBit(0);
		if(!GetControlRegisterTL(PORT2))
		{
			SetDataRegisterTL(PORT2, lineData.GetBit(0));
		}
		break;
	case LINE_PORT2_D3:
		inputLineState[PORT2-PORT1].lineAssertedD3 = lineData.GetBit(0);
		if(!GetControlRegisterD3(PORT2))
		{
			SetDataRegisterD3(PORT2, lineData.GetBit(0));
		}
		break;
	case LINE_PORT2_D2:
		inputLineState[PORT2-PORT1].lineAssertedD2 = lineData.GetBit(0);
		if(!GetControlRegisterD2(PORT2))
		{
			SetDataRegisterD2(PORT2, lineData.GetBit(0));
		}
		break;
	case LINE_PORT2_D1:
		inputLineState[PORT2-PORT1].lineAssertedD1 = lineData.GetBit(0);
		if(!GetControlRegisterD1(PORT2))
		{
			SetDataRegisterD1(PORT2, lineData.GetBit(0));
		}
		break;
	case LINE_PORT2_D0:
		inputLineState[PORT2-PORT1].lineAssertedD0 = lineData.GetBit(0);
		if(!GetControlRegisterD0(PORT2))
		{
			SetDataRegisterD0(PORT2, lineData.GetBit(0));
		}
		break;

	//Control port 3 interface
	case LINE_PORT3_TH:
		inputLineState[PORT3-PORT1].lineAssertedTH = lineData.GetBit(0);
		if(!GetControlRegisterTH(PORT3))
		{
			SetDataRegisterTH(PORT3, lineData.GetBit(0));
		}
		break;
	case LINE_PORT3_TR:
		inputLineState[PORT3-PORT1].lineAssertedTR = lineData.GetBit(0);
		if(!GetControlRegisterTR(PORT3))
		{
			SetDataRegisterTR(PORT3, lineData.GetBit(0));
		}
		break;
	case LINE_PORT3_TL:
		inputLineState[PORT3-PORT1].lineAssertedTL = lineData.GetBit(0);
		if(!GetControlRegisterTL(PORT3))
		{
			SetDataRegisterTL(PORT3, lineData.GetBit(0));
		}
		break;
	case LINE_PORT3_D3:
		inputLineState[PORT3-PORT1].lineAssertedD3 = lineData.GetBit(0);
		if(!GetControlRegisterD3(PORT3))
		{
			SetDataRegisterD3(PORT3, lineData.GetBit(0));
		}
		break;
	case LINE_PORT3_D2:
		inputLineState[PORT3-PORT1].lineAssertedD2 = lineData.GetBit(0);
		if(!GetControlRegisterD2(PORT3))
		{
			SetDataRegisterD2(PORT3, lineData.GetBit(0));
		}
		break;
	case LINE_PORT3_D1:
		inputLineState[PORT3-PORT1].lineAssertedD1 = lineData.GetBit(0);
		if(!GetControlRegisterD1(PORT3))
		{
			SetDataRegisterD1(PORT3, lineData.GetBit(0));
		}
		break;
	case LINE_PORT3_D0:
		inputLineState[PORT3-PORT1].lineAssertedD0 = lineData.GetBit(0);
		if(!GetControlRegisterD0(PORT3))
		{
			SetDataRegisterD0(PORT3, lineData.GetBit(0));
		}
		break;

	case LINE_JAP:
		SetOverseasFlag(!lineData.GetBit(0));
		break;
	case LINE_NTSC:
		SetPALFlag(!lineData.GetBit(0));
		break;
	case LINE_DISK:
		SetNoDiskFlag(!lineData.GetBit(0));
		break;
	}
}

//----------------------------------------------------------------------------------------
void A10000::ApplyPendingLineStateChanges(double currentTimesliceProgress)
{
	//If we have any pending line state changes waiting, apply any which we have now
	//reached.
	if(lineAccessPending)
	{
		bool done = false;
		while(!done)
		{
			//Remove the next line access that we've reached from the front of the line
			//access buffer
			std::list<LineAccess>::iterator i = lineAccessBuffer.begin();
			if((i == lineAccessBuffer.end()) || (i->accessTime > currentTimesliceProgress))
			{
				done = true;
				continue;
			}
			LineAccess lineAccess = *i;
			lineAccessBuffer.pop_front();
			lineAccessPending = !lineAccessBuffer.empty();

			//Apply the line state change
			ApplyLineStateChange(lineAccess.lineID, lineAccess.state);
		}
	}
}

//----------------------------------------------------------------------------------------
A10000::LineID A10000::GetLineIDForPort(unsigned int portNo, PortLine portLine) const
{
	switch(portNo)
	{
	case PORT1:
		switch(portLine)
		{
		case LINE_D0:
			return LINE_PORT1_D0;
		case LINE_D1:
			return LINE_PORT1_D1;
		case LINE_D2:
			return LINE_PORT1_D2;
		case LINE_D3:
			return LINE_PORT1_D3;
		case LINE_TL:
			return LINE_PORT1_TL;
		case LINE_TR:
			return LINE_PORT1_TR;
		case LINE_TH:
			return LINE_PORT1_TH;
		}
		break;
	case PORT2:
		switch(portLine)
		{
		case LINE_D0:
			return LINE_PORT2_D0;
		case LINE_D1:
			return LINE_PORT2_D1;
		case LINE_D2:
			return LINE_PORT2_D2;
		case LINE_D3:
			return LINE_PORT2_D3;
		case LINE_TL:
			return LINE_PORT2_TL;
		case LINE_TR:
			return LINE_PORT2_TR;
		case LINE_TH:
			return LINE_PORT2_TH;
		}
		break;
	case PORT3:
		switch(portLine)
		{
		case LINE_D0:
			return LINE_PORT3_D0;
		case LINE_D1:
			return LINE_PORT3_D1;
		case LINE_D2:
			return LINE_PORT3_D2;
		case LINE_D3:
			return LINE_PORT3_D3;
		case LINE_TL:
			return LINE_PORT3_TL;
		case LINE_TR:
			return LINE_PORT3_TR;
		case LINE_TH:
			return LINE_PORT3_TH;
		}
		break;
	}

	return (LineID)0;
}

//----------------------------------------------------------------------------------------
//Interrupt functions
//----------------------------------------------------------------------------------------
void A10000::UpdateHLInterruptState(IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Update the HL interrupt line state
	bool newHLLineState = (GetControlRegisterHL(PORT1) && !GetControlRegisterTH(PORT1) && GetDataRegisterTH(PORT1))
	                   || (GetControlRegisterHL(PORT2) && !GetControlRegisterTH(PORT2) && GetDataRegisterTH(PORT2))
	                   || (GetControlRegisterHL(PORT3) && !GetControlRegisterTH(PORT3) && GetDataRegisterTH(PORT3));
	if(newHLLineState != currentHLLineState)
	{
		currentHLLineState = newHLLineState;
		memoryBus->SetLineState(LINE_HL, Data(1, currentHLLineState), GetDeviceContext(), caller, accessTime, accessContext);
	}
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult A10000::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(accessMutex);

	//Trigger a system rollback if the device has been accessed out of order
	boost::mutex::scoped_lock lineLock(lineMutex);
	if(lastLineCheckTime > accessTime)
	{
		GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}
	lastLineCheckTime = accessTime;

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
		data = ReadDataRegister(caller, accessTime, accessContext, PORT1);
		break;
	case 0x02:
		data = ReadDataRegister(caller, accessTime, accessContext, PORT2);
		break;
	case 0x03:
		data = ReadDataRegister(caller, accessTime, accessContext, PORT3);
		break;
	case 0x04:
		data = ReadControlRegister(caller, accessTime, accessContext, PORT1);
		break;
	case 0x05:
		data = ReadControlRegister(caller, accessTime, accessContext, PORT2);
		break;
	case 0x06:
		data = ReadControlRegister(caller, accessTime, accessContext, PORT3);
		break;
	case 0x07:
		data = ReadTxDataRegister(caller, accessTime, accessContext, PORT1);
		break;
	case 0x08:
		data = ReadRxDataRegister(caller, accessTime, accessContext, PORT1);
		break;
	case 0x09:
		data = ReadSerialControlRegister(caller, accessTime, accessContext, PORT1);
		break;
	case 0x0A:
		data = ReadTxDataRegister(caller, accessTime, accessContext, PORT2);
		break;
	case 0x0B:
		data = ReadRxDataRegister(caller, accessTime, accessContext, PORT2);
		break;
	case 0x0C:
		data = ReadSerialControlRegister(caller, accessTime, accessContext, PORT2);
		break;
	case 0x0D:
		data = ReadTxDataRegister(caller, accessTime, accessContext, PORT3);
		break;
	case 0x0E:
		data = ReadRxDataRegister(caller, accessTime, accessContext, PORT3);
		break;
	case 0x0F:
		data = ReadSerialControlRegister(caller, accessTime, accessContext, PORT3);
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
	boost::mutex::scoped_lock lock(accessMutex);

	//##DEBUG##
	//InputLineState* inputLineStatePointer = &inputLineState[0];
	//if((location == 1) || (location == 4) || (location == 7) || (location == 8) || (location == 9))
	//{
	//	std::wcout << "A10000Write: " << std::hex << std::uppercase << location << '\t' << data.GetData() << '\t' << accessTime << '\t' << lastLineCheckTime << '\n';
	//}

	//Trigger a system rollback if the device has been accessed out of order
	boost::mutex::scoped_lock lineLock(lineMutex);
	if(lastLineCheckTime > accessTime)
	{
		GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}
	lastLineCheckTime = accessTime;

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
		WriteDataRegister(caller, accessTime, accessContext, PORT1, data);
		break;
	case 0x02:
		WriteDataRegister(caller, accessTime, accessContext, PORT2, data);
		break;
	case 0x03:
		WriteDataRegister(caller, accessTime, accessContext, PORT3, data);
		break;
	case 0x04:
		WriteControlRegister(caller, accessTime, accessContext, PORT1, data);
		break;
	case 0x05:
		WriteControlRegister(caller, accessTime, accessContext, PORT2, data);
		break;
	case 0x06:
		WriteControlRegister(caller, accessTime, accessContext, PORT3, data);
		break;
	case 0x07:
		WriteTxDataRegister(caller, accessTime, accessContext, PORT1, data);
		break;
	case 0x08:
		WriteRxDataRegister(caller, accessTime, accessContext, PORT1, data);
		break;
	case 0x09:
		WriteSerialControlRegister(caller, accessTime, accessContext, PORT1, data);
		break;
	case 0x0A:
		WriteTxDataRegister(caller, accessTime, accessContext, PORT2, data);
		break;
	case 0x0B:
		WriteRxDataRegister(caller, accessTime, accessContext, PORT2, data);
		break;
	case 0x0C:
		WriteSerialControlRegister(caller, accessTime, accessContext, PORT2, data);
		break;
	case 0x0D:
		WriteTxDataRegister(caller, accessTime, accessContext, PORT3, data);
		break;
	case 0x0E:
		WriteRxDataRegister(caller, accessTime, accessContext, PORT3, data);
		break;
	case 0x0F:
		WriteSerialControlRegister(caller, accessTime, accessContext, PORT3, data);
		break;
	}

	//##TODO## Re-implement support for the HL interrupt line
	//UpdateHLInterruptState(caller, accessTime, accessContext);
	return true;
}

//----------------------------------------------------------------------------------------
//Data register access
//----------------------------------------------------------------------------------------
Data A10000::ReadDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo) const
{
	//Return the current buffered data for each line
	return GetDataRegister(portNo);
}

//----------------------------------------------------------------------------------------
void A10000::WriteDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo, const Data& data)
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
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D0), Data(1, (unsigned int)newDataRegisterStateD0), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterD1(portNo) && (newDataRegisterStateD1 != GetDataRegisterD1(portNo)))
	{
		SetDataRegisterD1(portNo, newDataRegisterStateD1);
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D1), Data(1, (unsigned int)newDataRegisterStateD1), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterD2(portNo) && (newDataRegisterStateD2 != GetDataRegisterD2(portNo)))
	{
		SetDataRegisterD2(portNo, newDataRegisterStateD2);
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D2), Data(1, (unsigned int)newDataRegisterStateD2), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterD3(portNo) && (newDataRegisterStateD3 != GetDataRegisterD3(portNo)))
	{
		SetDataRegisterD3(portNo, newDataRegisterStateD3);
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D3), Data(1, (unsigned int)newDataRegisterStateD3), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterTL(portNo) && (newDataRegisterStateTL != GetDataRegisterTL(portNo)))
	{
		SetDataRegisterTL(portNo, newDataRegisterStateTL);
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TL), Data(1, (unsigned int)newDataRegisterStateTL), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterTR(portNo) && (newDataRegisterStateTR != GetDataRegisterTR(portNo)))
	{
		SetDataRegisterTR(portNo, newDataRegisterStateTR);
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TR), Data(1, (unsigned int)newDataRegisterStateTR), GetDeviceContext(), caller, accessTime, accessContext);
	}
	if(GetControlRegisterTH(portNo) && (newDataRegisterStateTH != GetDataRegisterTH(portNo)))
	{
		SetDataRegisterTH(portNo, newDataRegisterStateTH);
		controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TH), Data(1, (unsigned int)newDataRegisterStateTH), GetDeviceContext(), caller, accessTime, accessContext);
	}

	SetDataRegisterHL(portNo, newDataRegisterStateHL);
}

//----------------------------------------------------------------------------------------
//Control register access
//----------------------------------------------------------------------------------------
Data A10000::ReadControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo) const
{
	//Reads from the control register always return 0
	//##TODO## Verify on hardware
	return Data(8, 0);
}

//----------------------------------------------------------------------------------------
void A10000::WriteControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo, const Data& data)
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
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D0), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterD0(portNo, inputLineState[portNo].lineAssertedD0);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterD0(portNo))
			{
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D0), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
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
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D1), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterD1(portNo, inputLineState[portNo].lineAssertedD1);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterD1(portNo))
			{
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D1), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
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
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D2), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterD2(portNo, inputLineState[portNo].lineAssertedD2);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterD2(portNo))
			{
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D2), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
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
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D3), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterD3(portNo, inputLineState[portNo].lineAssertedD3);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterD3(portNo))
			{
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_D3), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
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
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TL), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterTL(portNo, inputLineState[portNo].lineAssertedTL);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterTL(portNo))
			{
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TL), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
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
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TR), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterTR(portNo, inputLineState[portNo].lineAssertedTR);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterTR(portNo))
			{
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TR), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
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
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TH), Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext);
			}

			//Set the contents of the data register for this line to the current input
			//line state
			SetDataRegisterTH(portNo, inputLineState[portNo].lineAssertedTH);
		}
		else
		{
			//if we're currently set to assert this output line, assert it now.
			if(GetDataRegisterTH(portNo))
			{
				controlPortBus->SetLineState(GetLineIDForPort(portNo, LINE_TH), Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
			}
		}
	}

	//Update the HL output line enable state
	SetControlRegisterHL(portNo, newControlRegisterStateHL);
}

//----------------------------------------------------------------------------------------
//Serial control register access
//----------------------------------------------------------------------------------------
Data A10000::ReadSerialControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo) const
{
	return GetSerialControlRegister(portNo);
}

//----------------------------------------------------------------------------------------
void A10000::WriteSerialControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo, const Data& data)
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
Data A10000::ReadTxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo) const
{
	//According to gen-hw.txt, it would seem that this port returns the last value
	//written when serial output is not enabled. We have not yet confirmed the correct
	//behaviour when serial output is enabled.
	//##TODO## Find out what to return when reading this port
	return GetTxDataRegister(portNo);
}

//----------------------------------------------------------------------------------------
void A10000::WriteTxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo, const Data& data)
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
Data A10000::ReadRxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo) const
{
	return rxDataRegisters[portNo];
}

//----------------------------------------------------------------------------------------
void A10000::WriteRxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo, const Data& data)
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
			unsigned int portNo;
			portNumberAttribute->ExtractHexValue(portNo);
			if(portNo < controlPortCount)
			{
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
							(*i)->ExtractData(inputLineState[portNo].lineAssertedD0);
						}
						else if(lineName == L"D1")
						{
							(*i)->ExtractData(inputLineState[portNo].lineAssertedD1);
						}
						else if(lineName == L"D2")
						{
							(*i)->ExtractData(inputLineState[portNo].lineAssertedD2);
						}
						else if(lineName == L"D3")
						{
							(*i)->ExtractData(inputLineState[portNo].lineAssertedD3);
						}
						else if(lineName == L"TL")
						{
							(*i)->ExtractData(inputLineState[portNo].lineAssertedTL);
						}
						else if(lineName == L"TR")
						{
							(*i)->ExtractData(inputLineState[portNo].lineAssertedTR);
						}
						else if(lineName == L"TH")
						{
							(*i)->ExtractData(inputLineState[portNo].lineAssertedTH);
						}
					}
				}
			}
		}
		else if((*i)->GetName() == L"VersionRegister")
		{
			versionRegister = (*i)->ExtractHexData<unsigned int>();
		}
		else if((*i)->GetName() == L"InputHardwareVersion")
		{
			inputHardwareVersion = (*i)->ExtractHexData<unsigned int>();
		}
		else if((*i)->GetName() == L"HLLineState")
		{
			currentHLLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"LastTimesliceLength")
		{
			lastTimesliceLength = (*i)->ExtractData<bool>();
		}
		//Restore the lineAccessBuffer state
		else if((*i)->GetName() == L"LineAccessBuffer")
		{
			lineAccessBuffer.clear();
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
						unsigned int lineID = GetLineID(lineName.c_str());
						if(lineID != 0)
						{
							Data lineState(GetLineWidth(lineID));
							lineStateAttribute->ExtractValue(lineState);
							LineAccess lineAccess(lineID, lineState, accessTime);

							//Find the correct location in the list to insert the entry. The
							//list must be sorted from earliest to latest.
							std::list<LineAccess>::reverse_iterator j = lineAccessBuffer.rbegin();
							while((j != lineAccessBuffer.rend()) && (j->accessTime > lineAccess.accessTime))
							{
								++j;
							}
							lineAccessBuffer.insert(j.base(), lineAccess);
						}
					}
				}
			}
			lineAccessPending = !lineAccessBuffer.empty();
		}
	}
}

//----------------------------------------------------------------------------------------
void A10000::SaveState(IHierarchicalStorageNode& node) const
{
	for(unsigned int i = 0; i < controlPortCount; ++i)
	{
		node.CreateChildHex(L"DataRegister", GetDataRegister(i).GetData(), GetDataRegister(i).GetHexCharCount()).CreateAttribute(L"PortNumber", i);
		node.CreateChildHex(L"ControlRegister", GetControlRegister(i).GetData(), GetControlRegister(i).GetHexCharCount()).CreateAttribute(L"PortNumber", i);
		node.CreateChildHex(L"SerialControlRegister", GetSerialControlRegister(i).GetData(), GetSerialControlRegister(i).GetHexCharCount()).CreateAttribute(L"PortNumber", i);
		node.CreateChildHex(L"TxDataRegister", GetTxDataRegister(i).GetData(), GetTxDataRegister(i).GetHexCharCount()).CreateAttribute(L"PortNumber", i);
		node.CreateChildHex(L"RxDataRegister", GetRxDataRegister(i).GetData(), GetRxDataRegister(i).GetHexCharCount()).CreateAttribute(L"PortNumber", i);

		node.CreateChild(L"LineAsserted", inputLineState[i].lineAssertedD0).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "D0");
		node.CreateChild(L"LineAsserted", inputLineState[i].lineAssertedD1).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "D1");
		node.CreateChild(L"LineAsserted", inputLineState[i].lineAssertedD2).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "D2");
		node.CreateChild(L"LineAsserted", inputLineState[i].lineAssertedD3).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "D3");
		node.CreateChild(L"LineAsserted", inputLineState[i].lineAssertedTL).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "TL");
		node.CreateChild(L"LineAsserted", inputLineState[i].lineAssertedTR).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "TR");
		node.CreateChild(L"LineAsserted", inputLineState[i].lineAssertedTH).CreateAttribute(L"PortNumber", i).CreateAttribute(L"LineName", "TH");
	}
	node.CreateChildHex(L"VersionRegister", versionRegister.GetData(), versionRegister.GetHexCharCount());
	node.CreateChildHex(L"InputHardwareVersion", inputHardwareVersion, 1);
	node.CreateChild(L"HLLineState", currentHLLineState);
	node.CreateChild(L"LastTimesliceLength", lastTimesliceLength);

	//Save the lineAccessBuffer state
	if(lineAccessPending)
	{
		IHierarchicalStorageNode& lineAccessState = node.CreateChild(L"LineAccessBuffer");
		for(std::list<LineAccess>::const_iterator i = lineAccessBuffer.begin(); i != lineAccessBuffer.end(); ++i)
		{
			IHierarchicalStorageNode& lineAccessEntry = lineAccessState.CreateChild(L"LineAccess");
			lineAccessEntry.CreateAttribute(L"LineName", GetLineName(i->lineID));
			lineAccessEntry.CreateAttribute(L"LineState", i->state);
			lineAccessEntry.CreateAttribute(L"AccessTime", i->accessTime);
		}
	}
}
