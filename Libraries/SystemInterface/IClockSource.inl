//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IClockSource::ClockType
{
	CLOCKTYPE_DIRECT,
	CLOCKTYPE_DIVIDER,
	CLOCKTYPE_MULTIPLIER
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IClockSource::ThisIClockSourceVersion()
{
	return 1;
}
