//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Watchpoint::Watchpoint(unsigned int aaddressBusWidth, unsigned int adataBusWidth, unsigned int aaddressBusCharWidth)
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

	read = true;
	write = true;

	readConditionEnabled = false;
	readConditionNot = false;
	readCondition = Condition::Equal;
	readConditionData1 = 0;
	readConditionData2 = 0;

	writeConditionEnabled = false;
	writeConditionNot = false;
	writeCondition = Condition::Equal;
	writeConditionData1 = 0;
	writeConditionData2 = 0;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void Watchpoint::Commit()
{
	hitCounter += hitCounterIncrement;
	hitCounterIncrement = 0;
}

//----------------------------------------------------------------------------------------
void Watchpoint::Rollback()
{
	hitCounterIncrement = 0;
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring Watchpoint::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetName(const std::wstring& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
//Hit counter functions
//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetLiveHitCounter() const
{
	return hitCounter + hitCounterIncrement;
}

//----------------------------------------------------------------------------------------
void Watchpoint::IncrementHitCounter()
{
	if(!preIncrementCounter)
	{
		++hitCounterIncrement;
	}
	preIncrementCounter = false;
}

//----------------------------------------------------------------------------------------
void Watchpoint::PreIncrementHitCounter()
{
	preIncrementCounter = true;
	++hitCounterIncrement;
}
