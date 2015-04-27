//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Breakpoint::Breakpoint(unsigned int aaddressBusWidth, unsigned int adataBusWidth, unsigned int aaddressBusCharWidth)
{
	addressBusWidth = aaddressBusWidth;
	dataBusWidth = adataBusWidth;
	addressBusCharWidth = aaddressBusCharWidth;

	name.clear();
	enabled = true;
	logEvent = true;
	breakEvent = true;

	locationConditionNot = false;
	locationCondition = Condition::Equal;
	locationConditionData1 = 0;
	locationConditionData2 = 0;
	locationMask = ((1 << addressBusWidth) - 1);

	hitCounter = 0;
	hitCounterIncrement = 0;
	breakOnCounter = false;
	breakCounter = 0;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void Breakpoint::Commit()
{
	hitCounter += hitCounterIncrement;
	hitCounterIncrement = 0;
}

//----------------------------------------------------------------------------------------
void Breakpoint::Rollback()
{
	hitCounterIncrement = 0;
}

//----------------------------------------------------------------------------------------
//Hit counter functions
//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetLiveHitCounter() const
{
	return hitCounter + hitCounterIncrement;
}

//----------------------------------------------------------------------------------------
void Breakpoint::IncrementHitCounter()
{
	if(!preIncrementCounter)
	{
		++hitCounterIncrement;
	}
	preIncrementCounter = false;
}

//----------------------------------------------------------------------------------------
void Breakpoint::PreIncrementHitCounter()
{
	preIncrementCounter = true;
	++hitCounterIncrement;
}
