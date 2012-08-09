//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct IBusInterface::AccessResult
{
	AccessResult(bool adeviceReplied = true, bool aaccessMaskUsed = false, unsigned int aaccessMask = 0, bool abusError = false, double aexecutionTime = 0, bool aunpredictableBusDelay = false)
	:deviceReplied(adeviceReplied), accessMaskUsed(aaccessMaskUsed), accessMask(aaccessMask), busError(abusError), executionTime(aexecutionTime), unpredictableBusDelay(aunpredictableBusDelay)
	{}

	bool deviceReplied;
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
	bool unpredictableBusDelay;
	bool accessMaskUsed;
	unsigned int accessMask;
	double executionTime;
};
