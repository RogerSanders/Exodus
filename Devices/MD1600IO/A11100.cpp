#include "A11100.h"
//##DEBUG##
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
A11100::A11100(const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(L"A11100", ainstanceName, amoduleID)
{
	memoryBus = 0;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool A11100::ValidateDevice()
{
	return (memoryBus != 0);
}

//----------------------------------------------------------------------------------------
void A11100::Initialize()
{
	initialized = false;

	lastLineCheckTime = 0;
	lineAccessPending = false;
	lastTimesliceLength = 0;
	lineAccessBuffer.clear();

	busRequested = false;
	busGranted = false;
	//Note that the reset line is asserted on power-on
	reset = true;
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool A11100::AddReference(const std::wstring& referenceName, IBusInterface* target)
{
	if(referenceName == L"BusInterface")
	{
		memoryBus = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool A11100::RemoveReference(IBusInterface* target)
{
	if(memoryBus == target)
	{
		memoryBus = 0;
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
void A11100::ExecuteRollback()
{
	initialized = binitialized;

	lastTimesliceLength = blastTimesliceLength;
	lineAccessBuffer = blineAccessBuffer;
	lineAccessPending = !lineAccessBuffer.empty();

	busRequested = bbusRequested;
	busGranted = bbusGranted;
	reset = breset;
}

//----------------------------------------------------------------------------------------
void A11100::ExecuteCommit()
{
	binitialized = initialized;

	blastTimesliceLength = lastTimesliceLength;
	if(lineAccessPending)
	{
		blineAccessBuffer = lineAccessBuffer;
	}
	else
	{
		blineAccessBuffer.clear();
	}

	bbusRequested = busRequested;
	bbusGranted = busGranted;
	breset = reset;
}

//----------------------------------------------------------------------------------------
bool A11100::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void A11100::NotifyUpcomingTimeslice(double nanoseconds)
{
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
bool A11100::SendNotifyAfterExecuteCalled() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void A11100::NotifyAfterExecuteCalled()
{
	//Ensure that any pending line state changes which we have passed in this timeslice
	//are applied
	ApplyPendingLineStateChanges(lastTimesliceLength);
}

//----------------------------------------------------------------------------------------
Device::UpdateMethod A11100::GetUpdateMethod() const
{
	return Device::UPDATEMETHOD_TIMESLICE;
}

//----------------------------------------------------------------------------------------
void A11100::ExecuteTimeslice(double nanoseconds)
{}

//----------------------------------------------------------------------------------------
void A11100::ExecuteTimesliceTimingPointStep(unsigned int accessContext)
{
	if(!initialized)
	{
		memoryBus->SetLineState(LINE_RESET, Data(1, (unsigned int)reset), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		initialized = true;
	}
}

//----------------------------------------------------------------------------------------
double A11100::GetNextTimingPointInDeviceTime(unsigned int& accessContext) const
{
	//This controller needs to power up asserting the RESET line. Since we can't access
	//another device during initialization, we generate a timing point immediately after
	//system construction to perform the task.
	return (!initialized? 0.0: -1.0);
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult A11100::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(accessMutex);

	//Apply any changes to the line state that are pending at this time
	ApplyPendingLineStateChanges(accessTime);

	//Return true if the Z80 bus is not accessible by the M68000
	data.SetBit(0, (reset || !busGranted));
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult A11100::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(accessMutex);

	//Apply any changes to the line state that are pending at this time
	ApplyPendingLineStateChanges(accessTime);

	if(location == 0x100)
	{
		//Reset
		bool newState = !data.GetBit(0);
		if(reset != newState)
		{
			reset = newState;
			memoryBus->SetLineState(LINE_RESET, Data(1, (unsigned int)reset), GetDeviceContext(), caller, accessTime, accessContext);
		}
	}
	if(location == 0)
	{
		//Busreq
		bool newState = data.GetBit(0);
		if(busRequested != newState)
		{
			busRequested = newState;
			memoryBus->SetLineState(LINE_BUSREQ, Data(1, (unsigned int)busRequested), GetDeviceContext(), caller, accessTime, accessContext);
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int A11100::GetLineID(const std::wstring& lineName) const
{
	if(lineName == L"RESET")
	{
		return LINE_RESET;
	}
	else if(lineName == L"BUSREQ")
	{
		return LINE_BUSREQ;
	}
	else if(lineName == L"BUSACK")
	{
		return LINE_BUSACK;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
std::wstring A11100::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_RESET:
		return L"RESET";
	case LINE_BUSREQ:
		return L"BUSREQ";
	case LINE_BUSACK:
		return L"BUSACK";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int A11100::GetLineWidth(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_RESET:
		return 1;
	case LINE_BUSREQ:
		return 1;
	case LINE_BUSACK:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void A11100::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(lineMutex);

	//Flag that an entry exists in the buffer. This flag is used to skip the expensive
	//locking operation in the active thread for this device when no line changes are
	//pending. Note that we set this flag before we've actually written the entry into
	//the buffer, as we want to force the active thread to lock on the beginning of the
	//next cycle while this function is executing, so that the current timeslice progress
	//of the device doesn't change after we've read it.
	lineAccessPending = true;

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(lastLineCheckTime > accessTime)
	{
		GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//Insert the line access into the buffer. Note that entries in the buffer are sorted
	//by access time from lowest to highest.
	std::list<LineAccess>::reverse_iterator i = lineAccessBuffer.rbegin();
	while((i != lineAccessBuffer.rend()) && (i->accessTime > accessTime))
	{
		++i;
	}
	lineAccessBuffer.insert(i.base(), LineAccess(targetLine, lineData, accessTime));
}

//----------------------------------------------------------------------------------------
void A11100::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{
	SetLineState(targetLine, lineData, 0, 0.0, 0);
}

//----------------------------------------------------------------------------------------
void A11100::RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext)
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
	}

	//Update the lineAccessPending flag
	lineAccessPending = !lineAccessBuffer.empty();
}

//----------------------------------------------------------------------------------------
void A11100::AssertCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(reset)        memoryBus->SetLineState(LINE_RESET, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(busRequested) memoryBus->SetLineState(LINE_BUSREQ, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void A11100::NegateCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(reset)        memoryBus->SetLineState(LINE_RESET, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(busRequested) memoryBus->SetLineState(LINE_BUSREQ, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void A11100::ApplyLineStateChange(unsigned int targetLine, const Data& lineData)
{
	switch(targetLine)
	{
	case LINE_BUSACK:
		busGranted = lineData.NonZero();
		break;
	}
}

//----------------------------------------------------------------------------------------
void A11100::ApplyPendingLineStateChanges(double accessTime)
{
	//If we have any pending line state changes waiting, apply any which we have now
	//reached.
	if(lineAccessPending)
	{
		boost::mutex::scoped_lock lock(lineMutex);
		double currentTimesliceProgress = accessTime;
		bool done = false;
		std::list<LineAccess>::iterator i = lineAccessBuffer.begin();
		while(!done && (i != lineAccessBuffer.end()))
		{
			if(i->accessTime <= currentTimesliceProgress)
			{
				ApplyLineStateChange(i->lineID, i->state);
				++i;
			}
			else
			{
				done = true;
			}
		}

		//Clear any completed entries from the list
		lineAccessBuffer.erase(lineAccessBuffer.begin(), i);
		lineAccessPending = !lineAccessBuffer.empty();
	}
	lastLineCheckTime = accessTime;
}
