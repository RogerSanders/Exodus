namespace Z80 {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExecuteTime::ExecuteTime()
:cycles(0), additionalTime(0)
{}

//----------------------------------------------------------------------------------------
ExecuteTime::ExecuteTime(unsigned int acycles, double aadditionalTime)
:cycles(acycles), additionalTime(aadditionalTime)
{}

//----------------------------------------------------------------------------------------
//Set functions
//----------------------------------------------------------------------------------------
void ExecuteTime::Set(unsigned int acycles, double aadditionalTime)
{
	cycles = acycles;
	additionalTime = aadditionalTime;
}

//----------------------------------------------------------------------------------------
//Addition operators
//----------------------------------------------------------------------------------------
ExecuteTime ExecuteTime::operator+(const ExecuteTime& target) const
{
	return ExecuteTime(cycles + target.cycles, additionalTime + target.additionalTime);
}

//----------------------------------------------------------------------------------------
ExecuteTime& ExecuteTime::operator+=(const ExecuteTime& target)
{
	cycles += target.cycles;
	additionalTime += target.additionalTime;
	return *this;
}

} //Close namespace Z80
