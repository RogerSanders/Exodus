/*--------------------------------------------------------------------------------------*\
Description:
This core emulates the Motorola M68000 CPU.

Known Inaccuracies:
-Opcodes are executed in a single indivisible step, with all bus access for that opcode
occurring at the same logical unit of time.
-Group 0 exceptions don't interrupt the current execution of an opcode, and are not taken
until the next opcode is being fetched. This means that a group 0 exception might not
always execute within 2 cycles as it would on real hardware, and the processor may
continue to access the bus or write to registers after the exception has occurred. This is
due to the opcode step inaccuracy noted above.
-Bus requests don't interrupt the current execution of an opcode. The bus is only granted
between instructions. This is due to the opcode step inaccuracy noted above.

Disassembly and debugging features to add:
-Add the ability to break on read/write to an internal CPU register, eg, break when D0 is
modified.

Things to do:
-Implement correct support for the 2-word prefetch pipeline. We now have a very detailed
reference for this in 68kPrefetch.htm.
-Check our savestate functions again. I'm pretty sure we're leaving some exception
related data out. Check processorState as well.
-Implement support for the undefined N flag state in the CHK instruction
-Implement support for the undefined V and C flag state in the DIVS/DIVU instructions
-Confirm the behaviour of the N flag in DIVU
-Implement accurate execution times for the DIVS/DIVU opcodes. We now have a reference for
this. See div68kCycleAccurate.c.
-Confirm the read behaviour of the MOVE.L instruction when the destination address is
predecremented.
-Have a very close look at the sign extension behaviour when reading immediate data from
the EffectiveAddress class. Find all cases where we rely on this behaviour, and see if we
can phase it out.
-Confirm whether the STOP instruction can trigger a privilege violation if the supplied
status register data doesn't have the supervisor bit set, and test whether the status
register contents are fully loaded in this case.
-Implement proper support for the TAS instruction
-Investigate the alignment behaviour of the stack pointer a little more closely. We
explicitly prevent A7 ending up on an odd address as the result of a postinc or predec
operation with a byte-size operand. We do not however prevent an odd address being loaded
into the stack pointer manually, nor do we prevent any arithmetic or similar operation
from setting A7 to an odd address. Note that we also have this postinc and predec
workaround for A7 in place all the time. It is possible the alignment behaviour of A7
only occurs when certain operations are taking place, and that some instructions would
allow A7 to hit an odd address after a read or a write, when A7 is being used as a
specific operand. Further research and testing is needed on the alignment behaviour of
the stack pointer to be sure of its correct behaviour.
-When A7 alignment happens as a result of a byte-wide access, confirm whether the upper
byte or the lower byte of the target location is affected. Also confirm the contents of
the other half of the value, EG, does sign extension occur? Is the data mirrored in both
halves?

References:
-M68000 Programmer's Reference Manual (M68000PM), Motorola, Rev 1 1992
-M68000 Microprocessors User's Manual (M68000UM), Motorola, Ninth Edition 1993
-68000 Undocumented Behavior Notes, Bart Trzynadlowski, Fourth Edition 2003
-MC680x0 Reference (M68000 Programmer's Reference Manual HTML Transcription), Flint/DARKNESS, v1.1 1995
\*--------------------------------------------------------------------------------------*/
#include "IM68000.h"
#ifndef __M68000_M68000_H__
#define __M68000_M68000_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Processor/Processor.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "Data.h"
#include "ExecuteTime.h"
#include <mutex>
#include <condition_variable>
#include <list>
namespace M68000 {
class M68000Instruction;

class M68000 :public Processor, public IM68000
{
public:
	//Enumerations
	enum class FunctionCode;
	enum class State;

public:
	//Constructors
	M68000(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~M68000();

	//Interface version functions
	virtual unsigned int GetIM68000Version() const;

	//Initialization functions
	virtual bool Construct(IHierarchicalStorageNode& node);
	virtual bool BuildDevice();
	virtual bool ValidateDevice();
	virtual void Initialize();
	virtual void Reset();
	virtual void BeginExecution();

	//Reference functions
	using Processor::AddReference;
	virtual bool AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Exception functions
	double PushStackFrame(const M68000Long& apc, const M68000Word& asr, bool processingInstruction = true);
	double PushStackFrame(const M68000Long& apc, const M68000Word& asr, const M68000Word& ainstructionRegister, const M68000Long& aaccessAddress, bool aread, bool aprocessingInstruction, FunctionCode afunctionCode);
	ExecuteTime ProcessException(Exceptions vector);
	ExecuteTime GetExceptionProcessingTime(Exceptions vector) const;
	bool ExceptionDisabled(Exceptions vector);
	void ExceptionLogIfRequested(Exceptions vector);
	void ExceptionBreakIfRequested(Exceptions vector);

	//Suspend functions
	virtual bool UsesExecuteSuspend() const;

	//Execute functions
	virtual double ExecuteStep();
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual void NotifyAfterExecuteStepFinishedTimeslice();

	//Instruction functions
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

	//Line functions
	virtual unsigned int GetLineID(const MarshalSupport::Marshal::In<std::wstring>& lineName) const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual bool AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	//Clock source functions
	virtual unsigned int GetClockSourceID(const MarshalSupport::Marshal::In<std::wstring>& clockSourceName) const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetClockSourceName(unsigned int clockSourceID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate);

	//Disassembly functions
	bool DisassemblyGetAddressRegisterLastAccessedInPostIncMode(unsigned int regNo) const;
	void DisassemblySetAddressRegisterLastAccessedInPostIncMode(unsigned int regNo, bool state);
	unsigned int DisassemblyGetAddressRegisterCurrentArrayID(unsigned int regNo) const;
	void DisassemblySetAddressRegisterCurrentArrayID(unsigned int regNo, unsigned int state);
	bool DisassemblyGetAddressRegisterUnmodified(unsigned int regNo, unsigned int& sourceLocation) const;
	bool DisassemblyGetDataRegisterUnmodified(unsigned int regNo, unsigned int& sourceLocation) const;
	void DisassemblySetAddressRegisterUnmodified(unsigned int regNo, bool state, unsigned int dataSize = 0, unsigned int sourceLocation = 0);
	void DisassemblySetDataRegisterUnmodified(unsigned int regNo, bool state, unsigned int dataSize = 0, unsigned int sourceLocation = 0);
	unsigned int DisassemblyGetAddressRegisterUnmodifiedSize(unsigned int regNo) const;
	unsigned int DisassemblyGetDataRegisterUnmodifiedSize(unsigned int regNo) const;

	//CCR flags
	inline bool GetX() const;
	inline void SetX(bool flag);
	inline bool GetN() const;
	inline void SetN(bool flag);
	inline bool GetZ() const;
	inline void SetZ(bool flag);
	inline bool GetV() const;
	inline void SetV(bool flag);
	inline bool GetC() const;
	inline void SetC(bool flag);

	//SR flags
	inline bool GetSR_T() const;
	inline void SetSR_T(bool flag);
	inline bool GetSR_S() const;
	inline void SetSR_S(bool flag);
	inline unsigned int GetSR_IPM() const;
	inline void SetSR_IPM(unsigned int aIPM);

	//Register functions
	inline M68000Long GetPC() const;
	inline void GetPC(Data& data) const;
	inline void SetPC(const M68000Long& data);
	inline M68000Word GetSR() const;
	inline void GetSR(Data& data) const;
	inline void SetSR(const M68000Word& data);
	inline M68000Byte GetCCR() const;
	inline void GetCCR(Data& data) const;
	inline void SetCCR(const M68000Byte& data);
	inline M68000Long GetSP() const;
	inline void GetSP(Data& data) const;
	inline void SetSP(const M68000Long& data);
	inline M68000Long GetSSP() const;
	inline void GetSSP(Data& data) const;
	inline void SetSSP(const M68000Long& data);
	inline M68000Long GetUSP() const;
	inline void GetUSP(Data& data) const;
	inline void SetUSP(const M68000Long& data);
	inline M68000Long GetA(unsigned int index) const;
	inline void GetA(unsigned int index, Data& data) const;
	inline void SetA(unsigned int index, const M68000Long& data);
	inline M68000Long GetD(unsigned int index) const;
	inline void GetD(unsigned int index, Data& data) const;
	inline void SetD(unsigned int index, const M68000Long& data);

	//##TODO## Phase out these functions, and just work with the state variable directly.
	inline void SetProcessorState(State executeState);

	//Special operation functions
	void TriggerExternalReset(double resetTimeBegin, double resetTimeEnd);

	//Memory access functions
	FunctionCode GetFunctionCode(bool programReference) const;
	double ReadMemory(const M68000Long& location, Data& data, FunctionCode code, bool transparent, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;
	double ReadMemory(const M68000Long& location, Data& data, FunctionCode code, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;
	void ReadMemoryTransparent(const M68000Long& location, Data& data, FunctionCode code, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;
	double WriteMemory(const M68000Long& location, const Data& data, FunctionCode code, bool transparent, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;
	double WriteMemory(const M68000Long& location, const Data& data, FunctionCode code, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;
	void WriteMemoryTransparent(const M68000Long& location, const Data& data, FunctionCode code, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;

	//CE line state functions
	virtual unsigned int GetCELineID(const MarshalSupport::Marshal::In<std::wstring>& lineName, bool inputLine) const;
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const;

	//Active disassembly functions
	virtual bool ActiveDisassemblySupported() const;

	//Active disassembly formatting functions
	virtual bool GetLeadingLinesForASMFile(unsigned int analysisStartAddress, unsigned int analysisEndAddress, std::list<std::wstring>& outputLines) const;
	virtual bool GetTrailingLinesForASMFile(unsigned int analysisStartAddress, unsigned int analysisEndAddress, std::list<std::wstring>& outputLines) const;
	virtual bool FormatOpcodeForDisassembly(unsigned int opcodeAddress, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& opcodeArguments, std::wstring& opcodeComments) const;
	virtual bool FormatDataForDisassembly(const std::vector<Data>& dataElements, unsigned int dataElementByteSize, DisassemblyDataType dataType, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& formattedData) const;
	virtual bool FormatOffsetForDisassembly(const Data& offsetData, bool relativeOffset, unsigned int relativeOffsetBaseAddress, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& formattedOffset) const;
	virtual bool FormatCommentForDisassembly(const std::wstring& rawComment, std::wstring& formattedComment) const;
	virtual bool FormatLabelPlacementForDisassembly(const std::wstring& rawLabel, std::wstring& formattedLabel) const;
	virtual bool FormatLabelUsageForDisassembly(const std::wstring& rawLabel, int labelOffset, std::wstring& formattedLabel) const;

	//Exception debugging functions
	virtual bool GetLogAllExceptions() const;
	virtual void SetLogAllExceptions(bool state);
	virtual bool GetBreakOnAllExceptions() const;
	virtual void SetBreakOnAllExceptions(bool state);
	virtual bool GetDisableAllExceptions() const;
	virtual void SetDisableAllExceptions(bool state);
	virtual MarshalSupport::Marshal::Ret<std::list<ExceptionDebuggingEntry>> GetExceptionDebugEntries() const;
	virtual void SetExceptionDebugEntries(const MarshalSupport::Marshal::In<std::list<ExceptionDebuggingEntry>>& state);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExceptionName(Exceptions vectorNumber) const;
	virtual void TriggerException(Exceptions vectorNumber);

	//Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHierarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHierarchicalStorageNode& node) const;

	//Data read/write functions
	using IGenericAccess::ReadGenericData;
	using IGenericAccess::WriteGenericData;
	virtual bool ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const;
	virtual bool WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue);

	//Highlight functions
	virtual bool GetGenericDataHighlightState(unsigned int dataID, const DataContext* dataContext) const;
	void PopulateChangedRegStateFromCurrentState();

private:
	//Enumerations
	enum class CELineID;
	enum class LineID;
	enum class ClockID;

	//Structures
	struct LineAccess;
	struct CalculateCELineStateContext;
	struct RegisterDisassemblyInfo
	{
		RegisterDisassemblyInfo()
		:unmodified(false), addressRegisterLastUsedInPostIncMode(false)
		{}

		bool unmodified;
		bool addressRegisterLastUsedInPostIncMode;
		unsigned int currentArrayID;
		unsigned int sourceLocation;
		unsigned int dataSize;
	};

private:
	//Line functions
	void ApplyLineStateChange(LineID targetLine, const Data& lineData, std::unique_lock<std::mutex>& lock);

	//Clock source functions
	void ApplyClockStateChange(ClockID targetClock, double clockRate);

private:
	//Bus interface
	IBusInterface* memoryBus;

	//Opcode decode table
	std::list<M68000Instruction*> opcodeList;
	OpcodeTable<M68000Instruction> opcodeTable;

	//Opcode allocation buffer for placement new
	void* opcodeBuffer;

	//User registers
	M68000Long a[addressRegCount - 1];
	M68000Long ba[addressRegCount - 1];
	M68000Long d[dataRegCount];
	M68000Long bd[dataRegCount];
	M68000Long ssp;
	M68000Long bssp;
	M68000Long usp;
	M68000Long busp;
	M68000Long pc;
	M68000Long bpc;
	M68000Word sr;
	M68000Word bsr;

	//Disassembly register info
	RegisterDisassemblyInfo aDisassemblyInfo[addressRegCount - 1];
	RegisterDisassemblyInfo baDisassemblyInfo[addressRegCount - 1];
	RegisterDisassemblyInfo dDisassemblyInfo[dataRegCount];
	RegisterDisassemblyInfo bdDisassemblyInfo[dataRegCount];

	//Processor status data
	State processorState;
	State bprocessorState;
	mutable M68000Word lastReadBusData;
	M68000Word blastReadBusData;
	bool wordIsPrefetched;
	bool bwordIsPrecached;
	M68000Word prefetchedWord;
	M68000Word bprefetchedWord;
	M68000Long prefetchedWordAddress;
	M68000Long bprefetchedWordAddress;
	bool powerOnDelayPending;
	bool bpowerOnDelayPending;

	//Group 0 exception info
	mutable bool group0ExceptionPending;
	bool bgroup0ExceptionPending;
	mutable M68000Word group0InstructionRegister;
	M68000Word bgroup0InstructionRegister;
	mutable M68000Long group0Address;
	M68000Long bgroup0Address;
	mutable M68000Long group0PC;
	M68000Long bgroup0PC;
	mutable M68000Word group0SR;
	M68000Word bgroup0SR;
	mutable bool group0ReadWriteFlag;
	bool bgroup0ReadWriteFlag;
	mutable bool group0InstructionFlag;
	bool bgroup0InstructionFlag;
	mutable Exceptions group0Vector;
	Exceptions bgroup0Vector;
	mutable FunctionCode group0FunctionCode;
	FunctionCode bgroup0FunctionCode;

	//Exception debugging
	mutable std::mutex debugMutex;
	std::list<ExceptionDebuggingEntry> exceptionList;
	volatile bool exceptionListEmpty;
	volatile bool logAllExceptions;
	volatile bool breakOnAllExceptions;
	volatile bool disableAllExceptions;
	volatile bool debugExceptionTriggerPending;
	Exceptions debugExceptionTriggerVector;

	//Changed register state
	unsigned int regChangedA[addressRegCount];
	unsigned int regChangedD[dataRegCount];
	unsigned int regChangedSP;
	unsigned int regChangedSSP;
	unsigned int regChangedUSP;
	unsigned int regChangedPC;
	unsigned int regChangedSR;
	unsigned int regChangedCCR;
	bool regChangedX;
	bool regChangedN;
	bool regChangedZ;
	bool regChangedV;
	bool regChangedC;
	bool regChangedS;
	bool regChangedT;
	unsigned int regChangedIPM;

	//CE line masks
	unsigned int ceLineMaskLowerDataStrobe;
	unsigned int ceLineMaskUpperDataStrobe;
	unsigned int ceLineMaskReadHighWriteLow;
	unsigned int ceLineMaskAddressStrobe;
	unsigned int ceLineMaskFunctionCode;
	unsigned int ceLineBitNumberFunctionCode;
	unsigned int ceLineMaskFCCPUSpace;
	unsigned int ceLineMaskRMWCycleInProgress;
	unsigned int ceLineMaskRMWCycleFirstOperation;

	//Line access
	std::mutex lineMutex;
	double lastLineCheckTime;
	volatile bool lineAccessPending;
	double lastTimesliceLength;
	double blastTimesliceLength;
	std::list<LineAccess> lineAccessBuffer;
	std::list<LineAccess> blineAccessBuffer;
	bool suspendWhenBusReleased;
	bool suspendUntilLineStateChangeReceived;
	bool bsuspendUntilLineStateChangeReceived;
	std::condition_variable lineStateChangeReceived;
	bool manualDeviceAdvanceInProgress;
	volatile bool executionReachedEndOfTimeslice;
	std::condition_variable advanceToTargetLineStateChanged;
	mutable bool autoVectorPendingInterrupt;
	mutable double autoVectorPendingInterruptChangeTime;
	bool resetLineState;
	bool haltLineState;
	bool brLineState;
	bool bgLineState;
	bool forceInterrupt;
	unsigned int interruptPendingLevel;
	bool bresetLineState;
	bool bhaltLineState;
	bool bbrLineState;
	bool bbgLineState;
	bool bforceInterrupt;
	unsigned int binterruptPendingLevel;
};

} //Close namespace M68000
#include "M68000.inl"
#endif
