namespace M68000 {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum M68000::Exceptions
{
	EX_RESET                    = 0,
	EX_BUS_ERROR                = 2,
	EX_ADDRESS_ERROR            = 3,
	EX_ILLEGAL_INSTRUCTION      = 4,
	EX_ZERO_DIVIDE              = 5,
	EX_CHK_INSTRUCTION          = 6,
	EX_TRAPV_INSTRUCTION        = 7,
	EX_PRIVILEGE_VIOLATION      = 8,
	EX_TRACE                    = 9,
	EX_LINE_1010                = 10,
	EX_LINE_1111                = 11,
	EX_INTERRUPT_UNINITIALIZED  = 15,
	EX_INTERRUPT_SPURIOUS       = 24,
	EX_INTERRUPT_AUTOVECTOR     = 25,
	EX_INTERRUPT_TRAP           = 32
};

//----------------------------------------------------------------------------------------
//For more info on what references happen in what address space, refer to the M68000
//Programmers Manual, section 2, page 1.
//----------------------------------------------------------------------------------------
enum M68000::FunctionCode
{
	FUNCTIONCODE_UNDEFINED0         = 0, //000
	FUNCTIONCODE_USERDATA           = 1, //001
	FUNCTIONCODE_USERPROGRAM        = 2, //010
	FUNCTIONCODE_UNDEFINED3         = 3, //011
	FUNCTIONCODE_UNDEFINED4         = 4, //100
	FUNCTIONCODE_SUPERVISORDATA     = 5, //101
	FUNCTIONCODE_SUPERVISORPROGRAM  = 6, //110
	FUNCTIONCODE_CPUSPACE           = 7  //111
};

//----------------------------------------------------------------------------------------
//For more info on internal processor states, refer to the M68000 Users Manual,
//section 6, page 1.
//----------------------------------------------------------------------------------------
enum M68000::State
{
	STATE_NORMAL    = 0,
	STATE_STOPPED   = 1,
	STATE_EXCEPTION = 2,
	STATE_HALTED    = 3
};

//----------------------------------------------------------------------------------------
enum M68000::CELineID
{
	CELINE_LDS = 1,
	CELINE_UDS,
	CELINE_RW,
	CELINE_AS,
	CELINE_FC,
	CELINE_FCCPUSPACE,
	CELINE_RMWCYCLEINPROGRESS,
	CELINE_RMWCYCLEFIRSTOPERATION
};

//----------------------------------------------------------------------------------------
enum M68000::LineID
{
	LINE_RESET = 1,
	LINE_BR,
	LINE_BG,
	LINE_HALT,
	LINE_IPL,
	LINE_VPA
	//##TODO## Add DTAK and BERR lines, with proper support for both. We need to phase out
	//the "busError" flag from the IBusInterface::AccessResult structure, and rely on line
	//based communication for bus errors to be flagged. We also need to implement actual
	//support for the DTAK line, where an external device is responsible for asserting
	//DTAK while the M68000 is accessing the bus. If DTAK is not asserted, the M68000
	//needs to enter a wait state until it is. Note that this might actually be
	//unachievable until a microcode-level M68000 core is developed.
};

//----------------------------------------------------------------------------------------
enum M68000::ClockID
{
	CLOCK_CLK = 1
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct M68000::LineAccess
{
	LineAccess(unsigned int alineLD, const Data& astate, double aaccessTime)
	:lineID(alineLD), state(astate), accessTime(aaccessTime), notifyWhenApplied(false), appliedFlag(0), waitingDevice(0), clockRateChange(false)
	{}
	LineAccess(unsigned int alineLD, double aclockRate, double aaccessTime)
	:lineID(alineLD), state(0, 0), clockRate(aclockRate), accessTime(aaccessTime), notifyWhenApplied(false), appliedFlag(0), waitingDevice(0), clockRateChange(true)
	{}

	unsigned int lineID;
	bool clockRateChange;
	double clockRate;
	Data state;
	double accessTime;
	volatile bool notifyWhenApplied;
	//Note the use of volatile here. Both these pointers are volatile themselves, IE, the
	//actual address stored in the pointer variables may change at any time from a
	//separate thread. The appliedFlag variable is the only one of the two where the value
	//itself is also volatile, IE, the value stored in this boolean flag may change at any
	//time from a separate thread.
	volatile bool* volatile appliedFlag;
	IDeviceContext* volatile waitingDevice;
};

//----------------------------------------------------------------------------------------
struct M68000::CalculateCELineStateContext
{
	CalculateCELineStateContext(FunctionCode afunctionCode, bool aupperDataStrobe, bool alowerDataStrobe, bool areadHighWriteLow, bool armwCycleInProgress, bool armwCycleFirstOperation)
	:functionCode(afunctionCode), upperDataStrobe(aupperDataStrobe), lowerDataStrobe(alowerDataStrobe), readHighWriteLow(areadHighWriteLow), rmwCycleInProgress(armwCycleInProgress), rmwCycleFirstOperation(armwCycleFirstOperation)
	{}

	FunctionCode functionCode;
	bool upperDataStrobe;
	bool lowerDataStrobe;
	bool readHighWriteLow;
	bool rmwCycleInProgress;
	bool rmwCycleFirstOperation;
};

//----------------------------------------------------------------------------------------
//CCR flags
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|-----------|-------------------------------|
//	| T | 0 | S | 0 | 0 | Int Level | 0 | 0 | 0 | X | N | Z | V | C |
//	-----------------------------------------------------------------
//----------------------------------------------------------------------------------------
bool M68000::GetX() const
{
	return sr.GetBit(4);
}

//----------------------------------------------------------------------------------------
void M68000::SetX(bool flag)
{
	sr.SetBit(4, flag);
}

//----------------------------------------------------------------------------------------
bool M68000::GetN() const
{
	return sr.GetBit(3);
}

//----------------------------------------------------------------------------------------
void M68000::SetN(bool flag)
{
	sr.SetBit(3, flag);
}

//----------------------------------------------------------------------------------------
bool M68000::GetZ() const
{
	return sr.GetBit(2);
}

//----------------------------------------------------------------------------------------
void M68000::SetZ(bool flag)
{
	sr.SetBit(2, flag);
}

//----------------------------------------------------------------------------------------
bool M68000::GetV() const
{
	return sr.GetBit(1);
}

//----------------------------------------------------------------------------------------
void M68000::SetV(bool flag)
{
	sr.SetBit(1, flag);
}

//----------------------------------------------------------------------------------------
bool M68000::GetC() const
{
	return sr.GetBit(0);
}

//----------------------------------------------------------------------------------------
void M68000::SetC(bool flag)
{
	sr.SetBit(0, flag);
}

//----------------------------------------------------------------------------------------
//SR flags
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|-----------|-------------------------------|
//	| T | 0 | S | 0 | 0 |    IPM    | 0 | 0 | 0 | X | N | Z | V | C |
//	-----------------------------------------------------------------
//----------------------------------------------------------------------------------------
bool M68000::GetSR_T() const
{
	return sr.GetBit(15);
}

//----------------------------------------------------------------------------------------
void M68000::SetSR_T(bool flag)
{
	sr.SetBit(15, flag);
}

//----------------------------------------------------------------------------------------
bool M68000::GetSR_S() const
{
	return sr.GetBit(13);
}

//----------------------------------------------------------------------------------------
void M68000::SetSR_S(bool flag)
{
	sr.SetBit(13, flag);
}

//----------------------------------------------------------------------------------------
unsigned int M68000::GetSR_IPM() const
{
	return sr.GetDataSegment(8, 3);
}

//----------------------------------------------------------------------------------------
void M68000::SetSR_IPM(unsigned int aIPM)
{
	sr.SetDataSegment(8, 3, aIPM);
}

//----------------------------------------------------------------------------------------
//Register functions
//----------------------------------------------------------------------------------------
M68000Long M68000::GetPC() const
{
	return pc;
}

//----------------------------------------------------------------------------------------
void M68000::GetPC(Data& data) const
{
	data = pc;
}

//----------------------------------------------------------------------------------------
void M68000::SetPC(const M68000Long& data)
{
	pc = data;
}

//----------------------------------------------------------------------------------------
M68000Word M68000::GetSR() const
{
	return sr;
}

//----------------------------------------------------------------------------------------
void M68000::GetSR(Data& data) const
{
	data = sr;
}

//----------------------------------------------------------------------------------------
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|-----------|-------------------------------|
//	| T | 0 | S | 0 | 0 |    IPM    | 0 | 0 | 0 | X | N | Z | V | C |
//	-----------------------------------------------------------------
//----------------------------------------------------------------------------------------
void M68000::SetSR(const M68000Word& data)
{
	const unsigned int srMask = 0xA71F;
	sr = (data & srMask);
}

//----------------------------------------------------------------------------------------
M68000Byte M68000::GetCCR() const
{
	return M68000Byte(sr);
}

//----------------------------------------------------------------------------------------
void M68000::GetCCR(Data& data) const
{
	data = M68000Byte(sr);
}

//----------------------------------------------------------------------------------------
void M68000::SetCCR(const M68000Byte& data)
{
	sr.SetLowerBits(5, data.GetData());
}

//----------------------------------------------------------------------------------------
M68000Long M68000::GetSP() const
{
	if(GetSR_S())
	{
		return GetSSP();
	}
	else
	{
		return GetUSP();
	}
}

//----------------------------------------------------------------------------------------
void M68000::GetSP(Data& data) const
{
	if(GetSR_S())
	{
		GetSSP(data);
	}
	else
	{
		GetUSP(data);
	}
}

//----------------------------------------------------------------------------------------
void M68000::SetSP(const M68000Long& data)
{
	if(GetSR_S())
	{
		SetSSP(data);
	}
	else
	{
		SetUSP(data);
	}
}

//----------------------------------------------------------------------------------------
M68000Long M68000::GetSSP() const
{
	return ssp;
}

//----------------------------------------------------------------------------------------
void M68000::GetSSP(Data& data) const
{
	data = ssp;
}

//----------------------------------------------------------------------------------------
void M68000::SetSSP(const M68000Long& data)
{
	ssp = data;
}

//----------------------------------------------------------------------------------------
M68000Long M68000::GetUSP() const
{
	return usp;
}

//----------------------------------------------------------------------------------------
void M68000::GetUSP(Data& data) const
{
	data = usp;
}

//----------------------------------------------------------------------------------------
void M68000::SetUSP(const M68000Long& data)
{
	usp = data;
}

//----------------------------------------------------------------------------------------
M68000Long M68000::GetA(unsigned int index) const
{
	if(index == SP)
	{
		return GetSP();
	}
	else
	{
		return a[index];
	}
}

//----------------------------------------------------------------------------------------
void M68000::GetA(unsigned int index, Data& data) const
{
	if(index == SP)
	{
		GetSP(data);
	}
	else
	{
		data = a[index];
	}
}

//----------------------------------------------------------------------------------------
void M68000::SetA(unsigned int index, const M68000Long& data)
{
	if(index == SP)
	{
		SetSP(data);
	}
	else
	{
		aDisassemblyInfo[index].addressRegisterLastUsedInPostIncMode = false;
		a[index] = data;
	}
}

//----------------------------------------------------------------------------------------
M68000Long M68000::GetD(unsigned int index) const
{
	return d[index];
}

//----------------------------------------------------------------------------------------
void M68000::GetD(unsigned int index, Data& data) const
{
	data = d[index];
}

//----------------------------------------------------------------------------------------
void M68000::SetD(unsigned int index, const M68000Long& data)
{
	d[index] = data;
}

//----------------------------------------------------------------------------------------
void M68000::SetProcessorState(State executeState)
{
	processorState = executeState;
}

} //Close namespace M68000
