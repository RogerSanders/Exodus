#ifndef __IPROCESSOR_H__
#define __IPROCESSOR_H__
#include "GenericAccess/GenericAccess.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include "IBreakpoint.h"
#include "IWatchpoint.h"
#include "IOpcodeInfo.h"
#include <string>
#include <list>

class IProcessor :public virtual IGenericAccess
{
public:
	//Enumerations
	enum class IProcessorDataSource;
	enum class IProcessorCommand;

	//Structures
	struct CallStackEntry;
	struct TraceLogEntry;
	struct BreakpointDataContext;

public:
	//Constructors
	virtual ~IProcessor() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIProcessorVersion() { return 1; }
	virtual unsigned int GetIProcessorVersion() const = 0;

	//Device access functions
	virtual IDevice* GetDevice() = 0;

	//Execute functions
	virtual void Reset() = 0;
	virtual void BreakOnCurrentOpcode() const = 0;
	virtual void BreakOnStepOverCurrentOpcode() = 0;
	virtual void BreakOnStepOutCurrentOpcode() = 0;

	//Control functions
	virtual double GetClockSpeed() const = 0;
	virtual void SetClockSpeed(double aclockSpeed) = 0;
	virtual void OverrideClockSpeed(double aclockSpeed) = 0;
	virtual void RestoreClockSpeed() = 0;

	//Instruction functions
	virtual unsigned int GetByteBitCount() const = 0;
	virtual unsigned int GetCurrentPC() const = 0;
	virtual unsigned int GetPCWidth() const = 0;
	virtual unsigned int GetAddressBusWidth() const = 0;
	virtual unsigned int GetDataBusWidth() const = 0;
	virtual unsigned int GetMinimumOpcodeByteSize() const = 0;
	virtual unsigned int GetMinimumDataByteSize() const = 0;
	virtual unsigned int GetByteCharWidth() const = 0;
	virtual unsigned int GetPCCharWidth() const = 0;
	virtual unsigned int GetAddressBusCharWidth() const = 0;
	virtual unsigned int GetDataBusCharWidth() const = 0;
	virtual unsigned int GetPCMask() const = 0;
	virtual unsigned int GetAddressBusMask() const = 0;
	virtual unsigned int GetDataBusMask() const = 0;
	virtual unsigned int GetMemorySpaceByte(unsigned int location) const = 0;
	virtual void SetMemorySpaceByte(unsigned int location, unsigned int data) = 0;
	virtual bool GetOpcodeInfo(unsigned int location, IOpcodeInfo& opcodeInfo) const = 0;

	//Breakpoint functions
	virtual MarshalSupport::Marshal::Ret<std::list<IBreakpoint*>> GetBreakpointList() const = 0;
	virtual IBreakpoint* CreateBreakpoint() = 0;
	virtual bool LockBreakpoint(IBreakpoint* breakpoint) const = 0;
	virtual void UnlockBreakpoint(IBreakpoint* breakpoint) const = 0;
	virtual void DeleteBreakpoint(IBreakpoint* breakpoint) = 0;

	//Watchpoint functions
	virtual MarshalSupport::Marshal::Ret<std::list<IWatchpoint*>> GetWatchpointList() const = 0;
	virtual IWatchpoint* CreateWatchpoint() = 0;
	virtual bool LockWatchpoint(IWatchpoint* watchpoint) const = 0;
	virtual void UnlockWatchpoint(IWatchpoint* watchpoint) const = 0;
	virtual void DeleteWatchpoint(IWatchpoint* watchpoint) = 0;

	//Call stack functions
	virtual bool GetCallStackDisassemble() const = 0;
	virtual void SetCallStackDisassemble(bool state) = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<CallStackEntry>> GetCallStack() const = 0;
	virtual unsigned int GetCallStackLastModifiedToken() const = 0;
	virtual void ClearCallStack() = 0;

	//Trace functions
	virtual bool GetTraceEnabled() const = 0;
	virtual void SetTraceEnabled(bool state) = 0;
	virtual bool GetTraceDisassemble() const = 0;
	virtual void SetTraceDisassemble(bool state) = 0;
	virtual unsigned int GetTraceLength() const = 0;
	virtual void SetTraceLength(unsigned int state) = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<TraceLogEntry>> GetTraceLog() const = 0;
	virtual unsigned int GetTraceLogLastModifiedToken() const = 0;
	virtual void ClearTraceLog() = 0;

	//Active disassembly info functions
	//##TODO## Strongly consider shifting all active disassembly properties into separate
	//object types, one for active disassembly, and another for active disassembly
	//analysis, where a preferences object is supplied when active disassembly is enabled
	//or an analysis is performed. This would allow active disassembly to evolve and
	//change independently from the IProcessor interface, with versioning happening on
	//these preference objects for active disassembly settings.
	virtual bool ActiveDisassemblySupported() const = 0;
	virtual bool ActiveDisassemblyEnabled() const = 0;
	virtual unsigned int GetActiveDisassemblyStartLocation() const = 0;
	virtual void SetActiveDisassemblyStartLocation(unsigned int state) = 0;
	virtual unsigned int GetActiveDisassemblyEndLocation() const = 0;
	virtual void SetActiveDisassemblyEndLocation(unsigned int state) = 0;
	virtual unsigned int GetActiveDisassemblyAnalysisStartLocation() const = 0;
	virtual void SetActiveDisassemblyAnalysisStartLocation(unsigned int state) = 0;
	virtual unsigned int GetActiveDisassemblyAnalysisEndLocation() const = 0;
	virtual void SetActiveDisassemblyAnalysisEndLocation(unsigned int state) = 0;
	virtual bool GetActiveDisassemblyAnalyzeCode() const = 0;
	virtual void SetActiveDisassemblyAnalyzeCode(bool state) = 0;
	virtual bool GetActiveDisassemblyAnalyzeData() const = 0;
	virtual void SetActiveDisassemblyAnalyzeData(bool state) = 0;
	virtual bool GetActiveDisassemblyAnalyzeCodeOffsets() const = 0;
	virtual void SetActiveDisassemblyAnalyzeCodeOffsets(bool state) = 0;
	virtual bool GetActiveDisassemblyAnalyzeDataOffsets() const = 0;
	virtual void SetActiveDisassemblyAnalyzeDataOffsets(bool state) = 0;
	virtual bool GetActiveDisassemblyAnalyzePredictedArrays() const = 0;
	virtual void SetActiveDisassemblyAnalyzePredictedArrays(bool state) = 0;
	virtual bool GetActiveDisassemblyAnalyzePredictedJumpTables() const = 0;
	virtual void SetActiveDisassemblyAnalyzePredictedJumpTables(bool state) = 0;
	virtual bool GetActiveDisassemblyExploreCodePaths() const = 0;
	virtual void SetActiveDisassemblyExploreCodePaths(bool state) = 0;
	virtual bool GetActiveDisassemblyPerformLabelSubstitution() const = 0;
	virtual void SetActiveDisassemblyPerformLabelSubstitution(bool state) = 0;
	virtual bool GetActiveDisassemblyDetectOffsets() const = 0;
	virtual void SetActiveDisassemblyDetectOffsets(bool state) = 0;
	virtual bool GetActiveDisassemblyDetectJumpTables() const = 0;
	virtual void SetActiveDisassemblyDetectJumpTables(bool state) = 0;
	virtual bool GetActiveDisassemblyDetectData() const = 0;
	virtual void SetActiveDisassemblyDetectData(bool state) = 0;
	virtual bool GetActiveDisassemblyDetectDataArrays() const = 0;
	virtual void SetActiveDisassemblyDetectDataArrays(bool state) = 0;
	virtual bool GetActiveDisassemblyDetectTextData() const = 0;
	virtual void SetActiveDisassemblyDetectTextData(bool state) = 0;
	virtual double GetActiveDisassemblyOffsetArrayIncreaseTolerance() const = 0;
	virtual void SetActiveDisassemblyOffsetArrayIncreaseTolerance(double state) = 0;
	virtual unsigned int GetActiveDisassemblyMinimumArrayEntryCount() const = 0;
	virtual void SetActiveDisassemblyMinimumArrayEntryCount(unsigned int state) = 0;
	virtual unsigned int GetActiveDisassemblyOffsetArrayDistanceTolerance() const = 0;
	virtual void SetActiveDisassemblyOffsetArrayDistanceTolerance(unsigned int state) = 0;
	virtual unsigned int GetActiveDisassemblyJumpTableDistanceTolerance() const = 0;
	virtual void SetActiveDisassemblyJumpTableDistanceTolerance(unsigned int state) = 0;
	virtual unsigned int GetActiveDisassemblyRecordedItemCount() const = 0;
	virtual unsigned int GetActiveDisassemblyAnalysisCodeEntryCount() const = 0;
	virtual unsigned int GetActiveDisassemblyAnalysisOffsetEntryCount() const = 0;
	virtual unsigned int GetActiveDisassemblyAnalysisDataEntryCount() const = 0;
	virtual unsigned int GetActiveDisassemblyAnalysisLabelEntryCount() const = 0;
	virtual unsigned int GetActiveDisassemblyAnalysisPredictedCodeEntryCount() const = 0;
	virtual unsigned int GetActiveDisassemblyAnalysisPredictedOffsetEntryCount() const = 0;
	virtual unsigned int GetActiveDisassemblyAnalysisPredictedDataEntryCount() const = 0;
	virtual unsigned int GetActiveDisassemblyAnalysisPredictedLabelEntryCount() const = 0;

	//Active disassembly operation functions
	virtual void EnableActiveDisassembly() = 0;
	virtual void DisableActiveDisassembly() = 0;
	virtual void ClearActiveDisassembly() = 0;

	//Active disassembly analysis functions
	virtual bool PerformActiveDisassemblyAnalysis() = 0;
	virtual void ClearActiveDisassemblyAnalysis() = 0;
	virtual bool ActiveDisassemblyExportAnalysisToASMFile(const MarshalSupport::Marshal::In<std::wstring>& filePath) const = 0;
	virtual bool ActiveDisassemblyExportAnalysisToTextFile(const MarshalSupport::Marshal::In<std::wstring>& filePath) const = 0;
	virtual bool ActiveDisassemblyExportAnalysisToIDCFile(const MarshalSupport::Marshal::In<std::wstring>& filePath) const = 0;
};

#include "IProcessor.inl"
#endif
