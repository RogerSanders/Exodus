#include "MDBusArbiter.h"
//##DEBUG##
#include <iostream>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MDBusArbiter::MDBusArbiter(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(aimplementationName, ainstanceName, amoduleID),
z80BankswitchDataCurrent(9),
bz80BankswitchDataCurrent(9),
z80BankswitchDataNew(9),
bz80BankswitchDataNew(9)
{
	bootROM = 0;
	m68kMemoryBus = 0;
	z80MemoryBus = 0;

	//Initialize our CE line state
	ceLineMaskReadHighWriteLow = 0;
	ceLineMaskUDS = 0;
	ceLineMaskLDS = 0;
	ceLineMaskOE0 = 0;
	ceLineMaskCE0 = 0;
	ceLineMaskBootROM = 0;
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
	lastLineCheckTime = 0;
	lineAccessPending = false;
	lastTimesliceLength = 0;
	lineAccessBuffer.clear();

	//Initialize the device settings
	activateTMSS = false;
	activateBootROM = false;

	//Initialize the TMSS Security settings
	bootROMEnabled = true;
	vdpLockoutActive = true;
	vdpLockoutTripped = false;

	//Initialize the Z80 bankswitch register state
	z80BankswitchBitsWritten = 0;
	z80BankswitchDataCurrent = 0;
	z80BankswitchDataNew = 0;

	//Initialize the external line state
	cartInLineState = false;
	z80BusRequestLineState = false;
	z80BusGrantLineState = false;
	//Note that the Z80 reset line is asserted when this device is initialized
	z80BusResetLineState = true;
	m68kBusRequestLineState = false;
	m68kBusGrantLineState = false;
	vresLineState = false;
	haltLineState = false;
	sresLineState = false;
	wresLineState = false;

	z80BusRequestLineStateChangeTimeLatchEnable = false;
	z80BusGrantLineStateChangeTimeLatchEnable = false;
	z80BusResetLineStateChangeTimeLatchEnable = false;
	m68kBusRequestLineStateChangeTimeLatchEnable = false;
	m68kBusGrantLineStateChangeTimeLatchEnable = false;
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool MDBusArbiter::AddReference(const std::wstring& referenceName, IDevice* target)
{
	if(referenceName == L"BootROM")
	{
		bootROM = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool MDBusArbiter::AddReference(const std::wstring& referenceName, IBusInterface* target)
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
bool MDBusArbiter::RemoveReference(IDevice* target)
{
	if(bootROM == target)
	{
		bootROM = 0;
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
	lastTimesliceLength = blastTimesliceLength;
	lineAccessBuffer = blineAccessBuffer;
	lineAccessPending = !lineAccessBuffer.empty();

	activateTMSS = bactivateTMSS;
	activateBootROM = bactivateBootROM;

	bootROMEnabled = bbootROMEnabled;
	vdpLockoutActive = bvdpLockoutActive;
	vdpLockoutTripped = bvdpLockoutTripped;

	z80BankswitchDataCurrent = bz80BankswitchDataCurrent;
	z80BankswitchDataNew = bz80BankswitchDataNew;
	z80BankswitchBitsWritten = bz80BankswitchBitsWritten;

	cartInLineState = bcartInLineState;
	z80BusRequestLineState = bz80BusRequestLineState;
	z80BusGrantLineState = bz80BusGrantLineState;
	z80BusResetLineState = bz80BusResetLineState;
	m68kBusRequestLineState = bm68kBusRequestLineState;
	m68kBusGrantLineState = bm68kBusGrantLineState;
	vresLineState = bvresLineState;
	haltLineState = bhaltLineState;
	sresLineState = bsresLineState;
	wresLineState = bwresLineState;
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::ExecuteCommit()
{
	blastTimesliceLength = lastTimesliceLength;
	if(lineAccessPending)
	{
		blineAccessBuffer = lineAccessBuffer;
	}
	else
	{
		blineAccessBuffer.clear();
	}

	bactivateTMSS = activateTMSS;
	bactivateBootROM = activateBootROM;

	bbootROMEnabled = bootROMEnabled;
	bvdpLockoutActive = vdpLockoutActive;
	bvdpLockoutTripped = vdpLockoutTripped;

	bz80BankswitchDataCurrent = z80BankswitchDataCurrent;
	bz80BankswitchDataNew = z80BankswitchDataNew;
	bz80BankswitchBitsWritten = z80BankswitchBitsWritten;

	bcartInLineState = cartInLineState;
	bz80BusRequestLineState = z80BusRequestLineState;
	bz80BusGrantLineState = z80BusGrantLineState;
	bz80BusResetLineState = z80BusResetLineState;
	bm68kBusRequestLineState = m68kBusRequestLineState;
	bm68kBusGrantLineState = m68kBusGrantLineState;
	bvresLineState = vresLineState;
	bhaltLineState = haltLineState;
	bsresLineState = bsresLineState;
	bwresLineState = wresLineState;
}

//----------------------------------------------------------------------------------------
bool MDBusArbiter::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::NotifyUpcomingTimeslice(double nanoseconds)
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
bool MDBusArbiter::SendNotifyAfterExecuteCalled() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::NotifyAfterExecuteCalled()
{
	//Ensure that any pending line state changes which we have passed in this timeslice
	//are applied
	ApplyPendingLineStateChanges(lastTimesliceLength);
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult MDBusArbiter::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	IBusInterface::AccessResult accessResult(true);

	//Apply any changes to the line state that are pending at this time
	ApplyPendingLineStateChanges(accessTime);

	switch(interfaceNumber)
	{
	case MEMORYINTERFACE_INTERRUPT_ACKNOWLEDGE_CYCLE:{
		//If the M68000 is performing an interrupt acknowledge cycle, assert VPA to
		//instruct it to autovector the interrupt, and assert the INTAK line to instruct
		//the VDP to negate the IPL lines. We return false to this read access, since we
		//didn't respond to the read request for the interrupt vector number itself
		//(didn't assert DTACK), but we asserted VPA instead, which also terminates this
		//bus cycle.
		//##TODO## We believe that there is a delay between when the interrupt acknowledge
		//cycle begins, and when the bus arbiter asserts VPA. Perform hardware testing to
		//confirm, and measure the length of the delay for horizontal, vertical, and
		//external interrupts.
		//##NOTE## Testing performed on 2012-11-02 indicates there is in fact only a
		//single clock cycle delay between when the CPU enters the interrupt acknowledge
		//cycle and when VPA is asserted by the bus arbiter. We had our interrupt timing
		//wrong on the VDP, which caused our timing issues with HINT triggering.
		double autoVectorDelayTime = 0.0;
		//##DEBUG##
		//autoVectorDelayTime = 4000.0;

		m68kMemoryBus->SetLineState(LINE_VPA, Data(1, 1), GetDeviceContext(), caller, accessTime + autoVectorDelayTime, accessContext);
		m68kMemoryBus->SetLineState(LINE_INTAK, Data(1, 1), GetDeviceContext(), caller, accessTime + autoVectorDelayTime, accessContext);
		accessResult.deviceReplied = false;
		break;}
	case MEMORYINTERFACE_M68K_TO_Z80_MEMORYWINDOW:{
		if(z80BusResetLineState || !z80BusGrantLineState)
		{
			//If the M68000 currently doesn't have access to the Z80 bus, either because
			//the Z80 is currently in the reset state, or the Z80 bus hasn't been granted,
			//the bus arbiter doesn't drive any of the data lines. We mask all the data
			//lines to emulate that behaviour here.
			accessResult.accessMaskUsed = true;
			accessResult.accessMask = 0x0;
		}
		else
		{
			//Note that hardware tests performed by Charles MacDonald indicate that
			//Z80 memory access location is masked to a 15-bit result, which is important
			//as it prevents the M68000 calling back into the M68000 memory window region.
			//##TODO## Consider if we should simply perform this mirroring through line
			//mappings to the Z80 window region during system construction.
			unsigned int z80MemoryAccessLocation = (location & 0x7FFF);

			//Note that results from the "68000 memory test" ROM indicate that word-wide
			//read into the Z80 memory space from the 68000 bus does not perform a 16-bit
			//operation, but rather, an 8-bit read is performed at the target address, and
			//the 8-bit result is mirrored in both the upper and lower 8 data lines.
			//##TODO## Perform hardware tests on the real system to verify the correct
			//behaviour for all read and write patterns.
			//##NOTE## Hardware tests have shown that when attempting a word-wide read
			//from the M68K to the Z80 memory map, it is performed as a single byte-wide
			//access at the target address, with the resulting byte mirrored in the upper
			//and lower bytes of the word-wide result. This applies for all 8-bit Z80
			//hardware accessed across the bridge. Note that it also applies for the IO
			//ports themselves, accessible at 0xA10000-0xA1001F on the M68K bus. This
			//demonstrates that the IO ports are actually implemented as an 8-bit device
			//on the Z80 bus.
			Data z80BusData(8, 0xFF);
			IBusInterface::AccessResult remoteAccessResult = z80MemoryBus->ReadMemory(z80MemoryAccessLocation, z80BusData, caller, accessTime, accessContext);

			//Note that in the case of reads into the Z80 memory space from the M68000,
			//any data lines which are not driven by a remote device as a result of the
			//read operation are forced to set. We emulate that behaviour here using the
			//returned access mask from the read operation.
			if(remoteAccessResult.accessMaskUsed)
			{
				z80BusData |= ~remoteAccessResult.accessMask;
			}

			//Note that hardware tests have shown that word-wide access to the Z80 memory
			//space is not possible. When performing a read operation, instead of a true
			//16-bit result being returned, the 8-bit result from the target address is
			//mirrored in both the upper and lower halves of the 16-bit data value. In the
			//case of a write, only one byte is written. In both cases, the state of the
			//LDS line determines whether the access occurs at an odd or even address in
			//the Z80 memory space. We emulate that behaviour here.
			data.SetUpperHalf(z80BusData.GetData());
			data.SetLowerHalf(z80BusData.GetData());

			//Set the execution time of this operation to match the execution time of the
			//remote bus operation.
			accessResult.executionTime = remoteAccessResult.executionTime;
		}
		break;}
	case MEMORYINTERFACE_Z80_TO_VDP_MEMORYWINDOW:{
		//This is basically the same as a Z80 to M68K access, just using a different
		//memory portal.

		//Calculate the target address in the M68000 memory space
		unsigned int m68kMemoryAccessLocation = 0xC00000 | (location & 0x00FF);
		bool accessAtOddAddress = (m68kMemoryAccessLocation & 0x1) != 0;
		m68kMemoryAccessLocation &= ~0x1;

		//Perform the read operation
		Data m68kBusData(16);
		double m68kBusAccessExecutionTime = 0.0;
		IBusInterface::AccessResult m68kBusAccessResult(true);
		if(!ReadZ80ToM68000(m68kMemoryAccessLocation, m68kBusData, caller, accessTime, accessContext, m68kBusAccessResult, m68kBusAccessExecutionTime))
		{
			//##TODO## Trigger an assert
			std::wcout << "-ReadZ80ToM68000 failed! " << location << '\t' << data.GetData() << '\n';
		}

		//Hardware tests have shown that when the Z80 attempts to read from the M68K bus,
		//any bits which are not explicitly driven by the responding device are set to 1.
		//It is currently unknown whether this is due to the behaviour of the Z80, or if
		//another device is driving these lines. All unused areas in the Z80 memory map
		//exhibit this same behaviour however, so it is most likely a Z80 feature.
		//##TODO## Perform hardware tests on this behaviour
		if(m68kBusAccessResult.accessMaskUsed)
		{
			m68kBusData |= ~m68kBusAccessResult.accessMask;
		}

		//The Z80 bus is only 8 bits wide, so we need to select the target half of the
		//16-bit M68000 bus result and assign it here.
		data = (accessAtOddAddress)? m68kBusData.GetLowerHalf(): m68kBusData.GetUpperHalf();

		//Set the execution time of this operation to match the execution time of the
		//remote bus operation.
		accessResult.executionTime = m68kBusAccessExecutionTime;
		break;}
	case MEMORYINTERFACE_Z80_TO_M68K_MEMORYWINDOW:{
		//Calculate the target address in the M68000 memory space
		boost::mutex::scoped_lock lock(bankswitchAccessMutex);
		unsigned int m68kMemoryAccessLocation = (z80BankswitchDataCurrent.GetData() << 15) | (location & 0x7FFF);
		lock.unlock();
		bool accessAtOddAddress = (m68kMemoryAccessLocation & 0x1) != 0;
		m68kMemoryAccessLocation &= ~0x1;

		//Perform the read operation
		Data m68kBusData(16, 0xFFFF);
		double m68kBusAccessExecutionTime = 0.0;
		IBusInterface::AccessResult m68kBusAccessResult(true);
		if(!ReadZ80ToM68000(m68kMemoryAccessLocation, m68kBusData, caller, accessTime, accessContext, m68kBusAccessResult, m68kBusAccessExecutionTime))
		{
			//##TODO## Trigger an assert
			std::wcout << "-ReadZ80ToM68000 failed! " << location << '\t' << data.GetData() << '\n';
		}

		//Hardware tests have shown that when the Z80 attempts to read from the M68K bus,
		//any bits which are not explicitly driven by the responding device are set to 1.
		//It is currently unknown whether this is due to the behaviour of the Z80, or if
		//another device is driving these lines. All unused areas in the Z80 memory map
		//exhibit this same behaviour however, so it is most likely a Z80 feature.
		//##TODO## Perform hardware tests on this behaviour
		if(m68kBusAccessResult.accessMaskUsed)
		{
			m68kBusData |= ~m68kBusAccessResult.accessMask;
		}

		//The Z80 bus is only 8 bits wide, so we need to select the target half of the
		//16-bit M68000 bus result and assign it here.
		data = (accessAtOddAddress)? m68kBusData.GetLowerHalf(): m68kBusData.GetUpperHalf();

		//Set the execution time of this operation to match the execution time of the
		//remote bus operation.
		accessResult.executionTime = m68kBusAccessExecutionTime;
		break;}
	case MEMORYINTERFACE_Z80_WINDOW_BANKSWITCH:
		//Hardware tests have shown that reads from the Z80 bankswitch register always
		//return 0xFFFF.
		//##TODO## This might indicate that the Z80 bankswitch register resides only on
		//the M68K bus, and that this read value is the default unmapped read value
		//returned by the bus arbiter itself when no response is received from a read
		//request. Perform more hardware tests on this behaviour. It might simply be
		//appropriate to design the bus mappings such that read requests never get
		//forwarded on for reads from this address.
		//##TODO## It seems likely now that the Z80 drives all lines high before a read
		//request, so any lines which are not driven by an external device read as set. If
		//this is the case, we should implement this behaviour in the Z80 core, and drop
		//manual forcing of the line state in areas like this.
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0x1;
		data = 0xFFFF;
		break;
	case MEMORYINTERFACE_MEMORYMODE:
		//##TODO##
		break;
	case MEMORYINTERFACE_Z80_BUSREQ:
		//Return true if the Z80 bus is not accessible by the M68000
		data.SetBit(0, (z80BusResetLineState || !z80BusRequestLineState || !z80BusGrantLineState));
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0x01;
		break;
	case MEMORYINTERFACE_Z80_RESET:
		//Don't assert any data lines
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0x0;
		break;
	case MEMORYINTERFACE_TMSS:
		//##TODO## Determine if reads from this location do anything
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0x0;
		break;
	case MEMORYINTERFACE_TMSS_BOOTROMSWITCH:
		//##TODO## Determine if reads from this location do anything
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0x0;
		break;
	}
	return accessResult;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult MDBusArbiter::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	IBusInterface::AccessResult accessResult(true);

	//Apply any changes to the line state that are pending at this time
	ApplyPendingLineStateChanges(accessTime);

	switch(interfaceNumber)
	{
	case MEMORYINTERFACE_INTERRUPT_ACKNOWLEDGE_CYCLE:
		//This should never happen if this device is mapped correctly
		accessResult.deviceReplied = false;
		break;
	case MEMORYINTERFACE_M68K_TO_Z80_MEMORYWINDOW:
		if(!z80BusResetLineState && z80BusGrantLineState)
		{
			//Note that hardware tests performed by Charles MacDonald indicate that
			//Z80 memory access location is masked to a 15-bit result, which is important
			//as it prevents the M68000 calling back into the M68000 memory window region.
			unsigned int z80MemoryAccessLocation = (location & 0x7FFF);

			//Note that hardware tests have shown that word-wide access to the Z80 memory
			//space is not possible. When performing a read operation, instead of a true
			//16-bit result being returned, the 8-bit result from the target address is
			//mirrored in both the upper and lower halves of the 16-bit data value. In the
			//case of a write, only one byte is written. In both cases, the state of the
			//LDS line determines whether the access occurs at an odd or even address in
			//the Z80 memory space. We emulate that behaviour here.
			//##TODO## It's quite probable that M68000 writes to the Z80 bus only perform
			//byte-wide writes, just like only byte-wide reads are possible. This is
			//strongly suggested also by the fact that all observed games so far only
			//perform byte-wide writes to the Z80 memory when loading Z80 code into
			//memory. Most likely, only the upper byte of a word-wide write is actually
			//written to Z80 RAM. We should perform hardware tests to confirm this.
			bool writeToOddAddress = (z80MemoryAccessLocation & 0x01) != 0;
			Data z80BusData(8);
			z80BusData = (writeToOddAddress)? data.GetLowerHalf(): data.GetUpperHalf();

			//Perform the write operation
			IBusInterface::AccessResult remoteAccessResult = z80MemoryBus->WriteMemory(z80MemoryAccessLocation, z80BusData, caller, accessTime, accessContext);

			//Set the execution time of this operation to match the execution time of the
			//remote bus operation.
			accessResult.executionTime = remoteAccessResult.executionTime;
		}
		break;
	case MEMORYINTERFACE_Z80_TO_VDP_MEMORYWINDOW:{
		//Calculate the access location
		unsigned int m68kMemoryAccessLocation = 0xC00000 | (location & 0x00FF);

		//Writes from the Z80 bus to the M68000 bus duplicate the 8-bit write data in the
		//upper and lower halves of the 16-bit M68000 data bus.
		Data m68kBusData(16);
		m68kBusData.SetUpperHalf(data.GetData());
		m68kBusData.SetLowerHalf(data.GetData());

		//Perform the write operation
		double m68kBusAccessExecutionTime = 0.0;
		IBusInterface::AccessResult m68kBusAccessResult(true);
		if(!WriteZ80ToM68000(m68kMemoryAccessLocation, m68kBusData, caller, accessTime, accessContext, m68kBusAccessResult, m68kBusAccessExecutionTime))
		{
			//##TODO## Trigger an assert
			std::wcout << "-WriteZ80ToM68000 failed! " << location << '\t' << data.GetData() << '\n';
		}

		//Set the execution time of this operation to match the execution time of the
		//remote bus operation.
		accessResult.executionTime = m68kBusAccessExecutionTime;
		break;}
	case MEMORYINTERFACE_Z80_TO_M68K_MEMORYWINDOW:{
		//Calculate the access location
		boost::mutex::scoped_lock lock(bankswitchAccessMutex);
		unsigned int m68kMemoryAccessLocation = (z80BankswitchDataCurrent.GetData() << 15) | (location & 0x7FFF);
		lock.unlock();

		//Writes from the Z80 bus to the M68000 bus duplicate the 8-bit write data in the
		//upper and lower halves of the 16-bit M68000 data bus.
		Data m68kBusData(16);
		m68kBusData.SetUpperHalf(data.GetData());
		m68kBusData.SetLowerHalf(data.GetData());

		//Perform the write operation
		double m68kBusAccessExecutionTime = 0.0;
		IBusInterface::AccessResult m68kBusAccessResult(true);
		if(!WriteZ80ToM68000(m68kMemoryAccessLocation, m68kBusData, caller, accessTime, accessContext, m68kBusAccessResult, m68kBusAccessExecutionTime))
		{
			//##TODO## Trigger an assert
			std::wcout << "-WriteZ80ToM68000 failed! " << location << '\t' << data.GetData() << '\n';
		}

		//Set the execution time of this operation to match the execution time of the
		//remote bus operation.
		accessResult.executionTime = m68kBusAccessExecutionTime;
		break;}
	case MEMORYINTERFACE_Z80_WINDOW_BANKSWITCH:{
		//Handle changes to the Z80 bankswitch register
		boost::mutex::scoped_lock lock(bankswitchAccessMutex);
		z80BankswitchDataNew.SetBit(z80BankswitchBitsWritten, data.NonZero());
		++z80BankswitchBitsWritten;
		if(z80BankswitchBitsWritten == 9)
		{
			//##DEBUG##
			//std::wcout << L"Z80 bankswitch data changed from " << std::hex << std::uppercase << z80BankswitchDataCurrent.GetData() << " to " << z80BankswitchDataNew.GetData() << L" (" << (z80BankswitchDataCurrent.GetData() << 15) << L" to " << (z80BankswitchDataNew.GetData() << 15) << L")\n";

			z80BankswitchDataCurrent = z80BankswitchDataNew;
			z80BankswitchDataNew = 0;
			z80BankswitchBitsWritten = 0;
		}
		break;}
	case MEMORYINTERFACE_MEMORYMODE:
		//##TODO##
		break;
	case MEMORYINTERFACE_Z80_BUSREQ:{
		//Z80 bus request
		bool newState = data.NonZero();
		if(z80BusRequestLineState != newState)
		{
			z80BusRequestLineState = newState;
			z80MemoryBus->SetLineState(LINE_ZBR, Data(1, (unsigned int)z80BusRequestLineState), GetDeviceContext(), caller, accessTime, accessContext);
		}
		break;}
	case MEMORYINTERFACE_Z80_RESET:{
		//Z80 reset
		bool newState = !data.NonZero();
		if(z80BusResetLineState != newState)
		{
			z80BusResetLineState = newState;
			z80MemoryBus->SetLineState(LINE_ZRES, Data(1, (unsigned int)(sresLineState | z80BusResetLineState)), GetDeviceContext(), caller, accessTime, accessContext);
		}
		break;}
	case MEMORYINTERFACE_TMSS:
		//##TODO## Although the official Sega security code writes the ASCII string "SEGA"
		//to this address as a long word write, it seems almost certain that this is
		//actually meaningless, and that all that matters is bit 9 of the write data.
		//Since this bit is set to true with the "GA" component of the write, this will
		//result in this bit being set to 1 when this register is written to. We assume
		//therefore, that like the boot ROM switch, the TMSS security register starts off
		//active, and is cleared when a single mapped data line disables this setting by
		//writing a value of 1 to it. This needs testing on the hardware.
		vdpLockoutActive = !data.GetBit(0);
		break;
	case MEMORYINTERFACE_TMSS_BOOTROMSWITCH:
		//##TODO## Perform hardware tests to determine exactly which addresses this
		//register is accessible from.
		bootROMEnabled = !data.GetBit(0);
		break;
	}
	return accessResult;
}

//----------------------------------------------------------------------------------------
bool MDBusArbiter::ReadZ80ToM68000(unsigned int m68kMemoryAccessLocation, Data& m68kBusData, IDeviceContext* caller, double accessTime, unsigned int accessContext, IBusInterface::AccessResult& m68kBusAccessResult, double& executionTime)
{
	boost::mutex::scoped_lock lock(lineMutex);

	//Set the initial access time for this memory operation
	double accessTimeCurrent = accessTime;

	//1. If the VDP is currently requesting the bus, wait until it is finished, IE, until
	//   BR is not asserted. (possible infinite delay)
	if(m68kBusRequestLineState)
	{
		lock.unlock();
		m68kBusRequestLineStateChangeTimeLatchEnable = true;
		if(!m68kMemoryBus->AdvanceToLineState(LINE_BR, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if(!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LINE_BR, Data(1, 0), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			//didn't find the requested line state change in our buffer. In this case
			//we need to trigger an assert.
			return false;
		}
		m68kBusRequestLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	//2. If the M68K is still in the process of re-acquiring the bus, wait until it is
	//   complete, IE, until BG is not asserted. (possible infinite delay)
	if(m68kBusGrantLineState)
	{
		lock.unlock();
		m68kBusGrantLineStateChangeTimeLatchEnable = true;
		if(!m68kMemoryBus->AdvanceToLineState(LINE_BG, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if(!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LINE_BG, Data(1, 0), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			//didn't find the requested line state change in our buffer. In this case
			//we need to trigger an assert.
			return false;
		}
		m68kBusGrantLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	//3. Assert BR
	lock.unlock();
	m68kMemoryBus->SetLineState(LINE_BR, Data(1, 1), GetDeviceContext(), caller, accessTimeCurrent, accessContext);
	lock.lock();

	//4. Wait for BG to be set (possible infinite delay)
	if(!m68kBusGrantLineState)
	{
		lock.unlock();
		m68kBusGrantLineStateChangeTimeLatchEnable = true;
		if(!m68kMemoryBus->AdvanceToLineState(LINE_BG, Data(1, 1), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if(!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LINE_BG, Data(1, 1), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			//didn't find the requested line state change in our buffer. In this case
			//we need to trigger an assert.
			return false;
		}
		m68kBusGrantLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	//Release lineMutex, now that we've finished working with the current line state.
	lock.unlock();

	//5. Perform the operation
	m68kBusAccessResult = m68kMemoryBus->ReadMemory(m68kMemoryAccessLocation, m68kBusData, caller, accessTimeCurrent, accessContext);
	accessTimeCurrent += m68kBusAccessResult.executionTime;

	//6. Negate BR
	m68kMemoryBus->SetLineState(LINE_BR, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext);

	//Calculate the total execution time for the operation
	executionTime = accessTimeCurrent - accessTime;

	return true;
}

//----------------------------------------------------------------------------------------
bool MDBusArbiter::WriteZ80ToM68000(unsigned int m68kMemoryAccessLocation, Data m68kBusData, IDeviceContext* caller, double accessTime, unsigned int accessContext, IBusInterface::AccessResult& m68kBusAccessResult, double& executionTime)
{
	boost::mutex::scoped_lock lock(lineMutex);

	//Set the initial access time for this memory operation
	double accessTimeCurrent = accessTime;

	//1. If the VDP is currently requesting the bus, wait until it is finished, IE, until
	//   BR is not asserted. (possible infinite delay)
	if(m68kBusRequestLineState)
	{
		lock.unlock();
		m68kBusRequestLineStateChangeTimeLatchEnable = true;
		if(!m68kMemoryBus->AdvanceToLineState(LINE_BR, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if(!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LINE_BR, Data(1, 0), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			//didn't find the requested line state change in our buffer. In this case
			//we need to trigger an assert.
			return false;
		}
		m68kBusRequestLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	//2. If the M68K is still in the process of re-acquiring the bus, wait until it is
	//   complete, IE, until BG is not asserted. (possible infinite delay)
	if(m68kBusGrantLineState)
	{
		lock.unlock();
		m68kBusGrantLineStateChangeTimeLatchEnable = true;
		if(!m68kMemoryBus->AdvanceToLineState(LINE_BG, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if(!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LINE_BG, Data(1, 0), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			//didn't find the requested line state change in our buffer. In this case
			//we need to trigger an assert.
			return false;
		}
		m68kBusGrantLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	//3. Assert BR
	lock.unlock();
	m68kMemoryBus->SetLineState(LINE_BR, Data(1, 1), GetDeviceContext(), caller, accessTimeCurrent, accessContext);
	lock.lock();

	//4. Wait for BG to be set (possible infinite delay)
	if(!m68kBusGrantLineState)
	{
		lock.unlock();
		m68kBusGrantLineStateChangeTimeLatchEnable = true;
		if(!m68kMemoryBus->AdvanceToLineState(LINE_BG, Data(1, 1), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if(!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LINE_BG, Data(1, 1), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			//didn't find the requested line state change in our buffer. In this case
			//we need to trigger an assert.
			return false;
		}
		m68kBusGrantLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	//Release lineMutex, now that we've finished working with the current line state.
	lock.unlock();

	//5. Perform the operation
	m68kBusAccessResult = m68kMemoryBus->WriteMemory(m68kMemoryAccessLocation, m68kBusData, caller, accessTimeCurrent, accessContext);
	accessTimeCurrent += m68kBusAccessResult.executionTime;

	//6. Negate BR
	m68kMemoryBus->SetLineState(LINE_BR, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext);

	//Calculate the total execution time for the operation
	executionTime = accessTimeCurrent - accessTime;

	return true;
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int MDBusArbiter::GetCELineID(const std::wstring& lineName, bool inputLine) const
{
	if(lineName == L"R/W")
	{
		return CELINE_RW;
	}
	else if(lineName == L"OE0")
	{
		return CELINE_OE0;
	}
	else if(lineName == L"UDS")
	{
		return CELINE_UDS;
	}
	else if(lineName == L"LDS")
	{
		return CELINE_LDS;
	}
	else if(lineName == L"CE0")
	{
		return CELINE_CE0;
	}
	else if(lineName == L"BootROM")
	{
		return CELINE_BOOTROM;
	}
	else if(lineName == L"ROM")
	{
		return CELINE_ROM;
	}
	else if(lineName == L"ASEL")
	{
		return CELINE_ASEL;
	}
	else if(lineName == L"FDC")
	{
		return CELINE_FDC;
	}
	else if(lineName == L"FDWR")
	{
		return CELINE_FDWR;
	}
	else if(lineName == L"TIME")
	{
		return CELINE_TIME;
	}
	else if(lineName == L"IO")
	{
		return CELINE_IO;
	}
	else if(lineName == L"EOE")
	{
		return CELINE_EOE;
	}
	else if(lineName == L"NOE")
	{
		return CELINE_NOE;
	}
	else if(lineName == L"ZRAM")
	{
		return CELINE_ZRAM;
	}
	else if(lineName == L"SOUND")
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
	case CELINE_BOOTROM:
		ceLineMaskBootROM = !lineMapped? 0: 1 << lineStartBitNumber;
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
unsigned int MDBusArbiter::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const
{
	unsigned int result = 0;
	if(sourceBusInterface == m68kMemoryBus)
	{
		bool operationIsWrite = (currentCELineState & ceLineMaskReadHighWriteLow) == 0;
		bool ceLineUDS = (currentCELineState & ceLineMaskUDS) != 0;
		bool ceLineLDS = (currentCELineState & ceLineMaskLDS) != 0;
		bool ceLineOE0 = (currentCELineState & ceLineMaskOE0) != 0;
		result = BuildCELineM68K(location, operationIsWrite, ceLineUDS, ceLineLDS, ceLineOE0, cartInLineState, caller, accessTime);
	}
	else if(sourceBusInterface == z80MemoryBus)
	{
		result = BuildCELineZ80(location);
	}
	return result;
}

//----------------------------------------------------------------------------------------
unsigned int MDBusArbiter::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const
{
	return CalculateCELineStateMemory(location, data, currentCELineState, sourceBusInterface, caller, calculateCELineStateContext, 0.0);
}

//----------------------------------------------------------------------------------------
unsigned int MDBusArbiter::BuildCELineM68K(unsigned int targetAddress, bool write, bool ceLineUDS, bool ceLineLDS, bool ceLineOE0, bool cartInLineAsserted, IDeviceContext* caller, double accessTime) const
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
	bool lineBootROM = cartInLineAsserted && bootROMEnabled && activateBootROM && (targetAddress <= 0x3FFFFF);
	bool lineCE0 = cartInLineAsserted? !lineBootROM && (targetAddress <= 0x3FFFFF): (targetAddress >= 0x400000) && (targetAddress <= 0x7FFFFF);
	bool lineROM = !cartInLineAsserted? (targetAddress <= 0x1FFFFF): (targetAddress >= 0x400000) && (targetAddress <= 0x5FFFFF);
	bool lineASEL = !lineBootROM && (targetAddress <= 0x7FFFFF);
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
	ceLineState |= lineBootROM? ceLineMaskBootROM: 0x0;
	ceLineState |= lineROM? ceLineMaskROM: 0x0;
	ceLineState |= lineASEL? ceLineMaskASEL: 0x0;
	ceLineState |= lineFDC? ceLineMaskFDC: 0x0;
	ceLineState |= lineFDWR? ceLineMaskFDWR: 0x0;
	ceLineState |= lineTIME? ceLineMaskTIME: 0x0;
	ceLineState |= lineIO? ceLineMaskIO: 0x0;
	ceLineState |= lineEOE? ceLineMaskEOE: 0x0;
	ceLineState |= lineNOE? ceLineMaskNOE: 0x0;

	//If TMSS is active, and a device is attempting to access the VDP address range,
	//assert the VRES and HALT lines. Note that according to tests performed by Charles
	//MacDonald, this state is not cleared on a soft reset. A full power cycle is required
	//in order to restore normal operation.
	if(vdpLockoutActive && activateTMSS && (!activateBootROM || !bootROMEnabled) && (targetAddress >= 0xC00000) && (targetAddress <= 0xDFFFFF))
	{
		vdpLockoutTripped = true;
		m68kMemoryBus->SetLineState(LINE_HALT, Data(1, (unsigned int)vdpLockoutTripped), GetDeviceContext(), caller, accessTime, 0);
		m68kMemoryBus->SetLineState(LINE_VRES, Data(1, (unsigned int)vdpLockoutTripped), GetDeviceContext(), caller, accessTime, 0);
	}

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
unsigned int MDBusArbiter::GetLineID(const std::wstring& lineName) const
{
	if(lineName == L"CART")
	{
		return LINE_CART;
	}
	else if(lineName == L"VPA")
	{
		return LINE_VPA;
	}
	else if(lineName == L"INTAK")
	{
		return LINE_INTAK;
	}
	else if(lineName == L"BR")
	{
		return LINE_BR;
	}
	else if(lineName == L"BG")
	{
		return LINE_BG;
	}
	else if(lineName == L"ZBR")
	{
		return LINE_ZBR;
	}
	else if(lineName == L"ZBAK")
	{
		return LINE_ZBAK;
	}
	else if(lineName == L"ZRES")
	{
		return LINE_ZRES;
	}
	else if(lineName == L"VRES")
	{
		return LINE_VRES;
	}
	else if(lineName == L"HALT")
	{
		return LINE_HALT;
	}
	else if(lineName == L"SRES")
	{
		return LINE_SRES;
	}
	else if(lineName == L"WRES")
	{
		return LINE_WRES;
	}
	else if(lineName == L"ActivateTMSS")
	{
		return LINE_ACTIVATETMSS;
	}
	else if(lineName == L"ActivateBootROM")
	{
		return LINE_ACTIVATEBOOTROM;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
std::wstring MDBusArbiter::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_CART:
		return L"CART";
	case LINE_VPA:
		return L"VPA";
	case LINE_INTAK:
		return L"INTAK";
	case LINE_BR:
		return L"BR";
	case LINE_BG:
		return L"BG";
	case LINE_ZBR:
		return L"ZBR";
	case LINE_ZBAK:
		return L"ZBAK";
	case LINE_ZRES:
		return L"ZRES";
	case LINE_VRES:
		return L"VRES";
	case LINE_HALT:
		return L"HALT";
	case LINE_SRES:
		return L"SRES";
	case LINE_WRES:
		return L"WRES";
	case LINE_ACTIVATETMSS:
		return L"ActivateTMSS";
	case LINE_ACTIVATEBOOTROM:
		return L"ActivateBootROM";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int MDBusArbiter::GetLineWidth(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_CART:
		return 1;
	case LINE_VPA:
		return 1;
	case LINE_INTAK:
		return 1;
	case LINE_BR:
		return 1;
	case LINE_BG:
		return 1;
	case LINE_ZBR:
		return 1;
	case LINE_ZBAK:
		return 1;
	case LINE_ZRES:
		return 1;
	case LINE_VRES:
		return 1;
	case LINE_HALT:
		return 1;
	case LINE_SRES:
		return 1;
	case LINE_WRES:
		return 1;
	case LINE_ACTIVATETMSS:
		return 1;
	case LINE_ACTIVATEBOOTROM:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
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

	//If this is a line state change which needs to be processed immediately, apply it now
	//and return.
	switch(targetLine)
	{
	case LINE_CART:
		cartInLineState = lineData.NonZero();
		return;
	case LINE_ACTIVATETMSS:
		activateTMSS = lineData.NonZero();
		return;
	case LINE_ACTIVATEBOOTROM:
		activateBootROM = lineData.NonZero();
		return;
	case LINE_WRES:{
		bool wresLineStateNew = lineData.LSB();
		if(wresLineStateNew != wresLineState)
		{
			//Determine the new line states
			bool vresLineStateNew = wresLineStateNew | vdpLockoutTripped;
			bool haltLineStateNew = wresLineStateNew | vdpLockoutTripped;
			bool sresLineStateNew = wresLineStateNew;
			bool z80BusResetLineStateNew = sresLineStateNew | z80BusResetLineState;

			//Apply the new line states
			wresLineState = wresLineStateNew;
			vresLineState = vresLineStateNew;
			haltLineState = haltLineStateNew;
			sresLineState = sresLineStateNew;

			//##TODO## Determine what other internal state should be cleared here, such as
			//the Z80/M68K bus request state.

			//Release our lock on the line mutex
			lock.unlock();

			//Apply our line state changes
			m68kMemoryBus->SetLineState(LINE_VRES, Data(1, (unsigned int)vresLineStateNew), GetDeviceContext(), caller, accessTime, accessContext);
			m68kMemoryBus->SetLineState(LINE_HALT, Data(1, (unsigned int)haltLineStateNew), GetDeviceContext(), caller, accessTime, accessContext);
			m68kMemoryBus->SetLineState(LINE_SRES, Data(1, (unsigned int)sresLineStateNew), GetDeviceContext(), caller, accessTime, accessContext);
			z80MemoryBus->SetLineState(LINE_ZRES, Data(1, (unsigned int)z80BusResetLineStateNew), GetDeviceContext(), caller, accessTime, accessContext);
		}
		return;}
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
void MDBusArbiter::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{
	SetLineState(targetLine, lineData, 0, 0.0, 0);
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext)
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
void MDBusArbiter::AssertCurrentOutputLineState() const
{
	if(z80MemoryBus != 0)
	{
		if(sresLineState | z80BusResetLineState)   z80MemoryBus->SetLineState(LINE_ZRES, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(z80BusRequestLineState) z80MemoryBus->SetLineState(LINE_ZBR, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(vresLineState)          m68kMemoryBus->SetLineState(LINE_VRES, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(haltLineState)          m68kMemoryBus->SetLineState(LINE_HALT, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(sresLineState)          m68kMemoryBus->SetLineState(LINE_SRES, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::NegateCurrentOutputLineState() const
{
	if(z80MemoryBus != 0)
	{
		if(sresLineState | z80BusResetLineState)   z80MemoryBus->SetLineState(LINE_ZRES, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(z80BusRequestLineState) z80MemoryBus->SetLineState(LINE_ZBR, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(vresLineState)          m68kMemoryBus->SetLineState(LINE_VRES, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(haltLineState)          m68kMemoryBus->SetLineState(LINE_HALT, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(sresLineState)          m68kMemoryBus->SetLineState(LINE_SRES, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::ApplyLineStateChange(unsigned int targetLine, const Data& lineData, double accessTime)
{
	bool newLineState = lineData.NonZero();
	switch(targetLine)
	{
	case LINE_BR:
		if(m68kBusRequestLineStateChangeTimeLatchEnable && (m68kBusRequestLineState != newLineState))
		{
			m68kBusRequestLineStateChangeTimeLatch = accessTime;
			m68kBusRequestLineStateChangeTimeLatchEnable = false;
		}
		m68kBusRequestLineState = newLineState;
		break;
	case LINE_BG:
		if(m68kBusGrantLineStateChangeTimeLatchEnable && (m68kBusGrantLineState != newLineState))
		{
			m68kBusGrantLineStateChangeTimeLatch = accessTime;
			m68kBusGrantLineStateChangeTimeLatchEnable = false;
		}
		m68kBusGrantLineState = newLineState;
		break;
	case LINE_ZBR:
		if(z80BusRequestLineStateChangeTimeLatchEnable && (z80BusRequestLineState != newLineState))
		{
			z80BusRequestLineStateChangeTimeLatch = accessTime;
			z80BusRequestLineStateChangeTimeLatchEnable = false;
		}
		z80BusRequestLineState = newLineState;
		break;
	case LINE_ZBAK:
		if(z80BusGrantLineStateChangeTimeLatchEnable && (z80BusGrantLineState != newLineState))
		{
			z80BusGrantLineStateChangeTimeLatch = accessTime;
			z80BusGrantLineStateChangeTimeLatchEnable = false;
		}
		z80BusGrantLineState = newLineState;
		break;
	case LINE_ZRES:
		if(z80BusResetLineStateChangeTimeLatchEnable && (z80BusResetLineState != newLineState))
		{
			z80BusResetLineStateChangeTimeLatch = accessTime;
			z80BusResetLineStateChangeTimeLatchEnable = false;
		}
		z80BusResetLineState = newLineState;
		break;
	}
}

//----------------------------------------------------------------------------------------
void MDBusArbiter::ApplyPendingLineStateChanges(double accessTime)
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
				ApplyLineStateChange(i->lineID, i->state, i->accessTime);
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

//----------------------------------------------------------------------------------------
bool MDBusArbiter::AdvanceUntilPendingLineStateChangeApplied(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int targetLine, Data targetLineState, double& lineStateReachedTime)
{
	boost::mutex::scoped_lock lock(lineMutex);

	//If the target line is currently sitting on the requested line state, return true.
	//##TODO## Update this comment
	bool targetLineCurrentlyMatchesState = false;
	switch(targetLine)
	{
	case LINE_BR:
		targetLineCurrentlyMatchesState = (m68kBusRequestLineState == targetLineState.NonZero());
		if(targetLineCurrentlyMatchesState)
		{
			lineStateReachedTime = m68kBusRequestLineStateChangeTimeLatch;
		}
		break;
	case LINE_BG:
		targetLineCurrentlyMatchesState = (m68kBusGrantLineState == targetLineState.NonZero());
		if(targetLineCurrentlyMatchesState)
		{
			lineStateReachedTime = m68kBusGrantLineStateChangeTimeLatch;
		}
		break;
	case LINE_ZBAK:
		targetLineCurrentlyMatchesState = (z80BusGrantLineState == targetLineState.NonZero());
		if(targetLineCurrentlyMatchesState)
		{
			lineStateReachedTime = z80BusGrantLineStateChangeTimeLatch;
		}
		break;
	}
	if(targetLineCurrentlyMatchesState)
	{
		return true;
	}

	//If we don't have a pending line state change in the buffer which matches the target
	//line and state, return false.
	bool foundTargetStateChange = false;
	std::list<LineAccess>::iterator i = lineAccessBuffer.begin();
	while(!foundTargetStateChange && (i != lineAccessBuffer.end()))
	{
		foundTargetStateChange = ((i->lineID == targetLine) && (i->state == targetLineState));
		++i;
	}
	if(!foundTargetStateChange)
	{
		return false;
	}

	//Advance the line state buffer until the target line state change is applied
	bool targetLineStateReached = false;
	i = lineAccessBuffer.begin();
	while(!targetLineStateReached && (i != lineAccessBuffer.end()))
	{
		ApplyLineStateChange(i->lineID, i->state, i->accessTime);
		targetLineStateReached = ((i->lineID == targetLine) && (i->state == targetLineState));
		lineStateReachedTime = i->accessTime;
		++i;
	}

	//Clear any completed entries from the list
	lineAccessBuffer.erase(lineAccessBuffer.begin(), i);
	lineAccessPending = !lineAccessBuffer.empty();

	//Return the result of the advance operation. If the logic of our above implementation
	//is correct, we should always return true at this point, since failure cases were
	//caught before the line state change buffer was advanced.
	return targetLineStateReached;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void MDBusArbiter::LoadState(IHeirarchicalStorageNode& node)
{
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"ActivateTMSS")
		{
			activateTMSS = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"ActivateBootROM")
		{
			activateBootROM = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"BootROMEnabled")
		{
			bootROMEnabled = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"VDPLockoutActive")
		{
			vdpLockoutActive = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"VDPLockoutTripped")
		{
			vdpLockoutTripped = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"Z80BankswitchDataCurrent")
		{
			z80BankswitchDataCurrent = (*i)->ExtractHexData<unsigned int>();
		}
		else if((*i)->GetName() == L"Z80BankswitchDataNew")
		{
			z80BankswitchDataNew = (*i)->ExtractHexData<unsigned int>();
		}
		else if((*i)->GetName() == L"Z80BankswitchBitsWritten")
		{
			z80BankswitchBitsWritten = (*i)->ExtractData<unsigned int>();
		}
		else if((*i)->GetName() == L"CartInLineState")
		{
			cartInLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"Z80BusRequestLineState")
		{
			z80BusRequestLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"Z80BusGrantLineState")
		{
			z80BusGrantLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"Z80BusResetLineState")
		{
			z80BusResetLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"M68KBusRequestLineState")
		{
			m68kBusRequestLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"M68KBusGrantLineState")
		{
			m68kBusGrantLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"VRESLineState")
		{
			vresLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"HALTLineState")
		{
			haltLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"SRESLineState")
		{
			sresLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"WRESLineState")
		{
			wresLineState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"LastTimesliceLength")
		{
			lastTimesliceLength = (*i)->ExtractData<bool>();
		}
		//Restore the lineAccessBuffer state
		else if((*i)->GetName() == L"LineAccessBuffer")
		{
			IHeirarchicalStorageNode& lineAccessBufferNode = *(*i);
			std::list<IHeirarchicalStorageNode*> lineAccessBufferChildList = lineAccessBufferNode.GetChildList();
			for(std::list<IHeirarchicalStorageNode*>::iterator lineAccessBufferEntry = lineAccessBufferChildList.begin(); lineAccessBufferEntry != lineAccessBufferChildList.end(); ++lineAccessBufferEntry)
			{
				if((*lineAccessBufferEntry)->GetName() == L"LineAccess")
				{
					IHeirarchicalStorageAttribute* lineNameAttribute = (*lineAccessBufferEntry)->GetAttribute(L"LineName");
					IHeirarchicalStorageAttribute* lineStateAttribute = (*lineAccessBufferEntry)->GetAttribute(L"LineState");
					IHeirarchicalStorageAttribute* accessTimeAttribute = (*lineAccessBufferEntry)->GetAttribute(L"AccessTime");
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
void MDBusArbiter::SaveState(IHeirarchicalStorageNode& node) const
{
	node.CreateChild(L"ActivateTMSS", activateTMSS);
	node.CreateChild(L"ActivateBootROM", activateBootROM);
	node.CreateChild(L"BootROMEnabled", bootROMEnabled);
	node.CreateChild(L"VDPLockoutActive", vdpLockoutActive);
	node.CreateChild(L"VDPLockoutTripped", vdpLockoutTripped);
	node.CreateChildHex(L"Z80BankswitchDataCurrent", z80BankswitchDataCurrent.GetData(), z80BankswitchDataCurrent.GetHexCharCount());
	node.CreateChildHex(L"Z80BankswitchDataNew", z80BankswitchDataNew.GetData(), z80BankswitchDataNew.GetHexCharCount());
	node.CreateChild(L"Z80BankswitchBitsWritten", z80BankswitchBitsWritten);
	node.CreateChild(L"CartInLineState", cartInLineState);
	node.CreateChild(L"Z80BusRequestLineState", z80BusRequestLineState);
	node.CreateChild(L"Z80BusGrantLineState", z80BusGrantLineState);
	node.CreateChild(L"Z80BusResetLineState", z80BusResetLineState);
	node.CreateChild(L"M68KBusRequestLineState", m68kBusRequestLineState);
	node.CreateChild(L"M68KBusGrantLineState", m68kBusGrantLineState);
	node.CreateChild(L"VRESLineState", vresLineState);
	node.CreateChild(L"HALTLineState", haltLineState);
	node.CreateChild(L"SRESLineState", sresLineState);
	node.CreateChild(L"WRESLineState", wresLineState);
	node.CreateChild(L"LastTimesliceLength", lastTimesliceLength);

	//Save the lineAccessBuffer state
	if(lineAccessPending)
	{
		IHeirarchicalStorageNode& lineAccessState = node.CreateChild(L"LineAccessBuffer");
		for(std::list<LineAccess>::const_iterator i = lineAccessBuffer.begin(); i != lineAccessBuffer.end(); ++i)
		{
			IHeirarchicalStorageNode& lineAccessEntry = lineAccessState.CreateChild(L"LineAccess");
			lineAccessEntry.CreateAttribute(L"LineName", GetLineName(i->lineID));
			lineAccessEntry.CreateAttribute(L"LineState", i->state);
			lineAccessEntry.CreateAttribute(L"AccessTime", i->accessTime);
		}
	}
}
