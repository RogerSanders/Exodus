#include "MDBusArbiter.h"
//##DEBUG##
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
MDBusArbiter::MDBusArbiter(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Device(implementationName, instanceName, moduleID),
_z80BankswitchDataCurrent(9),
_bz80BankswitchDataCurrent(9),
_z80BankswitchDataNew(9),
_bz80BankswitchDataNew(9)
{
	_bootROM = 0;
	_m68kMemoryBus = 0;
	_z80MemoryBus = 0;

	// Initialize our CE line state
	_ceLineMaskReadHighWriteLow = 0;
	_ceLineMaskUDS = 0;
	_ceLineMaskLDS = 0;
	_ceLineMaskOE0 = 0;
	_ceLineMaskCE0 = 0;
	_ceLineMaskBootROM = 0;
	_ceLineMaskROM = 0;
	_ceLineMaskASEL = 0;
	_ceLineMaskFDC = 0;
	_ceLineMaskFDWR = 0;
	_ceLineMaskTIME = 0;
	_ceLineMaskIO = 0;
	_ceLineMaskEOE = 0;
	_ceLineMaskNOE = 0;
	_ceLineMaskZRAM = 0;
	_ceLineMaskSOUND = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Initialization functions
//----------------------------------------------------------------------------------------------------------------------
bool MDBusArbiter::ValidateDevice()
{
	return (_m68kMemoryBus != 0) && (_z80MemoryBus != 0) && (_ceLineMaskReadHighWriteLow != 0) && (_ceLineMaskOE0 != 0) && (_ceLineMaskUDS != 0) && (_ceLineMaskLDS != 0);
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::Initialize()
{
	_lastLineCheckTime = 0;
	_lineAccessPending = false;
	_lastTimesliceLength = 0;
	_lineAccessBuffer.clear();

	// Initialize the device settings
	_activateTMSS = false;
	_activateBootROM = false;

	// Initialize the TMSS Security settings
	_bootROMEnabled = true;
	_vdpLockoutActive = true;
	_vdpLockoutTripped = false;

	// Initialize the Z80 bankswitch register state
	_z80BankswitchBitsWritten = 0;
	_z80BankswitchDataCurrent = 0;
	_z80BankswitchDataNew = 0;

	// Initialize the external line state
	_cartInLineState = false;
	_z80BusRequestLineState = false;
	_z80BusGrantLineState = false;
	// Note that the Z80 reset line is asserted when this device is initialized
	_z80BusResetLineState = true;
	_m68kBusRequestLineState = false;
	_m68kBusGrantLineState = false;
	_vresLineState = false;
	_haltLineState = false;
	_sresLineState = false;
	_wresLineState = false;

	_z80BusRequestLineStateChangeTimeLatchEnable = false;
	_z80BusGrantLineStateChangeTimeLatchEnable = false;
	_z80BusResetLineStateChangeTimeLatchEnable = false;
	_m68kBusRequestLineStateChangeTimeLatchEnable = false;
	_m68kBusGrantLineStateChangeTimeLatchEnable = false;
}

//----------------------------------------------------------------------------------------------------------------------
// Reference functions
//----------------------------------------------------------------------------------------------------------------------
bool MDBusArbiter::AddReference(const Marshal::In<std::wstring>& referenceName, IDevice* target)
{
	if (referenceName == L"BootROM")
	{
		_bootROM = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDBusArbiter::AddReference(const Marshal::In<std::wstring>& referenceName, IBusInterface* target)
{
	if (referenceName == L"M68000Bus")
	{
		_m68kMemoryBus = target;
	}
	else if (referenceName == L"Z80Bus")
	{
		_z80MemoryBus = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::RemoveReference(IDevice* target)
{
	if (_bootROM == target)
	{
		_bootROM = 0;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::RemoveReference(IBusInterface* target)
{
	if (_m68kMemoryBus == target)
	{
		_m68kMemoryBus = 0;
	}
	else if (_z80MemoryBus == target)
	{
		_z80MemoryBus = 0;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Execute functions
//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::ExecuteRollback()
{
	_lastTimesliceLength = _blastTimesliceLength;

	// Note that we need to clear then assign here. The reason is that if both lists aren't empty, the default
	// assignment operator will be called on each element of the LineAccess structure, which includes Data elements,
	// which won't resize the existing objects, meaning data may be truncated if different line widths exist.
	_lineAccessBuffer.clear();
	_lineAccessBuffer = _blineAccessBuffer;
	_lineAccessPending = !_lineAccessBuffer.empty();

	_activateTMSS = _bactivateTMSS;
	_activateBootROM = _bactivateBootROM;

	_bootROMEnabled = _bbootROMEnabled;
	_vdpLockoutActive = _bvdpLockoutActive;
	_vdpLockoutTripped = _bvdpLockoutTripped;

	_z80BankswitchDataCurrent = _bz80BankswitchDataCurrent;
	_z80BankswitchDataNew = _bz80BankswitchDataNew;
	_z80BankswitchBitsWritten = _bz80BankswitchBitsWritten;

	_cartInLineState = _bcartInLineState;
	_z80BusRequestLineState = _bz80BusRequestLineState;
	_z80BusGrantLineState = _bz80BusGrantLineState;
	_z80BusResetLineState = _bz80BusResetLineState;
	_m68kBusRequestLineState = _bm68kBusRequestLineState;
	_m68kBusGrantLineState = _bm68kBusGrantLineState;
	_vresLineState = _bvresLineState;
	_haltLineState = _bhaltLineState;
	_sresLineState = _bsresLineState;
	_wresLineState = _bwresLineState;
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::ExecuteCommit()
{
	_blastTimesliceLength = _lastTimesliceLength;

	// Note that we need to clear then assign here. The reason is that if both lists aren't empty, the default
	// assignment operator will be called on each element of the LineAccess structure, which includes Data elements,
	// which won't resize the existing objects, meaning data may be truncated if different line widths exist.
	_blineAccessBuffer.clear();
	_blineAccessBuffer = _lineAccessBuffer;

	_bactivateTMSS = _activateTMSS;
	_bactivateBootROM = _activateBootROM;

	_bbootROMEnabled = _bootROMEnabled;
	_bvdpLockoutActive = _vdpLockoutActive;
	_bvdpLockoutTripped = _vdpLockoutTripped;

	_bz80BankswitchDataCurrent = _z80BankswitchDataCurrent;
	_bz80BankswitchDataNew = _z80BankswitchDataNew;
	_bz80BankswitchBitsWritten = _z80BankswitchBitsWritten;

	_bcartInLineState = _cartInLineState;
	_bz80BusRequestLineState = _z80BusRequestLineState;
	_bz80BusGrantLineState = _z80BusGrantLineState;
	_bz80BusResetLineState = _z80BusResetLineState;
	_bm68kBusRequestLineState = _m68kBusRequestLineState;
	_bm68kBusGrantLineState = _m68kBusGrantLineState;
	_bvresLineState = _vresLineState;
	_bhaltLineState = _haltLineState;
	_bsresLineState = _sresLineState;
	_bwresLineState = _wresLineState;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDBusArbiter::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::NotifyUpcomingTimeslice(double nanoseconds)
{
	// Reset lastLineCheckTime for the beginning of the new timeslice, and force any
	// remaining line state changes to be evaluated at the start of the new timeslice.
	_lastLineCheckTime = 0;
	for (std::list<LineAccess>::iterator i = _lineAccessBuffer.begin(); i != _lineAccessBuffer.end(); ++i)
	{
		// We rebase accessTime here to the start of the new time block, in order to allow
		// line state changes to be flagged ahead of the time they actually take effect.
		// This rebasing allows changes flagged ahead of time to safely cross timeslice
		// boundaries.
		i->accessTime -= _lastTimesliceLength;
	}
	_lastTimesliceLength = nanoseconds;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDBusArbiter::SendNotifyAfterExecuteCalled() const
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::NotifyAfterExecuteCalled()
{
	// Ensure that any pending line state changes which we have passed in this timeslice
	// are applied
	ApplyPendingLineStateChanges(_lastTimesliceLength);
}

//----------------------------------------------------------------------------------------------------------------------
// Memory interface functions
//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult MDBusArbiter::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	IBusInterface::AccessResult accessResult(true);

	// Apply any changes to the line state that are pending at this time
	ApplyPendingLineStateChanges(accessTime);

	switch ((MemoryInterface)interfaceNumber)
	{
	case MemoryInterface::InterruptAcknowledgeCycle:{
		// If the M68000 is performing an interrupt acknowledge cycle, assert VPA to
		// instruct it to autovector the interrupt, and assert the INTAK line to instruct
		// the VDP to negate the IPL lines. We return false to this read access, since we
		// didn't respond to the read request for the interrupt vector number itself
		//(didn't assert DTACK), but we asserted VPA instead, which also terminates this
		// bus cycle.
		//##TODO## We believe that there is a delay between when the interrupt acknowledge
		// cycle begins, and when the bus arbiter asserts VPA. Perform hardware testing to
		// confirm, and measure the length of the delay for horizontal, vertical, and
		// external interrupts.
		//##NOTE## Testing performed on 2012-11-02 indicates there is in fact only a
		// single clock cycle delay between when the CPU enters the interrupt acknowledge
		// cycle and when VPA is asserted by the bus arbiter. We had our interrupt timing
		// wrong on the VDP, which caused our timing issues with HINT triggering.
		double autoVectorDelayTime = 0.0;
		//##DEBUG##
		// autoVectorDelayTime = 4000.0;

		_m68kMemoryBus->SetLineState((unsigned int)LineID::VPA, Data(1, 1), GetDeviceContext(), caller, accessTime + autoVectorDelayTime, accessContext);
		_m68kMemoryBus->SetLineState((unsigned int)LineID::INTAK, Data(1, 1), GetDeviceContext(), caller, accessTime + autoVectorDelayTime, accessContext);
		accessResult.deviceReplied = false;
		break;}
	case MemoryInterface::M68kToZ80MemoryWindow:{
		if (_z80BusResetLineState || !_z80BusGrantLineState)
		{
			// If the M68000 currently doesn't have access to the Z80 bus, either because
			// the Z80 is currently in the reset state, or the Z80 bus hasn't been granted,
			// the bus arbiter doesn't drive any of the data lines. We mask all the data
			// lines to emulate that behaviour here.
			accessResult.accessMaskUsed = true;
			accessResult.accessMask = 0x0;
		}
		else
		{
			// Note that hardware tests performed by Charles MacDonald indicate that
			// Z80 memory access location is masked to a 15-bit result, which is important
			// as it prevents the M68000 calling back into the M68000 memory window region.
			//##TODO## Consider if we should simply perform this mirroring through line
			// mappings to the Z80 window region during system construction.
			unsigned int z80MemoryAccessLocation = (location & 0x7FFF);

			// Note that results from the "68000 memory test" ROM indicate that word-wide
			// read into the Z80 memory space from the 68000 bus does not perform a 16-bit
			// operation, but rather, an 8-bit read is performed at the target address, and
			// the 8-bit result is mirrored in both the upper and lower 8 data lines.
			//##TODO## Perform hardware tests on the real system to verify the correct
			// behaviour for all read and write patterns.
			//##NOTE## Hardware tests have shown that when attempting a word-wide read
			// from the M68K to the Z80 memory map, it is performed as a single byte-wide
			// access at the target address, with the resulting byte mirrored in the upper
			// and lower bytes of the word-wide result. This applies for all 8-bit Z80
			// hardware accessed across the bridge. Note that it also applies for the IO
			// ports themselves, accessible at 0xA10000-0xA1001F on the M68K bus. This
			// demonstrates that the IO ports are actually implemented as an 8-bit device
			// on the Z80 bus.
			Data z80BusData(8, 0xFF);
			IBusInterface::AccessResult remoteAccessResult = _z80MemoryBus->ReadMemory(z80MemoryAccessLocation, z80BusData, caller, accessTime, accessContext);

			// Note that in the case of reads into the Z80 memory space from the M68000,
			// any data lines which are not driven by a remote device as a result of the
			// read operation are forced to set. We emulate that behaviour here using the
			// returned access mask from the read operation.
			if (remoteAccessResult.accessMaskUsed)
			{
				z80BusData |= ~remoteAccessResult.accessMask;
			}

			// Note that hardware tests have shown that word-wide access to the Z80 memory
			// space is not possible. When performing a read operation, instead of a true
			// 16-bit result being returned, the 8-bit result from the target address is
			// mirrored in both the upper and lower halves of the 16-bit data value. In the
			// case of a write, only one byte is written. In both cases, the state of the
			// LDS line determines whether the access occurs at an odd or even address in
			// the Z80 memory space. We emulate that behaviour here.
			data.SetUpperHalf(z80BusData.GetData());
			data.SetLowerHalf(z80BusData.GetData());

			// Set the execution time of this operation to match the execution time of the
			// remote bus operation.
			accessResult.executionTime = remoteAccessResult.executionTime;
		}
		break;}
	case MemoryInterface::Z80ToVDPMemoryWindow:{
		// This is basically the same as a Z80 to M68K access, just using a different
		// memory portal.

		// Calculate the target address in the M68000 memory space
		unsigned int m68kMemoryAccessLocation = 0xC00000 | (location & 0x00FF);
		bool accessAtOddAddress = (m68kMemoryAccessLocation & 0x1) != 0;
		m68kMemoryAccessLocation &= ~0x1;

		// Perform the read operation
		Data m68kBusData(16);
		double m68kBusAccessExecutionTime = 0.0;
		IBusInterface::AccessResult m68kBusAccessResult(true);
		if (!ReadZ80ToM68000(m68kMemoryAccessLocation, m68kBusData, caller, accessTime, accessContext, m68kBusAccessResult, m68kBusAccessExecutionTime))
		{
			//##TODO## Trigger an assert
			std::wcout << "-ReadZ80ToM68000 failed! " << location << '\t' << data.GetData() << '\n';
		}

		// Hardware tests have shown that when the Z80 attempts to read from the M68K bus,
		// any bits which are not explicitly driven by the responding device are set to 1.
		// It is currently unknown whether this is due to the behaviour of the Z80, or if
		// another device is driving these lines. All unused areas in the Z80 memory map
		// exhibit this same behaviour however, so it is most likely a Z80 feature.
		//##TODO## Perform hardware tests on this behaviour
		if (m68kBusAccessResult.accessMaskUsed)
		{
			m68kBusData |= ~m68kBusAccessResult.accessMask;
		}

		// The Z80 bus is only 8 bits wide, so we need to select the target half of the
		// 16-bit M68000 bus result and assign it here.
		data = (accessAtOddAddress)? m68kBusData.GetLowerHalf(): m68kBusData.GetUpperHalf();

		// Set the execution time of this operation to match the execution time of the
		// remote bus operation.
		accessResult.executionTime = m68kBusAccessExecutionTime;
		break;}
	case MemoryInterface::Z80ToM68kMemoryWindow:{
		// Calculate the target address in the M68000 memory space
		std::unique_lock<std::mutex> lock(_bankswitchAccessMutex);
		unsigned int m68kMemoryAccessLocation = (_z80BankswitchDataCurrent.GetData() << 15) | (location & 0x7FFF);
		lock.unlock();
		bool accessAtOddAddress = (m68kMemoryAccessLocation & 0x1) != 0;
		m68kMemoryAccessLocation &= ~0x1;

		// Perform the read operation
		Data m68kBusData(16, 0xFFFF);
		double m68kBusAccessExecutionTime = 0.0;
		IBusInterface::AccessResult m68kBusAccessResult(true);
		if (!ReadZ80ToM68000(m68kMemoryAccessLocation, m68kBusData, caller, accessTime, accessContext, m68kBusAccessResult, m68kBusAccessExecutionTime))
		{
			//##TODO## Trigger an assert
			std::wcout << "-ReadZ80ToM68000 failed! " << location << '\t' << data.GetData() << '\n';
		}

		// Hardware tests have shown that when the Z80 attempts to read from the M68K bus,
		// any bits which are not explicitly driven by the responding device are set to 1.
		// It is currently unknown whether this is due to the behaviour of the Z80, or if
		// another device is driving these lines. All unused areas in the Z80 memory map
		// exhibit this same behaviour however, so it is most likely a Z80 feature.
		//##TODO## Perform hardware tests on this behaviour
		if (m68kBusAccessResult.accessMaskUsed)
		{
			m68kBusData |= ~m68kBusAccessResult.accessMask;
		}

		// The Z80 bus is only 8 bits wide, so we need to select the target half of the
		// 16-bit M68000 bus result and assign it here.
		data = (accessAtOddAddress)? m68kBusData.GetLowerHalf(): m68kBusData.GetUpperHalf();

		// Set the execution time of this operation to match the execution time of the
		// remote bus operation.
		accessResult.executionTime = m68kBusAccessExecutionTime;
		break;}
	case MemoryInterface::Z80WindowBankswitch:
		// Hardware tests have shown that reads from the Z80 bankswitch register always
		// return 0xFFFF.
		//##TODO## This might indicate that the Z80 bankswitch register resides only on
		// the M68K bus, and that this read value is the default unmapped read value
		// returned by the bus arbiter itself when no response is received from a read
		// request. Perform more hardware tests on this behaviour. It might simply be
		// appropriate to design the bus mappings such that read requests never get
		// forwarded on for reads from this address.
		//##TODO## It seems likely now that the Z80 drives all lines high before a read
		// request, so any lines which are not driven by an external device read as set. If
		// this is the case, we should implement this behaviour in the Z80 core, and drop
		// manual forcing of the line state in areas like this.
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0x1;
		data = 0xFFFF;
		break;
	case MemoryInterface::MemoryMode:
		//##TODO##
		break;
	case MemoryInterface::Z80BusReq:
		// Return true if the Z80 bus is not accessible by the M68000
		data.SetBit(0, (_z80BusResetLineState || !_z80BusRequestLineState || !_z80BusGrantLineState));
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0x01;
		break;
	case MemoryInterface::Z80Reset:
		// Don't assert any data lines
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0x0;
		break;
	case MemoryInterface::TMSS:
		//##TODO## Determine if reads from this location do anything
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0x0;
		break;
	case MemoryInterface::TMSSBootROMSwitch:
		//##TODO## Determine if reads from this location do anything
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0x0;
		break;
	}
	return accessResult;
}

//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult MDBusArbiter::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	IBusInterface::AccessResult accessResult(true);

	// Apply any changes to the line state that are pending at this time
	ApplyPendingLineStateChanges(accessTime);

	switch ((MemoryInterface)interfaceNumber)
	{
	case MemoryInterface::InterruptAcknowledgeCycle:
		// This should never happen if this device is mapped correctly
		accessResult.deviceReplied = false;
		break;
	case MemoryInterface::M68kToZ80MemoryWindow:
		if (!_z80BusResetLineState && _z80BusGrantLineState)
		{
			// Note that hardware tests performed by Charles MacDonald indicate that
			// Z80 memory access location is masked to a 15-bit result, which is important
			// as it prevents the M68000 calling back into the M68000 memory window region.
			unsigned int z80MemoryAccessLocation = (location & 0x7FFF);

			// Note that hardware tests have shown that word-wide access to the Z80 memory
			// space is not possible. When performing a read operation, instead of a true
			// 16-bit result being returned, the 8-bit result from the target address is
			// mirrored in both the upper and lower halves of the 16-bit data value. In the
			// case of a write, only one byte is written. In both cases, the state of the
			// LDS line determines whether the access occurs at an odd or even address in
			// the Z80 memory space. We emulate that behaviour here.
			//##TODO## It's quite probable that M68000 writes to the Z80 bus only perform
			// byte-wide writes, just like only byte-wide reads are possible. This is
			// strongly suggested also by the fact that all observed games so far only
			// perform byte-wide writes to the Z80 memory when loading Z80 code into
			// memory. Most likely, only the upper byte of a word-wide write is actually
			// written to Z80 RAM. We should perform hardware tests to confirm this.
			bool writeToOddAddress = (z80MemoryAccessLocation & 0x01) != 0;
			Data z80BusData(8);
			z80BusData = (writeToOddAddress)? data.GetLowerHalf(): data.GetUpperHalf();

			// Perform the write operation
			IBusInterface::AccessResult remoteAccessResult = _z80MemoryBus->WriteMemory(z80MemoryAccessLocation, z80BusData, caller, accessTime, accessContext);

			// Set the execution time of this operation to match the execution time of the
			// remote bus operation.
			accessResult.executionTime = remoteAccessResult.executionTime;
		}
		break;
	case MemoryInterface::Z80ToVDPMemoryWindow:{
		// Calculate the access location
		unsigned int m68kMemoryAccessLocation = 0xC00000 | (location & 0x00FF);

		// Writes from the Z80 bus to the M68000 bus duplicate the 8-bit write data in the
		// upper and lower halves of the 16-bit M68000 data bus.
		Data m68kBusData(16);
		m68kBusData.SetUpperHalf(data.GetData());
		m68kBusData.SetLowerHalf(data.GetData());

		// Perform the write operation
		double m68kBusAccessExecutionTime = 0.0;
		IBusInterface::AccessResult m68kBusAccessResult(true);
		if (!WriteZ80ToM68000(m68kMemoryAccessLocation, m68kBusData, caller, accessTime, accessContext, m68kBusAccessResult, m68kBusAccessExecutionTime))
		{
			//##TODO## Trigger an assert
			std::wcout << "-WriteZ80ToM68000 failed! " << location << '\t' << data.GetData() << '\n';
		}

		// Set the execution time of this operation to match the execution time of the
		// remote bus operation.
		accessResult.executionTime = m68kBusAccessExecutionTime;
		break;}
	case MemoryInterface::Z80ToM68kMemoryWindow:{
		// Calculate the access location
		std::unique_lock<std::mutex> lock(_bankswitchAccessMutex);
		unsigned int m68kMemoryAccessLocation = (_z80BankswitchDataCurrent.GetData() << 15) | (location & 0x7FFF);
		lock.unlock();

		// Writes from the Z80 bus to the M68000 bus duplicate the 8-bit write data in the
		// upper and lower halves of the 16-bit M68000 data bus.
		Data m68kBusData(16);
		m68kBusData.SetUpperHalf(data.GetData());
		m68kBusData.SetLowerHalf(data.GetData());

		// Perform the write operation
		double m68kBusAccessExecutionTime = 0.0;
		IBusInterface::AccessResult m68kBusAccessResult(true);
		if (!WriteZ80ToM68000(m68kMemoryAccessLocation, m68kBusData, caller, accessTime, accessContext, m68kBusAccessResult, m68kBusAccessExecutionTime))
		{
			//##TODO## Trigger an assert
			std::wcout << "-WriteZ80ToM68000 failed! " << location << '\t' << data.GetData() << '\n';
		}

		// Set the execution time of this operation to match the execution time of the
		// remote bus operation.
		accessResult.executionTime = m68kBusAccessExecutionTime;
		break;}
	case MemoryInterface::Z80WindowBankswitch:{
		// Handle changes to the Z80 bankswitch register
		std::unique_lock<std::mutex> lock(_bankswitchAccessMutex);
		_z80BankswitchDataNew.SetBit(_z80BankswitchBitsWritten, data.NonZero());
		++_z80BankswitchBitsWritten;
		if (_z80BankswitchBitsWritten == 9)
		{
			//##DEBUG##
			// std::wcout << L"Z80 bankswitch data changed from " << std::hex << std::uppercase << z80BankswitchDataCurrent.GetData() << " to " << z80BankswitchDataNew.GetData() << L" (" << (z80BankswitchDataCurrent.GetData() << 15) << L" to " << (z80BankswitchDataNew.GetData() << 15) << L")\n";

			_z80BankswitchDataCurrent = _z80BankswitchDataNew;
			_z80BankswitchDataNew = 0;
			_z80BankswitchBitsWritten = 0;
		}
		break;}
	case MemoryInterface::MemoryMode:
		//##TODO##
		break;
	case MemoryInterface::Z80BusReq:{
		// Z80 bus request
		bool newState = data.NonZero();
		if (_z80BusRequestLineState != newState)
		{
			_z80BusRequestLineState = newState;
			_z80MemoryBus->SetLineState((unsigned int)LineID::ZBR, Data(1, (unsigned int)_z80BusRequestLineState), GetDeviceContext(), caller, accessTime, accessContext);
		}
		break;}
	case MemoryInterface::Z80Reset:{
		// Z80 reset
		bool newState = !data.NonZero();
		if (_z80BusResetLineState != newState)
		{
			_z80BusResetLineState = newState;
			_z80MemoryBus->SetLineState((unsigned int)LineID::ZRES, Data(1, (unsigned int)(_sresLineState | _z80BusResetLineState)), GetDeviceContext(), caller, accessTime, accessContext);
		}
		break;}
	case MemoryInterface::TMSS:
		//##TODO## Although the official Sega security code writes the ASCII string "SEGA"
		// to this address as a long word write, it seems almost certain that this is
		// actually meaningless, and that all that matters is bit 9 of the write data.
		// Since this bit is set to true with the "GA" component of the write, this will
		// result in this bit being set to 1 when this register is written to. We assume
		// therefore, that like the boot ROM switch, the TMSS security register starts off
		// active, and is cleared when a single mapped data line disables this setting by
		// writing a value of 1 to it. This needs testing on the hardware.
		_vdpLockoutActive = !data.GetBit(0);
		break;
	case MemoryInterface::TMSSBootROMSwitch:
		//##TODO## Perform hardware tests to determine exactly which addresses this
		// register is accessible from.
		_bootROMEnabled = !data.GetBit(0);
		break;
	}
	return accessResult;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDBusArbiter::ReadZ80ToM68000(unsigned int m68kMemoryAccessLocation, Data& m68kBusData, IDeviceContext* caller, double accessTime, unsigned int accessContext, IBusInterface::AccessResult& m68kBusAccessResult, double& executionTime)
{
	std::unique_lock<std::mutex> lock(_lineMutex);

	// Set the initial access time for this memory operation
	double accessTimeCurrent = accessTime;

	// 1. If the VDP is currently requesting the bus, wait until it is finished, IE, until
	//   BR is not asserted. (possible infinite delay)
	if (_m68kBusRequestLineState)
	{
		lock.unlock();
		_m68kBusRequestLineStateChangeTimeLatchEnable = true;
		if (!_m68kMemoryBus->AdvanceToLineState((unsigned int)LineID::BR, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if (!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LineID::BR, Data(1, 0), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			// didn't find the requested line state change in our buffer. In this case
			// we need to trigger an assert.
			return false;
		}
		_m68kBusRequestLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	// 2. If the M68K is still in the process of re-acquiring the bus, wait until it is
	//   complete, IE, until BG is not asserted. (possible infinite delay)
	if (_m68kBusGrantLineState)
	{
		lock.unlock();
		_m68kBusGrantLineStateChangeTimeLatchEnable = true;
		if (!_m68kMemoryBus->AdvanceToLineState((unsigned int)LineID::BG, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if (!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LineID::BG, Data(1, 0), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			// didn't find the requested line state change in our buffer. In this case
			// we need to trigger an assert.
			return false;
		}
		_m68kBusGrantLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	// 3. Assert BR
	lock.unlock();
	_m68kMemoryBus->SetLineState((unsigned int)LineID::BR, Data(1, 1), GetDeviceContext(), caller, accessTimeCurrent, accessContext);
	lock.lock();

	// 4. Wait for BG to be set (possible infinite delay)
	if (!_m68kBusGrantLineState)
	{
		lock.unlock();
		_m68kBusGrantLineStateChangeTimeLatchEnable = true;
		if (!_m68kMemoryBus->AdvanceToLineState((unsigned int)LineID::BG, Data(1, 1), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if (!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LineID::BG, Data(1, 1), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			// didn't find the requested line state change in our buffer. In this case
			// we need to trigger an assert.
			return false;
		}
		_m68kBusGrantLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	// Release lineMutex, now that we've finished working with the current line state.
	lock.unlock();

	// 5. Perform the operation
	m68kBusAccessResult = _m68kMemoryBus->ReadMemory(m68kMemoryAccessLocation, m68kBusData, caller, accessTimeCurrent, accessContext);
	accessTimeCurrent += m68kBusAccessResult.executionTime;

	// 6. Negate BR
	_m68kMemoryBus->SetLineState((unsigned int)LineID::BR, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext);

	// Calculate the total execution time for the operation
	executionTime = accessTimeCurrent - accessTime;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDBusArbiter::WriteZ80ToM68000(unsigned int m68kMemoryAccessLocation, Data m68kBusData, IDeviceContext* caller, double accessTime, unsigned int accessContext, IBusInterface::AccessResult& m68kBusAccessResult, double& executionTime)
{
	std::unique_lock<std::mutex> lock(_lineMutex);

	// Set the initial access time for this memory operation
	double accessTimeCurrent = accessTime;

	// 1. If the VDP is currently requesting the bus, wait until it is finished, IE, until
	//   BR is not asserted. (possible infinite delay)
	if (_m68kBusRequestLineState)
	{
		lock.unlock();
		_m68kBusRequestLineStateChangeTimeLatchEnable = true;
		if (!_m68kMemoryBus->AdvanceToLineState((unsigned int)LineID::BR, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if (!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LineID::BR, Data(1, 0), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			// didn't find the requested line state change in our buffer. In this case
			// we need to trigger an assert.
			return false;
		}
		_m68kBusRequestLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	// 2. If the M68K is still in the process of re-acquiring the bus, wait until it is
	//   complete, IE, until BG is not asserted. (possible infinite delay)
	if (_m68kBusGrantLineState)
	{
		lock.unlock();
		_m68kBusGrantLineStateChangeTimeLatchEnable = true;
		if (!_m68kMemoryBus->AdvanceToLineState((unsigned int)LineID::BG, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if (!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LineID::BG, Data(1, 0), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			// didn't find the requested line state change in our buffer. In this case
			// we need to trigger an assert.
			return false;
		}
		_m68kBusGrantLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	// 3. Assert BR
	lock.unlock();
	_m68kMemoryBus->SetLineState((unsigned int)LineID::BR, Data(1, 1), GetDeviceContext(), caller, accessTimeCurrent, accessContext);
	lock.lock();

	// 4. Wait for BG to be set (possible infinite delay)
	if (!_m68kBusGrantLineState)
	{
		lock.unlock();
		_m68kBusGrantLineStateChangeTimeLatchEnable = true;
		if (!_m68kMemoryBus->AdvanceToLineState((unsigned int)LineID::BG, Data(1, 1), GetDeviceContext(), caller, accessTimeCurrent, accessContext))
		{
			m68kBusAccessResult.unpredictableBusDelay = true;
			return false;
		}
		if (!AdvanceUntilPendingLineStateChangeApplied(caller, accessTimeCurrent, accessContext, LineID::BG, Data(1, 1), accessTimeCurrent))
		{
			//##TODO## Our devices reported the target line state was reached, but we
			// didn't find the requested line state change in our buffer. In this case
			// we need to trigger an assert.
			return false;
		}
		_m68kBusGrantLineStateChangeTimeLatchEnable = false;
		lock.lock();
	}

	// Release lineMutex, now that we've finished working with the current line state.
	lock.unlock();

	// 5. Perform the operation
	m68kBusAccessResult = _m68kMemoryBus->WriteMemory(m68kMemoryAccessLocation, m68kBusData, caller, accessTimeCurrent, accessContext);
	accessTimeCurrent += m68kBusAccessResult.executionTime;

	// 6. Negate BR
	_m68kMemoryBus->SetLineState((unsigned int)LineID::BR, Data(1, 0), GetDeviceContext(), caller, accessTimeCurrent, accessContext);

	// Calculate the total execution time for the operation
	executionTime = accessTimeCurrent - accessTime;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// CE line state functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int MDBusArbiter::GetCELineID(const Marshal::In<std::wstring>& lineName, bool inputLine) const
{
	if (lineName == L"R/W")
	{
		return (unsigned int)CELineID::RW;
	}
	else if (lineName == L"OE0")
	{
		return (unsigned int)CELineID::OE0;
	}
	else if (lineName == L"UDS")
	{
		return (unsigned int)CELineID::UDS;
	}
	else if (lineName == L"LDS")
	{
		return (unsigned int)CELineID::LDS;
	}
	else if (lineName == L"CE0")
	{
		return (unsigned int)CELineID::CE0;
	}
	else if (lineName == L"BootROM")
	{
		return (unsigned int)CELineID::BootROM;
	}
	else if (lineName == L"ROM")
	{
		return (unsigned int)CELineID::ROM;
	}
	else if (lineName == L"ASEL")
	{
		return (unsigned int)CELineID::ASEL;
	}
	else if (lineName == L"FDC")
	{
		return (unsigned int)CELineID::FDC;
	}
	else if (lineName == L"FDWR")
	{
		return (unsigned int)CELineID::FDWR;
	}
	else if (lineName == L"TIME")
	{
		return (unsigned int)CELineID::TIME;
	}
	else if (lineName == L"IO")
	{
		return (unsigned int)CELineID::IO;
	}
	else if (lineName == L"EOE")
	{
		return (unsigned int)CELineID::EOE;
	}
	else if (lineName == L"NOE")
	{
		return (unsigned int)CELineID::NOE;
	}
	else if (lineName == L"ZRAM")
	{
		return (unsigned int)CELineID::ZRAM;
	}
	else if (lineName == L"SOUND")
	{
		return (unsigned int)CELineID::SOUND;
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch ((CELineID)lineID)
	{
	case CELineID::RW:
		_ceLineMaskReadHighWriteLow = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::OE0:
		_ceLineMaskOE0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::UDS:
		_ceLineMaskUDS = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::LDS:
		_ceLineMaskLDS = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch ((CELineID)lineID)
	{
	case CELineID::CE0:
		_ceLineMaskCE0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::BootROM:
		_ceLineMaskBootROM = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::ROM:
		_ceLineMaskROM = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::ASEL:
		_ceLineMaskASEL = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::FDC:
		_ceLineMaskFDC = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::FDWR:
		_ceLineMaskFDWR = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::TIME:
		_ceLineMaskTIME = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::IO:
		_ceLineMaskIO = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::EOE:
		_ceLineMaskEOE = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::NOE:
		_ceLineMaskNOE = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::ZRAM:
		_ceLineMaskZRAM = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELineID::SOUND:
		_ceLineMaskSOUND = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int MDBusArbiter::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const
{
	unsigned int result = 0;
	if (sourceBusInterface == _m68kMemoryBus)
	{
		bool operationIsWrite = (currentCELineState & _ceLineMaskReadHighWriteLow) == 0;
		bool ceLineUDS = (currentCELineState & _ceLineMaskUDS) != 0;
		bool ceLineLDS = (currentCELineState & _ceLineMaskLDS) != 0;
		bool ceLineOE0 = (currentCELineState & _ceLineMaskOE0) != 0;
		if (_cartInLineState)
		{
			result = BuildCELineM68K<true>(location, operationIsWrite, ceLineUDS, ceLineLDS, ceLineOE0, caller, accessTime);
		}
		else
		{
			result = BuildCELineM68K<false>(location, operationIsWrite, ceLineUDS, ceLineLDS, ceLineOE0, caller, accessTime);
		}
	}
	else if (sourceBusInterface == _z80MemoryBus)
	{
		result = BuildCELineZ80(location);
	}
	return result;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int MDBusArbiter::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const
{
	return CalculateCELineStateMemory(location, data, currentCELineState, sourceBusInterface, caller, calculateCELineStateContext, 0.0);
}

//----------------------------------------------------------------------------------------------------------------------
template<bool CartInLineAsserted>
unsigned int MDBusArbiter::BuildCELineM68K(unsigned int targetAddress, bool write, bool ceLineUDS, bool ceLineLDS, bool ceLineOE0, IDeviceContext* caller, double accessTime) const
{
	//##TODO## It seems clear that if the FC lines from the M68000 indicate a CPU space
	// cycle (all asserted), then these CE output lines shouldn't be asserted. In
	// particular, EOE and NOE can't be asserted, otherwise the RAM would respond to an
	// interrupt vector request in the Mega Drive, since the RAS0 line would still be
	// asserted by the VDP, which doesn't know what the FC lines are outputting.
	//##TODO## Attempt to improve performance of CE line state generation, using a lookup
	// table containing pre-calculated ceLineState values for every address and input line
	// combination.

	// Calculate the state of all the various CE lines
	bool lineBootROM;
	bool lineCE0;
	bool lineROM;
	if (CartInLineAsserted)
	{
		bool targetAddressLessThan0x400000 = (targetAddress <= 0x3FFFFF);
		lineBootROM = _bootROMEnabled && _activateBootROM && targetAddressLessThan0x400000;
		lineCE0 = !lineBootROM && targetAddressLessThan0x400000;
		lineROM = !targetAddressLessThan0x400000 && (targetAddress <= 0x5FFFFF);
	}
	else
	{
		lineBootROM = false;
		lineCE0 = (targetAddress >= 0x400000) && (targetAddress <= 0x7FFFFF);
		lineROM = (targetAddress <= 0x1FFFFF);
	}
	bool lineASEL = !lineBootROM && (targetAddress <= 0x7FFFFF);
	bool lineFDC = (targetAddress >= 0xA12000) && (targetAddress <= 0xA120FF);
	bool lineFDWR = write && lineFDC;
	bool lineTIME = (targetAddress >= 0xA13000) && (targetAddress <= 0xA130FF);
	bool lineIO = (targetAddress >= 0xA10000) && (targetAddress <= 0xA100FF);
	bool lineEOE = ceLineOE0 && ceLineUDS;
	bool lineNOE = ceLineOE0 && ceLineLDS;

	//##TODO## Confirm the mapping of CAS2 and RAS2, and implement them here.
	// bool lineCAS2 = (targetAddress <= 0x7FFFFF);
	// bool lineRAS2 = (targetAddress >= 0xE00000) && (targetAddress <= 0xFFFFFF);

	// Build the actual CE line state based on the asserted CE lines
	unsigned int ceLineState = 0;
	ceLineState |= ~((unsigned int)lineCE0 - 1) & _ceLineMaskCE0; // lineCE0? _ceLineMaskCE0: 0x0;
	ceLineState |= ~((unsigned int)lineBootROM - 1) & _ceLineMaskBootROM; // lineBootROM? _ceLineMaskBootROM: 0x0;
	ceLineState |= ~((unsigned int)lineROM - 1) & _ceLineMaskROM; // lineROM? _ceLineMaskROM: 0x0;
	ceLineState |= ~((unsigned int)lineASEL - 1) & _ceLineMaskASEL; // lineASEL? _ceLineMaskASEL: 0x0;
	ceLineState |= ~((unsigned int)lineFDC - 1) & _ceLineMaskFDC; // lineFDC? _ceLineMaskFDC: 0x0;
	ceLineState |= ~((unsigned int)lineFDWR - 1) & _ceLineMaskFDWR; // lineFDWR? _ceLineMaskFDWR: 0x0;
	ceLineState |= ~((unsigned int)lineTIME - 1) & _ceLineMaskTIME; // lineTIME? _ceLineMaskTIME: 0x0;
	ceLineState |= ~((unsigned int)lineIO - 1) & _ceLineMaskIO; // lineIO? _ceLineMaskIO: 0x0;
	ceLineState |= ~((unsigned int)lineEOE - 1) & _ceLineMaskEOE; // lineEOE? _ceLineMaskEOE: 0x0;
	ceLineState |= ~((unsigned int)lineNOE - 1) & _ceLineMaskNOE; // lineNOE? _ceLineMaskNOE: 0x0;

	// If TMSS is active, and a device is attempting to access the VDP address range,
	// assert the VRES and HALT lines. Note that according to tests performed by Charles
	// MacDonald, this state is not cleared on a soft reset. A full power cycle is required
	// in order to restore normal operation.
	if (_vdpLockoutActive && _activateTMSS && (!_activateBootROM || !_bootROMEnabled) && (targetAddress >= 0xC00000) && (targetAddress <= 0xDFFFFF))
	{
		_vdpLockoutTripped = true;
		_m68kMemoryBus->SetLineState((unsigned int)LineID::HALT, Data(1, (unsigned int)_vdpLockoutTripped), GetDeviceContext(), caller, accessTime, 0);
		_m68kMemoryBus->SetLineState((unsigned int)LineID::VRES, Data(1, (unsigned int)_vdpLockoutTripped), GetDeviceContext(), caller, accessTime, 0);
	}

	// Return the generated CE line state
	return ceLineState;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int MDBusArbiter::BuildCELineZ80(unsigned int targetAddress) const
{
	// Calculate the state of all the various CE lines
	bool lineZRAM = (targetAddress <= 0x3FFF);
	bool lineSOUND = (targetAddress >= 0x4000) && (targetAddress <= 0x5FFF);

	// Build the actual CE line state based on the asserted CE lines
	unsigned int ceLineState = 0;
	ceLineState |= lineZRAM? _ceLineMaskZRAM: 0x0;
	ceLineState |= lineSOUND? _ceLineMaskSOUND: 0x0;

	// Return the generated CE line state
	return ceLineState;
}

//----------------------------------------------------------------------------------------------------------------------
// Line functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int MDBusArbiter::GetLineID(const Marshal::In<std::wstring>& lineName) const
{
	if (lineName == L"CART")
	{
		return (unsigned int)LineID::CART;
	}
	else if (lineName == L"VPA")
	{
		return (unsigned int)LineID::VPA;
	}
	else if (lineName == L"INTAK")
	{
		return (unsigned int)LineID::INTAK;
	}
	else if (lineName == L"BR")
	{
		return (unsigned int)LineID::BR;
	}
	else if (lineName == L"BG")
	{
		return (unsigned int)LineID::BG;
	}
	else if (lineName == L"ZBR")
	{
		return (unsigned int)LineID::ZBR;
	}
	else if (lineName == L"ZBAK")
	{
		return (unsigned int)LineID::ZBAK;
	}
	else if (lineName == L"ZRES")
	{
		return (unsigned int)LineID::ZRES;
	}
	else if (lineName == L"VRES")
	{
		return (unsigned int)LineID::VRES;
	}
	else if (lineName == L"HALT")
	{
		return (unsigned int)LineID::HALT;
	}
	else if (lineName == L"SRES")
	{
		return (unsigned int)LineID::SRES;
	}
	else if (lineName == L"WRES")
	{
		return (unsigned int)LineID::WRES;
	}
	else if (lineName == L"ActivateTMSS")
	{
		return (unsigned int)LineID::ActivateTMSS;
	}
	else if (lineName == L"ActivateBootROM")
	{
		return (unsigned int)LineID::ActivateBootROM;
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> MDBusArbiter::GetLineName(unsigned int lineID) const
{
	switch ((LineID)lineID)
	{
	case LineID::CART:
		return L"CART";
	case LineID::VPA:
		return L"VPA";
	case LineID::INTAK:
		return L"INTAK";
	case LineID::BR:
		return L"BR";
	case LineID::BG:
		return L"BG";
	case LineID::ZBR:
		return L"ZBR";
	case LineID::ZBAK:
		return L"ZBAK";
	case LineID::ZRES:
		return L"ZRES";
	case LineID::VRES:
		return L"VRES";
	case LineID::HALT:
		return L"HALT";
	case LineID::SRES:
		return L"SRES";
	case LineID::WRES:
		return L"WRES";
	case LineID::ActivateTMSS:
		return L"ActivateTMSS";
	case LineID::ActivateBootROM:
		return L"ActivateBootROM";
	}
	return L"";
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int MDBusArbiter::GetLineWidth(unsigned int lineID) const
{
	switch ((LineID)lineID)
	{
	case LineID::CART:
		return 1;
	case LineID::VPA:
		return 1;
	case LineID::INTAK:
		return 1;
	case LineID::BR:
		return 1;
	case LineID::BG:
		return 1;
	case LineID::ZBR:
		return 1;
	case LineID::ZBAK:
		return 1;
	case LineID::ZRES:
		return 1;
	case LineID::VRES:
		return 1;
	case LineID::HALT:
		return 1;
	case LineID::SRES:
		return 1;
	case LineID::WRES:
		return 1;
	case LineID::ActivateTMSS:
		return 1;
	case LineID::ActivateBootROM:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(_lineMutex);

	// Flag that an entry exists in the buffer. This flag is used to skip the expensive
	// locking operation in the active thread for this device when no line changes are
	// pending. Note that we set this flag before we've actually written the entry into
	// the buffer, as we want to force the active thread to lock on the beginning of the
	// next cycle while this function is executing, so that the current timeslice progress
	// of the device doesn't change after we've read it.
	_lineAccessPending = true;

	// Read the time at which this access is being made, and trigger a rollback if we've
	// already passed that time.
	if (_lastLineCheckTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, _lastLineCheckTime, accessContext);
	}

	// If this is a line state change which needs to be processed immediately, apply it now
	// and return.
	switch ((LineID)targetLine)
	{
	case LineID::CART:
		_cartInLineState = lineData.NonZero();
		return;
	case LineID::ActivateTMSS:
		_activateTMSS = lineData.NonZero();
		return;
	case LineID::ActivateBootROM:
		_activateBootROM = lineData.NonZero();
		return;
	case LineID::WRES:{
		bool wresLineStateNew = lineData.LSB();
		if (wresLineStateNew != _wresLineState)
		{
			// Determine the new line states
			bool vresLineStateNew = wresLineStateNew | _vdpLockoutTripped;
			bool haltLineStateNew = wresLineStateNew | _vdpLockoutTripped;
			bool sresLineStateNew = wresLineStateNew;
			bool z80BusResetLineStateNew = sresLineStateNew | _z80BusResetLineState;

			// Apply the new line states
			_wresLineState = wresLineStateNew;
			_vresLineState = vresLineStateNew;
			_haltLineState = haltLineStateNew;
			_sresLineState = sresLineStateNew;

			//##TODO## Determine what other internal state should be cleared here, such as
			// the Z80/M68K bus request state.

			// Release our lock on the line mutex
			lock.unlock();

			// Apply our line state changes
			_m68kMemoryBus->SetLineState((unsigned int)LineID::VRES, Data(1, (unsigned int)vresLineStateNew), GetDeviceContext(), caller, accessTime, accessContext);
			_m68kMemoryBus->SetLineState((unsigned int)LineID::HALT, Data(1, (unsigned int)haltLineStateNew), GetDeviceContext(), caller, accessTime, accessContext);
			_m68kMemoryBus->SetLineState((unsigned int)LineID::SRES, Data(1, (unsigned int)sresLineStateNew), GetDeviceContext(), caller, accessTime, accessContext);
			_z80MemoryBus->SetLineState((unsigned int)LineID::ZRES, Data(1, (unsigned int)z80BusResetLineStateNew), GetDeviceContext(), caller, accessTime, accessContext);
		}
		return;}
	}

	// Insert the line access into the buffer. Note that entries in the buffer are sorted
	// by access time from lowest to highest.
	std::list<LineAccess>::reverse_iterator i = _lineAccessBuffer.rbegin();
	while ((i != _lineAccessBuffer.rend()) && (i->accessTime > accessTime))
	{
		++i;
	}
	_lineAccessBuffer.insert(i.base(), LineAccess((LineID)targetLine, lineData, accessTime));
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{
	SetLineState(targetLine, lineData, 0, _lastTimesliceLength, 0);
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(_lineMutex);

	// Read the time at which this access is being made, and trigger a rollback if we've
	// already passed that time.
	if (_lastLineCheckTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, _lastLineCheckTime, accessContext);
	}

	// Find the matching line state change entry in the line access buffer
	std::list<LineAccess>::reverse_iterator i = _lineAccessBuffer.rbegin();
	bool foundTargetEntry = false;
	while (!foundTargetEntry && (i != _lineAccessBuffer.rend()))
	{
		if ((i->lineID == (LineID)targetLine) && (i->state == lineData) && (i->accessTime == reportedTime))
		{
			foundTargetEntry = true;
			continue;
		}
		++i;
	}

	// Erase the target line state change entry from the line access buffer
	if (foundTargetEntry)
	{
		_lineAccessBuffer.erase((++i).base());
	}
	else
	{
		//##DEBUG##
		std::wcout << "Failed to find matching line state change in RevokeSetLineState! " << GetLineName(targetLine) << '\t' << lineData.GetData() << '\t' << reportedTime << '\t' << accessTime << '\n';
	}

	// Update the lineAccessPending flag
	_lineAccessPending = !_lineAccessBuffer.empty();
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::AssertCurrentOutputLineState() const
{
	if (_z80MemoryBus != 0)
	{
		if (_sresLineState | _z80BusResetLineState)   _z80MemoryBus->SetLineState((unsigned int)LineID::ZRES, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_z80BusRequestLineState) _z80MemoryBus->SetLineState((unsigned int)LineID::ZBR, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_vresLineState)          _m68kMemoryBus->SetLineState((unsigned int)LineID::VRES, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_haltLineState)          _m68kMemoryBus->SetLineState((unsigned int)LineID::HALT, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_sresLineState)          _m68kMemoryBus->SetLineState((unsigned int)LineID::SRES, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::NegateCurrentOutputLineState() const
{
	if (_z80MemoryBus != 0)
	{
		if (_sresLineState | _z80BusResetLineState)   _z80MemoryBus->SetLineState((unsigned int)LineID::ZRES, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_z80BusRequestLineState) _z80MemoryBus->SetLineState((unsigned int)LineID::ZBR, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_vresLineState)          _m68kMemoryBus->SetLineState((unsigned int)LineID::VRES, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_haltLineState)          _m68kMemoryBus->SetLineState((unsigned int)LineID::HALT, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_sresLineState)          _m68kMemoryBus->SetLineState((unsigned int)LineID::SRES, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::ApplyLineStateChange(LineID targetLine, const Data& lineData, double accessTime)
{
	bool newLineState = lineData.NonZero();
	switch (targetLine)
	{
	case LineID::BR:
		if (_m68kBusRequestLineStateChangeTimeLatchEnable && (_m68kBusRequestLineState != newLineState))
		{
			_m68kBusRequestLineStateChangeTimeLatch = accessTime;
			_m68kBusRequestLineStateChangeTimeLatchEnable = false;
		}
		_m68kBusRequestLineState = newLineState;
		break;
	case LineID::BG:
		if (_m68kBusGrantLineStateChangeTimeLatchEnable && (_m68kBusGrantLineState != newLineState))
		{
			_m68kBusGrantLineStateChangeTimeLatch = accessTime;
			_m68kBusGrantLineStateChangeTimeLatchEnable = false;
		}
		_m68kBusGrantLineState = newLineState;
		break;
	case LineID::ZBR:
		if (_z80BusRequestLineStateChangeTimeLatchEnable && (_z80BusRequestLineState != newLineState))
		{
			_z80BusRequestLineStateChangeTimeLatch = accessTime;
			_z80BusRequestLineStateChangeTimeLatchEnable = false;
		}
		_z80BusRequestLineState = newLineState;
		break;
	case LineID::ZBAK:
		if (_z80BusGrantLineStateChangeTimeLatchEnable && (_z80BusGrantLineState != newLineState))
		{
			_z80BusGrantLineStateChangeTimeLatch = accessTime;
			_z80BusGrantLineStateChangeTimeLatchEnable = false;
		}
		_z80BusGrantLineState = newLineState;
		break;
	case LineID::ZRES:
		if (_z80BusResetLineStateChangeTimeLatchEnable && (_z80BusResetLineState != newLineState))
		{
			_z80BusResetLineStateChangeTimeLatch = accessTime;
			_z80BusResetLineStateChangeTimeLatchEnable = false;
		}
		_z80BusResetLineState = newLineState;
		break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::ApplyPendingLineStateChanges(double accessTime)
{
	// If we have any pending line state changes waiting, apply any which we have now
	// reached.
	if (_lineAccessPending)
	{
		std::unique_lock<std::mutex> lock(_lineMutex);
		double currentTimesliceProgress = accessTime;
		bool done = false;
		std::list<LineAccess>::iterator i = _lineAccessBuffer.begin();
		while (!done && (i != _lineAccessBuffer.end()))
		{
			if (i->accessTime <= currentTimesliceProgress)
			{
				ApplyLineStateChange(i->lineID, i->state, i->accessTime);
				++i;
			}
			else
			{
				done = true;
			}
		}

		// Clear any completed entries from the list
		_lineAccessBuffer.erase(_lineAccessBuffer.begin(), i);
		_lineAccessPending = !_lineAccessBuffer.empty();
	}
	_lastLineCheckTime = accessTime;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDBusArbiter::AdvanceUntilPendingLineStateChangeApplied(IDeviceContext* caller, double accessTime, unsigned int accessContext, LineID targetLine, Data targetLineState, double& lineStateReachedTime)
{
	std::unique_lock<std::mutex> lock(_lineMutex);

	// If the target line is currently sitting on the requested line state, return true.
	//##TODO## Update this comment
	bool targetLineCurrentlyMatchesState = false;
	switch (targetLine)
	{
	case LineID::BR:
		targetLineCurrentlyMatchesState = (_m68kBusRequestLineState == targetLineState.NonZero());
		if (targetLineCurrentlyMatchesState)
		{
			lineStateReachedTime = _m68kBusRequestLineStateChangeTimeLatch;
		}
		break;
	case LineID::BG:
		targetLineCurrentlyMatchesState = (_m68kBusGrantLineState == targetLineState.NonZero());
		if (targetLineCurrentlyMatchesState)
		{
			lineStateReachedTime = _m68kBusGrantLineStateChangeTimeLatch;
		}
		break;
	case LineID::ZBAK:
		targetLineCurrentlyMatchesState = (_z80BusGrantLineState == targetLineState.NonZero());
		if (targetLineCurrentlyMatchesState)
		{
			lineStateReachedTime = _z80BusGrantLineStateChangeTimeLatch;
		}
		break;
	}
	if (targetLineCurrentlyMatchesState)
	{
		return true;
	}

	// If we don't have a pending line state change in the buffer which matches the target
	// line and state, return false.
	bool foundTargetStateChange = false;
	std::list<LineAccess>::iterator i = _lineAccessBuffer.begin();
	while (!foundTargetStateChange && (i != _lineAccessBuffer.end()))
	{
		foundTargetStateChange = ((i->lineID == targetLine) && (i->state == targetLineState));
		++i;
	}
	if (!foundTargetStateChange)
	{
		return false;
	}

	// Advance the line state buffer until the target line state change is applied
	bool targetLineStateReached = false;
	i = _lineAccessBuffer.begin();
	while (!targetLineStateReached && (i != _lineAccessBuffer.end()))
	{
		ApplyLineStateChange(i->lineID, i->state, i->accessTime);
		targetLineStateReached = ((i->lineID == targetLine) && (i->state == targetLineState));
		lineStateReachedTime = i->accessTime;
		++i;
	}

	// Clear any completed entries from the list
	_lineAccessBuffer.erase(_lineAccessBuffer.begin(), i);
	_lineAccessPending = !_lineAccessBuffer.empty();

	// Return the result of the advance operation. If the logic of our above implementation
	// is correct, we should always return true at this point, since failure cases were
	// caught before the line state change buffer was advanced.
	return targetLineStateReached;
}

//----------------------------------------------------------------------------------------------------------------------
// Savestate functions
//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::LoadState(IHierarchicalStorageNode& node)
{
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for (std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if ((*i)->GetName() == L"ActivateTMSS")
		{
			_activateTMSS = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"ActivateBootROM")
		{
			_activateBootROM = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"BootROMEnabled")
		{
			_bootROMEnabled = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"VDPLockoutActive")
		{
			_vdpLockoutActive = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"VDPLockoutTripped")
		{
			_vdpLockoutTripped = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"Z80BankswitchDataCurrent")
		{
			_z80BankswitchDataCurrent = (*i)->ExtractHexData<unsigned int>();
		}
		else if ((*i)->GetName() == L"Z80BankswitchDataNew")
		{
			_z80BankswitchDataNew = (*i)->ExtractHexData<unsigned int>();
		}
		else if ((*i)->GetName() == L"Z80BankswitchBitsWritten")
		{
			_z80BankswitchBitsWritten = (*i)->ExtractData<unsigned int>();
		}
		else if ((*i)->GetName() == L"CartInLineState")
		{
			_cartInLineState = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"Z80BusRequestLineState")
		{
			_z80BusRequestLineState = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"Z80BusGrantLineState")
		{
			_z80BusGrantLineState = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"Z80BusResetLineState")
		{
			_z80BusResetLineState = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"M68KBusRequestLineState")
		{
			_m68kBusRequestLineState = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"M68KBusGrantLineState")
		{
			_m68kBusGrantLineState = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"VRESLineState")
		{
			_vresLineState = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"HALTLineState")
		{
			_haltLineState = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"SRESLineState")
		{
			_sresLineState = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"WRESLineState")
		{
			_wresLineState = (*i)->ExtractData<bool>();
		}
		else if ((*i)->GetName() == L"LastTimesliceLength")
		{
			_lastTimesliceLength = (*i)->ExtractData<bool>();
		}
		// Restore the lineAccessBuffer state
		else if ((*i)->GetName() == L"LineAccessBuffer")
		{
			_lineAccessBuffer.clear();
			IHierarchicalStorageNode& lineAccessBufferNode = *(*i);
			std::list<IHierarchicalStorageNode*> lineAccessBufferChildList = lineAccessBufferNode.GetChildList();
			for (std::list<IHierarchicalStorageNode*>::iterator lineAccessBufferEntry = lineAccessBufferChildList.begin(); lineAccessBufferEntry != lineAccessBufferChildList.end(); ++lineAccessBufferEntry)
			{
				if ((*lineAccessBufferEntry)->GetName() == L"LineAccess")
				{
					IHierarchicalStorageAttribute* lineNameAttribute = (*lineAccessBufferEntry)->GetAttribute(L"LineName");
					IHierarchicalStorageAttribute* lineStateAttribute = (*lineAccessBufferEntry)->GetAttribute(L"LineState");
					IHierarchicalStorageAttribute* accessTimeAttribute = (*lineAccessBufferEntry)->GetAttribute(L"AccessTime");
					if ((lineNameAttribute != 0) && (lineStateAttribute != 0) && (accessTimeAttribute != 0))
					{
						// Extract the entry from the XML stream
						std::wstring lineName = lineNameAttribute->ExtractValue<std::wstring>();
						double accessTime = accessTimeAttribute->ExtractValue<double>();
						unsigned int lineID = GetLineID(lineName.c_str());
						if (lineID != 0)
						{
							Data lineState(GetLineWidth(lineID));
							lineStateAttribute->ExtractValue(lineState);
							LineAccess lineAccess((LineID)lineID, lineState, accessTime);

							// Find the correct location in the list to insert the entry. The
							// list must be sorted from earliest to latest.
							std::list<LineAccess>::reverse_iterator j = _lineAccessBuffer.rbegin();
							while ((j != _lineAccessBuffer.rend()) && (j->accessTime > lineAccess.accessTime))
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

//----------------------------------------------------------------------------------------------------------------------
void MDBusArbiter::SaveState(IHierarchicalStorageNode& node) const
{
	node.CreateChild(L"ActivateTMSS", _activateTMSS);
	node.CreateChild(L"ActivateBootROM", _activateBootROM);
	node.CreateChild(L"BootROMEnabled", _bootROMEnabled);
	node.CreateChild(L"VDPLockoutActive", _vdpLockoutActive);
	node.CreateChild(L"VDPLockoutTripped", _vdpLockoutTripped);
	node.CreateChildHex(L"Z80BankswitchDataCurrent", _z80BankswitchDataCurrent.GetData(), _z80BankswitchDataCurrent.GetHexCharCount());
	node.CreateChildHex(L"Z80BankswitchDataNew", _z80BankswitchDataNew.GetData(), _z80BankswitchDataNew.GetHexCharCount());
	node.CreateChild(L"Z80BankswitchBitsWritten", _z80BankswitchBitsWritten);
	node.CreateChild(L"CartInLineState", _cartInLineState);
	node.CreateChild(L"Z80BusRequestLineState", _z80BusRequestLineState);
	node.CreateChild(L"Z80BusGrantLineState", _z80BusGrantLineState);
	node.CreateChild(L"Z80BusResetLineState", _z80BusResetLineState);
	node.CreateChild(L"M68KBusRequestLineState", _m68kBusRequestLineState);
	node.CreateChild(L"M68KBusGrantLineState", _m68kBusGrantLineState);
	node.CreateChild(L"VRESLineState", _vresLineState);
	node.CreateChild(L"HALTLineState", _haltLineState);
	node.CreateChild(L"SRESLineState", _sresLineState);
	node.CreateChild(L"WRESLineState", _wresLineState);
	node.CreateChild(L"LastTimesliceLength", _lastTimesliceLength);

	// Save the lineAccessBuffer state
	if (_lineAccessPending)
	{
		IHierarchicalStorageNode& lineAccessState = node.CreateChild(L"LineAccessBuffer");
		for (std::list<LineAccess>::const_iterator i = _lineAccessBuffer.begin(); i != _lineAccessBuffer.end(); ++i)
		{
			IHierarchicalStorageNode& lineAccessEntry = lineAccessState.CreateChild(L"LineAccess");
			lineAccessEntry.CreateAttribute(L"LineName", GetLineName((unsigned int)i->lineID));
			lineAccessEntry.CreateAttribute(L"LineState", i->state);
			lineAccessEntry.CreateAttribute(L"AccessTime", i->accessTime);
		}
	}
}
