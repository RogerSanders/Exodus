#ifndef __IM68000_H__
#define __IM68000_H__
#include "GenericAccess/GenericAccess.pkg"
#include "Processor/Processor.pkg"

class IM68000 :public virtual IGenericAccess
{
public:
	//Constants
	static const int addressRegCount = 8;
	static const int dataRegCount = 8;
	static const unsigned int SP = 7;

	//Enumerations
	enum class IM68000DataSource;
	enum class Exceptions;

	//Structures
	struct RegisterDataContext;
	struct ExceptionDebuggingEntry;

public:
	//Interface version functions
	static inline unsigned int ThisIM68000Version() { return 1; }
	virtual unsigned int GetIM68000Version() const = 0;

	//CCR flags
	inline bool GetX() const;
	inline void SetX(bool adata);
	inline bool GetN() const;
	inline void SetN(bool adata);
	inline bool GetZ() const;
	inline void SetZ(bool adata);
	inline bool GetV() const;
	inline void SetV(bool adata);
	inline bool GetC() const;
	inline void SetC(bool adata);

	//SR flags
	inline bool GetSR_T() const;
	inline void SetSR_T(bool adata);
	inline bool GetSR_S() const;
	inline void SetSR_S(bool adata);
	inline unsigned int GetSR_IPM() const;
	inline void SetSR_IPM(unsigned int adata);

	//Register functions
	inline unsigned int GetPC() const;
	inline void SetPC(unsigned int adata);
	inline unsigned int GetSR() const;
	inline void SetSR(unsigned int adata);
	inline unsigned int GetCCR() const;
	inline void SetCCR(unsigned int adata);
	inline unsigned int GetSP() const;
	inline void SetSP(unsigned int adata);
	inline unsigned int GetSSP() const;
	inline void SetSSP(unsigned int adata);
	inline unsigned int GetUSP() const;
	inline void SetUSP(unsigned int adata);
	inline unsigned int GetA(unsigned int regiserNo) const;
	inline void SetA(unsigned int regiserNo, unsigned int adata);
	inline unsigned int GetD(unsigned int regiserNo) const;
	inline void SetD(unsigned int regiserNo, unsigned int adata);

	//Latched register original value functions
	inline unsigned int GetOriginalValueChangeCounter() const;
	inline bool GetOriginalValueX() const;
	inline bool GetOriginalValueN() const;
	inline bool GetOriginalValueZ() const;
	inline bool GetOriginalValueV() const;
	inline bool GetOriginalValueC() const;
	inline bool GetOriginalValueSR_T() const;
	inline bool GetOriginalValueSR_S() const;
	inline unsigned int GetOriginalValueSR_IPM() const;
	inline unsigned int GetOriginalValuePC() const;
	inline unsigned int GetOriginalValueSR() const;
	inline unsigned int GetOriginalValueCCR() const;
	inline unsigned int GetOriginalValueSP() const;
	inline unsigned int GetOriginalValueSSP() const;
	inline unsigned int GetOriginalValueUSP() const;
	inline unsigned int GetOriginalValueA(unsigned int registerNo) const;
	inline unsigned int GetOriginalValueD(unsigned int registerNo) const;

	//Exception debugging functions
	virtual bool GetLogAllExceptions() const = 0;
	virtual void SetLogAllExceptions(bool state) = 0;
	virtual bool GetBreakOnAllExceptions() const = 0;
	virtual void SetBreakOnAllExceptions(bool state) = 0;
	virtual bool GetDisableAllExceptions() const = 0;
	virtual void SetDisableAllExceptions(bool state) = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<ExceptionDebuggingEntry>> GetExceptionDebugEntries() const = 0;
	virtual void SetExceptionDebugEntries(const MarshalSupport::Marshal::In<std::list<ExceptionDebuggingEntry>>& state) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExceptionName(Exceptions vectorNumber) const = 0;
	virtual void TriggerException(Exceptions vectorNumber) = 0;
};

#include "IM68000.inl"
#endif
