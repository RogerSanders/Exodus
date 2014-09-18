//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IM68000::IM68000DataSource
{
	DATASOURCE_REGISTER_SRX = IProcessor::IPROCESSORDATASOURCE_END + 1,
	DATASOURCE_REGISTER_SRN,
	DATASOURCE_REGISTER_SRZ,
	DATASOURCE_REGISTER_SRV,
	DATASOURCE_REGISTER_SRC,
	DATASOURCE_REGISTER_SRT,
	DATASOURCE_REGISTER_SRS,
	DATASOURCE_REGISTER_SRIPM,
	DATASOURCE_REGISTER_PC,
	DATASOURCE_REGISTER_SR,
	DATASOURCE_REGISTER_CCR,
	DATASOURCE_REGISTER_SP,
	DATASOURCE_REGISTER_SSP,
	DATASOURCE_REGISTER_USP,
	DATASOURCE_REGISTER_A,
	DATASOURCE_REGISTER_D,
	DATASOURCE_REGISTER_ORIGINALVALUE_CHANGECOUNTER,
	DATASOURCE_REGISTER_ORIGINALVALUE_SRX,
	DATASOURCE_REGISTER_ORIGINALVALUE_SRN,
	DATASOURCE_REGISTER_ORIGINALVALUE_SRZ,
	DATASOURCE_REGISTER_ORIGINALVALUE_SRV,
	DATASOURCE_REGISTER_ORIGINALVALUE_SRC,
	DATASOURCE_REGISTER_ORIGINALVALUE_SRT,
	DATASOURCE_REGISTER_ORIGINALVALUE_SRS,
	DATASOURCE_REGISTER_ORIGINALVALUE_SRIPM,
	DATASOURCE_REGISTER_ORIGINALVALUE_PC,
	DATASOURCE_REGISTER_ORIGINALVALUE_SR,
	DATASOURCE_REGISTER_ORIGINALVALUE_CCR,
	DATASOURCE_REGISTER_ORIGINALVALUE_SP,
	DATASOURCE_REGISTER_ORIGINALVALUE_SSP,
	DATASOURCE_REGISTER_ORIGINALVALUE_USP,
	DATASOURCE_REGISTER_ORIGINALVALUE_A,
	DATASOURCE_REGISTER_ORIGINALVALUE_D
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
	:vectorNumber(0), enableLogging(false), enableBreak(false), disable(false)
	{}

	unsigned int vectorNumber;
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
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SRX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetX(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SRX, 0, data);
}

//----------------------------------------------------------------------------------------
bool IM68000::GetN() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SRN, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetN(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SRN, 0, data);
}

//----------------------------------------------------------------------------------------
bool IM68000::GetZ() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SRZ, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetZ(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SRZ, 0, data);
}

//----------------------------------------------------------------------------------------
bool IM68000::GetV() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SRV, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetV(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SRV, 0, data);
}

//----------------------------------------------------------------------------------------
bool IM68000::GetC() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SRC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetC(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SRC, 0, data);
}

//----------------------------------------------------------------------------------------
//SR flags
//----------------------------------------------------------------------------------------
bool IM68000::GetSR_T() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SRT, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSR_T(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SRT, 0, data);
}

//----------------------------------------------------------------------------------------
bool IM68000::GetSR_S() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SRS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSR_S(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SRS, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetSR_IPM() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SRIPM, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSR_IPM(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SRIPM, 0, data);
}

//----------------------------------------------------------------------------------------
//Register functions
//----------------------------------------------------------------------------------------
unsigned int IM68000::GetPC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_PC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetPC(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_PC, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetSR() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSR(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SR, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetCCR() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_CCR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetCCR(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_CCR, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSP(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SP, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetSSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SSP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetSSP(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SSP, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetUSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_USP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetUSP(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_USP, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetA(unsigned int registerNo) const
{
	GenericAccessDataValueUInt data;
	RegisterDataContext dataContext(registerNo);
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_A, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetA(unsigned int registerNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	RegisterDataContext dataContext(registerNo);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_A, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetD(unsigned int registerNo) const
{
	GenericAccessDataValueUInt data;
	RegisterDataContext dataContext(registerNo);
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_D, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IM68000::SetD(unsigned int registerNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	RegisterDataContext dataContext(registerNo);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_D, &dataContext, data);
}

//----------------------------------------------------------------------------------------
//Latched register original value functions
//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueChangeCounter() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_CHANGECOUNTER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueX() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SRX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueN() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SRN, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueZ() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SRZ, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueV() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SRV, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueC() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SRC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueSR_T() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SRT, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IM68000::GetOriginalValueSR_S() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SRS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueSR_IPM() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SRIPM, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValuePC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_PC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueSR() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueCCR() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_CCR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueSSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SSP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueUSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_USP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueA(unsigned int registerNo) const
{
	GenericAccessDataValueUInt data;
	RegisterDataContext dataContext(registerNo);
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_A, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IM68000::GetOriginalValueD(unsigned int registerNo) const
{
	GenericAccessDataValueUInt data;
	RegisterDataContext dataContext(registerNo);
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_D, &dataContext, data);
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
std::wstring IM68000::GetExceptionName(unsigned int vectorNumber) const
{
	std::wstring result;
	GetExceptionNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result), vectorNumber);
	return result;
}
