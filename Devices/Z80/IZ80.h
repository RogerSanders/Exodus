#ifndef __IZ80_H__
#define __IZ80_H__
#include "GenericAccess/GenericAccess.pkg"

class IZ80 :public virtual IGenericAccess
{
public:
	//Enumerations
	enum IZ80DataSource;

public:
	//Interface version functions
	static inline unsigned int ThisIZ80Version();
	virtual unsigned int GetIZ80Version() const = 0;

	//Register functions
	inline unsigned int GetA() const;
	inline void SetA(unsigned int adata);
	inline unsigned int GetF() const;
	inline void SetF(unsigned int adata);
	inline unsigned int GetB() const;
	inline void SetB(unsigned int adata);
	inline unsigned int GetC() const;
	inline void SetC(unsigned int adata);
	inline unsigned int GetD() const;
	inline void SetD(unsigned int adata);
	inline unsigned int GetE() const;
	inline void SetE(unsigned int adata);
	inline unsigned int GetH() const;
	inline void SetH(unsigned int adata);
	inline unsigned int GetL() const;
	inline void SetL(unsigned int adata);

	inline unsigned int GetAF() const;
	inline void SetAF(unsigned int adata);
	inline unsigned int GetBC() const;
	inline void SetBC(unsigned int adata);
	inline unsigned int GetDE() const;
	inline void SetDE(unsigned int adata);
	inline unsigned int GetHL() const;
	inline void SetHL(unsigned int adata);

	inline unsigned int GetA2() const;
	inline void SetA2(unsigned int adata);
	inline unsigned int GetF2() const;
	inline void SetF2(unsigned int adata);
	inline unsigned int GetB2() const;
	inline void SetB2(unsigned int adata);
	inline unsigned int GetC2() const;
	inline void SetC2(unsigned int adata);
	inline unsigned int GetD2() const;
	inline void SetD2(unsigned int adata);
	inline unsigned int GetE2() const;
	inline void SetE2(unsigned int adata);
	inline unsigned int GetH2() const;
	inline void SetH2(unsigned int adata);
	inline unsigned int GetL2() const;
	inline void SetL2(unsigned int adata);

	inline unsigned int GetAF2() const;
	inline void SetAF2(unsigned int adata);
	inline unsigned int GetBC2() const;
	inline void SetBC2(unsigned int adata);
	inline unsigned int GetDE2() const;
	inline void SetDE2(unsigned int adata);
	inline unsigned int GetHL2() const;
	inline void SetHL2(unsigned int adata);

	inline unsigned int GetIXHigh() const;
	inline void SetIXHigh(unsigned int adata);
	inline unsigned int GetIXLow() const;
	inline void SetIXLow(unsigned int adata);
	inline unsigned int GetIYHigh() const;
	inline void SetIYHigh(unsigned int adata);
	inline unsigned int GetIYLow() const;
	inline void SetIYLow(unsigned int adata);

	inline unsigned int GetI() const;
	inline void SetI(unsigned int adata);
	inline unsigned int GetR() const;
	inline void SetR(unsigned int adata);
	inline unsigned int GetIX() const;
	inline void SetIX(unsigned int adata);
	inline unsigned int GetIY() const;
	inline void SetIY(unsigned int adata);
	inline unsigned int GetSP() const;
	inline void SetSP(unsigned int adata);
	inline unsigned int GetPC() const;
	inline void SetPC(unsigned int adata);

	//Interrupt register functions
	inline unsigned int GetInterruptMode() const;
	inline void SetInterruptMode(unsigned int ainterruptMode);
	inline bool GetIFF1() const;
	inline void SetIFF1(bool state);
	inline bool GetIFF2() const;
	inline void SetIFF2(bool state);

	//Flag functions
	inline bool GetFlagS() const;
	inline void SetFlagS(bool flag);
	inline bool GetFlagZ() const;
	inline void SetFlagZ(bool flag);
	inline bool GetFlagY() const;
	inline void SetFlagY(bool flag);
	inline bool GetFlagH() const;
	inline void SetFlagH(bool flag);
	inline bool GetFlagX() const;
	inline void SetFlagX(bool flag);
	inline bool GetFlagPV() const;
	inline void SetFlagPV(bool flag);
	inline bool GetFlagN() const;
	inline void SetFlagN(bool flag);
	inline bool GetFlagC() const;
	inline void SetFlagC(bool flag);

	//Latched register original value functions
	inline unsigned int GetOriginalValueChangeCounter() const;
	inline unsigned int GetOriginalValueA() const;
	inline unsigned int GetOriginalValueF() const;
	inline unsigned int GetOriginalValueB() const;
	inline unsigned int GetOriginalValueC() const;
	inline unsigned int GetOriginalValueD() const;
	inline unsigned int GetOriginalValueE() const;
	inline unsigned int GetOriginalValueH() const;
	inline unsigned int GetOriginalValueL() const;
	inline unsigned int GetOriginalValueAF() const;
	inline unsigned int GetOriginalValueBC() const;
	inline unsigned int GetOriginalValueDE() const;
	inline unsigned int GetOriginalValueHL() const;
	inline unsigned int GetOriginalValueA2() const;
	inline unsigned int GetOriginalValueF2() const;
	inline unsigned int GetOriginalValueB2() const;
	inline unsigned int GetOriginalValueC2() const;
	inline unsigned int GetOriginalValueD2() const;
	inline unsigned int GetOriginalValueE2() const;
	inline unsigned int GetOriginalValueH2() const;
	inline unsigned int GetOriginalValueL2() const;
	inline unsigned int GetOriginalValueAF2() const;
	inline unsigned int GetOriginalValueBC2() const;
	inline unsigned int GetOriginalValueDE2() const;
	inline unsigned int GetOriginalValueHL2() const;
	inline unsigned int GetOriginalValueIXHigh() const;
	inline unsigned int GetOriginalValueIXLow() const;
	inline unsigned int GetOriginalValueIYHigh() const;
	inline unsigned int GetOriginalValueIYLow() const;
	inline unsigned int GetOriginalValueI() const;
	inline unsigned int GetOriginalValueR() const;
	inline unsigned int GetOriginalValueIX() const;
	inline unsigned int GetOriginalValueIY() const;
	inline unsigned int GetOriginalValueSP() const;
	inline unsigned int GetOriginalValuePC() const;
	inline unsigned int GetOriginalValueInterruptMode() const;
	inline bool GetOriginalValueIFF1() const;
	inline bool GetOriginalValueIFF2() const;
	inline bool GetOriginalValueFlagS() const;
	inline bool GetOriginalValueFlagZ() const;
	inline bool GetOriginalValueFlagY() const;
	inline bool GetOriginalValueFlagH() const;
	inline bool GetOriginalValueFlagX() const;
	inline bool GetOriginalValueFlagPV() const;
	inline bool GetOriginalValueFlagN() const;
	inline bool GetOriginalValueFlagC() const;
};

#include "IZ80.inl"
#endif
