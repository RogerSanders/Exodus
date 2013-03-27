/*--------------------------------------------------------------------------------------*\
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
\*--------------------------------------------------------------------------------------*/
#ifndef __S315_5313_H__
#define __S315_5313_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "TimedBuffers/TimedBuffers.pkg"
#include <vector>
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include "Device/Device.pkg"

class S315_5313 :public Device
{
public:
	//Constructors
	S315_5313(const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual ~S315_5313();

	//Line functions
	virtual unsigned int GetLineID(const std::wstring& lineName) const;
	virtual std::wstring GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual bool AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	//Clock source functions
	virtual unsigned int GetClockSourceID(const std::wstring& clockSourceName) const;
	virtual std::wstring GetClockSourceName(unsigned int clockSourceID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate);

	//Initialization functions
	virtual bool BuildDevice();
	virtual bool ValidateDevice();
	virtual void Initialize();
	//##TODO## Determine whether we actually need a reset function for this device. Does
	//the VDP have a reset line? If it does, we should create it, and have the handler for
	//that line call the reset function at the appropriate time. If it does not, we should
	//remove the reset function, and just implement the Initialize() function. Also note,
	//if a reset line is present, we have to verify what gets cleared by a reset.
	void Reset();
	virtual void BeginExecution();
	virtual void SuspendExecution();

	//Reference functions
	using Device::AddReference;
	virtual bool AddReference(const std::wstring& referenceName, IDevice* target);
	virtual bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	virtual bool AddReference(const std::wstring& referenceName, IClockSource* target);
	virtual bool RemoveReference(IDevice* target);
	virtual bool RemoveReference(IBusInterface* target);
	virtual bool RemoveReference(IClockSource* target);

	//Suspend functions
	virtual bool UsesTransientExecution() const;

	//Execute functions
	virtual UpdateMethod GetUpdateMethod() const;
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual bool SendNotifyBeforeExecuteCalled() const;
	virtual void NotifyBeforeExecuteCalled();
	virtual bool SendNotifyAfterExecuteCalled() const;
	virtual void NotifyAfterExecuteCalled();
	virtual void ExecuteTimeslice(double nanoseconds);
	virtual void ExecuteTimesliceTimingPointStep(unsigned int accessContext);
	virtual double GetNextTimingPointInDeviceTime(unsigned int& accessContext) const;
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//CE line state functions
	virtual unsigned int GetCELineID(const std::wstring& lineName, bool inputLine) const;
	virtual void SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const;
	unsigned int BuildCELine(unsigned int targetAddress, bool vdpIsSource, bool transparentAccess, bool currentLowerDataStrobe, bool currentUpperDataStrobe, bool operationIsWrite, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);

	//Savestate functions
	virtual bool GetScreenshot(IImage& targetImage) const;
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;
	virtual void LoadSettingsState(IHeirarchicalStorageNode& node);
	virtual void SaveSettingsState(IHeirarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHeirarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHeirarchicalStorageNode& node) const;

protected:
	//Window functions
	void CreateMenuHandlers();
	virtual void AddSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher);

private:
	//Enumerations
	enum CELineID;
	enum LineID;
	enum ClockID;
	enum Layer;
	enum AccessContext;

	//Structures
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
	struct PortMonitorEntry;
	struct SpriteMappingTableEntry;
	struct ImageBufferColorEntry;
	struct SpriteBoundaryLineEntry;

	//Typedefs
	typedef RandomTimeAccessBuffer<Data, unsigned int> RegBuffer;
	typedef RegBuffer::AccessTarget AccessTarget;
	typedef ITimedBufferInt::AccessTarget RAMAccessTarget;

	//Constants
	static const unsigned int registerCount = 24;
	static const unsigned int registerCountM4 = 11;
	static const unsigned int vramSize = 0x10000;
	static const unsigned int cramSize = 0x80;
	static const unsigned int vsramSize = 0x50;
	static const unsigned int spriteCacheSize = 0x140; //4 bytes cached per sprite, with 80 sprites max in H40 mode.
	static const unsigned int fifoBufferSize = 4;
	static const unsigned int statusRegisterMask = 0x03FF;

	//Render constants
	static const unsigned int renderDigitalBlockPixelSizeY = 8;
	static const VRAMRenderOp vramOperationsH32ActiveLine[171];
	static const VRAMRenderOp vramOperationsH32InactiveLine[171];
	static const VRAMRenderOp vramOperationsH40ActiveLine[210];
	static const VRAMRenderOp vramOperationsH40InactiveLine[210];
	static const InternalRenderOp internalOperationsH32[342];
	static const InternalRenderOp internalOperationsH40[420];

	//Interrupt settings
	static const unsigned int exintIPLLineState = 2;
	static const unsigned int hintIPLLineState = 4;
	static const unsigned int vintIPLLineState = 6;

	//View and menu classes
	class SettingsMenuHandler;
	class DebugMenuHandler;
	class VRAMViewModel;
	class PaletteViewModel;
	class ImageViewModel;
	class RegistersViewModel;
	class LayerRemovalViewModel;
	class DebugSettingsViewModel;
	class SettingsViewModel;
	class SpriteListViewModel;
	class SpriteListDetailsViewModel;
	class PortMonitorViewModel;
	class PortMonitorDetailsViewModel;
	class VRAMView;
	class PaletteView;
	class ImageView;
	class RegistersView;
	class LayerRemovalView;
	class DebugSettingsView;
	class SettingsView;
	class SpriteListView;
	class SpriteListDetailsView;
	class PortMonitorView;
	class PortMonitorDetailsView;
	friend class VRAMViewModel;
	friend class PaletteViewModel;
	friend class ImageViewModel;
	friend class RegistersViewModel;
	friend class LayerRemovalViewModel;
	friend class DebugSettingsViewModel;
	friend class SettingsViewModel;
	friend class SpriteListViewModel;
	friend class SpriteListDetailsViewModel;
	friend class PortMonitorViewModel;
	friend class PortMonitorDetailsViewModel;
	friend class VRAMView;
	friend class PaletteView;
	friend class ImageView;
	friend class RegistersView;
	friend class LayerRemovalView;
	friend class DebugSettingsView;
	friend class SettingsView;
	friend class SpriteListView;
	friend class SpriteListDetailsView;
	friend class PortMonitorView;
	friend class PortMonitorDetailsView;

private:
	//Line functions
	unsigned int GetNewIPLLineState();
	void UpdatePredictedLineStateChanges(IDeviceContext* callingDevice, double accessTime, unsigned int accessContext);
	void UpdateLineStateChangePrediction(unsigned int lineNo, unsigned int lineStateChangeData, bool& lineStateChangePending, unsigned int& lineStateChangeMCLKCountdown, double& lineStateChangeTime, bool lineStateChangePendingNew, unsigned int lineStateChangeMCLKCountdownNew, IDeviceContext* callingDevice, double accessTime, unsigned int accessContext);

	//DMA functions
	void DMAWorkerThread();

	//Rendering functions
	void RenderThread();
	void AdvanceRenderProcess(unsigned int mclkCyclesToAdvance);
	void UpdateDigitalRenderProcess(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings);
	void PerformInternalRenderOperation(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, const InternalRenderOp& nextOperation, int renderDigitalCurrentRow);
	void PerformVRAMRenderOperation(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, const VRAMRenderOp& nextOperation, int renderDigitalCurrentRow);
	void UpdateAnalogRenderProcess(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings);
	void DigitalRenderReadHscrollData(unsigned int screenRowNumber, unsigned int hscrollDataBase, bool hscrState, bool lscrState, unsigned int& layerAHscrollPatternDisplacement, unsigned int& layerBHscrollPatternDisplacement, unsigned int& layerAHscrollMappingDisplacement, unsigned int& layerBHscrollMappingDisplacement) const;
	void DigitalRenderReadVscrollData(unsigned int screenColumnNumber, bool vscrState, bool interlaceMode2Active, unsigned int& layerAVscrollPatternDisplacement, unsigned int& layerBVscrollPatternDisplacement, unsigned int& layerAVscrollMappingDisplacement, unsigned int& layerBVscrollMappingDisplacement) const;
	void DigitalRenderReadVscrollDataNew(unsigned int screenColumnNumber, unsigned int layerNumber, bool vscrState, bool interlaceMode2Active, unsigned int& layerVscrollPatternDisplacement, unsigned int& layerVscrollMappingDisplacement, Data& vsramReadCache) const;
	void DigitalRenderReadMappingDataPair(unsigned int screenRowNumber, unsigned int screenColumnNumber, bool interlaceMode2Active, unsigned int nameTableBaseAddress, unsigned int layerHscrollMappingDisplacement, unsigned int layerVscrollMappingDisplacement, unsigned int layerVscrollPatternDisplacement, unsigned int hszState, unsigned int vszState, Data& mappingDataEntry1, Data& mappingDataEntry2) const;
	void DigitalRenderReadPatternDataRow(unsigned int patternRowNumberNoFlip, unsigned int patternCellOffset, bool interlaceMode2Active, const Data& mappingData, Data& patternData) const;
	void DigitalRenderBuildSpriteList(unsigned int screenRowNumber, bool interlaceMode2Active, bool screenModeRS1Active, unsigned int& nextTableEntryToRead, bool& spriteSearchComplete, bool& spriteOverflow, unsigned int& spriteDisplayCacheEntryCount, std::vector<SpriteDisplayCacheEntry>& spriteDisplayCache) const;
	void DigitalRenderBuildSpriteCellList(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int spriteDisplayCacheIndex, unsigned int spriteTableBaseAddress, bool interlaceMode2Active, bool screenModeRS1Active, bool& spriteDotOverflow, SpriteDisplayCacheEntry& spriteDisplayCacheEntry, unsigned int& spriteCellDisplayCacheEntryCount, std::vector<SpriteCellDisplayCacheEntry>& spriteCellDisplayCache) const;
	unsigned int DigitalRenderReadPixelIndex(const Data& patternRow, bool horizontalFlip, unsigned int pixelIndex) const;
	void CalculateLayerPriorityIndex(unsigned int& layerIndex, bool& shadow, bool& highlight, bool shadowHighlightEnabled, bool spriteIsShadowOperator, bool spriteIsHighlightOperator, bool foundSpritePixel, bool foundLayerAPixel, bool foundLayerBPixel, bool prioritySprite, bool priorityLayerA, bool priorityLayerB) const;

	//Memory interface functions
	Data GetHVCounter() const;
	void ProcessCommandDataWriteFirstHalf(const Data& data);
	void ProcessCommandDataWriteSecondHalf(const Data& data);
	void TransparentRegisterSpecialUpdateFunction(unsigned int registerNo, const Data& data);
	void RegisterSpecialUpdateFunction(unsigned int mclkCycle, double accessTime, double accessDelay, IDeviceContext* caller, unsigned int accessContext, unsigned int registerNo, const Data& data);

	//Port monitor functions
	void RecordPortMonitorEntry(const PortMonitorEntry& entry);
	void ClearPortMonitorList();

	//Sprite list debugging functions
	SpriteMappingTableEntry GetSpriteMappingTableEntry(unsigned int entryNo) const;
	void SetSpriteMappingTableEntry(unsigned int entryNo, const SpriteMappingTableEntry& entry, bool useSeparatedData);

	//HV counter internal/linear conversion
	static unsigned int HCounterValueFromVDPInternalToLinear(const HScanSettings& hscanSettings, unsigned int hcounterCurrent);
	static unsigned int VCounterValueFromVDPInternalToLinear(const VScanSettings& vscanSettings, unsigned int vcounterCurrent, bool oddFlagSet);
	static unsigned int HCounterValueFromLinearToVDPInternal(const HScanSettings& hscanSettings, unsigned int hcounterCurrent);
	static unsigned int VCounterValueFromLinearToVDPInternal(const VScanSettings& vscanSettings, unsigned int vcounterCurrent, bool oddFlagSet);

	//Video scan settings functions
	//##TODO## Consider removing the RS0 parameter here. We only need the RS1 bit.
	static const HScanSettings& GetHScanSettings(bool screenModeRS0Active, bool screenModeRS1Active);
	static const VScanSettings& GetVScanSettings(bool screenModeV30Active, bool palModeActive, bool interlaceActive);

	//HV counter comparison functions
	static bool EventOccursWithinCounterRange(const HScanSettings& hscanSettings, unsigned int hcounterStart, unsigned int vcounterStart, unsigned int hcounterEnd, unsigned int vcounterEnd, unsigned int hcounterEventPos, unsigned int vcounterEventPos);
	static unsigned int GetPixelClockStepsBetweenHVCounterValues(bool advanceIfValuesMatch, const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterTarget, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterTarget);
	static unsigned int GetPixelClockStepsBetweenHCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterTarget);
	static unsigned int GetPixelClockStepsBetweenVCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterTarget);

	//HV counter advancement functions
	void BeginHVCounterAdvanceSessionFromCurrentState(HVCounterAdvanceSession& advanceSession);
	static bool AdvanceHVCounterSession(HVCounterAdvanceSession& advanceSession, unsigned int hcounterTarget, unsigned int vcounterTarget, bool advanceIfValuesMatch);
	static unsigned int AddStepsToHCounter(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterStepsToAdd);
	static unsigned int AddStepsToVCounter(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterStepsToAdd);
	static void AdvanceHVCounters(const HScanSettings& hscanSettings, unsigned int& hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool& oddFlagSet, unsigned int& vcounterCurrent, unsigned int pixelClockSteps);

	//Pixel clock functions
	//##TODO## Move these functions somewhere more appropriate
	//static unsigned int GetUpdatedVSRAMReadCacheIndex(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int vsramReadCacheIndexCurrent, unsigned int hcounterInitial, unsigned int vcounterInitial, unsigned int hcounterFinal, unsigned int vcounterFinal, bool screenModeRS0Current, bool screenModeRS1Current, unsigned int vscrollMode);
	static unsigned int GetPixelClockTicksUntilNextAccessSlot(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int hcounterCurrent, bool screenModeRS0Current, bool screenModeRS1Current, bool displayEnabled, unsigned int vcounterCurrent);
	static unsigned int GetPixelClockTicksForMclkTicks(const HScanSettings& hscanSettings, unsigned int mclkTicks, unsigned int hcounterCurrent, bool screenModeRS0Active, bool screenModeRS1Active, unsigned int& mclkTicksUnused);
	static unsigned int GetMclkTicksForPixelClockTicks(const HScanSettings& hscanSettings, unsigned int pixelClockTicks, unsigned int hcounterCurrent, bool screenModeRS0Active, bool screenModeRS1Active);

	//Access time functions
	unsigned int ConvertAccessTimeToMclkCount(double accessTime) const;
	double ConvertMclkCountToAccessTime(unsigned int mclkCount) const;

	//Processor state advancement functions
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

	//FIFO functions
	bool IsWriteFIFOEmpty() const;
	bool IsWriteFIFOFull() const;

	//##TODO## Mode 4 control functions

	//Mode 5 control functions
	bool ValidReadTargetInCommandCode() const;
	void M5ReadVRAM8Bit(const Data& address, Data& data, const RAMAccessTarget& accessTarget);
	void M5ReadCRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget);
	void M5ReadVSRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget);
	void M5WriteVRAM8Bit(const Data& address, const Data& data, const RAMAccessTarget& accessTarget);
	void M5WriteCRAM(const Data& address, const Data& data, const RAMAccessTarget& accessTarget);
	void M5WriteVSRAM(const Data& address, const Data& data, const RAMAccessTarget& accessTarget);

	//Status register functions
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

	//Raw register functions
	inline Data GetRegisterData(unsigned int location, const AccessTarget& accessTarget) const;
	inline void SetRegisterData(unsigned int location, const AccessTarget& accessTarget, const Data& data);

	//Mode 4 register functions
	inline bool M4GetVScrollingDisabled(const AccessTarget& accessTarget) const;	//0x00(0)
	inline void M4SetVScrollingDisabled(const AccessTarget& accessTarget, bool data);
	inline bool M4GetHScrollingDisabled(const AccessTarget& accessTarget) const;
	inline void M4SetHScrollingDisabled(const AccessTarget& accessTarget, bool data);
	inline bool M4GetColumnZeroMasked(const AccessTarget& accessTarget) const;
	inline void M4SetColumnZeroMasked(const AccessTarget& accessTarget, bool data);
	inline bool M4GetLineInterruptEnabled(const AccessTarget& accessTarget) const;
	inline void M4SetLineInterruptEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M4GetSpriteShift(const AccessTarget& accessTarget) const;
	inline void M4SetSpriteShift(const AccessTarget& accessTarget, bool data);
	inline bool M4GetM4Bit(const AccessTarget& accessTarget) const;
	inline void M4SetM4Bit(const AccessTarget& accessTarget, bool data);
	inline bool M4GetM2Bit(const AccessTarget& accessTarget) const;
	inline void M4SetM2Bit(const AccessTarget& accessTarget, bool data);
	inline bool M4GetReg0Bit0(const AccessTarget& accessTarget) const;
	inline void M4SetReg0Bit0(const AccessTarget& accessTarget, bool data);

	inline bool M4GetDisplayEnabled(const AccessTarget& accessTarget) const;	//0x01(1)
	inline void M4SetDisplayEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M4GetFrameInterruptEnabled(const AccessTarget& accessTarget) const;
	inline void M4SetFrameInterruptEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M4GetM1Bit(const AccessTarget& accessTarget) const;
	inline void M4SetM1Bit(const AccessTarget& accessTarget, bool data);
	inline bool M4GetM3Bit(const AccessTarget& accessTarget) const;
	inline void M4SetM3Bit(const AccessTarget& accessTarget, bool data);
	inline bool M4GetSpritesDoubleHeight(const AccessTarget& accessTarget) const;
	inline void M4SetSpritesDoubleHeight(const AccessTarget& accessTarget, bool data);
	inline bool M4GetSpriteZooming(const AccessTarget& accessTarget) const;
	inline void M4SetSpriteZooming(const AccessTarget& accessTarget, bool data);

	inline unsigned int M4GetNameTableBaseScroll(const AccessTarget& accessTarget) const;	//0x02(2)
	inline void M4SetNameTableBaseScroll(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M4GetAttributeTableBaseSprite(const AccessTarget& accessTarget) const;	//0x05(5)
	inline void M4SetAttributeTableBaseSprite(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M4GetPatternBaseSprite(const AccessTarget& accessTarget) const;	//0x06(6)
	inline void M4SetPatternBaseSprite(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M4GetBackdropColorIndex(const AccessTarget& accessTarget) const;	//0x07(7)
	inline void M4SetBackdropColorIndex(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int M4GetBackgroundScrollX(const AccessTarget& accessTarget) const;	//0x08(8)
	inline void M4SetBackgroundScrollX(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M4GetBackgroundScrollY(const AccessTarget& accessTarget) const;	//0x09(9)
	inline void M4SetBackgroundScrollY(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M4GetLineCounter(const AccessTarget& accessTarget) const;	//0x0A(10)
	inline void M4SetLineCounter(const AccessTarget& accessTarget, unsigned int data);

	//Mode 5 register functions
	inline bool M5GetHInterruptEnabled(const AccessTarget& accessTarget) const;	//0x00(0)
	inline void M5SetHInterruptEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M5GetPSEnabled(const AccessTarget& accessTarget) const;
	inline void M5SetPSEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M5GetHVCounterLatchEnabled(const AccessTarget& accessTarget) const;
	inline void M5SetHVCounterLatchEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M5GetDisplayEnabled(const AccessTarget& accessTarget) const;	//0x01(1)
	inline void M5SetDisplayEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M5GetVInterruptEnabled(const AccessTarget& accessTarget) const;
	inline void M5SetVInterruptEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M5GetDMAEnabled(const AccessTarget& accessTarget) const;
	inline void M5SetDMAEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M5GetV30CellModeEnabled(const AccessTarget& accessTarget) const;
	inline void M5SetV30CellModeEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M5GetMode5Enabled(const AccessTarget& accessTarget) const;
	inline void M5SetMode5Enabled(const AccessTarget& accessTarget, bool data);

	inline unsigned int M5GetNameTableBaseScrollA(const AccessTarget& accessTarget) const;	//0x02(2)
	inline void M5SetNameTableBaseScrollA(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M5GetNameTableBaseWindow(const AccessTarget& accessTarget) const;	//0x03(3)
	inline void M5SetNameTableBaseWindow(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M5GetNameTableBaseScrollB(const AccessTarget& accessTarget) const;	//0x04(4)
	inline void M5SetNameTableBaseScrollB(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M5GetNameTableBaseSprite(const AccessTarget& accessTarget) const;	//0x05(5)
	inline void M5SetNameTableBaseSprite(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int M5GetBackgroundColorPalette(const AccessTarget& accessTarget) const;	//0x07(7)
	inline void M5SetBackgroundColorPalette(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M5GetBackgroundColorIndex(const AccessTarget& accessTarget) const;
	inline void M5SetBackgroundColorIndex(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int M5GetHInterruptData(const AccessTarget& accessTarget) const;	//0x0A(10)
	inline void M5SetHInterruptData(const AccessTarget& accessTarget, unsigned int data);
	inline bool M5GetEXInterruptEnabled(const AccessTarget& accessTarget) const;	//0x0B(11)
	inline void M5SetEXInterruptEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M5GetVSCR(const AccessTarget& accessTarget) const;
	inline void M5SetVSCR(const AccessTarget& accessTarget, bool data);
	inline bool M5GetHSCR(const AccessTarget& accessTarget) const;
	inline void M5SetHSCR(const AccessTarget& accessTarget, bool data);
	inline bool M5GetLSCR(const AccessTarget& accessTarget) const;
	inline void M5SetLSCR(const AccessTarget& accessTarget, bool data);

	inline bool M5GetRS0(const AccessTarget& accessTarget) const;	//0x0C(12)
	inline void M5SetRS0(const AccessTarget& accessTarget, bool data);
	inline bool M5GetRS1(const AccessTarget& accessTarget) const;
	inline void M5SetRS1(const AccessTarget& accessTarget, bool data);
	inline bool M5GetShadowHighlightEnabled(const AccessTarget& accessTarget) const;
	inline void M5SetShadowHighlightEnabled(const AccessTarget& accessTarget, bool data);
	inline bool M5GetLSM1(const AccessTarget& accessTarget) const;
	inline void M5SetLSM1(const AccessTarget& accessTarget, bool data);
	inline bool M5GetLSM0(const AccessTarget& accessTarget) const;
	inline void M5SetLSM0(const AccessTarget& accessTarget, bool data);

	inline unsigned int M5GetHScrollDataBase(const AccessTarget& accessTarget) const;	//0x0D(13)
	inline void M5SetHScrollDataBase(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int M5GetAutoIncrementData(const AccessTarget& accessTarget) const;	//0x0F(15)
	inline void M5SetAutoIncrementData(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int M5GetVSZ(const AccessTarget& accessTarget) const;	//0x10(16)
	inline void M5SetVSZ(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M5GetHSZ(const AccessTarget& accessTarget) const;
	inline void M5SetHSZ(const AccessTarget& accessTarget, unsigned int data);
	inline bool M5GetVSZ1(const AccessTarget& accessTarget) const;
	inline void M5SetVSZ1(const AccessTarget& accessTarget, bool data);
	inline bool M5GetVSZ0(const AccessTarget& accessTarget) const;
	inline void M5SetVSZ0(const AccessTarget& accessTarget, bool data);
	inline bool M5GetHSZ1(const AccessTarget& accessTarget) const;
	inline void M5SetHSZ1(const AccessTarget& accessTarget, bool data);
	inline bool M5GetHSZ0(const AccessTarget& accessTarget) const;
	inline void M5SetHSZ0(const AccessTarget& accessTarget, bool data);

	inline bool M5GetWindowRightAligned(const AccessTarget& accessTarget) const;	//0x11(17)
	inline void M5SetWindowRightAligned(const AccessTarget& accessTarget, bool data);
	inline unsigned int M5GetWindowBasePointX(const AccessTarget& accessTarget) const;
	inline void M5SetWindowBasePointX(const AccessTarget& accessTarget, unsigned int data);
	inline bool M5GetWindowBottomAligned(const AccessTarget& accessTarget) const;	//0x12(18)
	inline void M5SetWindowBottomAligned(const AccessTarget& accessTarget, bool data);
	inline unsigned int M5GetWindowBasePointY(const AccessTarget& accessTarget) const;
	inline void M5SetWindowBasePointY(const AccessTarget& accessTarget, unsigned int data);

	inline unsigned int M5GetDMALengthCounter(const AccessTarget& accessTarget) const;	//0x13-0x14(19-20)
	inline void M5SetDMALengthCounter(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M5GetDMASourceAddress(const AccessTarget& accessTarget) const;	//0x15-0x17(21-23)
	inline void M5SetDMASourceAddress(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M5GetDMASourceAddressByte1(const AccessTarget& accessTarget) const;	//0x15(21)
	inline void M5SetDMASourceAddressByte1(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M5GetDMASourceAddressByte2(const AccessTarget& accessTarget) const;	//0x16(22)
	inline void M5SetDMASourceAddressByte2(const AccessTarget& accessTarget, unsigned int data);
	inline unsigned int M5GetDMASourceAddressByte3(const AccessTarget& accessTarget) const;	//0x17(23)
	inline void M5SetDMASourceAddressByte3(const AccessTarget& accessTarget, unsigned int data);
	inline bool M5GetDMD1(const AccessTarget& accessTarget) const;
	inline void M5SetDMD1(const AccessTarget& accessTarget, bool data);
	inline bool M5GetDMD0(const AccessTarget& accessTarget) const;
	inline void M5SetDMD0(const AccessTarget& accessTarget, bool data);

	//Window functions
	void OpenSpriteListDetailsView(unsigned int aspriteIndex);
	void OpenPortMonitorDetailsView(const PortMonitorEntry& aentry);

private:
	//Debug output
	bool outputPortAccessDebugMessages;
	bool outputTimingDebugMessages;
	bool outputRenderSyncMessages;
	bool outputInterruptDebugMessages;
	bool videoDisableRenderOutput;
	bool videoEnableSpriteBoxing;
	bool videoHighlightRenderPos;
	volatile bool videoSingleBuffering;
	volatile bool videoFixedAspectRatio;
	volatile bool videoShowStatusBar;
	volatile bool currentRenderPosOnScreen;
	volatile unsigned int currentRenderPosScreenX;
	volatile unsigned int currentRenderPosScreenY;
	bool videoShowBoundaryActiveImage;
	bool videoShowBoundaryActionSafe;
	bool videoShowBoundaryTitleSafe;

	//Menu handling
	SettingsMenuHandler* settingsMenuHandler;
	DebugMenuHandler* debugMenuHandler;

	//Bus interface
	IBusInterface* memoryBus;
	volatile bool busGranted;
	bool bbusGranted;
	volatile bool palModeLineState;
	bool bpalModeLineState;
	volatile unsigned int lineStateIPL;
	unsigned int blineStateIPL;
	bool busRequestLineState;
	bool bbusRequestLineState;

	//Embedded PSG device
	IDevice* psg;

	//Clock sources
	IClockSource* clockSourceCLK0;
	IClockSource* clockSourceCLK1;
	double clockMclkCurrent;
	double bclockMclkCurrent;

	//Layer removal settings
	bool enableLayerAHigh;
	bool enableLayerALow;
	bool enableLayerBHigh;
	bool enableLayerBLow;
	bool enableWindowHigh;
	bool enableWindowLow;
	bool enableSpriteHigh;
	bool enableSpriteLow;

	//Port monitor settings
	mutable boost::mutex portMonitorMutex;
	bool logStatusRegisterRead;
	bool logDataPortRead;
	bool logHVCounterRead;
	bool logControlPortWrite;
	bool logDataPortWrite;
	unsigned int portMonitorListSize;
	std::list<PortMonitorEntry> portMonitorList;
	std::list<PortMonitorEntry> bportMonitorList;

	//Physical registers and memory buffers
	mutable boost::mutex accessMutex; //Top-level, protects against concurrent interface access.
	mutable boost::mutex lineMutex; //Top level, must never be held during a blocking operation
	double lastAccessTime;
	RegBuffer reg;
	bool registerLocked[registerCount];
	ITimedBufferInt* vram;
	ITimedBufferInt* cram;
	ITimedBufferInt* vsram;
	ITimedBufferInt* spriteCache;
	Data status;
	Data bstatus;
	Data hcounter;
	Data bhcounter;
	Data vcounter;
	Data bvcounter;
	Data hcounterLatchedData;
	Data bhcounterLatchedData;
	Data vcounterLatchedData;
	Data bvcounterLatchedData;
	unsigned int hintCounter;
	unsigned int bhintCounter;
	bool vintPending;
	bool bvintPending;
	bool hintPending;
	bool bhintPending;
	bool exintPending;
	bool bexintPending;

	//Active register settings
	bool interlaceEnabled;
	bool binterlaceEnabled;
	bool interlaceDouble;
	bool binterlaceDouble;
	bool screenModeRS0;
	bool bscreenModeRS0;
	bool screenModeRS1;
	bool bscreenModeRS1;
	bool screenModeV30;
	bool bscreenModeV30;
	bool palMode;
	bool bpalMode;

	//Cached register settings
	bool hvCounterLatchEnabled;
	bool bhvCounterLatchEnabled;
	bool vintEnabled;
	bool bvintEnabled;
	bool hintEnabled;
	bool bhintEnabled;
	bool exintEnabled;
	bool bexintEnabled;
	unsigned int hintCounterReloadValue;
	unsigned int bhintCounterReloadValue;
	bool dmaEnabled;
	bool bdmaEnabled;
	bool dmd0;
	bool bdmd0;
	bool dmd1;
	bool bdmd1;
	unsigned int dmaLengthCounter;
	unsigned int bdmaLengthCounter;
	unsigned int dmaSourceAddressByte1;
	unsigned int bdmaSourceAddressByte1;
	unsigned int dmaSourceAddressByte2;
	unsigned int bdmaSourceAddressByte2;
	unsigned int dmaSourceAddressByte3;
	unsigned int bdmaSourceAddressByte3;
	unsigned int autoIncrementData;
	unsigned int bautoIncrementData;
	bool interlaceEnabledCached;
	bool binterlaceEnabledCached;
	bool interlaceDoubleCached;
	bool binterlaceDoubleCached;
	bool screenModeRS0Cached;
	bool bscreenModeRS0Cached;
	bool screenModeRS1Cached;
	bool bscreenModeRS1Cached;
	bool screenModeV30Cached;
	bool bscreenModeV30Cached;
	bool screenModeM5Cached;
	bool bscreenModeM5Cached;
	bool displayEnabledCached;
	bool bdisplayEnabledCached;
	unsigned int spriteAttributeTableBaseAddressDecoded;
	unsigned int bspriteAttributeTableBaseAddressDecoded;
	bool verticalScrollModeCached;
	bool bverticalScrollModeCached;
	bool cachedDMASettingsChanged;

	//FIFO buffer registers
	unsigned int fifoNextReadEntry;
	unsigned int bfifoNextReadEntry;
	unsigned int fifoNextWriteEntry;
	unsigned int bfifoNextWriteEntry;
	std::vector<FIFOBufferEntry> fifoBuffer;
	std::vector<FIFOBufferEntry> bfifoBuffer;
	Data readBuffer;
	Data breadBuffer;
	bool readDataAvailable;
	bool breadDataAvailable;
	bool readDataHalfCached;
	bool breadDataHalfCached;
	bool dmaFillOperationRunning;
	bool bdmaFillOperationRunning;
	unsigned int vsramReadCacheIndex;
	unsigned int bvsramReadCacheIndex;
	Data vsramLastRenderReadCache;
	Data bvsramLastRenderReadCache;

	//##TODO## Sprite buffer registers
	//We know from this: http://gendev.spritesmind.net/forum/viewtopic.php?t=666&postdays=0&postorder=asc&highlight=fifo&start=0
	//And this: http://mamedev.emulab.it/haze/2006/08/09/mirror-mirror/
	//That the sprite table is partially buffered internally. We need to implement that
	//sprite buffer here, and snoop on VRAM writes to update it, just like the real VDP
	//does.
	//##TODO## We know from testing that the sprite table cache simply caches the first
	//4 bytes of each 8-byte sprite entry. Implement that here as a simple data buffer.

	//Update state
	double currentTimesliceLength; //The length of the current timeslice, as passed to NotifyUpcomingTimeslice().
	double bcurrentTimesliceLength;
	double lastTimesliceMclkCyclesRemainingTime; //The unused portion of the last timeslice which wasn't be consumed by an mclk cycle. Note that this does NOT factor in time we ran over the last timeslice, as specified in lastTimesliceMclkCyclesOverrun.
	double blastTimesliceMclkCyclesRemainingTime;
	double currentTimesliceMclkCyclesRemainingTime; //Rolling value, the unused portion of the current timeslice which can't be consumed by an mclk cycle, as predicted during NotifyUpcomingTimeslice().
	double bcurrentTimesliceMclkCyclesRemainingTime;
	unsigned int currentTimesliceTotalMclkCycles; //The total number of mclk cycles added by the current timeslice, taking into account free from the last timeslice as included in the currentTimesliceMclkCyclesRemainingTime value. Note that this does NOT factor in time we ran over the last timeslice, as specified in lastTimesliceMclkCyclesOverrun.
	unsigned int lastTimesliceMclkCyclesOverrun; //The total number of mclk cycles we advanced past the end of the last timeslice
	unsigned int blastTimesliceMclkCyclesOverrun;
	double stateLastUpdateTime;
	double bstateLastUpdateTime;
	unsigned int stateLastUpdateMclk;
	unsigned int bstateLastUpdateMclk;
	unsigned int stateLastUpdateMclkUnused;
	unsigned int bstateLastUpdateMclkUnused;
	unsigned int stateLastUpdateMclkUnusedFromLastTimeslice;
	unsigned int bstateLastUpdateMclkUnusedFromLastTimeslice;

	//Interrupt line rollback data
	bool lineStateChangePendingVINT;
	bool blineStateChangePendingVINT;
	unsigned int lineStateChangeVINTMClkCountFromCurrent;
	unsigned int blineStateChangeVINTMClkCountFromCurrent;
	double lineStateChangeVINTTime;
	double blineStateChangeVINTTime;
	bool lineStateChangePendingHINT;
	bool blineStateChangePendingHINT;
	unsigned int lineStateChangeHINTMClkCountFromCurrent;
	unsigned int blineStateChangeHINTMClkCountFromCurrent;
	double lineStateChangeHINTTime;
	double blineStateChangeHINTTime;
	bool lineStateChangePendingEXINT;
	bool blineStateChangePendingEXINT;
	unsigned int lineStateChangeEXINTMClkCountFromCurrent;
	unsigned int blineStateChangeEXINTMClkCountFromCurrent;
	double lineStateChangeEXINTTime;
	double blineStateChangeEXINTTime;
	bool lineStateChangePendingINTAsserted;
	bool blineStateChangePendingINTAsserted;
	unsigned int lineStateChangeINTAssertedMClkCountFromCurrent;
	unsigned int blineStateChangeINTAssertedMClkCountFromCurrent;
	double lineStateChangeINTAssertedTime;
	double blineStateChangeINTAssertedTime;
	bool lineStateChangePendingINTNegated;
	bool blineStateChangePendingINTNegated;
	unsigned int lineStateChangeINTNegatedMClkCountFromCurrent;
	unsigned int blineStateChangeINTNegatedMClkCountFromCurrent;
	double lineStateChangeINTNegatedTime;
	double blineStateChangeINTNegatedTime;

	//Control port registers
	bool codeAndAddressRegistersModifiedSinceLastWrite;
	bool bcodeAndAddressRegistersModifiedSinceLastWrite;
	bool commandWritePending;
	bool bcommandWritePending;
	Data originalCommandAddress;
	Data boriginalCommandAddress;
	Data commandAddress;
	Data bcommandAddress;
	Data commandCode;
	Data bcommandCode;

	//Render thread properties
	mutable boost::mutex renderThreadMutex; //Top level, timesliceMutex child
	mutable boost::mutex timesliceMutex; //Child of renderThreadMutex
	boost::condition renderThreadUpdate;
	boost::condition renderThreadStopped;
	bool renderThreadActive;
	bool pendingRenderOperation;
	bool renderTimeslicePending;
	RegBuffer::Timeslice regTimeslice;
	ITimedBufferInt::Timeslice* vramTimeslice;
	ITimedBufferInt::Timeslice* cramTimeslice;
	ITimedBufferInt::Timeslice* vsramTimeslice;
	ITimedBufferInt::Timeslice* spriteCacheTimeslice;
	double remainingRenderTime;
	RegBuffer::Timeslice regTimesliceCopy;
	ITimedBufferInt::Timeslice* vramTimesliceCopy;
	ITimedBufferInt::Timeslice* cramTimesliceCopy;
	ITimedBufferInt::Timeslice* vsramTimesliceCopy;
	ITimedBufferInt::Timeslice* spriteCacheTimesliceCopy;
	RegBuffer::AdvanceSession regSession;
	ITimedBufferInt::AdvanceSession vramSession;
	ITimedBufferInt::AdvanceSession cramSession;
	ITimedBufferInt::AdvanceSession vsramSession;
	ITimedBufferInt::AdvanceSession spriteCacheSession;
	unsigned int mclkCycleRenderProgress;
	static const unsigned int layerPriorityLookupTableSize = 0x200;
	std::vector<unsigned int> layerPriorityLookupTable;

	static const unsigned int maxPendingRenderOperationCount = 4;
	volatile bool renderThreadLagging;
	boost::condition renderThreadLaggingStateChange;
	unsigned int pendingRenderOperationCount;
	std::list<TimesliceRenderInfo> timesliceRenderInfoList;
	std::list<RegBuffer::Timeslice> regTimesliceList;
	std::list<ITimedBufferInt::Timeslice*> vramTimesliceList;
	std::list<ITimedBufferInt::Timeslice*> cramTimesliceList;
	std::list<ITimedBufferInt::Timeslice*> vsramTimesliceList;
	std::list<ITimedBufferInt::Timeslice*> spriteCacheTimesliceList;
	std::list<TimesliceRenderInfo> timesliceRenderInfoListUncommitted;
	std::list<RegBuffer::Timeslice> regTimesliceListUncommitted;
	std::list<ITimedBufferInt::Timeslice*> vramTimesliceListUncommitted;
	std::list<ITimedBufferInt::Timeslice*> cramTimesliceListUncommitted;
	std::list<ITimedBufferInt::Timeslice*> vsramTimesliceListUncommitted;
	std::list<ITimedBufferInt::Timeslice*> spriteCacheTimesliceListUncommitted;

	//Digital render data buffers
	//##TODO## Separate the analog and digital renderers into their own classes. Our
	//single VDP superclass is getting too large to be manageable.
	//##TODO## Initialize all these registers correctly, and store/load them during the
	//savestate process.
	static const unsigned int cellBlockSizeH = 8;
	static const unsigned int cellsPerColumn = 2;
	static const unsigned int maxCellsPerRow = 42;
	static const unsigned int maxSpriteDisplayCacheSize = 20;
	static const unsigned int maxSpriteDisplayCellCacheSize = 40;
	static const unsigned int spritePixelBufferSize = maxCellsPerRow*8;
	static const unsigned int renderSpritePixelBufferPlaneCount = 2;
	unsigned int renderDigitalHCounterPos;
	unsigned int renderDigitalVCounterPos;
	unsigned int renderDigitalVCounterPosPreviousLine;
	unsigned int renderDigitalRemainingMclkCycles;
	bool renderDigitalScreenModeRS0Active;
	bool renderDigitalScreenModeRS1Active;
	bool renderDigitalScreenModeV30Active;
	bool renderDigitalInterlaceEnabledActive;
	bool renderDigitalInterlaceDoubleActive;
	bool renderDigitalPalModeActive;
	bool renderDigitalOddFlagSet;
	unsigned int renderDigitalMclkCycleProgress; //No backup needed
	unsigned int renderLayerAHscrollPatternDisplacement;
	unsigned int renderLayerBHscrollPatternDisplacement;
	unsigned int renderLayerAHscrollMappingDisplacement;
	unsigned int renderLayerBHscrollMappingDisplacement;
	unsigned int renderLayerAVscrollPatternDisplacement;
	unsigned int renderLayerBVscrollPatternDisplacement;
	unsigned int renderLayerAVscrollMappingDisplacement;
	unsigned int renderLayerBVscrollMappingDisplacement;
	std::vector<bool> renderWindowActiveCache;
	std::vector<Data> renderMappingDataCacheLayerA;
	std::vector<Data> renderMappingDataCacheLayerB;
	std::vector<Data> renderMappingDataCacheSprite;
	std::vector<Data> renderPatternDataCacheLayerA;
	std::vector<Data> renderPatternDataCacheLayerB;
	std::vector<Data> renderPatternDataCacheSprite;
	std::vector<SpriteDisplayCacheEntry> renderSpriteDisplayCache;
	unsigned int renderSpriteDisplayCacheEntryCount;
	unsigned int renderSpriteDisplayCacheCurrentIndex;
	bool renderSpriteSearchComplete;
	bool renderSpriteOverflow;
	unsigned int renderSpriteNextAttributeTableEntryToRead;
	std::vector<SpriteCellDisplayCacheEntry> renderSpriteDisplayCellCache;
	unsigned int renderSpriteDisplayCellCacheEntryCount;
	unsigned int renderSpriteDisplayCellCacheCurrentIndex;
	bool renderSpriteDotOverflow;
	bool renderSpriteDotOverflowPreviousLine;
	unsigned int renderSpritePixelBufferDigitalRenderPlane;
	unsigned int renderSpritePixelBufferAnalogRenderPlane;
	std::vector<SpritePixelBufferEntry> spritePixelBuffer[renderSpritePixelBufferPlaneCount];
	bool nonSpriteMaskCellEncountered;
	bool renderSpriteMaskActive;
	bool renderSpriteCollision;
	Data renderVSRAMCachedRead;

	//Analog render data buffers
	mutable boost::mutex imageBufferMutex;
	static const unsigned int imageBufferWidth = 512;
	static const unsigned int imageBufferHeight = 512;
	static const unsigned int imageBufferPlanes = 3;
	unsigned int drawingImageBufferPlane;
	volatile bool frameReadyInImageBuffer;
	unsigned char imageBuffer[imageBufferPlanes][imageBufferHeight * imageBufferWidth * 4];
	bool imageBufferOddInterlaceFrame[imageBufferPlanes];
	unsigned int imageBufferLineCount[imageBufferPlanes];
	unsigned int imageBufferLineWidth[imageBufferPlanes][imageBufferHeight];
	unsigned int imageBufferActiveScanPosXStart[imageBufferPlanes][imageBufferHeight];
	unsigned int imageBufferActiveScanPosXEnd[imageBufferPlanes][imageBufferHeight];
	unsigned int imageBufferActiveScanPosYStart[imageBufferPlanes];
	unsigned int imageBufferActiveScanPosYEnd[imageBufferPlanes];
	mutable boost::mutex spriteBoundaryMutex[imageBufferPlanes];
	mutable std::vector<SpriteBoundaryLineEntry> imageBufferSpriteBoundaryLines[imageBufferPlanes];

	//DMA worker thread properties
	mutable boost::mutex workerThreadMutex; //Top-level, required in order to interact with state affecting DMA worker thread.
	boost::condition workerThreadUpdate;
	boost::condition workerThreadStopped;
	boost::condition workerThreadIdle;
	bool workerThreadActive;
	bool workerThreadPaused;
	bool bworkerThreadPaused;

	//DMA transfer registers
	//##FIX## Everything related to DMA transfers should be done in SC cycles, not MCLK
	//cycles.
	static const unsigned int dmaTransferReadTimeInMclkCycles = 16; //The number of mclk cycles required for a DMA operation to read a byte from the external bus
	bool dmaTransferActive;
	bool bdmaTransferActive;
	bool dmaTransferReadDataCached;
	bool bdmaTransferReadDataCached;
	Data dmaTransferReadCache;
	Data bdmaTransferReadCache;
	unsigned int dmaTransferNextReadMclk;
	unsigned int bdmaTransferNextReadMclk;
	unsigned int dmaTransferLastTimesliceUsedReadDelay;
	unsigned int bdmaTransferLastTimesliceUsedReadDelay;
	bool dmaTransferInvalidPortWriteCached;
	bool bdmaTransferInvalidPortWriteCached;
	unsigned int dmaTransferInvalidPortWriteAddressCache;
	unsigned int bdmaTransferInvalidPortWriteAddressCache;
	Data dmaTransferInvalidPortWriteDataCache;
	Data bdmaTransferInvalidPortWriteDataCache;
	volatile bool dmaAdvanceUntilDMAComplete;

	//External interrupt settings
	bool externalInterruptVideoTriggerPointPending;
	bool bexternalInterruptVideoTriggerPointPending;
	unsigned int externalInterruptVideoTriggerPointHCounter;
	unsigned int bexternalInterruptVideoTriggerPointHCounter;
	unsigned int externalInterruptVideoTriggerPointVCounter;
	unsigned int bexternalInterruptVideoTriggerPointVCounter;

	//CE line masks
	unsigned int ceLineMaskLowerDataStrobeInput;
	unsigned int ceLineMaskUpperDataStrobeInput;
	unsigned int ceLineMaskReadHighWriteLowInput;
	unsigned int ceLineMaskAddressStrobeInput;
	unsigned int ceLineMaskLowerDataStrobeOutput;
	unsigned int ceLineMaskUpperDataStrobeOutput;
	unsigned int ceLineMaskReadHighWriteLowOutput;
	unsigned int ceLineMaskAddressStrobeOutput;
	unsigned int ceLineMaskRMWCycleInProgress;
	unsigned int ceLineMaskRMWCycleFirstOperation;
	unsigned int ceLineMaskLWR;
	unsigned int ceLineMaskUWR;
	unsigned int ceLineMaskCAS0;
	unsigned int ceLineMaskRAS0;
	unsigned int ceLineMaskOE0;

	//Saved CE line state for Read-Modify-Write cycles
	mutable bool lineLWRSavedStateRMW;
	mutable bool lineUWRSavedStateRMW;
	mutable bool lineCAS0SavedStateRMW;
	mutable bool lineRAS0SavedStateRMW;
	mutable bool lineOE0SavedStateRMW;
	bool blineLWRSavedStateRMW;
	bool blineUWRSavedStateRMW;
	bool blineCAS0SavedStateRMW;
	bool blineRAS0SavedStateRMW;
	bool blineOE0SavedStateRMW;
};

#include "S315_5313.inl"
#endif
