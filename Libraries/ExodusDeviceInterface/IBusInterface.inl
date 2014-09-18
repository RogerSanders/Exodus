//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct IBusInterface::AccessResult
{
	AccessResult(bool adeviceReplied = true, bool aaccessMaskUsed = false, unsigned int aaccessMask = 0, bool abusError = false, double aexecutionTime = 0, bool aunpredictableBusDelay = false)
	:deviceReplied(adeviceReplied), accessMaskUsed(aaccessMaskUsed), accessMask(aaccessMask), busError(abusError), executionTime(aexecutionTime), unpredictableBusDelay(aunpredictableBusDelay)
	{}

	//##TODO## Replace this "deviceReplied" flag with something better. What this is
	//really attempting to support is the DTACK line for the M68000, but the actual target
	//device doesn't really know whether DTACK should be asserted in most cases, since
	//DTACK is only driven by the bus arbiter or the VDP. I think, in reality, devices
	//that assert DTACK should do so in response to a call to CalculateCELineStateMemory.
	//They know their own state, and they're given the access time, so they should be able
	//to assert the DTACK line if they're driving DTACK at that time based on whether the
	//target address should assert DTACK, and the M68000 can process this line state
	//change immediately outside a lock, and use it to determine whether it should
	//deadlock. This should work in all cases, and be quite efficient now that we've
	//optimized line-based communication.
	bool deviceReplied;
	//##TODO## This busError flag is completely inappropriate. Bus errors should be
	//flagged by asserting some kind of input line to the calling device that indicates a
	//bus error. This flag will never be used, and should be removed.
	bool busError;
	//##TODO## This failure state is designed to support unpredictable bus delays. Where an
	//unpredictable bus delay occurs through a normal bus access, this error flag is set,
	//which will allow the caller to then call a new order of bus access functions to
	//resolve the unpredictable bus grant time. I would suggest a pseudo-line system,
	//where the caller can call a function on this device to request a notification be
	//sent when the bus becomes available, then the calling device can enter a suspended
	//state until a pseudo-line to that device is asserted with a specific "token" value
	//for that device, indicating that the bus has been obtained, and that the access
	//should now be attempted again. This model will allow us to handle unpredictable
	//timing points in devices that can support this suspension model, which will most
	//likely only be microcode-level emulated processors, or RISC-style processors, which
	//only execute one bus operation within a single indivisible unit of execution.
	//##TODO## Eliminate this entirely. Rely on line-based communication to indicate that
	//an access couldn't be predicted as well, since for this kind of model, there needs
	//to be line-based interaction anyway.
	bool unpredictableBusDelay;
	//##TODO## Eliminate this "accessMaskUsed" flag. Instead, just always populate the
	//access mask in the BusInterface class, setting it to the data bus mask. This should
	//be provided to the called device. The called device can then modify it as it
	//chooses, with the BusInterface using that modified mask and returning it to the
	//caller. If the called device uses data line remapping, the BusInterface will check
	//the supplied access mask, and if it has been modified, the remapping table will be
	//used to convert it back to the same mapping as the caller expects.
	bool accessMaskUsed;
	//##TODO## Strongly consider making this, and all location arguments for access
	//functions, use the Data class rather than unsigned int. Making this argument be
	//unsigned int explicitly doesn't allow us to support systems with a bus width greater
	//than 32, while the Data class theoretically can be extended to allow us to support
	//systems with any bus width, without breaking existing code.
	unsigned int accessMask;
	//##TODO## Get rid of this "executionTime" argument entirely, and completely change
	//the implementation of all devices that rely on it to use line-based communication
	//concurrently with memory access in order to reproduce this behaviour. The rationale
	//for this change is rather simple: The M68000 has DTACK and the Z80 has WAIT, both
	//serve the purpose of inserting extra wait cycles before a given memory bus operation
	//is terminated. The DTACK and WAIT lines are the only means of altering the read or
	//write bus timing that an external device has. The two devices may share no common
	//clock source, and they don't need to, nor does it matter how long it takes a device
	//to fully process a read or a write, all that matters is whether DTACK or WAIT are
	//asserted, and when, and for how long. If we switch to this model, not only will we
	//be able to accurately bus interaction timing at a true cycle-level for device
	//communication, the implementation will actually become simpler.
	double executionTime;
};

//##TODO## Revise our interface based on the above changes, so that our memory access
//functions now look like this:
//bool ReadMemory(const Data& address, Data& data, Data& assertedBitMask, IDeviceContext* caller, double accessTime, unsigned int accessContext = 0, void* calculateCELineStateContext = 0);
//bool WriteMemory(const Data& address, const Data& data, const Data& assertedBitMask, IDeviceContext* caller, double accessTime, unsigned int accessContext = 0, void* calculateCELineStateContext = 0);
//address:
//  This argument replaces the old "location" argument, and gives the address in a Data
//  structure, for easier decoding at the target. This will actually probably boost
//  efficiency, since we seem to be always starting with a Data parameter on the source
//  anyway, and we often re-pack it into a Data structure on the other end for complex
//  devices anyway, to assist in decoding.
//assertedBitMask:
//  This argument defines which bits were modified during a read operation, and which bits
//  are being driven in a write operation. For a read operation, this is basically
//  equivalent to the legacy accessMask parameter. For writes, the assertedBitMask
//  parameter is new, and will be used to mask the provided input data before writing it
//  to the target. These parameters will also work with a new parameter defined on the bus
//  itself, which is "FloatingLineForcedState". This parameter defines a mask, which
//  specifies the default values for lines which are not driven. The BusInterface class
//  will combine the read or written data with the contents of this mask for any
//  non-asserted bits. This will allow us to fully support tri-state logic for bus access,
//  with full support for pull-up and pull-down resistors.
//return value:
//  The return value from these operations will be a boolean result which indicates
//  whether a device was actually mapped to the target. This information has never been
//  returned before, and a clear use is not identified, except that it might be useful for
//  debug purposes to report to the user when they access a memory address which is known
//  in the context of the system to be unmapped.

//##TODO## Make equivalent changes to the port-based communication functions
//##TODO## Make sure we default accessContext to 0 for all functions that take it
//##TODO## Add an assertedBitMask parameter to all the line-based communication functions
//too, so they can finally support full tri-state line changes.

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IBusInterface::ThisIBusInterfaceVersion()
{
	return 1;
}
