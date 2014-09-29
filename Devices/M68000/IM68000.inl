//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IM68000::IM68000DataSource
{
	RegisterSRX = (unsigned int)IProcessor::IProcessorDataSource::End + 1,
	RegisterSRN,
	RegisterSRZ,
	RegisterSRV,
	RegisterSRC,
	RegisterSRT,
	RegisterSRS,
	RegisterSRIPM,
	RegisterPC,
	RegisterSR,
	RegisterCCR,
	RegisterSP,
	RegisterSSP,
	RegisterUSP,
	RegisterA,
	RegisterD,
	RegisterOriginalValueChangeCounter,
	RegisterOriginalValueSRX,
	RegisterOriginalValueSRN,
	RegisterOriginalValueSRZ,
	RegisterOriginalValueSRV,
	RegisterOriginalValueSRC,
	RegisterOriginalValueSRT,
	RegisterOriginalValueSRS,
	RegisterOriginalValueSRIPM,
	RegisterOriginalValuePC,
	RegisterOriginalValueSR,
	RegisterOriginalValueCCR,
	RegisterOriginalValueSP,
	RegisterOriginalValueSSP,
	RegisterOriginalValueUSP,
	RegisterOriginalValueA,
	RegisterOriginalValueD
};

//----------------------------------------------------------------------------------------
enum class IM68000::Exceptions
{
	Reset                   = 0x00,
	Invalid01               = 0x01,
	BusError                = 0x02,
	AddressError            = 0x03,
	IllegalInstruction      = 0x04,
	ZeroDivide              = 0x05,
	CHKInstruction          = 0x06,
	TrapVInstruction        = 0x07,
	PrivilegeViolation      = 0x08,
	Trace                   = 0x09,
	Line1010                = 0x0A,
	Line1111                = 0x0B,
	Reserved0C              = 0x0C,
	Reserved0D              = 0x0D,
	Reserved0E              = 0x0E,
	InterruptUninitialized  = 0x0F,
	Reserved10              = 0x10,
	Reserved11              = 0x11,
	Reserved12              = 0x12,
	Reserved13              = 0x13,
	Reserved14              = 0x14,
	Reserved15              = 0x15,
	Reserved16              = 0x16,
	Reserved17              = 0x17,
	InterruptSpurious       = 0x18,
	InterruptAutoVectorL1   = 0x19,
	InterruptAutoVectorL2   = 0x1A,
	InterruptAutoVectorL3   = 0x1B,
	InterruptAutoVectorL4   = 0x1C,
	InterruptAutoVectorL5   = 0x1D,
	InterruptAutoVectorL6   = 0x1E,
	InterruptAutoVectorL7   = 0x1F,
	InterruptTrap0          = 0x20,
	InterruptTrap1          = 0x21,
	InterruptTrap2          = 0x22,
	InterruptTrap3          = 0x23,
	InterruptTrap4          = 0x24,
	InterruptTrap5          = 0x25,
	InterruptTrap6          = 0x26,
	InterruptTrap7          = 0x27,
	InterruptTrap8          = 0x28,
	InterruptTrap9          = 0x29,
	InterruptTrapA          = 0x2A,
	InterruptTrapB          = 0x2B,
	InterruptTrapC          = 0x2C,
	InterruptTrapD          = 0x2D,
	InterruptTrapE          = 0x2E,
	InterruptTrapF          = 0x2F,
	Reserved30              = 0x30,
	Reserved31              = 0x31,
	Reserved32              = 0x32,
	Reserved33              = 0x33,
	Reserved34              = 0x34,
	Reserved35              = 0x35,
	Reserved36              = 0x36,
	Reserved37              = 0x37,
	Reserved38              = 0x38,
	Reserved39              = 0x39,
	Reserved3A              = 0x3A,
	Reserved3B              = 0x3B,
	Reserved3C              = 0x3C,
	Reserved3D              = 0x3D,
	Reserved3E              = 0x3E,
	Reserved3F              = 0x3F
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct IM68000::RegisterDataContext :public IGenericAccess::DataContext
{
	RegisterDataContext(unsigned int aregisterNo = 0)
	:registerNo(aregisterNo)
	{}

	unsigned int registerNo;
};

//----------------------------------------------------------------------------------------
struct IM68000::ExceptionDebuggingEntry
{
	ExceptionDebuggingEntry()
	:vectorNumber(Exceptions::Reset), enableLogging(false), enableBreak(false), disable(false)
	{}

	Exceptions vectorNumber;
	bool enableLogging;
	bool enableBreak;
	bool disable;
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IM68000::ThisIM68000Version()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//CCR flags
//----------------------------------------------------------------------------------------
bool IM68000::GetX() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterSRX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetX(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterSRX, 0, data);
}

//----------------------------------------------------------------------------------------
bool IM68000::GetN() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterSRN, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetN(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterSRN, 0, data);
}

//----------------------------------------------------------------------------------------
bool IM68000::GetZ() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterSRZ, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetZ(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterSRZ, 0, data);
}

//----------------------------------------------------------------------------------------
bool IM68000::GetV() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterSRV, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetV(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterSRV, 0, data);
}

//----------------------------------------------------------------------------------------
bool IM68000::GetC() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterSRC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetC(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterSRC, 0, data);
}

//----------------------------------------------------------------------------------------
//SR flags
//----------------------------------------------------------------------------------------
bool IM68000::GetSR_T() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterSRT, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSR_T(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterSRT, 0, data);
}

//----------------------------------------------------------------------------------------
bool IM68000::GetSR_S() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterSRS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSR_S(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterSRS, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetSR_IPM() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterSRIPM, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSR_IPM(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterSRIPM, 0, data);
}

//----------------------------------------------------------------------------------------
//Register functions
//----------------------------------------------------------------------------------------
unsigned int IM68000::GetPC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterPC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetPC(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterPC, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetSR() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterSR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSR(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterSR, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetCCR() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterCCR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetCCR(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterCCR, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterSP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSP(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterSP, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetSSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterSSP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSSP(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterSSP, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetUSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterUSP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetUSP(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterUSP, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetA(unsigned int registerNo) const
{
	GenericAccessDataValueUInt data;
	RegisterDataContext dataContext(registerNo);
	ReadGenericData((unsigned int)IM68000DataSource::RegisterA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetA(unsigned int registerNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	RegisterDataContext dataContext(registerNo);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetD(unsigned int registerNo) const
{
	GenericAccessDataValueUInt data;
	RegisterDataContext dataContext(registerNo);
	ReadGenericData((unsigned int)IM68000DataSource::RegisterD, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetD(unsigned int registerNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	RegisterDataContext dataContext(registerNo);
	WriteGenericData((unsigned int)IM68000DataSource::RegisterD, &dataContext, data);
}

//----------------------------------------------------------------------------------------
//Latched register original value functions
//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueChangeCounter() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueChangeCounter, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueX() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueSRX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueN() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueSRN, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueZ() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueSRZ, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueV() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueSRV, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueC() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueSRC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueSR_T() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueSRT, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueSR_S() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueSRS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueSR_IPM() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueSRIPM, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValuePC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValuePC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueSR() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueSR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueCCR() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueCCR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueSP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueSSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueSSP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueUSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueUSP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueA(unsigned int registerNo) const
{
	GenericAccessDataValueUInt data;
	RegisterDataContext dataContext(registerNo);
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueD(unsigned int registerNo) const
{
	GenericAccessDataValueUInt data;
	RegisterDataContext dataContext(registerNo);
	ReadGenericData((unsigned int)IM68000DataSource::RegisterOriginalValueD, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
//Exception debugging functions
//----------------------------------------------------------------------------------------
std::list<IM68000::ExceptionDebuggingEntry> IM68000::GetExceptionDebugEntries() const
{
	std::list<ExceptionDebuggingEntry> result;
	GetExceptionDebugEntriesInternal(InteropSupport::STLObjectTarget<std::list<ExceptionDebuggingEntry>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IM68000::SetExceptionDebugEntries(const std::list<ExceptionDebuggingEntry>& state)
{
	SetExceptionDebugEntriesInternal(InteropSupport::STLObjectSource<std::list<ExceptionDebuggingEntry>>(state));
}

//----------------------------------------------------------------------------------------
std::wstring IM68000::GetExceptionName(Exceptions vectorNumber) const
{
	std::wstring result;
	GetExceptionNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result), vectorNumber);
	return result;
}
