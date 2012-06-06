#include "MDBusArbiter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MDBusArbiter::MDBusArbiter(const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(L"MDBusArbiter", ainstanceName, amoduleID)
{
	m68kMemoryBus = 0;
	z80MemoryBus = 0;

	//Initialize our CE line state
	ceLineMaskReadHighWriteLow = 0;
	ceLineMaskUDS = 0;
	ceLineMaskLDS = 0;
	ceLineMaskOE0 = 0;
	ceLineMaskCE0 = 0;
	ceLineMaskROM = 0;
	ceLineMaskASEL = 0;
	ceLineMaskFDC = 0;
	ceLineMaskFDWR = 0;
	ceLineMaskTIME = 0;
	ceLineMaskIO = 0;
	ceLineMaskEOE = 0;
	ceLineMaskNOE = 0;
	ceLineMaskZRAM = 0;
	ceLineMaskSOUND = 0;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool MDBusArbiter::ValidateDevice()
{
	return (m68kMemoryBus != 0) && (z80MemoryBus != 0) && (ceLineMaskReadHighWriteLow != 0) && (ceLineMaskOE0 != 0) && (ceLineMaskUDS != 0) && (ceLineMaskLDS != 0);
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::Initialize()
{
	//##TODO## Change this back once we actually have an external line implemented which
	//asserts the CART_IN line.
//	cartIn = false;
	cartIn = true;
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool MDBusArbiter::AddReference(const wchar_t* referenceName, IBusInterface* target)
{
	std::wstring referenceNameString = referenceName;
	if(referenceNameString == L"M68000Bus")
	{
		m68kMemoryBus = target;
	}
	else if(referenceNameString == L"Z80Bus")
	{
		z80MemoryBus = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool MDBusArbiter::RemoveReference(IBusInterface* target)
{
	if(m68kMemoryBus == target)
	{
		m68kMemoryBus = 0;
	}
	else if(z80MemoryBus == target)
	{
		z80MemoryBus = 0;
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
void MDBusArbiter::ExecuteRollback()
{
	cartIn = bcartIn;
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::ExecuteCommit()
{
	bcartIn = cartIn;
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult MDBusArbiter::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//If the M68000 is performing an interrupt acknowledge cycle, assert VPA to instruct
	//it to autovector the interrupt, and assert the INTAK line to instruct the VDP to
	//negate the IPL lines. We return false to this read access, since we didn't respond
	//to the read request for the interrupt vector number itself (didn't assert DTACK),
	//but we asserted VPA instead, which also terminates this bus cycle.
	if(interfaceNumber == 1)
	{
		m68kMemoryBus->SetLine(LINE_VPA, Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
		m68kMemoryBus->SetLine(LINE_INTAK, Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext);
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int MDBusArbiter::GetCELineID(const wchar_t* lineName, bool inputLine) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"R/W")
	{
		return CELINE_RW;
	}
	else if(lineNameString == L"OE0")
	{
		return CELINE_OE0;
	}
	else if(lineNameString == L"UDS")
	{
		return CELINE_UDS;
	}
	else if(lineNameString == L"LDS")
	{
		return CELINE_LDS;
	}
	else if(lineNameString == L"CE0")
	{
		return CELINE_CE0;
	}
	else if(lineNameString == L"ROM")
	{
		return CELINE_ROM;
	}
	else if(lineNameString == L"ASEL")
	{
		return CELINE_ASEL;
	}
	else if(lineNameString == L"FDC")
	{
		return CELINE_FDC;
	}
	else if(lineNameString == L"FDWR")
	{
		return CELINE_FDWR;
	}
	else if(lineNameString == L"TIME")
	{
		return CELINE_TIME;
	}
	else if(lineNameString == L"IO")
	{
		return CELINE_IO;
	}
	else if(lineNameString == L"EOE")
	{
		return CELINE_EOE;
	}
	else if(lineNameString == L"NOE")
	{
		return CELINE_NOE;
	}
	else if(lineNameString == L"ZRAM")
	{
		return CELINE_ZRAM;
	}
	else if(lineNameString == L"SOUND")
	{
		return CELINE_SOUND;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch(lineID)
	{
	case CELINE_RW:
		ceLineMaskReadHighWriteLow = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_OE0:
		ceLineMaskOE0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_UDS:
		ceLineMaskUDS = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_LDS:
		ceLineMaskLDS = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch(lineID)
	{
	case CELINE_CE0:
		ceLineMaskCE0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_ROM:
		ceLineMaskROM = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_ASEL:
		ceLineMaskASEL = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_FDC:
		ceLineMaskFDC = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_FDWR:
		ceLineMaskFDWR = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_TIME:
		ceLineMaskTIME = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_IO:
		ceLineMaskIO = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_EOE:
		ceLineMaskEOE = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_NOE:
		ceLineMaskNOE = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_ZRAM:
		ceLineMaskZRAM = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_SOUND:
		ceLineMaskSOUND = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
unsigned int MDBusArbiter::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, double accessTime) const
{
	unsigned int result = 0;
	if(sourceBusInterface == m68kMemoryBus)
	{
		bool operationIsWrite = (currentCELineState & ceLineMaskReadHighWriteLow) == 0;
		bool ceLineUDS = (currentCELineState & ceLineMaskUDS) != 0;
		bool ceLineLDS = (currentCELineState & ceLineMaskLDS) != 0;
		bool ceLineOE0 = (currentCELineState & ceLineMaskOE0) != 0;
		result = BuildCELineM68K(location, operationIsWrite, ceLineUDS, ceLineLDS, ceLineOE0, cartIn);
	}
	else if(sourceBusInterface == z80MemoryBus)
	{
		result = BuildCELineZ80(location);
	}
	return result;
}

//----------------------------------------------------------------------------------------
unsigned int MDBusArbiter::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller) const
{
	unsigned int result = 0;
	if(sourceBusInterface == m68kMemoryBus)
	{
		bool operationIsWrite = (currentCELineState & ceLineMaskReadHighWriteLow) == 0;
		bool ceLineUDS = (currentCELineState & ceLineMaskUDS) != 0;
		bool ceLineLDS = (currentCELineState & ceLineMaskLDS) != 0;
		bool ceLineOE0 = (currentCELineState & ceLineMaskOE0) != 0;
		result = BuildCELineM68K(location, operationIsWrite, ceLineUDS, ceLineLDS, ceLineOE0, cartIn);
	}
	else if(sourceBusInterface == z80MemoryBus)
	{
		result = BuildCELineZ80(location);
	}
	return result;
}

//----------------------------------------------------------------------------------------
unsigned int MDBusArbiter::BuildCELineM68K(unsigned int targetAddress, bool write, bool ceLineUDS, bool ceLineLDS, bool ceLineOE0, bool cartInLineAsserted) const
{
	//##TODO## It seems clear that if the FC lines from the M68000 indicate a CPU space
	//cycle (all asserted), then these CE output lines shouldn't be asserted. In
	//particular, EOE and NOE can't be asserted, otherwise the RAM would respond to an
	//interrupt vector request in the Mega Drive, since the RAS0 line would still be
	//asserted by the VDP, which doesn't know what the FC lines are outputting.
	//##TODO## Attempt to improve performance of CE line state generation, using a lookup
	//table containing pre-calculated ceLineState values for every address and input line
	//combination.

	//Calculate the state of all the various CE lines
	bool lineCE0 = cartInLineAsserted? (targetAddress <= 0x3FFFFF): (targetAddress >= 0x400000) && (targetAddress <= 0x7FFFFF);
	bool lineROM = !cartInLineAsserted? (targetAddress <= 0x1FFFFF): (targetAddress >= 0x400000) && (targetAddress <= 0x5FFFFF);
	bool lineASEL = (targetAddress <= 0x7FFFFF);
	bool lineFDC = (targetAddress >= 0xA12000) && (targetAddress <= 0xA120FF);
	bool lineFDWR = write && lineFDC;
	bool lineTIME = (targetAddress >= 0xA13000) && (targetAddress <= 0xA130FF);
	bool lineIO = (targetAddress >= 0xA10000) && (targetAddress <= 0xA100FF);
	bool lineEOE = ceLineOE0 && ceLineUDS;
	bool lineNOE = ceLineOE0 && ceLineLDS;

	//##TODO## Confirm the mapping of CAS2 and RAS2, and implement them here.
	//bool lineCAS2 = (targetAddress <= 0x7FFFFF);
	//bool lineRAS2 = (targetAddress >= 0xE00000) && (targetAddress <= 0xFFFFFF);

	//Build the actual CE line state based on the asserted CE lines
	unsigned int ceLineState = 0;
	ceLineState |= lineCE0? ceLineMaskCE0: 0x0;
	ceLineState |= lineROM? ceLineMaskROM: 0x0;
	ceLineState |= lineASEL? ceLineMaskASEL: 0x0;
	ceLineState |= lineFDC? ceLineMaskFDC: 0x0;
	ceLineState |= lineFDWR? ceLineMaskFDWR: 0x0;
	ceLineState |= lineTIME? ceLineMaskTIME: 0x0;
	ceLineState |= lineIO? ceLineMaskIO: 0x0;
	ceLineState |= lineEOE? ceLineMaskEOE: 0x0;
	ceLineState |= lineNOE? ceLineMaskNOE: 0x0;

	//Return the generated CE line state
	return ceLineState;
}

//----------------------------------------------------------------------------------------
unsigned int MDBusArbiter::BuildCELineZ80(unsigned int targetAddress) const
{
	//Calculate the state of all the various CE lines
	bool lineZRAM = (targetAddress <= 0x3FFF);
	bool lineSOUND = (targetAddress >= 0x4000) && (targetAddress <= 0x5FFF);

	//Build the actual CE line state based on the asserted CE lines
	unsigned int ceLineState = 0;
	ceLineState |= lineZRAM? ceLineMaskZRAM: 0x0;
	ceLineState |= lineSOUND? ceLineMaskSOUND: 0x0;

	//Return the generated CE line state
	return ceLineState;
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int MDBusArbiter::GetLineID(const wchar_t* lineName) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"CARTIN")
	{
		return LINE_CARTIN;
	}
	else if(lineNameString == L"VPA")
	{
		return LINE_VPA;
	}
	else if(lineNameString == L"INTAK")
	{
		return LINE_INTAK;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
const wchar_t* MDBusArbiter::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_CARTIN:
		return L"CARTIN";
	case LINE_VPA:
		return L"VPA";
	case LINE_INTAK:
		return L"INTAK";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int MDBusArbiter::GetLineWidth(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_CARTIN:
		return 1;
	case LINE_VPA:
		return 1;
	case LINE_INTAK:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(lineMutex);

	//Read the time at which this access is being made, and trigger a rollback if we've
	//already passed that time.
	if(lastLineCheckTime > accessTime)
	{
		GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}

	//##TODO## Implement this function
}
