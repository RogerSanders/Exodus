//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Breakpoint::Breakpoint(unsigned int addressBusWidth, unsigned int dataBusWidth, unsigned int addressBusCharWidth)
{
	_addressBusWidth = addressBusWidth;
	_dataBusWidth = dataBusWidth;
	_addressBusCharWidth = addressBusCharWidth;

	_name.clear();
	_enabled = true;
	_logEvent = true;
	_breakEvent = true;

	_locationConditionNot = false;
	_locationCondition = Condition::Equal;
	_locationConditionData1 = 0;
	_locationConditionData2 = 0;
	_locationMask = ((1 << _addressBusWidth) - 1);

	_hitCounter = 0;
	_hitCounterIncrement = 0;
	_breakOnCounter = false;
	_breakCounter = 0;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void Breakpoint::Commit()
{
	_hitCounter += _hitCounterIncrement;
	_hitCounterIncrement = 0;
}

//----------------------------------------------------------------------------------------
void Breakpoint::Rollback()
{
	_hitCounterIncrement = 0;
}

//----------------------------------------------------------------------------------------
//Hit counter functions
//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetLiveHitCounter() const
{
	return _hitCounter + _hitCounterIncrement;
}

//----------------------------------------------------------------------------------------
void Breakpoint::IncrementHitCounter()
{
	if (!_preIncrementCounter)
	{
		++_hitCounterIncrement;
	}
	_preIncrementCounter = false;
}

//----------------------------------------------------------------------------------------
void Breakpoint::PreIncrementHitCounter()
{
	_preIncrementCounter = true;
	++_hitCounterIncrement;
}
