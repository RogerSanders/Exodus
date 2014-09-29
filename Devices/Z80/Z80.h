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
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Processor/Processor.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "Data.h"
#include "ExecuteTime.h"
#include <mutex>
#include <list>
//View and menu classes
class RegistersViewPresenter;
class RegistersView;
namespace Z80 {
class Z80Instruction;

class Z80 :public Processor, public IZ80
{
public:
	//Constructors
	Z80(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~Z80();

	//Interface version functions
	virtual unsigned int GetIZ80Version() const;

	//Initialization functions
	virtual bool Construct(IHierarchicalStorageNode& node);
	virtual bool BuildDevice();
	virtual bool ValidateDevice();
	virtual void Initialize();
	virtual void Reset();
	virtual void BeginExecution();
	virtual void SuspendExecution();

	//Reference functions
	using Processor::AddReference;
	virtual bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Suspend functions
	virtual bool UsesExecuteSuspend() const;

	//Execute functions
	virtual double ExecuteStep();
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);

	//Line functions
	virtual unsigned int GetLineID(const std::wstring& lineName) const;
	virtual std::wstring GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;
	void ApplyLineStateChange(unsigned int targetLine, const Data& lineData, std::unique_lock<std::mutex>& lock);

	//Clock source functions
	virtual unsigned int GetClockSourceID(const std::wstring& clockSourceName) const;
	virtual std::wstring GetClockSourceName(unsigned int clockSourceID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate);
	void ApplyClockStateChange(unsigned int targetClock, double clockRate);

	//Instruction functions
	//##TODO##
	//virtual bool FormatDataForDisassembly(const std::vector<Data>& dataElements, unsigned int outputElementBitCount, std::wstring& disassembly) const;
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

	//Register functions
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

	//Interrupt register functions
	inline unsigned int GetInterruptMode() const;
	inline void SetInterruptMode(unsigned int ainterruptMode);
	inline bool GetIFF1() const;
	inline void SetIFF1(bool state);
	inline bool GetIFF2() const;
	inline void SetIFF2(bool state);
	inline bool GetMaskInterruptsNextOpcode() const;
	inline void SetMaskInterruptsNextOpcode(bool state);
	inline void SetProcessorStoppedState(bool astate);

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

	//Memory access functions
	double ReadMemory(const Z80Word& location, Data& data, bool transparent) const;
	double WriteMemory(const Z80Word& location, const Data& data, bool transparent) const;

	//CE line state functions
	virtual unsigned int GetCELineID(const std::wstring& lineName, bool inputLine) const;
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const;

	//Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;

	//Data read/write functions
	using IGenericAccess::ReadGenericData;
	using IGenericAccess::WriteGenericData;
	virtual bool ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const;
	virtual bool WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue);

private:
	//Enumerations
	enum class CELineID;
	enum class LineID;
	enum class ClockID;

	//Structures
	struct LineAccess;
	struct CalculateCELineStateContext;

	//View and menu classes
	friend class RegistersViewPresenter;
	friend class RegistersView;

private:
	//Bus interface
	IBusInterface* memoryBus;

	//Opcode decode tables
	std::list<Z80Instruction*> opcodeList;
	std::list<Z80Instruction*> opcodeListCB;
	std::list<Z80Instruction*> opcodeListED;
	OpcodeTable<Z80Instruction> opcodeTable;
	OpcodeTable<Z80Instruction> opcodeTableCB;
	OpcodeTable<Z80Instruction> opcodeTableED;

	//Opcode allocation buffer for placement new
	void* opcodeBuffer;

	//Main registers   Alternate registers
	Z80Word afreg;        Z80Word af2reg;
	Z80Word bcreg;        Z80Word bc2reg;
	Z80Word dereg;        Z80Word de2reg;
	Z80Word hlreg;        Z80Word hl2reg;
	Z80Word bafreg;       Z80Word baf2reg;
	Z80Word bbcreg;       Z80Word bbc2reg;
	Z80Word bdereg;       Z80Word bde2reg;
	Z80Word bhlreg;       Z80Word bhl2reg;

	//Special purpose registers
	Z80Byte ireg;
	Z80Byte rreg;
	Z80Word ixreg;
	Z80Word iyreg;
	Z80Word spreg;
	Z80Word pcreg;
	Z80Byte bireg;
	Z80Byte brreg;
	Z80Word bixreg;
	Z80Word biyreg;
	Z80Word bspreg;
	Z80Word bpcreg;

	//Interrupt registers
	unsigned int interruptMode;
	bool iff1;
	bool iff2;
	bool maskInterruptsNextOpcode;
	unsigned int binterruptMode;
	bool biff1;
	bool biff2;
	bool bmaskInterruptsNextOpcode;

	//External signals
	bool processorStopped;
	bool bprocessorStopped;

	//CE line masks
	unsigned int ceLineMaskRD;
	unsigned int ceLineMaskWR;

	//Changed register state
	volatile unsigned int systemPausedToggleCounter;
	volatile unsigned int regChangedAF;
	volatile unsigned int regChangedBC;
	volatile unsigned int regChangedDE;
	volatile unsigned int regChangedHL;
	volatile unsigned int regChangedAF2;
	volatile unsigned int regChangedBC2;
	volatile unsigned int regChangedDE2;
	volatile unsigned int regChangedHL2;
	volatile unsigned int regChangedA;
	volatile unsigned int regChangedF;
	volatile unsigned int regChangedB;
	volatile unsigned int regChangedC;
	volatile unsigned int regChangedD;
	volatile unsigned int regChangedE;
	volatile unsigned int regChangedH;
	volatile unsigned int regChangedL;
	volatile unsigned int regChangedA2;
	volatile unsigned int regChangedF2;
	volatile unsigned int regChangedB2;
	volatile unsigned int regChangedC2;
	volatile unsigned int regChangedD2;
	volatile unsigned int regChangedE2;
	volatile unsigned int regChangedH2;
	volatile unsigned int regChangedL2;
	volatile unsigned int regChangedI;
	volatile unsigned int regChangedR;
	volatile unsigned int regChangedIX;
	volatile unsigned int regChangedIY;
	volatile unsigned int regChangedIXHigh;
	volatile unsigned int regChangedIXLow;
	volatile unsigned int regChangedIYHigh;
	volatile unsigned int regChangedIYLow;
	volatile unsigned int regChangedSP;
	volatile unsigned int regChangedPC;
	volatile unsigned int regChangedIM;
	volatile bool regChangedIFF1;
	volatile bool regChangedIFF2;
	volatile bool regChangedFlagS;
	volatile bool regChangedFlagZ;
	volatile bool regChangedFlagY;
	volatile bool regChangedFlagH;
	volatile bool regChangedFlagX;
	volatile bool regChangedFlagPV;
	volatile bool regChangedFlagN;
	volatile bool regChangedFlagC;

	//Line access
	std::mutex lineMutex;
	mutable double lastLineCheckTime;
	volatile bool lineAccessPending;
	double lastTimesliceLength;
	double blastTimesliceLength;
	std::list<LineAccess> lineAccessBuffer;
	std::list<LineAccess> blineAccessBuffer;
	bool suspendWhenBusReleased;
	volatile bool suspendUntilLineStateChangeReceived;
	bool bsuspendUntilLineStateChangeReceived;
	bool resetLineState;
	bool resetLastStep;
	bool busreqLineState;
	bool busackLineState;
	bool intLineState;
	bool nmiLineState;
	bool bresetLineState;
	bool bbusreqLineState;
	bool bbusackLineState;
	bool bintLineState;
	bool bnmiLineState;
};

} //Close namespace Z80
#include "Z80.inl"
#endif
