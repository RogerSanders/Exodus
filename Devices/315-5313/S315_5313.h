/*----------------------------------------------------------------------------                              ----------*\
Description:
This core emulates the Sega video display processor model 315-5313, first used in the
Sega Mega Drive/Genesis console. This processor is an evolution of the VDP used in the
Sega Master System, which is in turn an evolution of the TMS9918 developed by Texas
Instruments.

Known Inaccuracies:
-Bit 2 of reg 1, when cleared should enable Mode 4. This mode is not currently supported.
-Vertical scroll inhibit is not currently supported
-Left column blank is not currently supported
-128Kb VRAM mode is not currently supported
-The test register is not currently supported
-The sprite collision and overflow flags
-Changes to the horizontal screen mode are latched at hblank, while it is believed the
real hardware allows changes to this register to take effect immediately, with corruption
of the current line often resulting.
-The sprite overflow and collision flags are not currently implemented, and always return
false.

Things to do:
-Support "invalid" sync outputs from the VDP. There are a few specific cases we want to
handle:
1. Mid-line changes to H32/H40 setting (line lengthened or cut short)
2. Internal fixed MCLK/4 H40 serial clock (sync too fast)
3. V30 mode for NTSC (no VSYNC)
All these screen settings cause a bad analog video output. We should detect them and
visually warn the user in some way that the analog output is bad. That said, we should
still display a valid image, as if the video equipment was manually synced to match the
video output.

Debug output changes:
-Modify the palette window to include palette row and column labels, as well as a popup
style window in the same style as the VRAM viewer, which shows the actual RGB palette
values that are being used to create it.
-Provide options to force all tiles in each layer to either high or low priority. This
can be used to bring hidden sprites into view for example. Also add an option to "mark"
sprites in some way, so that even transparent sprites are visible, and the boundaries of
all sprites on the screen are indicated clearly.
-Provide options to control which area of the screen to make visible. Have an option to
show all overscan regions to make each generated pixel visible.

References:
-Genesis Software Manual, Sega Enterprises, 1992, scans by antime
-Sega Genesis VDP Documentation (genvdp.txt), Charles MacDonald, v1.5f 10/8/2000
-Sega Master System VDP Documentation (msvdp.txt), Charles MacDonald, 12/11/2002
-315-5313 Information (vdppin.txt), Charles MacDonald, 2008
-TMS9918A/TMS9928A/TMS9929A Video Display Processors, Texas Instruments, 1982
-http://gendev.spritesmind.net/forum/viewtopic.php?t=666
-http://mamedev.emulab.it/haze/2006/08/09/mirror-mirror/
\*--------------------------------------------------------------------------------------------------------------------*/
#include "IS315_5313.h"
#ifndef __S315_5313_H__
#define __S315_5313_H__
#include "Device/Device.pkg"
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "TimedBuffers/TimedBuffers.pkg"
#include <vector>
#include <list>
#include <map>
#include <mutex>
#include <condition_variable>

class S315_5313 :public Device, public GenericAccessBase<IS315_5313>
{
public:
	// Constructors
	S315_5313(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);

	// Interface version functions
	virtual unsigned int GetIS315_5313Version() const;

	// Device access functions
	virtual IDevice* GetDevice();

	// Line functions
	virtual unsigned int GetLineID(const Marshal::In<std::wstring>& lineName) const;
	virtual Marshal::Ret<std::wstring> GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual bool AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	// Clock source functions
	virtual unsigned int GetClockSourceID(const Marshal::In<std::wstring>& clockSourceName) const;
	virtual Marshal::Ret<std::wstring> GetClockSourceName(unsigned int clockSourceID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate);

	// Initialization functions
	virtual bool BuildDevice();
	virtual bool ValidateDevice();
	virtual void Initialize();
	void Reset(double accessTime);
	virtual void BeginExecution();
	virtual void SuspendExecution();

	// Reference functions
	virtual bool AddReference(const Marshal::In<std::wstring>& referenceName, IDevice* target);
	virtual bool AddReference(const Marshal::In<std::wstring>& referenceName, IBusInterface* target);
	virtual bool AddReference(const Marshal::In<std::wstring>& referenceName, IClockSource* target);
	virtual void RemoveReference(IDevice* target);
	virtual void RemoveReference(IBusInterface* target);
	virtual void RemoveReference(IClockSource* target);

	// Suspend functions
	virtual bool UsesTransientExecution() const;

	// Execute functions
	virtual UpdateMethod GetUpdateMethod() const;
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual bool SendNotifyAfterExecuteCalled() const;
	virtual void NotifyAfterExecuteCalled();
	virtual void ExecuteTimeslice(double nanoseconds);
	virtual void ExecuteTimesliceTimingPointStep(unsigned int accessContext);
	virtual double GetNextTimingPointInDeviceTime(unsigned int& accessContext) const;
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	// CE line state functions
	virtual unsigned int GetCELineID(const Marshal::In<std::wstring>& lineName, bool inputLine) const;
	virtual void SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const;
	template<bool TransparentAccess, bool VdpIsSource>
	inline unsigned int BuildCELine(unsigned int targetAddress, bool currentLowerDataStrobe, bool currentUpperDataStrobe, bool operationIsWrite, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;

	// Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);

	// Savestate functions
	virtual bool GetScreenshot(IImage& targetImage) const;
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;
	virtual void LoadSettingsState(IHierarchicalStorageNode& node);
	virtual void SaveSettingsState(IHierarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHierarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHierarchicalStorageNode& node) const;

	// Data read/write functions
	using IGenericAccess::ReadGenericData;
	using IGenericAccess::WriteGenericData;
	virtual bool ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const;
	virtual bool WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue);

	// Data locking functions
	virtual bool GetGenericDataLocked(unsigned int dataID, const DataContext* dataContext) const;
	virtual bool SetGenericDataLocked(unsigned int dataID, const DataContext* dataContext, bool state);

private:
	// Enumerations
	enum class CELineID;
	enum class LineID;
	enum class ClockID;
	enum LayerIndex :unsigned int;
	enum class AccessContext;

	// Structures
	struct HScanSettings;
	struct VScanSettings;
	struct TimesliceRenderInfo;
	struct SpriteDisplayCacheEntry;
	struct SpriteCellDisplayCacheEntry;
	struct SpritePixelBufferEntry;
	struct VRAMRenderOp;
	struct InternalRenderOp;
	struct FIFOBufferEntry;
	struct HVCounterAdvanceSession;
	struct ImageBufferColorEntry;

	// Typedefs
	typedef RandomTimeAccessBuffer<Data, unsigned int> RegBuffer;
	typedef RegBuffer::AccessTarget AccessTarget;
	typedef ITimedBufferInt::AccessTarget RAMAccessTarget;

	// Render constants
	static const unsigned int RenderDigitalBlockPixelSizeY = 8;
	static const unsigned char PaletteEntryTo8Bit[8];
	static const unsigned char PaletteEntryTo8BitShadow[8];
	static const unsigned char PaletteEntryTo8BitHighlight[8];
	static const VRAMRenderOp VramOperationsH32ActiveLine[171];
	static const VRAMRenderOp VramOperationsH32InactiveLine[171];
	static const VRAMRenderOp VramOperationsH40ActiveLine[210];
	static const VRAMRenderOp VramOperationsH40InactiveLine[210];
	static const InternalRenderOp InternalOperationsH32[342];
	static const InternalRenderOp InternalOperationsH40[420];

	// Interrupt settings
	static const unsigned int ExintIPLLineState = 2;
	static const unsigned int HintIPLLineState = 4;
	static const unsigned int VintIPLLineState = 6;

	// Horizontal scan timing settings
	static const HScanSettings H32ScanSettingsStatic;
	static const HScanSettings H40ScanSettingsStatic;

	// Vertical scan timing settings
	static const VScanSettings V28PalNoIntScanSettingsStatic;
	static const VScanSettings V28PalIntEnScanSettingsStatic;
	static const VScanSettings V30PalNoIntScanSettingsStatic;
	static const VScanSettings V30PalIntEnScanSettingsStatic;
	static const VScanSettings V28NtscNoIntScanSettingsStatic;
	static const VScanSettings V28NtscIntEnScanSettingsStatic;
	static const VScanSettings V30NtscNoIntScanSettingsStatic;
	static const VScanSettings V30NtscIntEnScanSettingsStatic;

private:
	// Debug functions
	static void GensKModStopSystemCallback(void* params);

	// Line functions
	unsigned int GetNewIPLLineState();
	void UpdatePredictedLineStateChanges(IDeviceContext* callingDevice, double accessTime, unsigned int accessContext);
	void UpdateLineStateChangePrediction(unsigned int lineNo, unsigned int lineStateChangeData, bool& lineStateChangePending, unsigned int& lineStateChangeMCLKCountdown, double& lineStateChangeTime, bool lineStateChangePendingNew, unsigned int lineStateChangeMCLKCountdownNew, IDeviceContext* callingDevice, double accessTime, unsigned int accessContext);

	// External buffer functions
	virtual void LockExternalBuffers();
	virtual void UnlockExternalBuffers();
	virtual ITimedBufferInt* GetVRAMBuffer() const;
	virtual ITimedBufferInt* GetCRAMBuffer() const;
	virtual ITimedBufferInt* GetVSRAMBuffer() const;
	virtual ITimedBufferInt* GetSpriteCacheBuffer() const;

	// Image buffer functions
	virtual unsigned int GetImageLastRenderedFrameToken() const;
	virtual unsigned int GetImageCompletedBufferPlaneNo() const;
	virtual unsigned int GetImageDrawingBufferPlaneNo() const;
	virtual void LockImageBufferData(unsigned int planeNo);
	virtual void UnlockImageBufferData(unsigned int planeNo);
	virtual const unsigned char* GetImageBufferData(unsigned int planeNo) const;
	virtual const ImageBufferInfo* GetImageBufferInfo(unsigned int planeNo) const;
	virtual const ImageBufferInfo* GetImageBufferInfo(unsigned int planeNo, unsigned int lineNo, unsigned int pixelNo) const;
	virtual bool GetImageBufferOddInterlaceFrame(unsigned int planeNo) const;
	virtual unsigned int GetImageBufferLineCount(unsigned int planeNo) const;
	virtual unsigned int GetImageBufferLineWidth(unsigned int planeNo, unsigned int lineNo) const;
	virtual void GetImageBufferActiveScanPosX(unsigned int planeNo, unsigned int lineNo, unsigned int& startPosX, unsigned int& endPosX) const;
	virtual void GetImageBufferActiveScanPosY(unsigned int planeNo, unsigned int& startPosY, unsigned int& endPosY) const;

	// DMA functions
	void DMAWorkerThread();

	// Rendering functions
	void RenderThread();
	void AdvanceRenderProcess(unsigned int mclkCyclesToAdvance);
	void UpdateDigitalRenderProcess(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings);
	void PerformInternalRenderOperation(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, const InternalRenderOp& nextOperation, int renderDigitalCurrentRow);
	void PerformVRAMRenderOperation(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, const VRAMRenderOp& nextOperation, int renderDigitalCurrentRow);
	void UpdateAnalogRenderProcess(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings);
	virtual void DigitalRenderReadHscrollData(unsigned int screenRowNumber, unsigned int hscrollDataBase, bool hscrState, bool lscrState, unsigned int& layerAHscrollPatternDisplacement, unsigned int& layerBHscrollPatternDisplacement, unsigned int& layerAHscrollMappingDisplacement, unsigned int& layerBHscrollMappingDisplacement) const;
	virtual void DigitalRenderReadVscrollData(unsigned int screenColumnNumber, unsigned int layerNumber, bool vscrState, bool interlaceMode2Active, unsigned int& layerVscrollPatternDisplacement, unsigned int& layerVscrollMappingDisplacement, Data& vsramReadCache) const;
	static unsigned int DigitalRenderCalculateMappingVRAMAddess(unsigned int screenRowNumber, unsigned int screenColumnNumber, bool interlaceMode2Active, unsigned int nameTableBaseAddress, unsigned int layerHscrollMappingDisplacement, unsigned int layerVscrollMappingDisplacement, unsigned int layerVscrollPatternDisplacement, unsigned int hszState, unsigned int vszState);
	void DigitalRenderBuildSpriteList(unsigned int screenRowNumber, bool interlaceMode2Active, bool screenModeRS1Active, unsigned int& nextTableEntryToRead, bool& spriteSearchComplete, bool& spriteOverflow, unsigned int& spriteDisplayCacheEntryCount, std::vector<SpriteDisplayCacheEntry>& spriteDisplayCache) const;
	void DigitalRenderBuildSpriteCellList(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int spriteDisplayCacheIndex, unsigned int spriteTableBaseAddress, bool interlaceMode2Active, bool screenModeRS1Active, bool& spriteDotOverflow, SpriteDisplayCacheEntry& spriteDisplayCacheEntry, unsigned int& spriteCellDisplayCacheEntryCount, std::vector<SpriteCellDisplayCacheEntry>& spriteCellDisplayCache) const;
	unsigned int DigitalRenderReadPixelIndex(const Data& patternRow, bool horizontalFlip, unsigned int pixelIndex) const;
	void CalculateLayerPriorityIndex(unsigned int& layerIndex, bool& shadow, bool& highlight, bool shadowHighlightEnabled, bool spriteIsShadowOperator, bool spriteIsHighlightOperator, bool foundSpritePixel, bool foundLayerAPixel, bool foundLayerBPixel, bool prioritySprite, bool priorityLayerA, bool priorityLayerB) const;
	virtual unsigned int CalculatePatternDataRowNumber(unsigned int patternRowNumberNoFlip, bool interlaceMode2Active, const Data& mappingData) const;
	virtual unsigned int CalculatePatternDataRowAddress(unsigned int patternRowNumber, unsigned int patternCellOffset, bool interlaceMode2Active, const Data& mappingData) const;
	virtual void CalculateEffectiveCellScrollSize(unsigned int hszState, unsigned int vszState, unsigned int& effectiveScrollWidth, unsigned int& effectiveScrollHeight) const;
	virtual DecodedPaletteColorEntry ReadDecodedPaletteColor(unsigned int paletteRow, unsigned int paletteIndex) const;
	virtual unsigned char ColorValueTo8BitValue(unsigned int colorValue, bool shadow, bool highlight) const;
	virtual Marshal::Ret<std::list<SpriteBoundaryLineEntry>> GetSpriteBoundaryLines(unsigned int planeNo) const;

	// Sprite list debugging functions
	virtual SpriteMappingTableEntry GetSpriteMappingTableEntry(unsigned int spriteTableBaseAddress, unsigned int entryNo) const;
	virtual void SetSpriteMappingTableEntry(unsigned int spriteTableBaseAddress, unsigned int entryNo, const SpriteMappingTableEntry& entry, bool useSeparatedData);

	// Memory interface functions
	Data GetHVCounter() const;
	void ProcessCommandDataWriteFirstHalf(const Data& data);
	void ProcessCommandDataWriteSecondHalf(const Data& data);
	void TransparentRegisterSpecialUpdateFunction(unsigned int registerNo, const Data& data);
	void RegisterSpecialUpdateFunction(unsigned int mclkCycle, double accessTime, double accessDelay, IDeviceContext* caller, unsigned int accessContext, unsigned int registerNo, const Data& data);

	// Port monitor functions
	virtual bool GetPortMonitorStatusReadEnabled() const;
	virtual void SetPortMonitorStatusReadEnabled(bool state);
	virtual bool GetPortMonitorDataReadEnabled() const;
	virtual void SetPortMonitorDataReadEnabled(bool state);
	virtual bool GetPortMonitorHVReadEnabled() const;
	virtual void SetPortMonitorHVReadEnabled(bool state);
	virtual bool GetPortMonitorControlWriteEnabled() const;
	virtual void SetPortMonitorControlWriteEnabled(bool state);
	virtual bool GetPortMonitorDataWriteEnabled() const;
	virtual void SetPortMonitorDataWriteEnabled(bool state);
	virtual unsigned int GetPortMonitorLength() const;
	virtual void SetPortMonitorLength(unsigned int state);
	virtual Marshal::Ret<std::list<PortMonitorEntry>> GetPortMonitorLog() const;
	virtual unsigned int GetPortMonitorLogLastModifiedToken() const;
	virtual void ClearPortMonitorLog();
	void RecordPortMonitorEntry(const PortMonitorEntry& entry);

	// HV counter internal/linear conversion
	static unsigned int HCounterValueFromVDPInternalToLinear(const HScanSettings& hscanSettings, unsigned int hcounterCurrent);
	static unsigned int VCounterValueFromVDPInternalToLinear(const VScanSettings& vscanSettings, unsigned int vcounterCurrent, bool oddFlagSet);
	static unsigned int HCounterValueFromLinearToVDPInternal(const HScanSettings& hscanSettings, unsigned int hcounterCurrent);
	static unsigned int VCounterValueFromLinearToVDPInternal(const VScanSettings& vscanSettings, unsigned int vcounterCurrent, bool oddFlagSet);

	// Video scan settings functions
	//##TODO## Consider removing the RS0 parameter here. We only need the RS1 bit.
	static const HScanSettings& GetHScanSettings(bool screenModeRS0Active, bool screenModeRS1Active);
	static const VScanSettings& GetVScanSettings(bool screenModeV30Active, bool palModeActive, bool interlaceActive);

	// HV counter comparison functions
	static bool EventOccursWithinCounterRange(const HScanSettings& hscanSettings, unsigned int hcounterStart, unsigned int vcounterStart, unsigned int hcounterEnd, unsigned int vcounterEnd, unsigned int hcounterEventPos, unsigned int vcounterEventPos);
	static unsigned int GetPixelClockStepsBetweenHVCounterValues(bool advanceIfValuesMatch, const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterTarget, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterTarget);
	static unsigned int GetPixelClockStepsBetweenHCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterTarget);
	static unsigned int GetPixelClockStepsBetweenVCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterTarget);

	// HV counter advancement functions
	void BeginHVCounterAdvanceSessionFromCurrentState(HVCounterAdvanceSession& advanceSession);
	static bool AdvanceHVCounterSession(HVCounterAdvanceSession& advanceSession, unsigned int hcounterTarget, unsigned int vcounterTarget, bool advanceIfValuesMatch);
	static unsigned int AddStepsToHCounter(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterStepsToAdd);
	static unsigned int AddStepsToVCounter(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterStepsToAdd);
	static void AdvanceHVCounters(const HScanSettings& hscanSettings, unsigned int& hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool& oddFlagSet, unsigned int& vcounterCurrent, unsigned int pixelClockSteps);
	static void AdvanceHVCountersOneStep(const HScanSettings& hscanSettings, unsigned int& hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool& oddFlagSet, unsigned int& vcounterCurrent);

	// Pixel clock functions
	//##TODO## Move these functions somewhere more appropriate
	// static unsigned int GetUpdatedVSRAMReadCacheIndex(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int vsramReadCacheIndexCurrent, unsigned int hcounterInitial, unsigned int vcounterInitial, unsigned int hcounterFinal, unsigned int vcounterFinal, bool screenModeRS0Current, bool screenModeRS1Current, unsigned int vscrollMode);
	static unsigned int GetPixelClockTicksUntilNextAccessSlot(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int hcounterCurrent, bool screenModeRS0Current, bool screenModeRS1Current, bool displayEnabled, unsigned int vcounterCurrent);
	static unsigned int GetPixelClockTicksForMclkTicks(const HScanSettings& hscanSettings, unsigned int mclkTicks, unsigned int hcounterCurrent, bool screenModeRS0Active, bool screenModeRS1Active, unsigned int& mclkTicksUnused);
	static unsigned int GetMclkTicksForPixelClockTicks(const HScanSettings& hscanSettings, unsigned int pixelClockTicks, unsigned int hcounterCurrent, bool screenModeRS0Active, bool screenModeRS1Active);
	static unsigned int GetMclkTicksForOnePixelClockTick(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, bool screenModeRS0Active, bool screenModeRS1Active);

	// Access time functions
	unsigned int ConvertAccessTimeToMclkCount(double accessTime) const;
	double ConvertMclkCountToAccessTime(unsigned int mclkCount) const;

	// Processor state advancement functions
	void UpdateInternalState(unsigned int mclkCyclesTarget, bool checkFifoStateBeforeUpdate, bool stopWhenFifoEmpty, bool stopWhenFifoFull, bool stopWhenFifoNotFull, bool stopWhenReadDataAvailable, bool stopWhenNoDMAOperationInProgress, bool allowAdvancePastCycleTarget);
	bool AdvanceProcessorState(unsigned int mclkCyclesTarget, bool stopAtNextAccessSlot, bool allowAdvancePastTargetForAccessSlot);
	void PerformReadCacheOperation();
	void PerformFIFOWriteOperation();
	void PerformDMACopyOperation();
	void PerformDMAFillOperation();
	void CacheDMATransferReadData(unsigned int mclkTime);
	void PerformDMATransferOperation();
	void AdvanceDMAState();
	bool TargetProcessorStateReached(bool stopWhenFifoEmpty, bool stopWhenFifoFull, bool stopWhenFifoNotFull, bool stopWhenReadDataAvailable, bool stopWhenNoDMAOperationInProgress);
	double GetProcessorStateTime() const;
	unsigned int GetProcessorStateMclkCurrent() const;

	// FIFO functions
	bool IsWriteFIFOEmpty() const;
	bool IsWriteFIFOFull() const;

	//##TODO## Mode 4 control functions

	// Mode 5 control functions
	bool ValidReadTargetInCommandCode() const;
	void M5ReadVRAM8Bit(const Data& address, Data& data, const RAMAccessTarget& accessTarget);
	void M5ReadCRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget);
	void M5ReadVSRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget);
	void M5WriteVRAM8Bit(const Data& address, const Data& data, const RAMAccessTarget& accessTarget);
	void M5WriteCRAM(const Data& address, const Data& data, const RAMAccessTarget& accessTarget);
	void M5WriteVSRAM(const Data& address, const Data& data, const RAMAccessTarget& accessTarget);

	// Status register functions
	inline bool GetStatusFlagFIFOEmpty() const;
	inline void SetStatusFlagFIFOEmpty(bool state);
	inline bool GetStatusFlagFIFOFull() const;
	inline void SetStatusFlagFIFOFull(bool state);
	inline bool GetStatusFlagF() const;
	inline void SetStatusFlagF(bool state);
	inline bool GetStatusFlagSpriteOverflow() const;
	inline void SetStatusFlagSpriteOverflow(bool state);
	inline bool GetStatusFlagSpriteCollision() const;
	inline void SetStatusFlagSpriteCollision(bool state);
	inline bool GetStatusFlagOddInterlaceFrame() const;
	inline void SetStatusFlagOddInterlaceFrame(bool state);
	inline bool GetStatusFlagVBlank() const;
	inline void SetStatusFlagVBlank(bool state);
	inline bool GetStatusFlagHBlank() const;
	inline void SetStatusFlagHBlank(bool state);
	inline bool GetStatusFlagDMA() const;
	inline void SetStatusFlagDMA(bool state);
	inline bool GetStatusFlagPAL() const;
	inline void SetStatusFlagPAL(bool state);

	// Raw register functions
	inline Data GetRegisterData(unsigned int location, const AccessTarget& accessTarget) const;
	inline void SetRegisterData(unsigned int location, const AccessTarget& accessTarget, const Data& data);

	// Interpreted register functions
	inline bool RegGetVSI(const AccessTarget& accessTarget) const;	// 0x00(0)
	inline void RegSetVSI(const AccessTarget& accessTarget, bool data);
	inline bool RegGetHSI(const AccessTarget& accessTarget) const;
	inline void RegSetHSI(const AccessTarget& accessTarget, bool data);
	inline bool RegGetLCB(const AccessTarget& accessTarget) const;
	inline void RegSetLCB(const AccessTarget& accessTarget, bool data);
	inline bool RegGetIE1(const AccessTarget& accessTarget) const;
	inline void RegSetIE1(const AccessTarget& accessTarget, bool data);
	inline bool RegGetSS(const AccessTarget& accessTarget) const;
	inline void RegSetSS(const AccessTarget& accessTarget, bool data);
	inline bool RegGetPS(const AccessTarget& accessTarget) const;
	inline void RegSetPS(const AccessTarget& accessTarget, bool data);
	inline bool RegGetM2(const AccessTarget& accessTarget) const;
	inline void RegSetM2(const AccessTarget& accessTarget, bool data);
	inline bool RegGetES(const AccessTarget& accessTarget) const;
	inline void RegSetES(const AccessTarget& accessTarget, bool data);

	inline bool RegGetEVRAM(const AccessTarget& accessTarget) const;	// 0x01(1)
	inline void RegSetEVRAM(const AccessTarget& accessTarget, bool data);
	inline bool RegGetDisplayEnabled(const AccessTarget& accessTarget) const;
	inline void RegSetDisplayEnabled(const AccessTarget& accessTarget, bool data);
	inline bool RegGetIE0(const AccessTarget& accessTarget) const;
	inline void RegSetIE0(const AccessTarget& accessTarget, bool data);
	inline bool RegGetDMAEnabled(const AccessTarget& accessTarget) const;
	inline void RegSetDMAEnabled(const AccessTarget& accessTarget, bool data);
	inline bool RegGetM3(const AccessTarget& accessTarget) const;
	inline void RegSetM3(const AccessTarget& accessTarget, bool data);
	inline bool RegGetMode5(const AccessTarget& accessTarget) const;
	inline void RegSetMode5(const AccessTarget& accessTarget, bool data);
	inline bool RegGetSZ(const AccessTarget& accessTarget) const;
	inline void RegSetSZ(const AccessTarget& accessTarget, bool data);
	inline bool RegGetMAG(const AccessTarget& accessTarget) const;
	inline void RegSetMAG(const AccessTarget& accessTarget, bool data);

	inline unsigned int RegGetNameTableBaseScrollA(const AccessTarget& accessTarget, bool mode4Enabled, bool extendedVRAMModeEnabled) const;	// 0x02(2)
	inline void RegSetNameTableBaseScrollA(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetNameTableBaseWindow(const AccessTarget& accessTarget, bool h40ModeActive, bool extendedVRAMModeEnabled) const;	// 0x03(3)
	inline void RegSetNameTableBaseWindow(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetNameTableBaseScrollB(const AccessTarget& accessTarget, bool extendedVRAMModeEnabled) const;	// 0x04(4)
	inline void RegSetNameTableBaseScrollB(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetNameTableBaseSprite(const AccessTarget& accessTarget, bool mode4Enabled, bool h40ModeActive, bool extendedVRAMModeEnabled) const;	// 0x05(5)
	inline void RegSetNameTableBaseSprite(const AccessTarget& accessTarget, unsigned int data, bool mode4Enabled);
	inline unsigned int RegGetPatternBaseSprite(const AccessTarget& accessTarget, bool mode4Enabled, bool extendedVRAMModeEnabled) const;	// 0x06(6)
	inline void RegSetPatternBaseSprite(const AccessTarget& accessTarget, unsigned int data, bool mode4Enabled);

	inline bool RegGet077(const AccessTarget& accessTarget) const;	// 0x07(7)
	inline void RegSet077(const AccessTarget& accessTarget, bool data);
	inline bool RegGet076(const AccessTarget& accessTarget) const;
	inline void RegSet076(const AccessTarget& accessTarget, bool data);
	inline unsigned int RegGetBackgroundPaletteRow(const AccessTarget& accessTarget) const;
	inline void RegSetBackgroundPaletteRow(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetBackgroundPaletteColumn(const AccessTarget& accessTarget) const;
	inline void RegSetBackgroundPaletteColumn(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int RegGetBackgroundScrollX(const AccessTarget& accessTarget) const;	// 0x08(8)
	inline void RegSetBackgroundScrollX(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetBackgroundScrollY(const AccessTarget& accessTarget) const;	// 0x09(9)
	inline void RegSetBackgroundScrollY(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int RegGetHInterruptData(const AccessTarget& accessTarget) const;	// 0x0A(10)
	inline void RegSetHInterruptData(const AccessTarget& accessTarget, unsigned int data);
	inline bool RegGet0B7(const AccessTarget& accessTarget) const;	// 0x0B(11)
	inline void RegSet0B7(const AccessTarget& accessTarget, bool data);
	inline bool RegGet0B6(const AccessTarget& accessTarget) const;
	inline void RegSet0B6(const AccessTarget& accessTarget, bool data);
	inline bool RegGet0B5(const AccessTarget& accessTarget) const;
	inline void RegSet0B5(const AccessTarget& accessTarget, bool data);
	inline bool RegGet0B4(const AccessTarget& accessTarget) const;
	inline void RegSet0B4(const AccessTarget& accessTarget, bool data);
	inline bool RegGetIE2(const AccessTarget& accessTarget) const;
	inline void RegSetIE2(const AccessTarget& accessTarget, bool data);
	inline bool RegGetVSCR(const AccessTarget& accessTarget) const;
	inline void RegSetVSCR(const AccessTarget& accessTarget, bool data);
	inline bool RegGetHSCR(const AccessTarget& accessTarget) const;
	inline void RegSetHSCR(const AccessTarget& accessTarget, bool data);
	inline bool RegGetLSCR(const AccessTarget& accessTarget) const;
	inline void RegSetLSCR(const AccessTarget& accessTarget, bool data);

	inline bool RegGetRS0(const AccessTarget& accessTarget) const;	// 0x0C(12)
	inline void RegSetRS0(const AccessTarget& accessTarget, bool data);
	inline bool RegGetU1(const AccessTarget& accessTarget) const;
	inline void RegSetU1(const AccessTarget& accessTarget, bool data);
	inline bool RegGetU2(const AccessTarget& accessTarget) const;
	inline void RegSetU2(const AccessTarget& accessTarget, bool data);
	inline bool RegGetU3(const AccessTarget& accessTarget) const;
	inline void RegSetU3(const AccessTarget& accessTarget, bool data);
	inline bool RegGetSTE(const AccessTarget& accessTarget) const;
	inline void RegSetSTE(const AccessTarget& accessTarget, bool data);
	inline bool RegGetLSM1(const AccessTarget& accessTarget) const;
	inline void RegSetLSM1(const AccessTarget& accessTarget, bool data);
	inline bool RegGetLSM0(const AccessTarget& accessTarget) const;
	inline void RegSetLSM0(const AccessTarget& accessTarget, bool data);
	inline bool RegGetRS1(const AccessTarget& accessTarget) const;
	inline void RegSetRS1(const AccessTarget& accessTarget, bool data);

	inline unsigned int RegGetHScrollDataBase(const AccessTarget& accessTarget, bool extendedVRAMModeEnabled) const;	// 0x0D(13)
	inline void RegSetHScrollDataBase(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int RegGet0E57(const AccessTarget& accessTarget) const;	// 0x0E(14)
	inline void RegSet0E57(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetPatternBaseScrollA(const AccessTarget& accessTarget, bool extendedVRAMModeEnabled) const;
	inline void RegSetPatternBaseScrollA(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGet0E13(const AccessTarget& accessTarget) const;
	inline void RegSet0E13(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetPatternBaseScrollB(const AccessTarget& accessTarget, bool extendedVRAMModeEnabled) const;
	inline void RegSetPatternBaseScrollB(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int RegGetAutoIncrementData(const AccessTarget& accessTarget) const;	// 0x0F(15)
	inline void RegSetAutoIncrementData(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int RegGet1067(const AccessTarget& accessTarget) const;	// 0x10(16)
	inline void RegSet1067(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetVSZ(const AccessTarget& accessTarget) const;
	inline void RegSetVSZ(const AccessTarget& accessTarget, unsigned int data);
	inline bool RegGetVSZ1(const AccessTarget& accessTarget) const;
	inline void RegSetVSZ1(const AccessTarget& accessTarget, bool data);
	inline bool RegGetVSZ0(const AccessTarget& accessTarget) const;
	inline void RegSetVSZ0(const AccessTarget& accessTarget, bool data);
	inline unsigned int RegGet1023(const AccessTarget& accessTarget) const;
	inline void RegSet1023(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetHSZ(const AccessTarget& accessTarget) const;
	inline void RegSetHSZ(const AccessTarget& accessTarget, unsigned int data);
	inline bool RegGetHSZ1(const AccessTarget& accessTarget) const;
	inline void RegSetHSZ1(const AccessTarget& accessTarget, bool data);
	inline bool RegGetHSZ0(const AccessTarget& accessTarget) const;
	inline void RegSetHSZ0(const AccessTarget& accessTarget, bool data);

	inline unsigned int RegGet1156(const AccessTarget& accessTarget) const;	// 0x11(17)
	inline void RegSet1156(const AccessTarget& accessTarget, unsigned int data);
	inline bool RegGetWindowRightAligned(const AccessTarget& accessTarget) const;
	inline void RegSetWindowRightAligned(const AccessTarget& accessTarget, bool data);
	inline unsigned int RegGetWindowBasePointX(const AccessTarget& accessTarget) const;
	inline void RegSetWindowBasePointX(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGet1256(const AccessTarget& accessTarget) const;	// 0x12(18)
	inline void RegSet1256(const AccessTarget& accessTarget, unsigned int data);
	inline bool RegGetWindowBottomAligned(const AccessTarget& accessTarget) const;
	inline void RegSetWindowBottomAligned(const AccessTarget& accessTarget, bool data);
	inline unsigned int RegGetWindowBasePointY(const AccessTarget& accessTarget) const;
	inline void RegSetWindowBasePointY(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int RegGetDMALengthCounter(const AccessTarget& accessTarget) const;	// 0x13-0x14(19-20)
	inline void RegSetDMALengthCounter(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetDMASourceAddress(const AccessTarget& accessTarget) const;	// 0x15-0x17(21-23)
	inline void RegSetDMASourceAddress(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetDMASourceAddressByte1(const AccessTarget& accessTarget) const;	// 0x15(21)
	inline void RegSetDMASourceAddressByte1(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetDMASourceAddressByte2(const AccessTarget& accessTarget) const;	// 0x16(22)
	inline void RegSetDMASourceAddressByte2(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int RegGetDMASourceAddressByte3(const AccessTarget& accessTarget) const;	// 0x17(23)
	inline void RegSetDMASourceAddressByte3(const AccessTarget& accessTarget, unsigned int data);
	inline bool RegGetDMD1(const AccessTarget& accessTarget) const;
	inline void RegSetDMD1(const AccessTarget& accessTarget, bool data);
	inline bool RegGetDMD0(const AccessTarget& accessTarget) const;
	inline void RegSetDMD0(const AccessTarget& accessTarget, bool data);

private:
	// Debug output
	bool _outputPortAccessDebugMessages;
	bool _outputTimingDebugMessages;
	bool _outputRenderSyncMessages;
	bool _outputInterruptDebugMessages;
	bool _videoDisableRenderOutput;
	bool _videoEnableSpriteBoxing;
	bool _videoHighlightRenderPos;
	volatile bool _videoSingleBuffering;
	volatile bool _videoFixedAspectRatio;
	volatile bool _videoShowStatusBar;
	volatile bool _videoEnableLineSmoothing;
	volatile bool _currentRenderPosOnScreen;
	volatile unsigned int _currentRenderPosScreenX;
	volatile unsigned int _currentRenderPosScreenY;
	bool _videoShowBoundaryActiveImage;
	bool _videoShowBoundaryActionSafe;
	bool _videoShowBoundaryTitleSafe;
	bool _videoEnableFullImageBufferInfo;

	// Gens KMod debugging variables
	bool _gensKmodDebugActive;
	bool _gensKmodIgnoreNextDebugStop;
	std::string _gensKmodDebugString;
	std::string _bgensKmodDebugString;
	bool _gensKmodDebugTimerRunning;
	bool _bgensKmodDebugTimerRunning;
	double _gensKmodDebugTimerAccumulatedTime;
	double _bgensKmodDebugTimerAccumulatedTime;
	double _gensKmodDebugTimerCurrentTimesliceStart;
	double _bgensKmodDebugTimerCurrentTimesliceStart;

	// Bus interface
	IBusInterface* _memoryBus;
	volatile bool _busGranted;
	bool _bbusGranted;
	volatile bool _palModeLineState;
	bool _bpalModeLineState;
	volatile bool _resetLineState;
	bool _bresetLineState;
	volatile unsigned int _lineStateIPL;
	unsigned int _blineStateIPL;
	bool _busRequestLineState;
	bool _bbusRequestLineState;

	// Embedded PSG device
	IDevice* _psg;

	// Clock sources
	IClockSource* _clockSourceCLK0;
	IClockSource* _clockSourceCLK1;
	double _clockMclkCurrent;
	double _bclockMclkCurrent;

	// Layer removal settings
	bool _enableLayerAHigh;
	bool _enableLayerALow;
	bool _enableLayerBHigh;
	bool _enableLayerBLow;
	bool _enableWindowHigh;
	bool _enableWindowLow;
	bool _enableSpriteHigh;
	bool _enableSpriteLow;

	// Port monitor settings
	mutable std::mutex _portMonitorMutex;
	bool _logStatusRegisterRead;
	bool _logDataPortRead;
	bool _logHVCounterRead;
	bool _logControlPortWrite;
	bool _logDataPortWrite;
	unsigned int _portMonitorListSize;
	unsigned int _portMonitorLastModifiedToken;
	std::list<PortMonitorEntry> _portMonitorList;
	std::list<PortMonitorEntry> _bportMonitorList;

	// Physical registers and memory buffers
	mutable std::mutex _accessMutex; // Top-level, protects against concurrent interface access.
	mutable std::mutex _lineMutex; // Top level, must never be held during a blocking operation
	double _lastAccessTime;
	RegBuffer _reg;
	ITimedBufferInt* _vram;
	ITimedBufferInt* _cram;
	ITimedBufferInt* _vsram;
	ITimedBufferInt* _spriteCache;
	ReadWriteLock _externalReferenceLock;
	Data _status;
	Data _bstatus;
	Data _hcounter;
	Data _bhcounter;
	Data _vcounter;
	Data _bvcounter;
	Data _hcounterLatchedData;
	Data _bhcounterLatchedData;
	Data _vcounterLatchedData;
	Data _bvcounterLatchedData;
	unsigned int _hintCounter;
	unsigned int _bhintCounter;
	bool _vintPending;
	bool _bvintPending;
	bool _hintPending;
	bool _bhintPending;
	bool _exintPending;
	bool _bexintPending;

	// Register locking
	mutable std::mutex _registerLockMutex;
	bool _rawRegisterLocking[RegisterCount];
	std::map<unsigned int, std::wstring> _lockedRegisterState;

	// Active register settings
	bool _interlaceEnabled;
	bool _binterlaceEnabled;
	bool _interlaceDouble;
	bool _binterlaceDouble;
	bool _screenModeRS0;
	bool _bscreenModeRS0;
	bool _screenModeRS1;
	bool _bscreenModeRS1;
	bool _screenModeV30;
	bool _bscreenModeV30;
	bool _palMode;
	bool _bpalMode;

	// Cached register settings
	bool _hvCounterLatchEnabled;
	bool _bhvCounterLatchEnabled;
	bool _vintEnabled;
	bool _bvintEnabled;
	bool _hintEnabled;
	bool _bhintEnabled;
	bool _exintEnabled;
	bool _bexintEnabled;
	unsigned int _hintCounterReloadValue;
	unsigned int _bhintCounterReloadValue;
	bool _dmaEnabled;
	bool _bdmaEnabled;
	bool _dmd0;
	bool _bdmd0;
	bool _dmd1;
	bool _bdmd1;
	unsigned int _dmaLengthCounter;
	unsigned int _bdmaLengthCounter;
	unsigned int _dmaSourceAddressByte1;
	unsigned int _bdmaSourceAddressByte1;
	unsigned int _dmaSourceAddressByte2;
	unsigned int _bdmaSourceAddressByte2;
	unsigned int _dmaSourceAddressByte3;
	unsigned int _bdmaSourceAddressByte3;
	unsigned int _autoIncrementData;
	unsigned int _bautoIncrementData;
	bool _interlaceEnabledCached;
	bool _binterlaceEnabledCached;
	bool _interlaceDoubleCached;
	bool _binterlaceDoubleCached;
	bool _screenModeRS0Cached;
	bool _bscreenModeRS0Cached;
	bool _screenModeRS1Cached;
	bool _bscreenModeRS1Cached;
	bool _screenModeV30Cached;
	bool _bscreenModeV30Cached;
	bool _screenModeM5Cached;
	bool _bscreenModeM5Cached;
	bool _displayEnabledCached;
	bool _bdisplayEnabledCached;
	unsigned int _spriteAttributeTableBaseAddressDecoded;
	unsigned int _bspriteAttributeTableBaseAddressDecoded;
	bool _verticalScrollModeCached;
	bool _bverticalScrollModeCached;
	bool _cachedDMASettingsChanged;

	// FIFO buffer registers
	unsigned int _fifoNextReadEntry;
	unsigned int _bfifoNextReadEntry;
	unsigned int _fifoNextWriteEntry;
	unsigned int _bfifoNextWriteEntry;
	std::vector<FIFOBufferEntry> _fifoBuffer;
	std::vector<FIFOBufferEntry> _bfifoBuffer;
	Data _readBuffer;
	Data _breadBuffer;
	bool _readDataAvailable;
	bool _breadDataAvailable;
	bool _readDataHalfCached;
	bool _breadDataHalfCached;
	bool _dmaFillOperationRunning;
	bool _bdmaFillOperationRunning;
	Data _vsramLastRenderReadCache;
	Data _bvsramLastRenderReadCache;

	// Update state
	double _currentTimesliceLength; // The length of the current timeslice, as passed to NotifyUpcomingTimeslice().
	double _bcurrentTimesliceLength;
	double _lastTimesliceMclkCyclesRemainingTime; // The unused portion of the last timeslice which wasn't be consumed by an mclk cycle. Note that this does NOT factor in time we ran over the last timeslice, as specified in lastTimesliceMclkCyclesOverrun.
	double _blastTimesliceMclkCyclesRemainingTime;
	double _currentTimesliceMclkCyclesRemainingTime; // Rolling value, the unused portion of the current timeslice which can't be consumed by an mclk cycle, as predicted during NotifyUpcomingTimeslice().
	double _bcurrentTimesliceMclkCyclesRemainingTime;
	unsigned int _currentTimesliceTotalMclkCycles; // The total number of mclk cycles added by the current timeslice, taking into account free from the last timeslice as included in the currentTimesliceMclkCyclesRemainingTime value. Note that this does NOT factor in time we ran over the last timeslice, as specified in lastTimesliceMclkCyclesOverrun.
	unsigned int _lastTimesliceMclkCyclesOverrun; // The total number of mclk cycles we advanced past the end of the last timeslice
	unsigned int _blastTimesliceMclkCyclesOverrun;
	double _stateLastUpdateTime;
	double _bstateLastUpdateTime;
	unsigned int _stateLastUpdateMclk;
	unsigned int _bstateLastUpdateMclk;
	unsigned int _stateLastUpdateMclkUnused;
	unsigned int _bstateLastUpdateMclkUnused;
	unsigned int _stateLastUpdateMclkUnusedFromLastTimeslice;
	unsigned int _bstateLastUpdateMclkUnusedFromLastTimeslice;

	// Interrupt line rollback data
	bool _lineStateChangePendingVINT;
	bool _blineStateChangePendingVINT;
	unsigned int _lineStateChangeVINTMClkCountFromCurrent;
	unsigned int _blineStateChangeVINTMClkCountFromCurrent;
	double _lineStateChangeVINTTime;
	double _blineStateChangeVINTTime;
	bool _lineStateChangePendingHINT;
	bool _blineStateChangePendingHINT;
	unsigned int _lineStateChangeHINTMClkCountFromCurrent;
	unsigned int _blineStateChangeHINTMClkCountFromCurrent;
	double _lineStateChangeHINTTime;
	double _blineStateChangeHINTTime;
	bool _lineStateChangePendingEXINT;
	bool _blineStateChangePendingEXINT;
	unsigned int _lineStateChangeEXINTMClkCountFromCurrent;
	unsigned int _blineStateChangeEXINTMClkCountFromCurrent;
	double _lineStateChangeEXINTTime;
	double _blineStateChangeEXINTTime;
	bool _lineStateChangePendingINTAsserted;
	bool _blineStateChangePendingINTAsserted;
	unsigned int _lineStateChangeINTAssertedMClkCountFromCurrent;
	unsigned int _blineStateChangeINTAssertedMClkCountFromCurrent;
	double _lineStateChangeINTAssertedTime;
	double _blineStateChangeINTAssertedTime;
	bool _lineStateChangePendingINTNegated;
	bool _blineStateChangePendingINTNegated;
	unsigned int _lineStateChangeINTNegatedMClkCountFromCurrent;
	unsigned int _blineStateChangeINTNegatedMClkCountFromCurrent;
	double _lineStateChangeINTNegatedTime;
	double _blineStateChangeINTNegatedTime;

	// Control port registers
	bool _codeAndAddressRegistersModifiedSinceLastWrite;
	bool _bcodeAndAddressRegistersModifiedSinceLastWrite;
	bool _commandWritePending;
	bool _bcommandWritePending;
	Data _originalCommandAddress;
	Data _boriginalCommandAddress;
	Data _commandAddress;
	Data _bcommandAddress;
	Data _commandCode;
	Data _bcommandCode;

	// Render thread properties
	mutable std::mutex _renderThreadMutex; // Top level, timesliceMutex child
	mutable std::mutex _timesliceMutex; // Child of renderThreadMutex
	std::condition_variable _renderThreadUpdate;
	std::condition_variable _renderThreadStopped;
	bool _renderThreadActive;
	RegBuffer::Timeslice _regTimesliceCopy;
	ITimedBufferInt::Timeslice* _vramTimesliceCopy;
	ITimedBufferInt::Timeslice* _cramTimesliceCopy;
	ITimedBufferInt::Timeslice* _vsramTimesliceCopy;
	ITimedBufferInt::Timeslice* _spriteCacheTimesliceCopy;
	RegBuffer::AdvanceSession _regSession;
	ITimedBufferInt::AdvanceSession _vramSession;
	ITimedBufferInt::AdvanceSession _cramSession;
	ITimedBufferInt::AdvanceSession _vsramSession;
	ITimedBufferInt::AdvanceSession _spriteCacheSession;
	static const unsigned int layerPriorityLookupTableSize = 0x200;
	std::vector<unsigned int> _layerPriorityLookupTable;

	static const unsigned int maxPendingRenderOperationCount = 4;
	volatile bool _renderThreadLagging;
	std::condition_variable _renderThreadLaggingStateChange;
	unsigned int _pendingRenderOperationCount;
	std::list<TimesliceRenderInfo> _timesliceRenderInfoList;
	std::list<RegBuffer::Timeslice> _regTimesliceList;
	std::list<ITimedBufferInt::Timeslice*> _vramTimesliceList;
	std::list<ITimedBufferInt::Timeslice*> _cramTimesliceList;
	std::list<ITimedBufferInt::Timeslice*> _vsramTimesliceList;
	std::list<ITimedBufferInt::Timeslice*> _spriteCacheTimesliceList;
	std::list<TimesliceRenderInfo> _timesliceRenderInfoListUncommitted;
	std::list<RegBuffer::Timeslice> _regTimesliceListUncommitted;
	std::list<ITimedBufferInt::Timeslice*> _vramTimesliceListUncommitted;
	std::list<ITimedBufferInt::Timeslice*> _cramTimesliceListUncommitted;
	std::list<ITimedBufferInt::Timeslice*> _vsramTimesliceListUncommitted;
	std::list<ITimedBufferInt::Timeslice*> _spriteCacheTimesliceListUncommitted;

	// Digital render data buffers
	//##TODO## Separate the analog and digital renderers into their own classes. Our
	// single VDP superclass is getting too large to be manageable.
	static const unsigned int cellBlockSizeH = 8;
	static const unsigned int maxCellsPerRow = 42;
	static const unsigned int maxSpriteDisplayCacheSize = 20;
	static const unsigned int maxSpriteDisplayCellCacheSize = 40;
	static const unsigned int spritePixelBufferSize = maxCellsPerRow*8;
	static const unsigned int renderSpritePixelBufferPlaneCount = 2;
	unsigned int _renderDigitalHCounterPos;
	unsigned int _renderDigitalVCounterPos;
	unsigned int _renderDigitalVCounterPosPreviousLine;
	unsigned int _renderDigitalRemainingMclkCycles;
	bool _renderDigitalScreenModeRS0Active;
	bool _renderDigitalScreenModeRS1Active;
	bool _renderDigitalScreenModeV30Active;
	bool _renderDigitalInterlaceEnabledActive;
	bool _renderDigitalInterlaceDoubleActive;
	bool _renderDigitalPalModeActive;
	bool _renderDigitalOddFlagSet;
	unsigned int _renderDigitalMclkCycleProgress; // No backup needed
	unsigned int _renderLayerAHscrollPatternDisplacement;
	unsigned int _renderLayerBHscrollPatternDisplacement;
	unsigned int _renderLayerAHscrollMappingDisplacement;
	unsigned int _renderLayerBHscrollMappingDisplacement;
	unsigned int _renderLayerAVscrollPatternDisplacement;
	unsigned int _renderLayerBVscrollPatternDisplacement;
	unsigned int _renderLayerAVscrollMappingDisplacement;
	unsigned int _renderLayerBVscrollMappingDisplacement;
	std::vector<bool> _renderWindowActiveCache;
	std::vector<Data> _renderMappingDataCacheLayerA;
	std::vector<Data> _renderMappingDataCacheLayerB;
	std::vector<unsigned int> _renderMappingDataCacheSourceAddressLayerA;
	std::vector<unsigned int> _renderMappingDataCacheSourceAddressLayerB;
	std::vector<Data> _renderPatternDataCacheLayerA;
	std::vector<Data> _renderPatternDataCacheLayerB;
	std::vector<unsigned int> _renderPatternDataCacheRowNoLayerA;
	std::vector<unsigned int> _renderPatternDataCacheRowNoLayerB;
	std::vector<SpriteDisplayCacheEntry> _renderSpriteDisplayCache;
	unsigned int _renderSpriteDisplayCacheEntryCount;
	unsigned int _renderSpriteDisplayCacheCurrentIndex;
	bool _renderSpriteSearchComplete;
	bool _renderSpriteOverflow;
	unsigned int _renderSpriteNextAttributeTableEntryToRead;
	std::vector<SpriteCellDisplayCacheEntry> _renderSpriteDisplayCellCache;
	unsigned int _renderSpriteDisplayCellCacheEntryCount;
	unsigned int _renderSpriteDisplayCellCacheCurrentIndex;
	bool _renderSpriteDotOverflow;
	bool _renderSpriteDotOverflowPreviousLine;
	unsigned int _renderSpritePixelBufferDigitalRenderPlane;
	unsigned int _renderSpritePixelBufferAnalogRenderPlane;
	std::vector<SpritePixelBufferEntry> _spritePixelBuffer[renderSpritePixelBufferPlaneCount];
	bool _nonSpriteMaskCellEncountered;
	bool _renderSpriteMaskActive;
	bool _renderSpriteCollision;
	Data _renderVSRAMCachedRead;

	// Analog render data buffers
	unsigned int _drawingImageBufferPlane;
	volatile unsigned int _lastRenderedFrameToken;
	mutable ReadWriteLock _imageBufferLock[ImageBufferPlanes];
	unsigned char _imageBuffer[ImageBufferPlanes][ImageBufferHeight * ImageBufferWidth * 4];
	ImageBufferInfo _imageBufferInfo[ImageBufferPlanes][ImageBufferHeight * ImageBufferWidth];
	bool _imageBufferOddInterlaceFrame[ImageBufferPlanes];
	unsigned int _imageBufferLineCount[ImageBufferPlanes];
	unsigned int _imageBufferLineWidth[ImageBufferPlanes][ImageBufferHeight];
	unsigned int _imageBufferActiveScanPosXStart[ImageBufferPlanes][ImageBufferHeight];
	unsigned int _imageBufferActiveScanPosXEnd[ImageBufferPlanes][ImageBufferHeight];
	unsigned int _imageBufferActiveScanPosYStart[ImageBufferPlanes];
	unsigned int _imageBufferActiveScanPosYEnd[ImageBufferPlanes];
	mutable std::mutex _spriteBoundaryMutex[ImageBufferPlanes];
	mutable std::list<SpriteBoundaryLineEntry> _imageBufferSpriteBoundaryLines[ImageBufferPlanes];

	// DMA worker thread properties
	mutable std::mutex _workerThreadMutex; // Top-level, required in order to interact with state affecting DMA worker thread.
	std::condition_variable _workerThreadUpdate;
	std::condition_variable _workerThreadStopped;
	std::condition_variable _workerThreadIdle;
	bool _workerThreadActive;
	bool _workerThreadPaused;
	bool _bworkerThreadPaused;

	// DMA transfer registers
	//##FIX## Everything related to DMA transfers should be done in SC cycles, not MCLK
	// cycles.
	static const unsigned int dmaTransferReadTimeInMclkCycles = 16; // The number of mclk cycles required for a DMA operation to read a byte from the external bus
	bool _dmaTransferActive;
	bool _bdmaTransferActive;
	bool _dmaTransferReadDataCached;
	bool _bdmaTransferReadDataCached;
	Data _dmaTransferReadCache;
	Data _bdmaTransferReadCache;
	unsigned int _dmaTransferNextReadMclk;
	unsigned int _bdmaTransferNextReadMclk;
	unsigned int _dmaTransferLastTimesliceUsedReadDelay;
	unsigned int _bdmaTransferLastTimesliceUsedReadDelay;
	bool _dmaTransferInvalidPortWriteCached;
	bool _bdmaTransferInvalidPortWriteCached;
	unsigned int _dmaTransferInvalidPortWriteAddressCache;
	unsigned int _bdmaTransferInvalidPortWriteAddressCache;
	Data _dmaTransferInvalidPortWriteDataCache;
	Data _bdmaTransferInvalidPortWriteDataCache;
	volatile bool _dmaAdvanceUntilDMAComplete;

	// External interrupt settings
	bool _externalInterruptVideoTriggerPointPending;
	bool _bexternalInterruptVideoTriggerPointPending;
	unsigned int _externalInterruptVideoTriggerPointHCounter;
	unsigned int _bexternalInterruptVideoTriggerPointHCounter;
	unsigned int _externalInterruptVideoTriggerPointVCounter;
	unsigned int _bexternalInterruptVideoTriggerPointVCounter;

	// CE line masks
	unsigned int _ceLineMaskLowerDataStrobeInput;
	unsigned int _ceLineMaskUpperDataStrobeInput;
	unsigned int _ceLineMaskReadHighWriteLowInput;
	unsigned int _ceLineMaskAddressStrobeInput;
	unsigned int _ceLineMaskLowerDataStrobeOutput;
	unsigned int _ceLineMaskUpperDataStrobeOutput;
	unsigned int _ceLineMaskReadHighWriteLowOutput;
	unsigned int _ceLineMaskAddressStrobeOutput;
	unsigned int _ceLineMaskRMWCycleInProgress;
	unsigned int _ceLineMaskRMWCycleFirstOperation;
	unsigned int _ceLineMaskLWR;
	unsigned int _ceLineMaskUWR;
	unsigned int _ceLineMaskCAS0;
	unsigned int _ceLineMaskRAS0;
	unsigned int _ceLineMaskOE0;

	// Saved CE line state for Read-Modify-Write cycles
	mutable bool _lineLWRSavedStateRMW;
	mutable bool _lineUWRSavedStateRMW;
	mutable bool _lineCAS0SavedStateRMW;
	mutable bool _lineRAS0SavedStateRMW;
	mutable bool _lineOE0SavedStateRMW;
	bool _blineLWRSavedStateRMW;
	bool _blineUWRSavedStateRMW;
	bool _blineCAS0SavedStateRMW;
	bool _blineRAS0SavedStateRMW;
	bool _blineOE0SavedStateRMW;
};

#include "S315_5313.inl"
#endif
