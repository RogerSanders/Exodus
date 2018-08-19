//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
Watchpoint::Watchpoint(unsigned int addressBusWidth, unsigned int dataBusWidth, unsigned int addressBusCharWidth)
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

	_read = true;
	_write = true;

	_readConditionEnabled = false;
	_readConditionNot = false;
	_readCondition = Condition::Equal;
	_readConditionData1 = 0;
	_readConditionData2 = 0;

	_writeConditionEnabled = false;
	_writeConditionNot = false;
	_writeCondition = Condition::Equal;
	_writeConditionData1 = 0;
	_writeConditionData2 = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Execute functions
//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::Commit()
{
	_hitCounter += _hitCounterIncrement;
	_hitCounterIncrement = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::Rollback()
{
	_hitCounterIncrement = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Hit counter functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int Watchpoint::GetLiveHitCounter() const
{
	return _hitCounter + _hitCounterIncrement;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::IncrementHitCounter()
{
	if (!_preIncrementCounter)
	{
		++_hitCounterIncrement;
	}
	_preIncrementCounter = false;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::PreIncrementHitCounter()
{
	_preIncrementCounter = true;
	++_hitCounterIncrement;
}
