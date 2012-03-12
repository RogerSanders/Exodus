//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct IBusInterface::AccessResult
{
	AccessResult(bool adeviceReplied = true, bool aaccessMaskUsed = false, unsigned int aaccessMask = 0, bool abusError = false, double aexecutionTime = 0)
	:deviceReplied(adeviceReplied), accessMaskUsed(aaccessMaskUsed), accessMask(aaccessMask), busError(abusError), executionTime(aexecutionTime)
	{}

	bool deviceReplied;
	bool busError;
	bool accessMaskUsed;
	unsigned int accessMask;
	double executionTime;
};
