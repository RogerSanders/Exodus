//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Watchpoint::Watchpoint(unsigned int addressBusWidth)
:Breakpoint(addressBusWidth)
{
	Initialize(addressBusWidth);
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
void Watchpoint::Initialize(unsigned int addressBusWidth)
{
	Breakpoint::Initialize(addressBusWidth);
	read = true;
	write = true;

	readConditionEnabled = false;
	readConditionNot = false;
	readCondition = CONDITION_EQUAL;
	readConditionData1 = 0;
	readConditionData2 = 0;

	writeConditionEnabled = false;
	writeConditionNot = false;
	writeCondition = CONDITION_EQUAL;
	writeConditionData1 = 0;
	writeConditionData2 = 0;
}

//----------------------------------------------------------------------------------------
//Read/write condition flag functions
//----------------------------------------------------------------------------------------
bool Watchpoint::GetOnRead() const
{
	return read;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetOnRead(bool state)
{
	read = state;
}

//----------------------------------------------------------------------------------------
bool Watchpoint::GetOnWrite() const
{
	return write;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetOnWrite(bool state)
{
	write = state;
}

//----------------------------------------------------------------------------------------
//Read condition functions
//----------------------------------------------------------------------------------------
bool Watchpoint::GetReadConditionEnabled() const
{
	return readConditionEnabled;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionEnabled(bool state)
{
	readConditionEnabled = state;
}

//----------------------------------------------------------------------------------------
bool Watchpoint::GetReadConditionNot() const
{
	return readConditionNot;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionNot(bool state)
{
	readConditionNot = state;
}

//----------------------------------------------------------------------------------------
Watchpoint::Condition Watchpoint::GetReadCondition() const
{
	return readCondition;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetReadCondition(Condition condition)
{
	readCondition = condition;
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetReadConditionData1() const
{
	return readConditionData1;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionData1(unsigned int data, unsigned int dataBusWidth)
{
	readConditionData1 = data & ((1 << dataBusWidth) - 1);
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetReadConditionData2() const
{
	return readConditionData2;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionData2(unsigned int data, unsigned int dataBusWidth)
{
	readConditionData2 = data & ((1 << dataBusWidth) - 1);
}


//----------------------------------------------------------------------------------------
//Write condition functions
//----------------------------------------------------------------------------------------
bool Watchpoint::GetWriteConditionEnabled() const
{
	return writeConditionEnabled;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionEnabled(bool state)
{
	writeConditionEnabled = state;
}

//----------------------------------------------------------------------------------------
bool Watchpoint::GetWriteConditionNot() const
{
	return writeConditionNot;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionNot(bool state)
{
	writeConditionNot = state;
}

//----------------------------------------------------------------------------------------
Watchpoint::Condition Watchpoint::GetWriteCondition() const
{
	return writeCondition;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetWriteCondition(Condition condition)
{
	writeCondition = condition;
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetWriteConditionData1() const
{
	return writeConditionData1;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionData1(unsigned int data, unsigned int dataBusWidth)
{
	writeConditionData1 = data & ((1 << dataBusWidth) - 1);
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetWriteConditionData2() const
{
	return writeConditionData2;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionData2(unsigned int data, unsigned int dataBusWidth)
{
	writeConditionData2 = data & ((1 << dataBusWidth) - 1);
}
