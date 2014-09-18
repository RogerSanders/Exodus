#include "Processor/Processor.pkg"

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IZ80::IZ80DataSource
{
	DATASOURCE_REGISTER_A = IProcessor::IPROCESSORDATASOURCE_END + 1,
	DATASOURCE_REGISTER_F,
	DATASOURCE_REGISTER_B,
	DATASOURCE_REGISTER_C,
	DATASOURCE_REGISTER_D,
	DATASOURCE_REGISTER_E,
	DATASOURCE_REGISTER_H,
	DATASOURCE_REGISTER_L,
	DATASOURCE_REGISTER_AF,
	DATASOURCE_REGISTER_BC,
	DATASOURCE_REGISTER_DE,
	DATASOURCE_REGISTER_HL,
	DATASOURCE_REGISTER_A2,
	DATASOURCE_REGISTER_F2,
	DATASOURCE_REGISTER_B2,
	DATASOURCE_REGISTER_C2,
	DATASOURCE_REGISTER_D2,
	DATASOURCE_REGISTER_E2,
	DATASOURCE_REGISTER_H2,
	DATASOURCE_REGISTER_L2,
	DATASOURCE_REGISTER_AF2,
	DATASOURCE_REGISTER_BC2,
	DATASOURCE_REGISTER_DE2,
	DATASOURCE_REGISTER_HL2,
	DATASOURCE_REGISTER_IXHIGH,
	DATASOURCE_REGISTER_IXLOW,
	DATASOURCE_REGISTER_IYHIGH,
	DATASOURCE_REGISTER_IYLOW,
	DATASOURCE_REGISTER_I,
	DATASOURCE_REGISTER_R,
	DATASOURCE_REGISTER_IX,
	DATASOURCE_REGISTER_IY,
	DATASOURCE_REGISTER_SP,
	DATASOURCE_REGISTER_PC,
	DATASOURCE_REGISTER_IM,
	DATASOURCE_REGISTER_IFF1,
	DATASOURCE_REGISTER_IFF2,
	DATASOURCE_REGISTER_FLAG_S,
	DATASOURCE_REGISTER_FLAG_Z,
	DATASOURCE_REGISTER_FLAG_Y,
	DATASOURCE_REGISTER_FLAG_H,
	DATASOURCE_REGISTER_FLAG_X,
	DATASOURCE_REGISTER_FLAG_PV,
	DATASOURCE_REGISTER_FLAG_N,
	DATASOURCE_REGISTER_FLAG_C,
	DATASOURCE_REGISTER_ORIGINALVALUE_CHANGECOUNTER,
	DATASOURCE_REGISTER_ORIGINALVALUE_A,
	DATASOURCE_REGISTER_ORIGINALVALUE_F,
	DATASOURCE_REGISTER_ORIGINALVALUE_B,
	DATASOURCE_REGISTER_ORIGINALVALUE_C,
	DATASOURCE_REGISTER_ORIGINALVALUE_D,
	DATASOURCE_REGISTER_ORIGINALVALUE_E,
	DATASOURCE_REGISTER_ORIGINALVALUE_H,
	DATASOURCE_REGISTER_ORIGINALVALUE_L,
	DATASOURCE_REGISTER_ORIGINALVALUE_AF,
	DATASOURCE_REGISTER_ORIGINALVALUE_BC,
	DATASOURCE_REGISTER_ORIGINALVALUE_DE,
	DATASOURCE_REGISTER_ORIGINALVALUE_HL,
	DATASOURCE_REGISTER_ORIGINALVALUE_A2,
	DATASOURCE_REGISTER_ORIGINALVALUE_F2,
	DATASOURCE_REGISTER_ORIGINALVALUE_B2,
	DATASOURCE_REGISTER_ORIGINALVALUE_C2,
	DATASOURCE_REGISTER_ORIGINALVALUE_D2,
	DATASOURCE_REGISTER_ORIGINALVALUE_E2,
	DATASOURCE_REGISTER_ORIGINALVALUE_H2,
	DATASOURCE_REGISTER_ORIGINALVALUE_L2,
	DATASOURCE_REGISTER_ORIGINALVALUE_AF2,
	DATASOURCE_REGISTER_ORIGINALVALUE_BC2,
	DATASOURCE_REGISTER_ORIGINALVALUE_DE2,
	DATASOURCE_REGISTER_ORIGINALVALUE_HL2,
	DATASOURCE_REGISTER_ORIGINALVALUE_IXHIGH,
	DATASOURCE_REGISTER_ORIGINALVALUE_IXLOW,
	DATASOURCE_REGISTER_ORIGINALVALUE_IYHIGH,
	DATASOURCE_REGISTER_ORIGINALVALUE_IYLOW,
	DATASOURCE_REGISTER_ORIGINALVALUE_I,
	DATASOURCE_REGISTER_ORIGINALVALUE_R,
	DATASOURCE_REGISTER_ORIGINALVALUE_IX,
	DATASOURCE_REGISTER_ORIGINALVALUE_IY,
	DATASOURCE_REGISTER_ORIGINALVALUE_SP,
	DATASOURCE_REGISTER_ORIGINALVALUE_PC,
	DATASOURCE_REGISTER_ORIGINALVALUE_IM,
	DATASOURCE_REGISTER_ORIGINALVALUE_IFF1,
	DATASOURCE_REGISTER_ORIGINALVALUE_IFF2,
	DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_S,
	DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_Z,
	DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_Y,
	DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_H,
	DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_X,
	DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_PV,
	DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_N,
	DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_C
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IZ80::ThisIZ80Version()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Register functions
//----------------------------------------------------------------------------------------
unsigned int IZ80::GetA() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_A, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetA(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_A, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetF() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_F, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetF(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_F, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetB() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_B, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetB(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_B, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_C, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetC(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_C, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetD() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_D, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetD(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_D, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetE() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_E, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetE(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_E, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetH() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_H, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetH(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_H, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetL() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_L, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetL(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_L, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetAF() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_AF, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetAF(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_AF, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetBC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_BC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetBC(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_BC, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetDE() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_DE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetDE(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_DE, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetHL() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_HL, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetHL(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_HL, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetA2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_A2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetA2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_A2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetF2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_F2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetF2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_F2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetB2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_B2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetB2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_B2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetC2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_C2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetC2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_C2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetD2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_D2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetD2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_D2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetE2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_E2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetE2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_E2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetH2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_H2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetH2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_H2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetL2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_L2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetL2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_L2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetAF2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_AF2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetAF2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_AF2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetBC2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_BC2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetBC2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_BC2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetDE2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_DE2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetDE2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_DE2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetHL2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_HL2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetHL2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_HL2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIXHigh() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_IXHIGH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIXHigh(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_IXHIGH, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIXLow() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_IXLOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIXLow(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_IXLOW, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIYHigh() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_IYHIGH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIYHigh(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_IYHIGH, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIYLow() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_IYLOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIYLow(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_IYLOW, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetI() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_I, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetI(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_I, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetR() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_R, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetR(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_R, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIX() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_IX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIX(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_IX, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetIY() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_IY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIY(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_IY, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_SP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetSP(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_SP, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetPC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_PC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetPC(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_PC, 0, data);
}

//----------------------------------------------------------------------------------------
//Interrupt register functions
//----------------------------------------------------------------------------------------
unsigned int IZ80::GetInterruptMode() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_IM, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetInterruptMode(unsigned int ainterruptMode)
{
	GenericAccessDataValueUInt data(ainterruptMode);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_IM, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetIFF1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_IFF1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIFF1(bool state)
{
	GenericAccessDataValueBool data(state);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_IFF1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetIFF2() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_IFF2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetIFF2(bool state)
{
	GenericAccessDataValueBool data(state);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_IFF2, 0, data);
}

//----------------------------------------------------------------------------------------
//Flag functions
//----------------------------------------------------------------------------------------
bool IZ80::GetFlagS() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_S, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagS(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_S, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagZ() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_Z, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagZ(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_Z, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagY() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_Y, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagY(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_Y, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagH() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_H, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagH(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_H, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagX() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_X, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagX(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_X, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagPV() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_PV, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagPV(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_PV, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagN() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_N, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagN(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_N, 0, data);
}

//----------------------------------------------------------------------------------------
bool IZ80::GetFlagC() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_C, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IZ80::SetFlagC(bool flag)
{
	GenericAccessDataValueBool data(flag);
	WriteGenericData((unsigned int)DATASOURCE_REGISTER_FLAG_C, 0, data);
}

//----------------------------------------------------------------------------------------
//Latched register original value functions
//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueChangeCounter() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_CHANGECOUNTER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueA() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_A, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueF() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_F, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueB() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_B, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_C, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueD() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_D, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueE() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_E, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueH() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_H, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueL() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_L, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueAF() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_AF, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueBC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_BC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueDE() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_DE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueHL() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_HL, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueA2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_A2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueF2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_F2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueB2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_B2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueC2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_C2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueD2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_D2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueE2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_E2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueH2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_H2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueL2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_L2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueAF2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_AF2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueBC2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_BC2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueDE2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_DE2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueHL2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_HL2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueIXHigh() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_IXHIGH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueIXLow() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_IXLOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueIYHigh() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_IYHIGH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueIYLow() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_IYLOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueI() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_I, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueR() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_R, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueIX() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_IX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueIY() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_IY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueSP() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_SP, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValuePC() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_PC, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IZ80::GetOriginalValueInterruptMode() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_IM, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IZ80::GetOriginalValueIFF1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_IFF1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IZ80::GetOriginalValueIFF2() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_IFF2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IZ80::GetOriginalValueFlagS() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_S, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IZ80::GetOriginalValueFlagZ() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_Z, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IZ80::GetOriginalValueFlagY() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_Y, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IZ80::GetOriginalValueFlagH() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_H, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IZ80::GetOriginalValueFlagX() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_X, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IZ80::GetOriginalValueFlagPV() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_PV, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IZ80::GetOriginalValueFlagN() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_N, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
bool IZ80::GetOriginalValueFlagC() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REGISTER_ORIGINALVALUE_FLAG_C, 0, data);
	return data.GetValue();
}
