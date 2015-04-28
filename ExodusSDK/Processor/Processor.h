/*--------------------------------------------------------------------------------------*\
Things to do:
-Revamp the disassembly dialog so that it only disassembles the opcodes that are within
the window, and make it actively disassemble them on each refresh, so self-modifying code
will have the disassembly updates applied immediately.
-Our revamped disassembly window should show the actual data bytes which form the opcode
as an extra column.
-Note that one potential problem a disassembly window would have if we are to confine the
disassembly to the area inside the window, is that the base address from which we start
the disassembly from affects all the opcodes that follow it. Simply beginning the
disassembly from the current location of the window would often give us a bad opcode as
the first entry, as the latter part of a multi-word instruction is disassembled. This
effect could cascade onto following opcodes. The base address of the disassembly is
important. I suggest establishing a "safety buffer" before the actual contents of the
disassembly window, where instructions are disassembled, but not displayed. It may be
best to make this safety buffer configurable by the user, to increase the size, or turn
it off altogether if desired.
-Add support for adding and removing breakpoints within the disassembly window by
clicking on an area of the line where the breakpoint should be toggled. Suggest also
adding a ctrl+click feature where an existing breakpoint can be enabled or disabled when
clicked.
-Currently, the M68000 and Z80 disassembly windows display relative jump offsets as the
actual data which is stored in the opcode, even if that data is calculated based on an
incremented PC partway through the opcode. The actual assembly would represent such
relative jumps from the base address of the opcode. We should probably do the same. This
would require an enhancement to the EffectiveAddress class to store the base address of
the opcode, in addition to the base address of the extension word.
-Implement the "Enable All" and "Disable All" options for breakpoint/watchpoint lists
-Implement the "Save" and "Load" options for breakpoint/watchpoint lists

-Implement support for Python scripting, to extend the functionality of our active
disassembly process. In particular, we want custom scripts to be able to be written in
order to extend, format, tweak, etc the disassembly process. For example, we might have a
script for the Mega Drive to format the ROM header and disassemble the target of all the
vector table entries. We also definitely want a script to try and guess and disassemble
each entry in jump tables, but since this kind of analysis is highly subjective, and may
easily be improved or tweaked for particular cases, we don't want to hard-code it in our
emulator. This scripting support is critical in order to make disassembly most useful for
advanced users.

-Expand our plans for an active IDA disassembly link. Suggestions for data analysis:
#Each identified "block" of data should be flagged for where it was written to. Eg, if
a block of data was repeatedly read and and all of it was written to 0xC00000, that says
a lot. We should mark the data block as such.
#Data blocks of course need to be set to the size they were read as. Single continuous
blocks must always be read in the same format to be treated as such. These blocks will be
added to an array contruct.
#IDA scripts can read files. The output of our emulator should be a simple file which our
IDA script is able to read and interpret. Our script will be a basic algorithm to read
the target addresses and operations specified in the output file, and apply them as
appropriate.
#Some of the formatting we need to apply:
-Disassemble as code. This must be the first pass. All code must be disassembled before
any further processing.
-Format individual data value to size
-Optional build data value into array
-Optional build array into offset from specified base
-Every element must allow comments and labels to be specified
#In order to determine what is a continuous, discrete data block, I suggest we'll need
some very sophisticated tracking in place. It seems to me, on the M68000, the logical way
to identify individual data blocks is to monitor the address registers, and look for data
which is accessed via a post-incremented addressing mode. Even when the target data is
being "decoded" as part of a compression algorithm or the like, where there may be
multiple points within the decompression algorithm at which the data is read, the address
register which is tracking the data source will remain constant. In this scenario, we
just have to monitor the address registers. Data blocks will be identified based on the
usage of the address registers. Any time the register is manually written to, the current
data block will be considered to be complete. Note that reading the register must not
interrupt the data block analysis, as an algorithm may be testing the address register,
and terminate when it reaches a certain value.
#There is a more complex data analysis task which will be required in order to get good
results for many Mega Drive games, and that relates to cases where decompression is saved
partway through, and resumed later. This is the case for the Nemesis compression format
for example. This scenario would require much more complex tracking methods, where a data
block isn't closed off until the last copy of a reference to that block has been
discarded. In order to resume the decompression task, the source address register must be
saved to RAM, either as an individual write or as part of a MOVEM instruction. In order
for the entire archive to be correctly identified as a single data block, we would need
a way to realize that when that value is loaded back from RAM and the decompression
process resumes, we are continuing to parse the same data block we started earlier. We
also must ensure that the address value has not been modified in any way.
#To tackle the case of the Nemesis decompression algorithm, I suggest a system where we
track the location of all address values which have already initiated a data block
analysis, when they are copied from a register into memory. We must then track those
locations in memory. If any attempt is made to modify the saved register data, the
association must be broken. I also suggest a saved register should only be considered to
be resuming a previously established data block if it is loaded back directly into the
same register slot, and of course, if it is continued to be post-incremented.
#In order to prevent things like checksum routines throwing off our data analysis, we
will consider any data block identification to be unreliable if at any time during that
data block read, a known code location is read as data. This will result in the entire
block analysis being flagged as unreliable. Unreliable data block information will not be
used in our IDA script.

-Implement a link between the disassembly window and the breakpoint window, so
breakpoints can be toggled from the disassembly view.
-Consider a link between the code coverage data and the disassembly window, where the
disassembly is optionally linked to the code coverage data, and ensures opcodes which are
actually executed are disassembled correctly.
\*--------------------------------------------------------------------------------------*/
#include "IProcessor.h"
#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "Device/Device.pkg"
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include "Breakpoint.h"
#include "Watchpoint.h"
#include "ThinContainers/ThinContainers.pkg"
#include <mutex>
#include <condition_variable>

class Processor :public Device, public GenericAccessBase<IProcessor>
{
public:
	//Enumerations
	enum class DisassemblyDataType;

	//Structures
	struct LabelSubstitutionSettings;
	struct LabelSubstitutionEntry;
	struct LabelEntry;

public:
	//Constructors
	Processor(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~Processor();

	//Interface version functions
	virtual unsigned int GetIProcessorVersion() const;

	//Device access functions
	virtual IDevice* GetDevice();

	//Initialization functions
	virtual bool Construct(IHierarchicalStorageNode& node);
	virtual bool BuildDevice();

	//Execute functions
	virtual double ExecuteStep() = 0;
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();
	virtual UpdateMethod GetUpdateMethod() const;
	virtual void BreakOnCurrentOpcode() const;
	virtual void BreakOnStepOverCurrentOpcode();
	virtual void BreakOnStepOutCurrentOpcode();

	//Control functions
	virtual double GetClockSpeed() const;
	virtual void SetClockSpeed(double aclockSpeed);
	virtual void OverrideClockSpeed(double aclockSpeed);
	virtual void RestoreClockSpeed();
	inline double CalculateExecutionTime(unsigned int cycles) const;

	//Instruction functions
	//##TODO## Make these following virtual functions inline sealed when we have C++11
	//support, to improve performance.
	virtual unsigned int GetByteCharWidth() const;
	virtual unsigned int GetPCCharWidth() const;
	virtual unsigned int GetAddressBusCharWidth() const;
	virtual unsigned int GetDataBusCharWidth() const;
	virtual unsigned int GetPCMask() const;
	virtual unsigned int GetAddressBusMask() const;
	virtual unsigned int GetDataBusMask() const;
	virtual unsigned int GetMemorySpaceByte(unsigned int location) const;
	virtual void SetMemorySpaceByte(unsigned int location, unsigned int data);
	virtual bool GetOpcodeInfo(unsigned int location, IOpcodeInfo& opcodeInfo) const;

	//Breakpoint functions
	virtual MarshalSupport::Marshal::Ret<std::list<IBreakpoint*>> GetBreakpointList() const;
	virtual IBreakpoint* CreateBreakpoint();
	virtual bool LockBreakpoint(IBreakpoint* breakpoint) const;
	virtual void UnlockBreakpoint(IBreakpoint* breakpoint) const;
	virtual void DeleteBreakpoint(IBreakpoint* breakpoint);
	//##FIX## Make all these functions non-const once we fix our const issues surrounding
	//disassembly for processors
	inline void CheckExecution(unsigned int location) const;

	//Watchpoint functions
	virtual MarshalSupport::Marshal::Ret<std::list<IWatchpoint*>> GetWatchpointList() const;
	virtual IWatchpoint* CreateWatchpoint();
	virtual bool LockWatchpoint(IWatchpoint* watchpoint) const;
	virtual void UnlockWatchpoint(IWatchpoint* watchpoint) const;
	virtual void DeleteWatchpoint(IWatchpoint* watchpoint);
	//##FIX## Make all these functions non-const once we fix our const issues surrounding
	//disassembly for processors
	inline void CheckMemoryRead(unsigned int location, unsigned int data) const;
	inline void CheckMemoryWrite(unsigned int location, unsigned int data) const;

	//Call stack functions
	virtual bool GetCallStackDisassemble() const;
	virtual void SetCallStackDisassemble(bool state);
	virtual MarshalSupport::Marshal::Ret<std::list<CallStackEntry>> GetCallStack() const;
	virtual unsigned int GetCallStackLastModifiedToken() const;
	virtual void ClearCallStack();
	void PushCallStack(unsigned int sourceAddress, unsigned int targetAddress, unsigned int returnAddress, const std::wstring& entry, bool fixedDisassembly = false);
	void PopCallStack(unsigned int returnAddress);

	//Trace functions
	virtual bool GetTraceEnabled() const;
	virtual void SetTraceEnabled(bool state);
	virtual bool GetTraceDisassemble() const;
	virtual void SetTraceDisassemble(bool state);
	virtual unsigned int GetTraceLength() const;
	virtual void SetTraceLength(unsigned int state);
	virtual MarshalSupport::Marshal::Ret<std::list<TraceLogEntry>> GetTraceLog() const;
	virtual unsigned int GetTraceLogLastModifiedToken() const;
	virtual void ClearTraceLog();
	inline void RecordTrace(unsigned int pc);

	//Active disassembly info functions
	virtual bool ActiveDisassemblySupported() const;
	//##TODO## Make this following virtual function inline sealed when we have C++11
	//support, to improve performance.
	virtual bool ActiveDisassemblyEnabled() const;
	virtual unsigned int GetActiveDisassemblyStartLocation() const;
	virtual void SetActiveDisassemblyStartLocation(unsigned int state);
	virtual unsigned int GetActiveDisassemblyEndLocation() const;
	virtual void SetActiveDisassemblyEndLocation(unsigned int state);
	virtual unsigned int GetActiveDisassemblyAnalysisStartLocation() const;
	virtual void SetActiveDisassemblyAnalysisStartLocation(unsigned int state);
	virtual unsigned int GetActiveDisassemblyAnalysisEndLocation() const;
	virtual void SetActiveDisassemblyAnalysisEndLocation(unsigned int state);
	virtual bool GetActiveDisassemblyAnalyzeCode() const;
	virtual void SetActiveDisassemblyAnalyzeCode(bool state);
	virtual bool GetActiveDisassemblyAnalyzeData() const;
	virtual void SetActiveDisassemblyAnalyzeData(bool state);
	virtual bool GetActiveDisassemblyAnalyzeCodeOffsets() const;
	virtual void SetActiveDisassemblyAnalyzeCodeOffsets(bool state);
	virtual bool GetActiveDisassemblyAnalyzeDataOffsets() const;
	virtual void SetActiveDisassemblyAnalyzeDataOffsets(bool state);
	virtual bool GetActiveDisassemblyAnalyzePredictedArrays() const;
	virtual void SetActiveDisassemblyAnalyzePredictedArrays(bool state);
	virtual bool GetActiveDisassemblyAnalyzePredictedJumpTables() const;
	virtual void SetActiveDisassemblyAnalyzePredictedJumpTables(bool state);
	virtual bool GetActiveDisassemblyExploreCodePaths() const;
	virtual void SetActiveDisassemblyExploreCodePaths(bool state);
	virtual bool GetActiveDisassemblyPerformLabelSubstitution() const;
	virtual void SetActiveDisassemblyPerformLabelSubstitution(bool state);
	virtual bool GetActiveDisassemblyDetectOffsets() const;
	virtual void SetActiveDisassemblyDetectOffsets(bool state);
	virtual bool GetActiveDisassemblyDetectJumpTables() const;
	virtual void SetActiveDisassemblyDetectJumpTables(bool state);
	virtual bool GetActiveDisassemblyDetectData() const;
	virtual void SetActiveDisassemblyDetectData(bool state);
	virtual bool GetActiveDisassemblyDetectDataArrays() const;
	virtual void SetActiveDisassemblyDetectDataArrays(bool state);
	virtual bool GetActiveDisassemblyDetectTextData() const;
	virtual void SetActiveDisassemblyDetectTextData(bool state);
	virtual double GetActiveDisassemblyOffsetArrayIncreaseTolerance() const;
	virtual void SetActiveDisassemblyOffsetArrayIncreaseTolerance(double state);
	virtual unsigned int GetActiveDisassemblyMinimumArrayEntryCount() const;
	virtual void SetActiveDisassemblyMinimumArrayEntryCount(unsigned int state);
	virtual unsigned int GetActiveDisassemblyOffsetArrayDistanceTolerance() const;
	virtual void SetActiveDisassemblyOffsetArrayDistanceTolerance(unsigned int state);
	virtual unsigned int GetActiveDisassemblyJumpTableDistanceTolerance() const;
	virtual void SetActiveDisassemblyJumpTableDistanceTolerance(unsigned int state);
	virtual unsigned int GetActiveDisassemblyRecordedItemCount() const;
	virtual unsigned int GetActiveDisassemblyAnalysisCodeEntryCount() const;
	virtual unsigned int GetActiveDisassemblyAnalysisOffsetEntryCount() const;
	virtual unsigned int GetActiveDisassemblyAnalysisDataEntryCount() const;
	virtual unsigned int GetActiveDisassemblyAnalysisLabelEntryCount() const;
	virtual unsigned int GetActiveDisassemblyAnalysisPredictedCodeEntryCount() const;
	virtual unsigned int GetActiveDisassemblyAnalysisPredictedOffsetEntryCount() const;
	virtual unsigned int GetActiveDisassemblyAnalysisPredictedDataEntryCount() const;
	virtual unsigned int GetActiveDisassemblyAnalysisPredictedLabelEntryCount() const;

	//Active disassembly operation functions
	virtual void EnableActiveDisassembly();
	virtual void DisableActiveDisassembly();
	virtual void ClearActiveDisassembly();

	//Active disassembly logging functions
	unsigned int MakeDataArrayAtLocation(unsigned int location, unsigned int dataSize, DisassemblyDataType dataType);
	void AddDisassemblyAddressInfoCode(unsigned int location, unsigned int dataSize, const std::wstring& comment = L"");
	void AddDisassemblyAddressInfoData(unsigned int location, unsigned int dataSize, DisassemblyDataType dataType, unsigned int arrayID = 0, const std::wstring& comment = L"");
	void AddDisassemblyAddressInfoOffset(unsigned int location, unsigned int dataSize, bool offsetToCode, bool relativeOffset, unsigned int relativeOffsetBaseAddress);
	void AddDisassemblyPossibleBranchTable(unsigned int baseAddress, unsigned int confirmedEntry, unsigned int entrySize);

	//Active disassembly analysis functions
	virtual bool PerformActiveDisassemblyAnalysis();
	virtual void ClearActiveDisassemblyAnalysis();
	virtual bool ActiveDisassemblyExportAnalysisToASMFile(const MarshalSupport::Marshal::In<std::wstring>& filePath) const;
	virtual bool ActiveDisassemblyExportAnalysisToTextFile(const MarshalSupport::Marshal::In<std::wstring>& filePath) const;
	virtual bool ActiveDisassemblyExportAnalysisToIDCFile(const MarshalSupport::Marshal::In<std::wstring>& filePath) const;

	//Active disassembly formatting functions
	virtual bool GetLeadingLinesForASMFile(unsigned int analysisStartAddress, unsigned int analysisEndAddress, std::list<std::wstring>& outputLines) const;
	virtual bool GetTrailingLinesForASMFile(unsigned int analysisStartAddress, unsigned int analysisEndAddress, std::list<std::wstring>& outputLines) const;
	virtual bool FormatOpcodeForDisassembly(unsigned int opcodeAddress, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& opcodeArguments, std::wstring& opcodeComments) const;
	virtual bool FormatDataForDisassembly(const std::vector<Data>& dataElements, unsigned int dataElementByteSize, DisassemblyDataType dataType, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& formattedData) const;
	virtual bool FormatOffsetForDisassembly(const Data& offsetData, bool relativeOffset, unsigned int relativeOffsetBaseAddress, const LabelSubstitutionSettings& labelSettings, std::wstring& opcodePrefix, std::wstring& formattedOffset) const;
	virtual bool FormatCommentForDisassembly(const std::wstring& rawComment, std::wstring& formattedComment) const;
	virtual bool FormatLabelPlacementForDisassembly(const std::wstring& rawLabel, std::wstring& formattedLabel) const;
	virtual bool FormatLabelUsageForDisassembly(const std::wstring& rawLabel, int labelOffset, std::wstring& formattedLabel) const;

	//Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHierarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHierarchicalStorageNode& node) const;
	void LoadCallStack(IHierarchicalStorageNode& node);
	void SaveCallStack(IHierarchicalStorageNode& node) const;

	//Data read/write functions
	virtual bool ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const;
	virtual bool WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue);

	//Command execution functions
	virtual bool ExecuteGenericCommand(unsigned int commandID, const DataContext* dataContext);

private:
	//Enumerations
	enum class DisassemblyEntryType;

	//Structures
	struct BreakpointCallbackParams;
	struct WatchpointCallbackParams;
	struct DisassemblyAddressInfo;
	struct DisassemblyArrayInfo;
	struct DisassemblyJumpTableInfo;
	struct ActiveDisassemblyAnalysisData;

	//Typedefs
	typedef std::map<unsigned int, DisassemblyArrayInfo> DisassemblyArrayInfoMap;
	typedef std::pair<unsigned int, DisassemblyArrayInfo> DisassemblyArrayInfoMapEntry;
	typedef std::map<unsigned int, DisassemblyJumpTableInfo> DisassemblyJumpTableInfoMap;
	typedef std::pair<unsigned int, DisassemblyJumpTableInfo> DisassemblyJumpTableInfoMapEntry;

private:
	//Breakpoint functions
	void CheckExecutionInternal(unsigned int location) const;
	void TriggerBreakpoint(Breakpoint* breakpoint) const;
	static void BreakpointCallbackRaw(void* aparams);
	void BreakpointCallback(Breakpoint* breakpoint) const;

	//Watchpoint functions
	void CheckMemoryReadInternal(unsigned int location, unsigned int data) const;
	void CheckMemoryWriteInternal(unsigned int location, unsigned int data) const;
	void TriggerWatchpoint(Watchpoint* watchpoint) const;
	static void WatchpointCallbackRaw(void* aparams);
	void WatchpointCallback(Watchpoint* watchpoint) const;

	//Trace functions
	void RecordTraceInternal(unsigned int pc);

	//Active disassembly operation functions
	void EnableActiveDisassembly(unsigned int startLocation, unsigned int endLocation);
	void DisableActiveDisassemblyInternal();
	void ClearActiveDisassemblyInternal();

	//Active disassembly logging functions
	void AddDisassemblyAddressInfoEntryToArray(DisassemblyAddressInfo* newEntry);

	//Active disassembly analysis functions
	bool PerformActiveDisassemblyAnalysis(unsigned int minAddress, unsigned int maxAddress, ActiveDisassemblyAnalysisData& analysis) const;
	bool ActiveDisassemblyEntrySeemsToBePartOfArray(ActiveDisassemblyAnalysisData& analysis, const DisassemblyAddressInfo* currentEntry, unsigned int firstKnownEntryLocation, unsigned int lastKnownEntryLocation, unsigned int entrySize, DisassemblyEntryType offsetType, bool relativeOffset, unsigned int relativeOffsetBaseAddress) const;
	void ActiveDisassemblyGeneratePredictedOffsetArrayEntries(ActiveDisassemblyAnalysisData& analysis, std::map<unsigned int, const DisassemblyAddressInfo*> knownOffsetArrayEntries, unsigned int firstKnownEntryLocation, unsigned int lastKnownEntryLocation, unsigned int entrySize, DisassemblyEntryType offsetType, bool relativeOffset, unsigned int relativeOffsetBaseAddress) const;
	bool ActiveDisassemblySuspectedJumpTableSeemsToBeValid(ActiveDisassemblyAnalysisData& analysis, const DisassemblyJumpTableInfo& jumpTableInfo) const;
	void ActiveDisassemblyGeneratePredictedJumpTableEntries(ActiveDisassemblyAnalysisData& analysis, const DisassemblyJumpTableInfo& jumpTableInfo) const;
	void ActiveDisassemblyGenerateLabelsForOffset(ActiveDisassemblyAnalysisData& analysis, const DisassemblyAddressInfo* entry, bool predicted) const;
	void ActiveDisassemblyAddLabelForTarget(ActiveDisassemblyAnalysisData& analysis, unsigned int targetAddress, bool predicted) const;
	static bool ActiveDisassemblyArraySeemsToBeCharArray(DisassemblyDataType dataType, unsigned int dataElementByteSize, const std::list<Data>& dataElements);
	static bool ActiveDisassemblyDecodeIDADataString(unsigned int byteSize, DisassemblyDataType dataType, std::wstring& outputString);
	static bool ActiveDisassemblyDecodeIDAOffsetString(unsigned int byteSize, std::wstring& outputString);
	static std::wstring ActiveDisassemblyGenerateCommentForDataArrayLine(unsigned int dataEntryCountAlreadyWritten, unsigned int arrayStartLocation, unsigned int arrayEndLocation, bool unknownData, unsigned int addressCharWidth);
	static std::wstring ActiveDisassemblyGenerateTextLabelForDataType(DisassemblyDataType dataType);
	bool ActiveDisassemblyExportAnalysisToASMFile(const ActiveDisassemblyAnalysisData& analysis, const std::wstring& filePath) const;
	bool ActiveDisassemblyWriteDataArrayToASMFile(Stream::ViewText& asmFileView, const std::list<Data>& dataElements, unsigned int arrayStartLocation, unsigned int dataElementByteSize, DisassemblyDataType dataType, bool unknownData, const LabelSubstitutionSettings& labelSettings, const std::wstring& filePath) const;
	bool ActiveDisassemblyExportAnalysisToTextFile(const ActiveDisassemblyAnalysisData& analysis, const std::wstring& filePath) const;
	bool ActiveDisassemblyExportAnalysisToIDCFile(const ActiveDisassemblyAnalysisData& analysis, const std::wstring& filePath) const;

private:
	//Clock speed
	double clockSpeed;
	double bclockSpeed;
	bool clockSpeedOverridden;
	double reportedClockSpeed;

	//Breakpoints
	//##FIX## Make these non-mutable once we fix our const issues
	mutable std::vector<Breakpoint*> breakpoints;
	mutable std::vector<Watchpoint*> watchpoints;
	mutable std::set<IBreakpoint*> lockedBreakpoints;
	mutable std::set<IWatchpoint*> lockedWatchpoints;
	mutable std::condition_variable breakpointLockReleased;
	mutable std::condition_variable watchpointLockReleased;
	volatile bool breakpointExists;
	volatile bool watchpointExists;

	//Call stack
	volatile mutable bool breakOnNextOpcode;
	bool bbreakOnNextOpcode;
	std::list<CallStackEntry> callStack;
	std::list<CallStackEntry> bcallStack;
	mutable bool stepOver;
	mutable bool bstepOver;
	bool stepOut;
	bool bstepOut;
	int stackLevel;
	int bstackLevel;
	bool stackDisassemble;
	unsigned int callStackLastModifiedToken;

	//Trace
	std::list<TraceLogEntry> traceLog;
	std::list<TraceLogEntry> btraceLog;
	volatile bool traceLogEnabled;
	bool traceLogDisassemble;
	unsigned int traceLogLength;
	unsigned int traceLogLastModifiedToken;

	//Active disassembly
	bool activeDisassemblyEnabled;
	unsigned int activeDisassemblyArrayNextFreeID;
	unsigned int activeDisassemblyStartLocation;
	unsigned int activeDisassemblyEndLocation;
	unsigned int activeDisassemblyUncommittedStartLocation;
	unsigned int activeDisassemblyUncommittedEndLocation;
	std::vector<std::list<DisassemblyAddressInfo*>> activeDisassemblyAddressInfo;
	std::set<DisassemblyAddressInfo*> activeDisassemblyAddressInfoSet;
	DisassemblyArrayInfoMap activeDisassemblyArrayInfo;
	DisassemblyJumpTableInfoMap activeDisassemblyJumpTableInfo;
	unsigned int activeDisassemblyAnalysisStartLocation;
	unsigned int activeDisassemblyAnalysisEndLocation;
	bool activeDisassemblyAnalyzeCode;
	bool activeDisassemblyAnalyzeData;
	bool activeDisassemblyAnalyzeCodeOffsets;
	bool activeDisassemblyAnalyzeDataOffsets;
	bool activeDisassemblyAnalyzePredictedArrays;
	bool activeDisassemblyAnalyzePredictedJumpTables;
	bool activeDisassemblyExploreCodePaths;
	bool activeDisassemblyPerformLabelSubstitution;
	bool activeDisassemblyDetectOffsets;
	bool activeDisassemblyDetectJumpTables;
	bool activeDisassemblyDetectData;
	bool activeDisassemblyDetectDataArrays;
	bool activeDisassemblyDetectTextData;
	double activeDisassemblyOffsetArrayIncreaseTolerance;
	unsigned int activeDisassemblyMinimumArrayEntryCount;
	unsigned int activeDisassemblyOffsetArrayDistanceTolerance;
	unsigned int activeDisassemblyJumpTableDistanceTolerance;
	ActiveDisassemblyAnalysisData* activeDisassemblyAnalysis;

	//Generic access page groups
	GenericAccessGroupCollectionEntry* breakpointCollection;

	//Debug
	mutable std::mutex debugMutex;
};

#include "Processor.inl"
#endif
