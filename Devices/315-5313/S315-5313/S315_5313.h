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

-Make everything in this core which needs a way of determining the current progression
through the horizontal and vertical scanlines use a different counting mechanism, rather
than using hcounter and vcounter values like they do currently. I would suggest relating
the hcounter back to the serial clock, and make the vertical counter a simple raster
count. This will eliminate the necessity to constantly convert from the "external"
hcounter and vcounter, where the counter values jump at various points, and will also
allow extra accuracy in the case of the horizontal counter, where some events occur at a
sub-pixel level of timing. It appears that SC is the true master clock signal which drives
the video functions of the VDP.
-Within the VDP core, switch to using SC cycles rather than MCLK cycles, since this is the
way the real VDP works, and it'll simplify things. Place MCLK cycles on the outer edge,
and provide functions to convert MCLK cycle counts into SC cycle counts where required.
Note that within a line, SC and the H32/H40 mode is sufficient to convert SC cycles back
to MCLK cycles, but within a frame, SC is not sufficient, because H32/H40 mode changes can
occur between successive lines within a frame.
-We know now that while the FIFO allows an entire word to be read or written to CRAM or
VSRAM on each access slot, VRAM only allows one byte to be transferred per access slot. We
need to update our FIFO implementation to support this. We also need to confirm through
hardware tests if a write to VRAM ends up in two separate byte-wide FIFO entries, or just
in a single FIFO entry which takes two access slots to complete.
-We need to determine when screen mode settings are latched for a line. Here are the
properties we know can be modified on a line-by-line basis: H32/H40, M4/M5, TMS9918A text
mode (R1,B7), Shadow/Highlight). These settings are most likely latched together at one
point in the raster, and this is when we need to base our start of line from. Note
however, that we don't know for sure yet that some or all of these settings don't take
effect immediately, even during a line. There's also the test register to consider.

Tests to run:
-We suspect that CRAM has a buffer register just like VSRAM, and that the CRAM flicker bug
occurs because a CRAM read cycle is stolen from the render process in order to perform the
CRAM write, and that the buffer is loaded with the data to be written, which then gets
read out by the render process as if it was the requested CRAM data. If this is the case
however, it seems logical there should be another CRAM flicker bug, this time caused by
reads from CRAM. If it is possible to perform a CRAM read outside of hblank, a CRAM read
would need to steal a cycle from the render process as well, most likely resulting in the
read value from CRAM replacing the pixel that is being rendered at that time. We should
perform a test on this behaviour, by constantly reading from CRAM with a colourful CRAM
palette loaded, but the first entry set to 0, and see if we get speckles on our display.

//#######################################Old todo#########################################
-Add savestate support
-We have seen a case where invalid sprite data caused flickering on the screen between
successive frames, even though all the VDP state was static between frames. The cause of
this flickering is completely unknown, and we have no theory as to why one frame would
render differently from the next. I can't find any test rom saved, so we'll have to do
tests on the hardware to try and reproduce this behaviour first.

-Check what happens with a data port write to trigger a DMA fill, when that write is held
in the FIFO. The FIFO will always be empty when a data port write to trigger a DMA fill
occurs, but if it's during active scan, that write might not be processed straight away.
In the case that this happens, I would expect that the VDP would not report that DMA is in
progress until the write leaves the FIFO, which would mean an app just monitoring the DMA
flag might think the DMA operation is complete before the write has even been processed.
This needs testing on the hardware. We should confirm that this can occur.
-An additional test for the interaction of DMA and the FIFO relates to the end of a DMA
operation. It is possible that all DMA writes are added to the back of the FIFO. This may
mean that when a DMA operation ends, and the M68000 bus is released if it was locked,
there are still some writes remaining in the FIFO which were added by DMA. Check the FIFO
empty flag in the status register at the completion of a DMA operation during active scan
to determine whether this is the case.
-Check what happens when the data port or control ports are written to during DMA fill
and copy operations. According to genvdp.txt, writing to the control or data ports during
a DMA fill corrupts VDP registers and VRAM somehow. Investigate this behaviour. In
addition, we need to check the behaviour of attempted data port reads during a DMA
operation.
-There are warnings in the official docs that DMA must only be enabled when a DMA
operation is about to be performed, and must be disabled afterwards, otherwise they
"cannot guarantee the operation". Perform some tests to see if anything breaks when DMA
is left enabled for normal port access.
-We know that fills can be done to CRAM and VSRAM, but the way in which this occurs isn't
clear. Test all possible quirks, such as writing to odd addresses, and verify the
behaviour.
-It is currently unknown whether DMA copy operations can be performed on CRAM and VSRAM.
Run some tests to determine whether this is possible.
-Note that further testing has also shown that the VRAM is NOT initialized to all 0. It
appears the VRAM is initialized with a logical, repeating pattern, but further tests must
be carried out to determine what this pattern is. I would suggest reading the entire
contents of VRAM, CRAM, and VSRAM back into RAM for analysis.
-The CRAM is initialized to 0x0EEE in all slots, except for the second entry in the
second palette line, which is initialized to 0x0CEE. This means byte 0x22 in the CRAM is
0x0C instead of 0x0E.
-The VSRAM is initialized to 0x07FF, except for the entry at 0x00, which is 0x07DF, and
the entry at 0x22, which is 0x07FB.

!!!!NOTE!!!!
-As per cfgm2_notes.pdf, "sprites are processed on the previous line than the one they
are shown on, and are not processed when the display is blanked". Also other important
comments about when major changes to screen mode take effect.
-According to a note by Charles Macdonald in newreg.htm, the first column of a layer
cannot be controlled in 2-cell vscroll mode. Instead, the column remains unaffected. This
is demonstrated in the Ecco II globe holder level, where sprites are actually used to
mask the first column. Scroll B is the layer in question in this case, but Layer A is
most likely affected as well. Note that currently we do NOT emulate this behaviour.
-Further on the 2-cell vscroll problem, I think this occurs simply because the designers
ran out of access slots to read the vscroll data. If this is true, the vscroll value
probably uses the last value read for the first column, rather than 0. This may mean that
the very last value in in the vscroll table will affect the first column. I also seem to
remember reading something like this on SpritesMind. Do more research into this behaviour.

Debug outputs changes:
-Modify the palette window to include palette row and column labels, as well as a popup
style window in the same style as the VRAM viewer, which shows the actual RGB palette
values that are being used to create it.
-Note that there's a bug in the layer removal process currently. When layer A is
disabled and shadow highlight mode is in use, the window layer ends up being used for
priority calculations for shadow highlight mode. See the Ecco II globe holder, code
SBFPWWJE for an example.
-Provide options to force all tiles in each layer to either high or low priority. This
can be used to bring hidden sprites into view for example. Also add an option to "mark"
sprites in some way, so that even transparent sprites are visible, and the boundaries of
all sprites on the screen are indicated clearly.
-Provide options to control which area of the screen to make visible. Have an option to
show all overscan regions to make each generated pixel visible.

Known inaccuracies:
-On the real hardware, changes to the H32/H40 cell mode take effect immediately, even
during a line, which can cause the current line to be corrupted. We latch the register
settings during hblank, for convenience, so that each line rendered maintains a rate of
3420 MCLK cycles per line.

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
	virtual unsigned int GetLineID(const wchar_t* lineName) const;
	virtual const wchar_t* GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual bool AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);

	//Clock source functions
	virtual unsigned int GetClockSourceID(const wchar_t* clockSourceName) const;
	virtual const wchar_t* GetClockSourceName(unsigned int clockSourceID) const;
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
	virtual bool AddReference(const wchar_t* referenceName, IDevice* target);
	virtual bool AddReference(const wchar_t* referenceName, IBusInterface* target);
	virtual bool AddReference(const wchar_t* referenceName, IClockSource* target);
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
	virtual unsigned int GetCELineID(const wchar_t* lineName, bool inputLine) const;
	virtual void SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller) const;
	unsigned int BuildCELine(unsigned int targetAddress, bool vdpIsSource, bool currentLowerDataStrobe, bool currentUpperDataStrobe, bool operationIsWrite, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const;

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);

protected:
	//Window functions
	virtual void AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher);

private:
	//Enumerations
	enum CELineID
	{
		CELINE_LDS = 1,
		CELINE_UDS,
		CELINE_RW,
		CELINE_AS,
		CELINE_RMWCYCLEINPROGRESS,
		CELINE_RMWCYCLEFIRSTOPERATION,
		CELINE_LWR,
		CELINE_UWR,
		CELINE_CAS0,
		CELINE_RAS0,
		CELINE_OE0
	};
	enum LineID
	{
		LINE_IPL = 1,
		LINE_INT,
		LINE_INTAK,
		LINE_BR,
		LINE_BG,
		LINE_PAL
	};
	enum ClockID
	{
		CLOCK_MCLK = 1
	};
	enum Event;
	enum Layer;
	enum AccessContext;

	//Structures
	struct HScanSettings;
	struct VScanSettings;
	//##TODO## Remove this old render structure
	//struct LineRenderSettings;
	struct EventProperties
	{
		Event event;
		unsigned int mclkCycleCounter;
		unsigned int hcounter;
		unsigned int vcounter;
	};
	struct TimesliceRenderInfo;
	struct SpriteDisplayCacheEntry;
	struct SpriteCellDisplayCacheEntry;
	struct SpritePixelBufferEntry;
	struct RenderOp;
	struct FIFOBufferEntry;

	//Typedefs
	typedef RandomTimeAccessBuffer<Data, unsigned int> RegBuffer;
	typedef RegBuffer::AccessTarget AccessTarget;
	typedef ITimedBufferInt::AccessTarget RAMAccessTarget;

	//Constants
	static const unsigned int registerCount = 24;
	static const unsigned int vramSize = 0x10000;
	static const unsigned int cramSize = 0x80;
	static const unsigned int vsramSize = 0x50;
	static const unsigned int spriteCacheSize = 0x140; //4 bytes cached per sprite, with 80 sprites max in H40 mode.
	static const unsigned int fifoBufferSize = 4;
	static const unsigned int statusRegisterMask = 0x03FF;

	//Render constants
	static const unsigned int renderDigitalBlockPixelSizeY = 8;
	static const RenderOp operationsH32[];
	static const RenderOp operationsH40[];

	//Interrupt settings
	static const unsigned int exintIPLLineState = 2;
	static const unsigned int hintIPLLineState = 4;
	static const unsigned int vintIPLLineState = 6;

	//View and menu classes
	class DebugMenuHandler;
	class VRAMViewModel;
	class PaletteViewModel;
	class ImageViewModel;
	class RegistersViewModel;
	class VRAMView;
	class PaletteView;
	class ImageView;
	class RegistersView;
	friend class VRAMViewModel;
	friend class PaletteViewModel;
	friend class ImageViewModel;
	friend class RegistersViewModel;
	friend class VRAMView;
	friend class PaletteView;
	friend class ImageView;
	friend class RegistersView;

private:
	//Line functions
	unsigned int GetNewIPLLineState();

	//Execute functions
//	void RenderThread();
//	void RenderFrame();
//	void RenderPrefetchLineForSprites(const AccessTarget& accessTarget);
//	void RenderBorderLine(const AccessTarget& accessTarget, unsigned int renderBufferLineNo, unsigned int sectionLineNo);
//	void RenderActiveLine(const AccessTarget& accessTarget, unsigned int renderBufferLineNo, unsigned int sectionLineNo);
	void RenderThreadNew();
	void AdvanceRenderProcess(unsigned int mclkCyclesToAdvance);
	void UpdateDigitalRenderProcess(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings);
	void UpdateAnalogRenderProcess(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings);
	void DigitalRenderReadHscrollData(unsigned int screenRowNumber, unsigned int hscrollDataBase, bool hscrState, bool lscrState, unsigned int& layerAHscrollPatternDisplacement, unsigned int& layerBHscrollPatternDisplacement, unsigned int& layerAHscrollMappingDisplacement, unsigned int& layerBHscrollMappingDisplacement) const;
	void DigitalRenderReadVscrollData(unsigned int screenColumnNumber, bool vscrState, bool interlaceMode2Active, unsigned int& layerAVscrollPatternDisplacement, unsigned int& layerBVscrollPatternDisplacement, unsigned int& layerAVscrollMappingDisplacement, unsigned int& layerBVscrollMappingDisplacement) const;
	void DigitalRenderReadVscrollDataNew(unsigned int screenColumnNumber, unsigned int layerNumber, bool vscrState, bool interlaceMode2Active, unsigned int& layerVscrollPatternDisplacement, unsigned int& layerVscrollMappingDisplacement, Data& vsramReadCache) const;
	void DigitalRenderReadMappingDataPair(unsigned int screenRowNumber, unsigned int screenColumnNumber, bool interlaceMode2Active, unsigned int nameTableBaseAddress, unsigned int layerHscrollMappingDisplacement, unsigned int layerVscrollMappingDisplacement, unsigned int layerVscrollPatternDisplacement, unsigned int hszState, unsigned int vszState, Data& mappingDataEntry1, Data& mappingDataEntry2) const;
	void DigitalRenderReadPatternDataRow(unsigned int patternRowNumberNoFlip, unsigned int patternCellOffset, bool interlaceMode2Active, const Data& mappingData, Data& patternData) const;
	void DigitalRenderBuildSpriteList(unsigned int screenRowNumber, bool interlaceMode2Active, bool screenModeRS1Active, unsigned int& nextTableEntryToRead, bool& spriteSearchComplete, bool& spriteOverflow, unsigned int& spriteDisplayCacheEntryCount, std::vector<SpriteDisplayCacheEntry>& spriteDisplayCache) const;
	void DigitalRenderBuildSpriteCellList(unsigned int spriteDisplayCacheIndex, unsigned int spriteTableBaseAddress, bool interlaceMode2Active, bool screenModeRS1Active, bool& spriteDotOverflow, SpriteDisplayCacheEntry& spriteDisplayCacheEntry, unsigned int& spriteCellDisplayCacheEntryCount, std::vector<SpriteCellDisplayCacheEntry>& spriteCellDisplayCache) const;
	unsigned int DigitalRenderReadPixelIndex(const Data& patternRow, bool horizontalFlip, unsigned int pixelIndex) const;
//	void RenderColumnBlockPair(unsigned int columnNumber, unsigned int scrollValueDisplacement, const Data& mappingDataCell1, const Data& mappingDataCell2, const Data& patternDataCell1, const Data& patternDataCell2, std::vector<unsigned int>& outputPaletteLines, std::vector<unsigned int>& outputPaletteEntries, unsigned int& currentRenderPixel) const;

	void AdvanceAnalogRenderProcess(unsigned int pixelsToOutput);
	void CalculateLayerPriorityIndex(unsigned int& layerIndex, bool& shadow, bool& highlight, bool shadowHighlightEnabled, bool spriteIsShadowOperator, bool spriteIsHighlightOperator, bool foundSpritePixel, bool foundLayerAPixel, bool foundLayerBPixel, bool prioritySprite, bool priorityLayerA, bool priorityLayerB) const;
//	void WriteColorValueToImageBuffer(unsigned int paletteLine, unsigned int paletteEntry, bool shadow, bool highlight, unsigned int xpos, unsigned int ypos);
//	void ReadMappingDataPair(Data& mappingDataEntry1, Data& mappingDataEntry2, unsigned int nameTableBaseAddress, unsigned int mappingColumnNumber, unsigned int mappingRowNumber, unsigned int screenSizeModeH, unsigned int screenSizeModeV) const;
//	void ReadPatternDataRow(const LineRenderSettings& renderSettings, Data& patternData, const Data& mappingData, unsigned int patternRowNumber) const;
//	void RenderColumnBlockPair(unsigned int columnNumber, unsigned int scrollValueDisplacement, const Data& mappingDataCell1, const Data& mappingDataCell2, const Data& patternDataCell1, const Data& patternDataCell2, std::vector<unsigned int>& outputPaletteLines, std::vector<unsigned int>& outputPaletteEntries, unsigned int& currentRenderPixel) const;
	void DMAWorkerThread();

	//Event functions
	void ExecuteEvent(EventProperties event, double accessTime, unsigned int ahcounter, unsigned int avcounter, bool ascreenModeRS0, bool ascreenModeRS1, bool ascreenModeV30, bool apalMode, bool ainterlaceEnabled);
//	void GetNextEvent(unsigned int currentMclkCycleCounter, bool timingPointsOnly, unsigned int currentHIntCounter, unsigned int currentPosHCounter, unsigned int currentPosVCounter, EventProperties& nextEvent) const;
	void GetNextEvent(unsigned int currentMclkCycleCounter, bool timingPointsOnly, unsigned int currentHIntCounter, unsigned int currentPosHCounter, unsigned int currentPosVCounter, EventProperties& nextEvent, bool& eventOddFlagSet, bool& eventInterlaceEnabled, bool& eventInterlaceDouble, bool& eventPalMode, bool& eventScreenModeV30, bool& eventScreenModeRS0, bool& eventScreenModeRS1, bool eventInterlaceEnabledNew, bool eventInterlaceDoubleNew, bool eventPalModeNew, bool eventScreenModeV30New, bool eventScreenModeRS0New, bool eventScreenModeRS1New) const;
	static bool EventOccursWithinCounterRange(const HScanSettings& hscanSettings, unsigned int hcounterStart, unsigned int vcounterStart, unsigned int hcounterEnd, unsigned int vcounterEnd, unsigned int hcounterEventPos, unsigned int vcounterEventPos);

	//Port functions
	Data GetHVCounter() const;
	void ProcessCommandDataWriteFirstHalf(const Data& data);
	void ProcessCommandDataWriteSecondHalf(const Data& data);
	void RegisterSpecialUpdateFunction(unsigned int mclkCycle, double accessTime, double accessDelay, IDeviceContext* caller, unsigned int accessContext, unsigned int registerNo, const Data& data);

	bool ValidReadTargetInCommandCode() const;
//	bool ReadVideoMemory(unsigned int mclkCycle, Data& data);
//	void WriteVideoMemory(unsigned int mclkCycle, const Data& data);

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
	static unsigned int GetPixelClockStepsBetweenHVCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterTarget, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterTarget);
	static unsigned int GetPixelClockStepsBetweenHCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterTarget);
	static unsigned int GetPixelClockStepsBetweenVCounterValues(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterTarget);

	//HV counter advancement functions
	static unsigned int AddStepsToHCounter(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, unsigned int hcounterStepsToAdd);
	static unsigned int AddStepsToVCounter(const HScanSettings& hscanSettings, unsigned int hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool oddFlagSet, unsigned int vcounterCurrent, unsigned int vcounterStepsToAdd);
	static void AdvanceHVCounters(const HScanSettings& hscanSettings, unsigned int& hcounterCurrent, const VScanSettings& vscanSettings, bool interlaceIsEnabled, bool& oddFlagSet, unsigned int& vcounterCurrent, unsigned int pixelClockSteps);

	//Pixel clock functions
	//##TODO## Move these functions somewhere more appropriate
	static unsigned int GetUpdatedVSRAMReadCacheIndex(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int vsramReadCacheIndexCurrent, unsigned int hcounterInitial, unsigned int vcounterInitial, unsigned int hcounterFinal, unsigned int vcounterFinal, bool screenModeRS0Current, bool screenModeRS1Current, unsigned int vscrollMode);
	static unsigned int GetPixelClockTicksUntilNextVSRAMRead(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int hcounterCurrent, bool screenModeRS0Current, bool screenModeRS1Current, unsigned int vcounterCurrent);
	static unsigned int GetPixelClockTicksUntilNextAccessSlot(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int hcounterCurrent, bool screenModeRS0Current, bool screenModeRS1Current, bool displayEnabled, unsigned int vcounterCurrent);
	static unsigned int GetPixelClockTicksForMclkTicks(const HScanSettings& hscanSettings, unsigned int mclkTicks, unsigned int hcounterCurrent, bool screenModeRS0Active, bool screenModeRS1Active, unsigned int& mclkTicksUnused);
	static unsigned int GetMclkTicksForPixelClockTicks(const HScanSettings& hscanSettings, unsigned int pixelClockTicks, unsigned int hcounterCurrent, bool screenModeRS0Active, bool screenModeRS1Active);

	//Access time functions
	unsigned int ConvertAccessTimeToMclkCount(double accessTime) const;
	double ConvertMclkCountToAccessTime(unsigned int mclkCount) const;

	//Processor state advancement functions
	void UpdateInternalState(unsigned int mclkCyclesTarget, bool checkFifoStateBeforeUpdate, bool stopWhenFifoEmpty, bool stopWhenFifoFull, bool stopWhenFifoNotFull, bool stopWhenReadDataAvailable, bool stopWhenNoDMAOperationInProgress, bool allowAdvancePastCycleTarget);
	bool AdvanceProcessorState(unsigned int mclkCyclesTarget, bool stopAtNextAccessSlot, bool allowAdvancePastTargetForAccessSlot);
	bool AdvanceProcessorStateNew(unsigned int mclkCyclesTarget, bool stopAtNextAccessSlot, bool allowAdvancePastTargetForAccessSlot);
	void PerformReadCacheOperation();
	void PerformFIFOWriteOperation();
	void PerformDMACopyOperation();
	void PerformDMAFillOperation();
	void CacheDMATransferReadData(unsigned int mclkTime);
	void PerformDMATransferOperation();
	void AdvanceDMAState();
	bool TargetProcessorStateReached(bool stopWhenFifoEmpty, bool stopWhenFifoFull, bool stopWhenFifoNotFull, bool stopWhenReadDataAvailable, bool stopWhenNoDMAOperationInProgress);
	void AdvanceStatusRegisterAndHVCounter(unsigned int pixelClockSteps);
	void AdvanceStatusRegisterAndHVCounterWithCurrentSettings(unsigned int pixelClockSteps);
	double GetProcessorStateTime() const;
	unsigned int GetProcessorStateMclkCurrent() const;

	//FIFO functions
	bool IsWriteFIFOEmpty() const;
	bool IsWriteFIFOFull() const;

	//##TODO## Mode 4 control functions

	//Mode 5 control functions
//	void M5ReadVRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget);
	void M5ReadVRAM8Bit(const Data& address, Data& data, const RAMAccessTarget& accessTarget);
	void M5ReadCRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget);
	void M5ReadVSRAM(const Data& address, Data& data, const RAMAccessTarget& accessTarget);
//	void M5WriteVRAM(const Data& address, const Data& data, const RAMAccessTarget& accessTarget);
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

private:
	//##DEBUG##
	bool outputTestDebugMessages;
	bool outputTimingDebugMessages;
	bool outputHINTDebugMessages;

	//Menu handling
	DebugMenuHandler* menuHandler;

	//Bus interface
	IBusInterface* memoryBus;
	volatile bool busGranted;
	bool bbusGranted;
	volatile bool palModeLineState;
	bool bpalModeLineState;
	volatile unsigned int lineStateIPL;
	unsigned int blineStateIPL;
	bool interruptPendingHint;
	bool binterruptPendingHint;
	bool interruptPendingVint;
	bool binterruptPendingVint;
	bool interruptPendingEXint;
	bool binterruptPendingEXint;

	//Embedded PSG device
	IDevice* psg;

	//Clock sources
	IClockSource* clockSourceCLK0;
	IClockSource* clockSourceCLK1;
	double clockMclkCurrent;
	double bclockMclkCurrent;

	//Physical registers and memory buffers
	mutable boost::mutex accessMutex; //Top-level, protects against concurrent interface access.
	mutable boost::mutex lineMutex; //Top level, must never be held during a blocking operation
	double lastAccessTime;
	RegBuffer reg;
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
	bool hvCounterLatched;
	bool bhvCounterLatched;
	unsigned int hintCounter;
	unsigned int bhintCounter;
	bool vintPending;
	bool bvintPending;
	bool hintPending;
	bool bhintPending;
	bool exintPending;
	bool bexintPending;

	//Cached register settings
	bool hvCounterLatchEnabled;
	bool bhvCounterLatchEnabled;
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
	double lastTimesliceMclkCyclesRemainingTime; //The unused portion of the last timeslice which wasn't be consumed by an mclk cycle. Note that this does NOT factor in time we ran over the last timeslice, as specified in lastTimesliceMclkCyclesOverrun.
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
	unsigned int renderSpriteNextSpriteRow;
	Data renderVSRAMCachedRead;

	//Analog render data buffers
	//##TODO## Complete this list, and implement these buffers.
	struct ImageBufferColorEntry
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
	static const unsigned int imageBufferWidth = 512;
	static const unsigned int imageBufferHeight = 512;
	std::vector<ImageBufferColorEntry> imageBuffer;

	//Image buffer
	//##TODO## Fix up these dimensions
	mutable boost::mutex imageBufferMutex; //##TODO## Use this properly
	static const unsigned int imageWidth = 512;
	static const unsigned int imageHeight = 512;
	static const unsigned int imageBufferPlanes = 3;
	unsigned char image[imageBufferPlanes][imageHeight * imageWidth * 4];
	unsigned int drawingImageBufferPlane;

	//DMA worker thread properties
	mutable boost::mutex workerThreadMutex; //Top-level, required in order to interact with state affecting DMA worker thread.
	boost::condition workerThreadUpdate;
	boost::condition workerThreadStopped;
	boost::condition workerThreadIdle;
	bool workerThreadActive;
	bool workerThreadPaused;
	bool bworkerThreadPaused;

	//DMA transfer registers
	//##TODO## Calculate on the hardware how many mclk cycles it takes a DMA transfer
	//update step to read data from memory over the external bus. We can do this by
	//measuring how quickly the data is transferred back to back while the display is
	//disabled. We need to know the actual delay time between successive reads, so
	//that we don't perform DMA transfers too quickly when the display is disabled, or
	//not in an active display region.
	//##FIX## Hardware tests have shown it takes 4 SC cycles to perform an external read,
	//but since the serial clock changes, we need to calculate the read time in SC cycles,
	//not MCLK cycles.
	//##TODO## Calculate the relative synchronization of the DMA transfer read slots to
	//the HCounter. The very fact that the DMA bitmap demos work show that these slots do
	//in fact exist, otherwise it wouldn't be possible to achieve a stable display.
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
	unsigned int bdmaTransferInvalidAddressCache;
	Data dmaTransferInvalidPortWriteDataCache;
	Data bdmaTransferInvalidDataCache;
	volatile bool dmaAdvanceUntilDMAComplete;

	//Interrupt settings
	//##TODO## Add rollback settings for these values
	bool externalInterruptPending;
	unsigned int externalInterruptHCounter;
	unsigned int externalInterruptVCounter;

	//Event handling
	bool eventsProcessedForTimeslice;
	boost::condition eventProcessingCompleted;
	mutable EventProperties nextEventToExecute;
	EventProperties bnextEventToExecute;
	mutable double nextEventToExecuteTime;
	double bnextEventToExecuteTime;
	//##TODO## Implement event snapshots
	bool eventSnapshotSaved;
	bool beventSnapshotSaved;
	bool eventSnapshotScreenModeRS0;
	bool beventSnapshotScreenModeRS0;
	bool eventSnapshotScreenModeRS1;
	bool beventSnapshotScreenModeRS1;
	bool eventSnapshotScreenModeV30;
	bool beventSnapshotScreenModeV30;
	bool eventSnapshotPalMode;
	bool beventSnapshotPalMode;
	bool eventSnapshotInterlaceEnabled;
	bool beventSnapshotInterlaceEnabled;

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
