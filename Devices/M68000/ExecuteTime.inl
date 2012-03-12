namespace M68000 {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExecuteTime::ExecuteTime()
:cycles(0), busRead(0), busWrite(0), additionalTime(0)
{}

//----------------------------------------------------------------------------------------
ExecuteTime::ExecuteTime(unsigned int acycles, unsigned int abusRead, unsigned int abusWrite, double aadditionalTime)
:cycles(acycles), busRead(abusRead), busWrite(abusWrite), additionalTime(aadditionalTime)
{}

//----------------------------------------------------------------------------------------
//Set functions
//----------------------------------------------------------------------------------------
void ExecuteTime::Set(unsigned int acycles, unsigned int abusRead, unsigned int abusWrite, double aadditionalTime)
{
	cycles = acycles;
	busRead = abusRead;
	busWrite = abusWrite;
	additionalTime = aadditionalTime;
}

//----------------------------------------------------------------------------------------
//Addition operators
//----------------------------------------------------------------------------------------
ExecuteTime ExecuteTime::operator+(const ExecuteTime& target) const
{
	return ExecuteTime(cycles + target.cycles, busRead + target.busRead, busWrite + target.busWrite, additionalTime + target.additionalTime);
}

//----------------------------------------------------------------------------------------
ExecuteTime& ExecuteTime::operator+=(const ExecuteTime& target)
{
	cycles += target.cycles;
	busRead += target.busRead;
	busWrite += target.busWrite;
	additionalTime += target.additionalTime;
	return *this;
}

} //Close namespace M68000
