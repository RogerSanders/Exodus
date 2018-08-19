/*--------------------------------------------------------------------------------------*\
Description:
This core emulates the Zilog Z80 CPU.

Known Inaccuracies:
-Opcodes are executed in a single indivisible step, with all bus access for that opcode
occurring at the same logical unit of time.
-The undocumented Y and X flag results after a BIT opcode will not be correct in all
cases, due to incomplete information. See the notes in the BIT opcode.

Things to do:
-Implement port-based communication in the memory bus, so we can add support for the I/O
opcodes.
-Implement all undefined behaviour described in the undocumented Z80 documented
-Implement interrupt modes 0 and 2
-There's a lot of ED prefixed opcodes we haven't implemented, including the port access
functions of course, as well as undocumented versions of other opcodes. Go through every
opcode in the ED range and add support for as many as possible, and clearly list the ones
which aren't supported yet.
-Check all your opcode execution times with the latest info in the undocumented z80
documented. Some of the numbers appear to be different to the old text document you based
your execution times on, and I've noticed the BIT opcodes appear to be off for example.
Also make sure you confirm the execution times for the double-output prefixed CB opcodes.

References:
-Z80 Family CPU User Manual (UM008005-0205), Zilog, 2004
-Z80 Processor Technical Manual, Mostek, 1979
-The Undocumented Z80 Documented, Sean Young and Jan, v0.91 2005
-Interrupt Behaviour of the Z80 CPU, http://www.z80.info/interrup.htm, retrieved 15/4/2008
\*--------------------------------------------------------------------------------------*/
#include "IZ80.h"
#ifndef __Z80_Z80_H__
#define __Z80_Z80_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "Processor/Processor.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "Data.h"
#include "ExecuteTime.h"
#include <mutex>
#include <list>
// View and menu classes
class RegistersViewPresenter;
class RegistersView;
namespace Z80 {
class Z80Instruction;

class Z80 :public Processor, public IZ80
{
public:
	// Constructors
	Z80(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	~Z80();

	// Interface version functions
	virtual unsigned int GetIZ80Version() const;

	// Initialization functions
	virtual bool Construct(IHierarchicalStorageNode& node);
	virtual bool BuildDevice();
	virtual bool ValidateDevice();
	virtual void Initialize();
	virtual void Reset();
	virtual void BeginExecution();

	// Reference functions
	using Processor::AddReference;
	virtual bool AddReference(const Marshal::In<std::wstring>& referenceName, IBusInterface* target);
	virtual void RemoveReference(IBusInterface* target);

	// Suspend functions
	virtual bool UsesExecuteSuspend() const;

	// Execute functions
	virtual double ExecuteStep();
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);

	// Line functions
	virtual unsigned int GetLineID(const Marshal::In<std::wstring>& lineName) const;
	virtual Marshal::Ret<std::wstring> GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;
	void ApplyLineStateChange(unsigned int targetLine, const Data& lineData, std::unique_lock<std::mutex>& lock);

	// Clock source functions
	virtual unsigned int GetClockSourceID(const Marshal::In<std::wstring>& clockSourceName) const;
	virtual Marshal::Ret<std::wstring> GetClockSourceName(unsigned int clockSourceID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate);
	void ApplyClockStateChange(unsigned int targetClock, double clockRate);

	// Instruction functions
	//##TODO##
	// virtual bool FormatDataForDisassembly(const std::vector<Data>& dataElements, unsigned int outputElementBitCount, std::wstring& disassembly) const;
	virtual unsigned int GetByteBitCount() const;
	virtual unsigned int GetCurrentPC() const;
	virtual unsigned int GetPCWidth() const;
	virtual unsigned int GetAddressBusWidth() const;
	virtual unsigned int GetDataBusWidth() const;
	virtual unsigned int GetMinimumOpcodeByteSize() const;
	virtual unsigned int GetMinimumDataByteSize() const;
	virtual unsigned int GetMemorySpaceByte(unsigned int location) const;
	virtual void SetMemorySpaceByte(unsigned int location, unsigned int data);
	virtual bool GetOpcodeInfo(unsigned int location, IOpcodeInfo& opcodeInfo) const;

	// Register functions
	inline Z80Byte GetA() const;
	inline void GetA(Data& data) const;
	inline void SetA(const Z80Byte& data);
	inline Z80Byte GetF() const;
	inline void GetF(Data& data) const;
	inline void SetF(const Z80Byte& data);
	inline Z80Byte GetB() const;
	inline void GetB(Data& data) const;
	inline void SetB(const Z80Byte& data);
	inline Z80Byte GetC() const;
	inline void GetC(Data& data) const;
	inline void SetC(const Z80Byte& data);
	inline Z80Byte GetD() const;
	inline void GetD(Data& data) const;
	inline void SetD(const Z80Byte& data);
	inline Z80Byte GetE() const;
	inline void GetE(Data& data) const;
	inline void SetE(const Z80Byte& data);
	inline Z80Byte GetH() const;
	inline void GetH(Data& data) const;
	inline void SetH(const Z80Byte& data);
	inline Z80Byte GetL() const;
	inline void GetL(Data& data) const;
	inline void SetL(const Z80Byte& data);

	inline Z80Word GetAF() const;
	inline void GetAF(Data& data) const;
	inline void SetAF(const Z80Word& data);
	inline Z80Word GetBC() const;
	inline void GetBC(Data& data) const;
	inline void SetBC(const Z80Word& data);
	inline Z80Word GetDE() const;
	inline void GetDE(Data& data) const;
	inline void SetDE(const Z80Word& data);
	inline Z80Word GetHL() const;
	inline void GetHL(Data& data) const;
	inline void SetHL(const Z80Word& data);

	inline Z80Byte GetA2() const;
	inline void GetA2(Data& data) const;
	inline void SetA2(const Z80Byte& data);
	inline Z80Byte GetF2() const;
	inline void GetF2(Data& data) const;
	inline void SetF2(const Z80Byte& data);
	inline Z80Byte GetB2() const;
	inline void GetB2(Data& data) const;
	inline void SetB2(const Z80Byte& data);
	inline Z80Byte GetC2() const;
	inline void GetC2(Data& data) const;
	inline void SetC2(const Z80Byte& data);
	inline Z80Byte GetD2() const;
	inline void GetD2(Data& data) const;
	inline void SetD2(const Z80Byte& data);
	inline Z80Byte GetE2() const;
	inline void GetE2(Data& data) const;
	inline void SetE2(const Z80Byte& data);
	inline Z80Byte GetH2() const;
	inline void GetH2(Data& data) const;
	inline void SetH2(const Z80Byte& data);
	inline Z80Byte GetL2() const;
	inline void GetL2(Data& data) const;
	inline void SetL2(const Z80Byte& data);

	inline Z80Word GetAF2() const;
	inline void GetAF2(Data& data) const;
	inline void SetAF2(const Z80Word& data);
	inline Z80Word GetBC2() const;
	inline void GetBC2(Data& data) const;
	inline void SetBC2(const Z80Word& data);
	inline Z80Word GetDE2() const;
	inline void GetDE2(Data& data) const;
	inline void SetDE2(const Z80Word& data);
	inline Z80Word GetHL2() const;
	inline void GetHL2(Data& data) const;
	inline void SetHL2(const Z80Word& data);

	inline Z80Byte GetIXHigh() const;
	inline void GetIXHigh(Data& data) const;
	inline void SetIXHigh(const Z80Byte& data);
	inline Z80Byte GetIXLow() const;
	inline void GetIXLow(Data& data) const;
	inline void SetIXLow(const Z80Byte& data);
	inline Z80Byte GetIYHigh() const;
	inline void GetIYHigh(Data& data) const;
	inline void SetIYHigh(const Z80Byte& data);
	inline Z80Byte GetIYLow() const;
	inline void GetIYLow(Data& data) const;
	inline void SetIYLow(const Z80Byte& data);

	inline Z80Byte GetI() const;
	inline void GetI(Data& data) const;
	inline void SetI(const Z80Byte& data);
	inline Z80Byte GetR() const;
	inline void GetR(Data& data) const;
	inline void SetR(const Z80Byte& data);
	inline Z80Word GetIX() const;
	inline void GetIX(Data& data) const;
	inline void SetIX(const Z80Word& data);
	inline Z80Word GetIY() const;
	inline void GetIY(Data& data) const;
	inline void SetIY(const Z80Word& data);
	inline Z80Word GetSP() const;
	inline void GetSP(Data& data) const;
	inline void SetSP(const Z80Word& data);
	inline Z80Word GetPC() const;
	inline void GetPC(Data& data) const;
	inline void SetPC(const Z80Word& data);

	inline void AddRefresh(unsigned int increase);

	// Interrupt register functions
	inline unsigned int GetInterruptMode() const;
	inline void SetInterruptMode(unsigned int interruptMode);
	inline bool GetIFF1() const;
	inline void SetIFF1(bool state);
	inline bool GetIFF2() const;
	inline void SetIFF2(bool state);
	inline bool GetMaskInterruptsNextOpcode() const;
	inline void SetMaskInterruptsNextOpcode(bool state);
	inline void SetProcessorStoppedState(bool state);

	// Flag functions
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

	// Memory access functions
	double ReadMemory(const Z80Word& location, Data& data, bool transparent) const;
	double WriteMemory(const Z80Word& location, const Data& data, bool transparent) const;

	// CE line state functions
	virtual unsigned int GetCELineID(const Marshal::In<std::wstring>& lineName, bool inputLine) const;
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const;

	// Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;

	// Data read/write functions
	using IGenericAccess::ReadGenericData;
	using IGenericAccess::WriteGenericData;
	virtual bool ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const;
	virtual bool WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue);

	// Highlight functions
	virtual bool GetGenericDataHighlightState(unsigned int dataID, const DataContext* dataContext) const;
	void PopulateChangedRegStateFromCurrentState();

private:
	// Enumerations
	enum class CELineID;
	enum class LineID;
	enum class ClockID;

	// Structures
	struct LineAccess;
	struct CalculateCELineStateContext;

	// View and menu classes
	friend class RegistersViewPresenter;
	friend class RegistersView;

private:
	// Bus interface
	mutable ReadWriteLock _externalReferenceLock;
	IBusInterface* _memoryBus;

	// Opcode decode tables
	std::list<Z80Instruction*> _opcodeList;
	std::list<Z80Instruction*> _opcodeListCB;
	std::list<Z80Instruction*> _opcodeListED;
	OpcodeTable<Z80Instruction> _opcodeTable;
	OpcodeTable<Z80Instruction> _opcodeTableCB;
	OpcodeTable<Z80Instruction> _opcodeTableED;

	// Opcode allocation buffer for placement new
	void* _opcodeBuffer;

	// Main registers       Alternate registers
	Z80Word _afreg;        Z80Word _af2reg;
	Z80Word _bcreg;        Z80Word _bc2reg;
	Z80Word _dereg;        Z80Word _de2reg;
	Z80Word _hlreg;        Z80Word _hl2reg;
	Z80Word _bafreg;       Z80Word _baf2reg;
	Z80Word _bbcreg;       Z80Word _bbc2reg;
	Z80Word _bdereg;       Z80Word _bde2reg;
	Z80Word _bhlreg;       Z80Word _bhl2reg;

	// Special purpose registers
	Z80Byte _ireg;
	Z80Byte _rreg;
	Z80Word _ixreg;
	Z80Word _iyreg;
	Z80Word _spreg;
	Z80Word _pcreg;
	Z80Byte _bireg;
	Z80Byte _brreg;
	Z80Word _bixreg;
	Z80Word _biyreg;
	Z80Word _bspreg;
	Z80Word _bpcreg;

	// Interrupt registers
	unsigned int _interruptMode;
	bool _iff1;
	bool _iff2;
	bool _maskInterruptsNextOpcode;
	unsigned int _binterruptMode;
	bool _biff1;
	bool _biff2;
	bool _bmaskInterruptsNextOpcode;

	// External signals
	bool _processorStopped;
	bool _bprocessorStopped;

	// CE line masks
	unsigned int _ceLineMaskRD;
	unsigned int _ceLineMaskWR;

	// Changed register state
	volatile unsigned int _regChangedAF;
	volatile unsigned int _regChangedBC;
	volatile unsigned int _regChangedDE;
	volatile unsigned int _regChangedHL;
	volatile unsigned int _regChangedAF2;
	volatile unsigned int _regChangedBC2;
	volatile unsigned int _regChangedDE2;
	volatile unsigned int _regChangedHL2;
	volatile unsigned int _regChangedA;
	volatile unsigned int _regChangedF;
	volatile unsigned int _regChangedB;
	volatile unsigned int _regChangedC;
	volatile unsigned int _regChangedD;
	volatile unsigned int _regChangedE;
	volatile unsigned int _regChangedH;
	volatile unsigned int _regChangedL;
	volatile unsigned int _regChangedA2;
	volatile unsigned int _regChangedF2;
	volatile unsigned int _regChangedB2;
	volatile unsigned int _regChangedC2;
	volatile unsigned int _regChangedD2;
	volatile unsigned int _regChangedE2;
	volatile unsigned int _regChangedH2;
	volatile unsigned int _regChangedL2;
	volatile unsigned int _regChangedI;
	volatile unsigned int _regChangedR;
	volatile unsigned int _regChangedIX;
	volatile unsigned int _regChangedIY;
	volatile unsigned int _regChangedIXHigh;
	volatile unsigned int _regChangedIXLow;
	volatile unsigned int _regChangedIYHigh;
	volatile unsigned int _regChangedIYLow;
	volatile unsigned int _regChangedSP;
	volatile unsigned int _regChangedPC;
	volatile unsigned int _regChangedIM;
	volatile bool _regChangedIFF1;
	volatile bool _regChangedIFF2;
	volatile bool _regChangedFlagS;
	volatile bool _regChangedFlagZ;
	volatile bool _regChangedFlagY;
	volatile bool _regChangedFlagH;
	volatile bool _regChangedFlagX;
	volatile bool _regChangedFlagPV;
	volatile bool _regChangedFlagN;
	volatile bool _regChangedFlagC;

	// Line access
	std::mutex _lineMutex;
	mutable double _lastLineCheckTime;
	volatile bool _lineAccessPending;
	double _lastTimesliceLength;
	double _blastTimesliceLength;
	std::list<LineAccess> _lineAccessBuffer;
	std::list<LineAccess> _blineAccessBuffer;
	bool _suspendWhenBusReleased;
	volatile bool _suspendUntilLineStateChangeReceived;
	bool _bsuspendUntilLineStateChangeReceived;
	bool _resetLineState;
	bool _resetLastStep;
	bool _busreqLineState;
	bool _busackLineState;
	bool _intLineState;
	bool _nmiLineState;
	bool _bresetLineState;
	bool _bbusreqLineState;
	bool _bbusackLineState;
	bool _bintLineState;
	bool _bnmiLineState;
};

} // Close namespace Z80
#include "Z80.inl"
#endif
