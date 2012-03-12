//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum Breakpoint::Condition
{
	CONDITION_EQUAL,
	CONDITION_GREATER,
	CONDITION_LESS,
	CONDITION_GREATER_AND_LESS
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Breakpoint::Breakpoint()
{
	Initialize();
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
void Breakpoint::Initialize()
{
	logEvent = true;
	breakEvent = true;
	name.clear();

	locationConditionNot = false;
	locationCondition = CONDITION_EQUAL;
	locationConditionData1 = 0;
	locationConditionData2 = 0;

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
//Breakpoint event triggers
//----------------------------------------------------------------------------------------
bool Breakpoint::GetLogEvent() const
{
	return logEvent;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLogEvent(bool state)
{
	logEvent = state;
}

//----------------------------------------------------------------------------------------
bool Breakpoint::GetBreakEvent() const
{
	return breakEvent;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetBreakEvent(bool state)
{
	breakEvent = state;
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring Breakpoint::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetName(const std::wstring& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
//Location condition functions
//----------------------------------------------------------------------------------------
bool Breakpoint::GetLocationConditionNot() const
{
	return locationConditionNot;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationConditionNot(bool state)
{
	locationConditionNot = state;
}

//----------------------------------------------------------------------------------------
Breakpoint::Condition Breakpoint::GetLocationCondition() const
{
	return locationCondition;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationCondition(Condition condition)
{
	locationCondition = condition;
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetLocationConditionData1() const
{
	return locationConditionData1;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationConditionData1(unsigned int data, unsigned int addressBusWidth)
{
	locationConditionData1 = data & ((1 << addressBusWidth) - 1);
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetLocationConditionData2() const
{
	return locationConditionData2;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationConditionData2(unsigned int data, unsigned int addressBusWidth)
{
	locationConditionData2 = data & ((1 << addressBusWidth) - 1);
}

//----------------------------------------------------------------------------------------
//Hit counter functions
//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetHitCounter() const
{
	return hitCounter;
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetLiveHitCounter() const
{
	return hitCounter + hitCounterIncrement;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetHitCounter(unsigned int ahitCounter)
{
	hitCounter = ahitCounter;
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

//----------------------------------------------------------------------------------------
bool Breakpoint::GetBreakOnCounter() const
{
	return breakOnCounter;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetBreakOnCounter(bool state)
{
	breakOnCounter = state;
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetBreakCounter() const
{
	return breakCounter;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetBreakCounter(unsigned int abreakCounter)
{
	breakCounter = abreakCounter;
}
