//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct ClockSource::ClockSourceParams
{
	ClockSourceParams()
	:clockTypeDefined(false),
	 initialValueDefined(false)
	{}

	bool clockTypeDefined;
	bool initialValueDefined;

	ClockType clockType;
	double initialValue;
};
