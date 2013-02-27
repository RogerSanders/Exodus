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
-Implement a boolean flag for each CPU register, which indicates whether the register has
been modified since the last time the processor reached the stop state. Use this flag to
colour register values in red in the debugger window if the register has been flagged as
modified. This will allow for easier debugging when stepping through code. Note that
interrupts could totally mess this up. For this reason, it might be better to use a more
primitive method, and simply snapshot the register data, and highlight in red if the
register data has changed since the last time the processor was stopped. With this
implementation, the user can set two breakpoints, and easily see what has changed when
stepping from one breakpoint to the next.
-Note that it would be a good idea to see if we can introduce shading for modified memory
data as well as register data. This should be implemented on the memory buffers
themselves.
-Add the ability to break on read/write to an internal CPU register, eg, break when D0 is
modified.
-Track the origin of data register contents. Where a data register value is used as a
displacement from PC for a branching instruction, eg, bra/bsr/jmp/jsr, and we can detect
that the contents of that register were simply loaded from an address and not modified,
tag the memory location the offset was read from as an offset from the base address used
in the branch instruction.
-Where any branching instruction uses an address register as the source, keep a record of
all resolved target addresses that are jumped to from that branch, and to assist in
disassembly, record whether it is a call (branch), or a jump.
-Track all access to memory, recording whether it has been accessed as code or data. For
data references, record the size of the data access. During disassembly, format all data
using its indicated data size. Where two or more data references at consecutive memory
addresses share the same data access size, fold the data into an array.
-Rather than just folding into an array when two consecutive addresses have the same data
access size, implement advanced tracking of post-increment address register modes. Detect
when an address register is used in post-increment address mode to access consecutive data
values, without the address register being directly modified. Fold each consecutive
address accessed into an array. When the address register is modified, the array
definition is complete.
-Optionally link our disassembly window into the stored data on memory addresses,
indicating whether they are code or data, and use this advanced information to assist in
the disassembly process, and format the disassembly nicely. This will be the beginning of
building up to our emulator doing all the disassembly internally, without needing IDA Pro
at all.
-With the more powerful disassembler introduced, create an "active disassembly" mode,
where the disassembler will proceed down all available code paths, performing disassembly
of instructions which have not actually been executed. Record these addresses differently,
so that we can differentiate between observed and verified instructions actually executed
as code during runtime, and those merely believed to be valid code as a result of
traversing all visible code branches. The user should be able to see in the disassembly
which memory locations are known to be code and which are simply believed to be code.
-Note that most of the actual code for the above disassembly features should be
implemented in the base Processor class, with the M68000 core simply feeding data to the
Processor class during code execution. Active disassembly will simply require each device
deriving from the Processor class to provide additional information in the OpcodeInfo
class, in the form of a list of known possible PC values after executing that instruction.
In the case of an RTS opcode, this list might be empty, while in the case of non-branching
opcodes, it should simply be pc+opcodeSize. With this information provided to the
Processor class, active disassembly should be able to be implemented.

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
-Reportedly (from http://cgfm2.emuviews.com) the M68000 doesn't actually generate illegal
 instruction exceptions for all illegal instructions. Some of them may be aliases for
 valid instructions. Perform hardware tests to look for these undocumented instructions.

References:
-M68000 Programmer's Reference Manual (M68000PM), Motorola, Rev 1 1992
-M68000 Microprocessors User's Manual (M68000UM), Motorola, Ninth Edition 1993
-68000 Undocumented Behavior Notes, Bart Trzynadlowski, Fourth Edition 2003
-MC680x0 Reference (M68000 Programmer's Reference Manual HTML Transcription), Flint/DARKNESS, v1.1 1995
\*--------------------------------------------------------------------------------------*/
#ifndef __M68000_M68000_H__
#define __M68000_M68000_H__
#include "SystemInterface/SystemInterface.pkg"
#include "Processor/Processor.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "Data.h"
#include "ExecuteTime.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <list>
namespace M68000 {
class M68000Instruction;

class M68000 :public Processor
{
public:
	//Enumerations
	enum Exceptions;
	enum FunctionCode;
	enum State;

	//Constants
	static const int addressRegCount = 8;
	static const int dataRegCount = 8;
	static const unsigned int SP = 7;

	//Constructors
	M68000(const std::wstring& ainstanceName, unsigned int amoduleID);
	~M68000();
	virtual bool Construct(IHeirarchicalStorageNode& node);

	//Initialization functions
	virtual bool BuildDevice();
	virtual bool ValidateDevice();
	virtual void Initialize();
	virtual void Reset();

	//Reference functions
	virtual bool AddReference(const wchar_t* referenceName, IBusInterface* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Exception functions
	double PushStackFrame(const M68000Long& apc, const M68000Word& asr, bool processingInstruction = true);
	double PushStackFrame(const M68000Long& apc, const M68000Word& asr, const M68000Word& ainstructionRegister, const M68000Long& aaccessAddress, bool aread, bool aprocessingInstruction, FunctionCode afunctionCode);
	ExecuteTime ProcessException(unsigned int vector);
	ExecuteTime GetExceptionProcessingTime(unsigned int vector) const;
	std::wstring GetExceptionName(unsigned int vector) const;
	bool ExceptionDisabled(unsigned int vector);
	void ExceptionLogIfRequested(unsigned int vector);
	void ExceptionBreakIfRequested(unsigned int vector);
	void TriggerExceptionFromDebugger(unsigned int vector);

	//Suspend functions
	virtual bool UsesExecuteSuspend() const;

	//Execute functions
	virtual double ExecuteStep();
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual void NotifyAfterExecuteStepFinishedTimeslice();
	virtual OpcodeInfo GetOpcodeInfo(unsigned int location);
	virtual Data GetRawData(unsigned int location);
	virtual unsigned int GetCurrentPC() const;
	virtual unsigned int GetPCWidth() const;
	virtual unsigned int GetAddressBusWidth() const;
	virtual unsigned int GetDataBusWidth() const;
	virtual unsigned int GetMinimumOpcodeByteSize() const;

	//Line functions
	virtual unsigned int GetLineID(const wchar_t* lineName) const;
	virtual const wchar_t* GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual bool AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;
	void ApplyLineStateChange(unsigned int targetLine, const Data& lineData, boost::mutex::scoped_lock& lock);

	//Clock source functions
	virtual unsigned int GetClockSourceID(const wchar_t* clockSourceName) const;
	virtual const wchar_t* GetClockSourceName(unsigned int clockSourceID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate);
	void ApplyClockStateChange(unsigned int targetClock, double clockRate);

	//Disassembly functions
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
	FunctionCode GetFunctionCode(bool programReference);
	double ReadMemory(const M68000Long& location, Data& data, FunctionCode code, bool transparent, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;
	double ReadMemory(const M68000Long& location, Data& data, FunctionCode code, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;
	void ReadMemoryTransparent(const M68000Long& location, Data& data, FunctionCode code, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;
	double WriteMemory(const M68000Long& location, const Data& data, FunctionCode code, bool transparent, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;
	double WriteMemory(const M68000Long& location, const Data& data, FunctionCode code, const M68000Long& currentPC, bool processingInstruction, const M68000Word& instructionRegister, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;
	void WriteMemoryTransparent(const M68000Long& location, const Data& data, FunctionCode code, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;

	//CE line state functions
	virtual unsigned int GetCELineID(const wchar_t* lineName, bool inputLine) const;
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const;

	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHeirarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHeirarchicalStorageNode& node) const;

	//Window functions
	virtual void AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher);

private:
	//Enumerations
	enum CELineID;
	enum LineID;
	enum ClockID;

	//Structures
	struct ExceptionDebuggingEntry;
	struct LineAccess;
	struct CalculateCELineStateContext;
	struct RegisterDisassemblyInfo
	{
		RegisterDisassemblyInfo()
		:unmodified(false)
		{}

		bool unmodified;
		unsigned int sourceLocation;
		unsigned int dataSize;
	};

	//View and menu classes
	class DebugMenuHandler;
	class RegistersViewModel;
	class ExceptionsViewModel;
	class RegistersView;
	class ExceptionsView;
	friend class RegistersViewModel;
	friend class ExceptionsViewModel;
	friend class RegistersView;
	friend class ExceptionsView;

private:
	//Menu handling
	DebugMenuHandler* menuHandler;

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
	mutable unsigned int group0Vector;
	unsigned int bgroup0Vector;
	mutable FunctionCode group0FunctionCode;
	FunctionCode bgroup0FunctionCode;

	//Exception debugging
	mutable boost::mutex debugMutex;
	typedef std::list<ExceptionDebuggingEntry*> ExceptionList;
	ExceptionList exceptionList;
	volatile bool exceptionListEmpty;
	volatile bool logAllExceptions;
	volatile bool breakOnAllExceptions;
	volatile bool disableAllExceptions;
	volatile bool debugExceptionTriggerPending;
	volatile unsigned int debugExceptionTriggerVector;

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
	boost::mutex lineMutex;
	double lastLineCheckTime;
	volatile bool lineAccessPending;
	double lastTimesliceLength;
	double blastTimesliceLength;
	std::list<LineAccess> lineAccessBuffer;
	std::list<LineAccess> blineAccessBuffer;
	bool suspendWhenBusReleased;
	bool suspendUntilLineStateChangeReceived;
	bool bsuspendUntilLineStateChangeReceived;
	boost::condition lineStateChangeReceived;
	bool manualDeviceAdvanceInProgress;
	volatile bool executionReachedEndOfTimeslice;
	boost::condition advanceToTargetLineStateChanged;
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
