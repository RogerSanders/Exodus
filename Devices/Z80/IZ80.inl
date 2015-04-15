#include "Processor/Processor.pkg"

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IZ80::IZ80DataSource
{
	RegisterA = (unsigned int)IProcessor::IProcessorDataSource::End + 1,
	RegisterF,
	RegisterB,
	RegisterC,
	RegisterD,
	RegisterE,
	RegisterH,
	RegisterL,
	RegisterAF,
	RegisterBC,
	RegisterDE,
	RegisterHL,
	RegisterA2,
	RegisterF2,
	RegisterB2,
	RegisterC2,
	RegisterD2,
	RegisterE2,
	RegisterH2,
	RegisterL2,
	RegisterAF2,
	RegisterBC2,
	RegisterDE2,
	RegisterHL2,
	RegisterIXHigh,
	RegisterIXLow,
	RegisterIYHigh,
	RegisterIYLow,
	RegisterI,
	RegisterR,
	RegisterIX,
	RegisterIY,
	RegisterSP,
	RegisterPC,
	RegisterIM,
	RegisterIFF1,
	RegisterIFF2,
	RegisterFlagS,
	RegisterFlagZ,
	RegisterFlagY,
	RegisterFlagH,
	RegisterFlagX,
	RegisterFlagPV,
	RegisterFlagN,
	RegisterFlagC
};

//----------------------------------------------------------------------------------------
//Register functions
//----------------------------------------------------------------------------------------
unsigned int IZ80::GetA() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetA(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterA, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetF() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterF, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetF(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterF, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetB() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterB, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetB(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterB, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetC(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterC, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetD() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterD, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetD(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterD, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetE() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetE(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterE, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetH() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetH(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterH, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetL() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterL, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetL(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterL, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetAF() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterAF, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetAF(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterAF, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetBC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterBC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetBC(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterBC, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetDE() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterDE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetDE(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterDE, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetHL() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterHL, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetHL(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterHL, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetA2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterA2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetA2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterA2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetF2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterF2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetF2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterF2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetB2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterB2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetB2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterB2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetC2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterC2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetC2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterC2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetD2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterD2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetD2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterD2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetE2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterE2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetE2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterE2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetH2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterH2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetH2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterH2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetL2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterL2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetL2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterL2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetAF2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterAF2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetAF2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterAF2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetBC2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterBC2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetBC2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterBC2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetDE2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterDE2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetDE2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterDE2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetHL2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterHL2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetHL2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterHL2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIXHigh() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterIXHigh, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIXHigh(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterIXHigh, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIXLow() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterIXLow, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIXLow(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterIXLow, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIYHigh() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterIYHigh, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIYHigh(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterIYHigh, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIYLow() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterIYLow, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIYLow(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterIYLow, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetI() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterI, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetI(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterI, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetR() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetR(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterR, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIX() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterIX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIX(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterIX, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIY() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterIY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIY(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterIY, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterSP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetSP(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterSP, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetPC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterPC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetPC(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterPC, 0, data);
}

//----------------------------------------------------------------------------------------
//Interrupt register functions
//----------------------------------------------------------------------------------------
unsigned int IZ80::GetInterruptMode() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterIM, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetInterruptMode(unsigned int ainterruptMode)
{
	GenericAccessDataValueUInt data(ainterruptMode);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterIM, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetIFF1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterIFF1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIFF1(bool state)
{
	GenericAccessDataValueBool data(state);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterIFF1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetIFF2() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterIFF2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIFF2(bool state)
{
	GenericAccessDataValueBool data(state);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterIFF2, 0, data);
}

//----------------------------------------------------------------------------------------
//Flag functions
//----------------------------------------------------------------------------------------
bool IZ80::GetFlagS() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterFlagS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagS(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterFlagS, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagZ() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterFlagZ, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagZ(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterFlagZ, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagY() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterFlagY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagY(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterFlagY, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagH() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterFlagH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagH(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterFlagH, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagX() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterFlagX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagX(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterFlagX, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagPV() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterFlagPV, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagPV(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterFlagPV, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagN() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterFlagN, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagN(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterFlagN, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagC() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IZ80DataSource::RegisterFlagC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagC(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)IZ80DataSource::RegisterFlagC, 0, data);
}
