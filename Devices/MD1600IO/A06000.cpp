#include "A06000.h"
//##DEBUG##
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
A06000::A06000(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(aimplementationName, ainstanceName, amoduleID)
{
	m68kMemoryBus = 0;
	z80MemoryBus = 0;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool A06000::ValidateDevice()
{
	//Ensure both memory bus bindings have been successfully made
	return (m68kMemoryBus != 0) && (z80MemoryBus != 0);
}

//----------------------------------------------------------------------------------------
void A06000::Initialize()
{
	//##TODO## Check what area the window is mapped to when the system is first powered up
	bitsWritten = 0;
	newMappingPos = 0;
	currentMappingPos = 0xFF8000;
	bbitsWritten = 0;
	bnewMappingPos = 0;
	bcurrentMappingPos = 0xFF8000;
	SetMappingPos(currentMappingPos);
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool A06000::AddReference(const std::wstring& referenceName, IBusInterface* target)
{
	if(referenceName == L"M68000Bus")
	{
		m68kMemoryBus = target;
	}
	else if(referenceName == L"Z80Bus")
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
bool A06000::RemoveReference(IBusInterface* target)
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
void A06000::ExecuteRollback()
{
	bitsWritten = bbitsWritten;
	newMappingPos = bnewMappingPos;
	if(currentMappingPos != bcurrentMappingPos)
	{
		SetMappingPos(bcurrentMappingPos);
	}
	currentMappingPos = bcurrentMappingPos;
}

//----------------------------------------------------------------------------------------
void A06000::ExecuteCommit()
{
	bbitsWritten = bitsWritten;
	bnewMappingPos = newMappingPos;
	bcurrentMappingPos = currentMappingPos;
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult A06000::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//##TODO## Perform comprehensive tests on the hardware to determine exactly how access
	//is forwarded over the bus, and determine from that which CE lines are asserted. This
	//will help us correctly determine how the hardware behaves from any write. In
	//particular, if we're really getting 0xFF returned from all attempted reads from M68K
	//RAM, we need to determine where this value is coming from, whether internally or
	//from another device. We also need to determine if it's possible to lock up the
	//system by attempting to access unmapped areas.
	//##TODO## New tests performed on spritesmind indicate that the Z80 can reliably read
	//and write to and from M68000 RAM. This makes more sense, but needs to be confirmed
	//through hardware tests.
	if(interfaceNumber == 1)
	{
		data = 0xFFFF;
	}
	else
	{
		if((currentMappingPos + location) < 0x400000)
		{
			m68kMemoryBus->ReadMemory(currentMappingPos + location, data, caller, accessTime, accessContext);
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult A06000::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	if(interfaceNumber == 1)
	{
		//##TODO## Add a comment here describing what we're doing, and how the mapping
		//hardware works.
		//##TODO## Add debug windows which allow the current mapping position to be
		//viewed and modified.
		//##TODO## Add a logging option which allows all changes to the mapping window to
		//be logged and viewed.
		//##TODO## Make newMappingPos use the Data class, and restructure this code to use
		//the GetBit and SetBit functions.
		newMappingPos |= (data.GetDataSegment(0, 1) << (15 + bitsWritten));
		++bitsWritten;
		if(bitsWritten == 9)
		{
			SetMappingPos(newMappingPos);
			currentMappingPos = newMappingPos;
			newMappingPos = 0;
			bitsWritten = 0;
		}
	}
	else
	{
		if((currentMappingPos + location) < 0x400000)
		{
			//##TODO## Figure out why this is disabled
//			m68kMemoryBus->WriteMemory(currentMappingPos + location, data, caller, accessTime, accessContext);
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Memory map functions
//----------------------------------------------------------------------------------------
//##TODO## Clean this up. Now that we actually correctly bridge the M68K and Z80 buses
//during execution, we don't need this hack.
void A06000::SetMappingPos(unsigned int mappingPos)
{
//	MemoryBus* z80MemoryBus = GetDeviceContext()->GetSystem()->GetDevice(L"Z80")->GetMemoryBus();
////	z80MemoryBus->UnmapDevice(0x8000);
//	if(mappingPos < 0x400000)
//	{
//		z80MemoryBus->MapDevice(GetDeviceContext()->GetSystem()->GetDevice(L"ROM"), 0x8000, 0x8000, ~0, 0, 0, mappingPos);
//	}
//	//##NOTE## According to tests done by Stef and posted on spritesmind.net, no access
//	//from the Z80 to 68K RAM is possible. Current notes indicating it is possible would
//	//appear to be misinformation. Writes are ignored and reads return 0xFF. See
//	//http://www.spritesmind.net/_GenDev/forum/viewtopic.php?t=347
//	//else if(mappingPos >= 0xFF0000)
//	//{
//	//	z80MemoryBus->MapDevice(L"RAM", 0x8000, 0x8000, mappingPos - 0xFF0000);
//	//}
//	else
//	{
//		//##DEBUG##
//		z80MemoryBus->MapDevice(GetDeviceContext()->GetSystem()->GetDevice(L"Test Stub"), 0x8000, 0x8000, ~0, 0, 0, mappingPos);
////		std::wcout << "WARNING: Z80 window set to unsupported position 0x" << std::setw(6) << std::setfill(L'0') << std::hex << std::uppercase << mappingPos << '\n';
//	}

//	LogEntry logEntry(LogEntry::EVENTLEVEL_DEBUG);
//	logEntry << "Z80 window set to 0x" << std::setw(6) << std::setfill(L'0') << std::hex << std::uppercase << mappingPos;
//	GetDeviceContext()->WriteLogEvent(logEntry);

	//	std::wstringstream entry;
//	entry << "Z80 window set to 0x" << std::setw(6) << std::setfill(L'0') << std::hex << std::uppercase << mappingPos;
	//##DEBUG##
//	std::wcout << entry.str() << '\n';
//	GetDeviceContext()->WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_DEBUG, entry.str()));
}
