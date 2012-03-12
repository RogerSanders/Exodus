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
	busLocked = false;
	reset = true;
	initialized = false;

	bbusLocked = busLocked;
	breset = reset;
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool A11100::AddReference(const wchar_t* referenceName, IBusInterface* target)
{
	std::wstring referenceNameString = referenceName;
	if(referenceNameString == L"BusInterface")
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
	busLocked = bbusLocked;
	reset = breset;
}

//----------------------------------------------------------------------------------------
void A11100::ExecuteCommit()
{
	bbusLocked = busLocked;
	breset = reset;
}

//----------------------------------------------------------------------------------------
Device::UpdateMethod A11100::GetUpdateMethod() const
{
	return Device::UPDATEMETHOD_TIMESLICE;
}

//----------------------------------------------------------------------------------------
void A11100::ExecuteTimeslice(double nanoseconds)
{
	//This controller needs to power up asserting the RESET line. Since we can't access
	//another device during initialization, we implement the access here.
	if(!initialized)
	{
		Data lineData(1, 0);
		lineData.SetBit(0, true);
		memoryBus->SetLine(LINE_RESET, lineData, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress());
		initialized = true;
	}
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult A11100::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime)
{
	//Return true if the Z80 bus is not accessible by the M68000
	data.SetBit(0, (reset || !busLocked));
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult A11100::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime)
{
	if(location == 0x100)
	{
		//Reset
		bool state = !data.GetBit(0);

		//##DEBUG##
//		std::wstringstream message;
//		message << "Z80 reset state: " << state << '\n';
//		std::wcout << message.str();

		reset = state;

		Data lineData(1, 0);
		lineData.SetBit(0, state);
		memoryBus->SetLine(LINE_RESET, lineData, GetDeviceContext(), caller, accessTime);
	}
	if(location == 0)
	{
		//Busreq
		bool state = !data.GetBit(0);

		//##DEBUG##
//		std::wstringstream message;
//		message << "Z80 bus running: " << state << '\n';
//		std::wcout << message.str();

		busLocked = !state;

		Data lineData(1, 0);
		lineData.SetBit(0, busLocked);
		memoryBus->SetLine(LINE_BUSREQ, lineData, GetDeviceContext(), caller, accessTime);
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int A11100::GetLineID(const wchar_t* lineName) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"RESET")
	{
		return LINE_RESET;
	}
	else if(lineNameString == L"BUSREQ")
	{
		return LINE_BUSREQ;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
const wchar_t* A11100::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_RESET:
		return L"RESET";
	case LINE_BUSREQ:
		return L"BUSREQ";
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
	}
	return 0;
}
