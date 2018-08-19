#ifndef __IS315_5313_H__
#define __IS315_5313_H__
#include "GenericAccess/GenericAccess.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "TimedBuffers/TimedBuffers.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include <list>
using namespace MarshalSupport::Operators;

class IS315_5313 :public virtual IGenericAccess
{
public:
	// Enumerations
	enum class IS315_5313DataSource;
	enum class PixelSource;

	// Structures
	struct RegisterDataContext;
	struct FIFOEntryDataContext;
	struct PortMonitorEntry;
	struct SpriteMappingTableEntry;
	struct DecodedPaletteColorEntry;
	struct SpriteBoundaryLineEntry;
	struct ImageBufferInfo;

	// Constants
	static const unsigned int RegisterCount = 24;
	static const unsigned int RegisterCountM4 = 11;
	static const unsigned int VramSize = 0x10000;
	static const unsigned int CramSize = 0x80;
	static const unsigned int VsramSize = 0x50; // Note that this has been observed to be 0x80 bytes on the Genesis 3
	static const unsigned int SpriteCacheSize = 0x140; // 4 bytes cached per sprite, with 80 sprites max in H40 mode.
	static const unsigned int FifoBufferSize = 4;
	static const unsigned int StatusRegisterMask = 0x03FF;
	static const unsigned int CellsPerColumn = 2;
	static const unsigned int ImageBufferWidth = 512;
	static const unsigned int ImageBufferHeight = 512;
	static const unsigned int ImageBufferPlanes = 3;

public:
	// Interface version functions
	static inline unsigned int ThisIS315_5313Version() { return 1; }
	virtual unsigned int GetIS315_5313Version() const = 0;

	// Device access functions
	virtual IDevice* GetDevice() = 0;

	// External buffer functions
	virtual void LockExternalBuffers() = 0;
	virtual void UnlockExternalBuffers() = 0;
	virtual ITimedBufferInt* GetVRAMBuffer() const = 0;
	virtual ITimedBufferInt* GetCRAMBuffer() const = 0;
	virtual ITimedBufferInt* GetVSRAMBuffer() const = 0;
	virtual ITimedBufferInt* GetSpriteCacheBuffer() const = 0;

	// Image buffer functions
	virtual unsigned int GetImageLastRenderedFrameToken() const = 0;
	virtual unsigned int GetImageCompletedBufferPlaneNo() const = 0;
	virtual unsigned int GetImageDrawingBufferPlaneNo() const = 0;
	virtual void LockImageBufferData(unsigned int planeNo) = 0;
	virtual void UnlockImageBufferData(unsigned int planeNo) = 0;
	virtual const unsigned char* GetImageBufferData(unsigned int planeNo) const = 0;
	virtual const ImageBufferInfo* GetImageBufferInfo(unsigned int planeNo) const = 0;
	virtual const ImageBufferInfo* GetImageBufferInfo(unsigned int planeNo, unsigned int lineNo, unsigned int pixelNo) const = 0;
	virtual bool GetImageBufferOddInterlaceFrame(unsigned int planeNo) const = 0;
	virtual unsigned int GetImageBufferLineCount(unsigned int planeNo) const = 0;
	virtual unsigned int GetImageBufferLineWidth(unsigned int planeNo, unsigned int lineNo) const = 0;
	virtual void GetImageBufferActiveScanPosX(unsigned int planeNo, unsigned int lineNo, unsigned int& startPosX, unsigned int& endPosX) const = 0;
	virtual void GetImageBufferActiveScanPosY(unsigned int planeNo, unsigned int& startPosY, unsigned int& endPosY) const = 0;

	// Rendering functions
	virtual void DigitalRenderReadHscrollData(unsigned int screenRowNumber, unsigned int hscrollDataBase, bool hscrState, bool lscrState, unsigned int& layerAHscrollPatternDisplacement, unsigned int& layerBHscrollPatternDisplacement, unsigned int& layerAHscrollMappingDisplacement, unsigned int& layerBHscrollMappingDisplacement) const = 0;
	virtual void DigitalRenderReadVscrollData(unsigned int screenColumnNumber, unsigned int layerNumber, bool vscrState, bool interlaceMode2Active, unsigned int& layerVscrollPatternDisplacement, unsigned int& layerVscrollMappingDisplacement, Data& vsramReadCache) const = 0;
	virtual unsigned int CalculatePatternDataRowNumber(unsigned int patternRowNumberNoFlip, bool interlaceMode2Active, const Data& mappingData) const = 0;
	virtual unsigned int CalculatePatternDataRowAddress(unsigned int patternRowNumberNoFlip, unsigned int patternCellOffset, bool interlaceMode2Active, const Data& mappingData) const = 0;
	virtual void CalculateEffectiveCellScrollSize(unsigned int hszState, unsigned int vszState, unsigned int& effectiveScrollWidth, unsigned int& effectiveScrollHeight) const = 0;
	virtual DecodedPaletteColorEntry ReadDecodedPaletteColor(unsigned int paletteRow, unsigned int paletteIndex) const = 0;
	virtual unsigned char ColorValueTo8BitValue(unsigned int colorValue, bool shadow, bool highlight) const = 0;
	virtual Marshal::Ret<std::list<SpriteBoundaryLineEntry>> GetSpriteBoundaryLines(unsigned int planeNo) const = 0;

	// Sprite list debugging functions
	virtual SpriteMappingTableEntry GetSpriteMappingTableEntry(unsigned int spriteTableBaseAddress, unsigned int entryNo) const = 0;
	virtual void SetSpriteMappingTableEntry(unsigned int spriteTableBaseAddress, unsigned int entryNo, const SpriteMappingTableEntry& entry, bool useSeparatedData) = 0;

	// Port monitor functions
	virtual bool GetPortMonitorStatusReadEnabled() const = 0;
	virtual void SetPortMonitorStatusReadEnabled(bool state) = 0;
	virtual bool GetPortMonitorDataReadEnabled() const = 0;
	virtual void SetPortMonitorDataReadEnabled(bool state) = 0;
	virtual bool GetPortMonitorHVReadEnabled() const = 0;
	virtual void SetPortMonitorHVReadEnabled(bool state) = 0;
	virtual bool GetPortMonitorControlWriteEnabled() const = 0;
	virtual void SetPortMonitorControlWriteEnabled(bool state) = 0;
	virtual bool GetPortMonitorDataWriteEnabled() const = 0;
	virtual void SetPortMonitorDataWriteEnabled(bool state) = 0;
	virtual unsigned int GetPortMonitorLength() const = 0;
	virtual void SetPortMonitorLength(unsigned int state) = 0;
	virtual Marshal::Ret<std::list<PortMonitorEntry>> GetPortMonitorLog() const = 0;
	virtual unsigned int GetPortMonitorLogLastModifiedToken() const = 0;
	virtual void ClearPortMonitorLog() = 0;

	// Debug output
	inline bool GetOutputPortAccessDebugMessages() const;
	inline void SetOutputPortAccessDebugMessages(bool data);
	inline bool GetOutputTimingDebugMessages() const;
	inline void SetOutputTimingDebugMessages(bool data);
	inline bool GetOutputRenderSyncMessages() const;
	inline void SetOutputRenderSyncMessages(bool data);
	inline bool GetOutputInterruptDebugMessages() const;
	inline void SetOutputInterruptDebugMessages(bool data);
	inline bool GetVideoDisableRenderOutput() const;
	inline void SetVideoDisableRenderOutput(bool data);
	inline bool GetVideoEnableSpriteBoxing() const;
	inline void SetVideoEnableSpriteBoxing(bool data);
	inline bool GetVideoHighlightRenderPos() const;
	inline void SetVideoHighlightRenderPos(bool data);
	inline bool GetVideoSingleBuffering() const;
	inline void SetVideoSingleBuffering(bool data);
	inline bool GetVideoFixedAspectRatio() const;
	inline void SetVideoFixedAspectRatio(bool data);
	inline bool GetVideoShowStatusBar() const;
	inline void SetVideoShowStatusBar(bool data);
	inline bool GetVideoEnableLineSmoothing() const;
	inline void SetVideoEnableLineSmoothing(bool data);
	inline bool GetCurrentRenderPosOnScreen() const;
	inline void SetCurrentRenderPosOnScreen(bool data);
	inline unsigned int GetCurrentRenderPosScreenX() const;
	inline void SetCurrentRenderPosScreenX(unsigned int data);
	inline unsigned int GetCurrentRenderPosScreenY() const;
	inline void SetCurrentRenderPosScreenY(unsigned int data);
	inline bool GetVideoShowBoundaryActiveImage() const;
	inline void SetVideoShowBoundaryActiveImage(bool data);
	inline bool GetVideoShowBoundaryActionSafe() const;
	inline void SetVideoShowBoundaryActionSafe(bool data);
	inline bool GetVideoShowBoundaryTitleSafe() const;
	inline void SetVideoShowBoundaryTitleSafe(bool data);
	inline bool GetVideoEnableFullImageBufferInfo() const;
	inline void SetVideoEnableFullImageBufferInfo(bool data);

	// Layer removal
	inline bool GetEnableLayerA() const;
	inline void SetEnableLayerA(bool data);
	inline bool GetEnableLayerAHigh() const;
	inline void SetEnableLayerAHigh(bool data);
	inline bool GetEnableLayerALow() const;
	inline void SetEnableLayerALow(bool data);
	inline bool GetEnableLayerB() const;
	inline void SetEnableLayerB(bool data);
	inline bool GetEnableLayerBHigh() const;
	inline void SetEnableLayerBHigh(bool data);
	inline bool GetEnableLayerBLow() const;
	inline void SetEnableLayerBLow(bool data);
	inline bool GetEnableWindow() const;
	inline void SetEnableWindow(bool data);
	inline bool GetEnableWindowHigh() const;
	inline void SetEnableWindowHigh(bool data);
	inline bool GetEnableWindowLow() const;
	inline void SetEnableWindowLow(bool data);
	inline bool GetEnableSprite() const;
	inline void SetEnableSprite(bool data);
	inline bool GetEnableSpriteHigh() const;
	inline void SetEnableSpriteHigh(bool data);
	inline bool GetEnableSpriteLow() const;
	inline void SetEnableSpriteLow(bool data);

	// Raw register functions
	inline unsigned int GetRegisterData(unsigned int location) const;
	inline void SetRegisterData(unsigned int location, unsigned int data);

	// Interpreted register functions
	inline bool RegGetVSI() const;
	inline void RegSetVSI(bool data);
	inline bool RegGetHSI() const;
	inline void RegSetHSI(bool data);
	inline bool RegGetLCB() const;
	inline void RegSetLCB(bool data);
	inline bool RegGetIE1() const;
	inline void RegSetIE1(bool data);
	inline bool RegGetSS() const;
	inline void RegSetSS(bool data);
	inline bool RegGetPS() const;
	inline void RegSetPS(bool data);
	inline bool RegGetM2() const;
	inline void RegSetM2(bool data);
	inline bool RegGetES() const;
	inline void RegSetES(bool data);
	inline bool RegGetEVRAM() const;
	inline void RegSetEVRAM(bool data);
	inline bool RegGetDisplayEnabled() const;
	inline void RegSetDisplayEnabled(bool data);
	inline bool RegGetIE0() const;
	inline void RegSetIE0(bool data);
	inline bool RegGetDMAEnabled() const;
	inline void RegSetDMAEnabled(bool data);
	inline bool RegGetM3() const;
	inline void RegSetM3(bool data);
	inline bool RegGetMode5() const;
	inline void RegSetMode5(bool data);
	inline bool RegGetSZ() const;
	inline void RegSetSZ(bool data);
	inline bool RegGetMAG() const;
	inline void RegSetMAG(bool data);
	inline unsigned int RegGetNameTableBaseScrollA() const;
	inline void RegSetNameTableBaseScrollA(unsigned int data);
	inline unsigned int RegGetNameTableBaseWindow() const;
	inline void RegSetNameTableBaseWindow(unsigned int data);
	inline unsigned int RegGetNameTableBaseScrollB() const;
	inline void RegSetNameTableBaseScrollB(unsigned int data);
	inline unsigned int RegGetNameTableBaseSprite() const;
	inline void RegSetNameTableBaseSprite(unsigned int data);
	inline unsigned int RegGetPatternBaseSprite() const;
	inline void RegSetPatternBaseSprite(unsigned int data);
	inline bool RegGet077() const;
	inline void RegSet077(bool data);
	inline bool RegGet076() const;
	inline void RegSet076(bool data);
	inline unsigned int RegGetBackgroundPaletteRow() const;
	inline void RegSetBackgroundPaletteRow(unsigned int data);
	inline unsigned int RegGetBackgroundPaletteColumn() const;
	inline void RegSetBackgroundPaletteColumn(unsigned int data);
	inline unsigned int RegGetBackgroundScrollX() const;
	inline void RegSetBackgroundScrollX(unsigned int data);
	inline unsigned int RegGetBackgroundScrollY() const;
	inline void RegSetBackgroundScrollY(unsigned int data);
	inline unsigned int RegGetHInterruptData() const;
	inline void RegSetHInterruptData(unsigned int data);
	inline bool RegGet0B7() const;
	inline void RegSet0B7(bool data);
	inline bool RegGet0B6() const;
	inline void RegSet0B6(bool data);
	inline bool RegGet0B5() const;
	inline void RegSet0B5(bool data);
	inline bool RegGet0B4() const;
	inline void RegSet0B4(bool data);
	inline bool RegGetIE2() const;
	inline void RegSetIE2(bool data);
	inline bool RegGetVSCR() const;
	inline void RegSetVSCR(bool data);
	inline bool RegGetHSCR() const;
	inline void RegSetHSCR(bool data);
	inline bool RegGetLSCR() const;
	inline void RegSetLSCR(bool data);
	inline bool RegGetRS0() const;
	inline void RegSetRS0(bool data);
	inline bool RegGetU1() const;
	inline void RegSetU1(bool data);
	inline bool RegGetU2() const;
	inline void RegSetU2(bool data);
	inline bool RegGetU3() const;
	inline void RegSetU3(bool data);
	inline bool RegGetSTE() const;
	inline void RegSetSTE(bool data);
	inline bool RegGetLSM1() const;
	inline void RegSetLSM1(bool data);
	inline bool RegGetLSM0() const;
	inline void RegSetLSM0(bool data);
	inline bool RegGetRS1() const;
	inline void RegSetRS1(bool data);
	inline unsigned int RegGetHScrollDataBase() const;
	inline void RegSetHScrollDataBase(unsigned int data);
	inline unsigned int RegGet0E57() const;
	inline void RegSet0E57(unsigned int data);
	inline unsigned int RegGetPatternBaseScrollA() const;
	inline void RegSetPatternBaseScrollA(unsigned int data);
	inline unsigned int RegGet0E13() const;
	inline void RegSet0E13(unsigned int data);
	inline unsigned int RegGetPatternBaseScrollB() const;
	inline void RegSetPatternBaseScrollB(unsigned int data);
	inline unsigned int RegGetAutoIncrementData() const;
	inline void RegSetAutoIncrementData(unsigned int data);
	inline unsigned int RegGet1067() const;
	inline void RegSet1067(unsigned int data);
	inline unsigned int RegGetVSZ() const;
	inline void RegSetVSZ(unsigned int data);
	inline bool RegGetVSZ1() const;
	inline void RegSetVSZ1(bool data);
	inline bool RegGetVSZ0() const;
	inline void RegSetVSZ0(bool data);
	inline unsigned int RegGet1023() const;
	inline void RegSet1023(unsigned int data);
	inline unsigned int RegGetHSZ() const;
	inline void RegSetHSZ(unsigned int data);
	inline bool RegGetHSZ1() const;
	inline void RegSetHSZ1(bool data);
	inline bool RegGetHSZ0() const;
	inline void RegSetHSZ0(bool data);
	inline unsigned int RegGet1156() const;
	inline void RegSet1156(unsigned int data);
	inline bool RegGetWindowRightAligned() const;
	inline void RegSetWindowRightAligned(bool data);
	inline unsigned int RegGetWindowBasePointX() const;
	inline void RegSetWindowBasePointX(unsigned int data);
	inline unsigned int RegGet1256() const;
	inline void RegSet1256(unsigned int data);
	inline bool RegGetWindowBottomAligned() const;
	inline void RegSetWindowBottomAligned(bool data);
	inline unsigned int RegGetWindowBasePointY() const;
	inline void RegSetWindowBasePointY(unsigned int data);
	inline unsigned int RegGetDMALengthCounter() const;
	inline void RegSetDMALengthCounter(unsigned int data);
	inline unsigned int RegGetDMASourceAddress() const;
	inline void RegSetDMASourceAddress(unsigned int data);
	inline unsigned int RegGetDMASourceAddressByte1() const;
	inline void RegSetDMASourceAddressByte1(unsigned int data);
	inline unsigned int RegGetDMASourceAddressByte2() const;
	inline void RegSetDMASourceAddressByte2(unsigned int data);
	inline unsigned int RegGetDMASourceAddressByte3() const;
	inline void RegSetDMASourceAddressByte3(unsigned int data);
	inline bool RegGetDMD1() const;
	inline void RegSetDMD1(bool data);
	inline bool RegGetDMD0() const;
	inline void RegSetDMD0(bool data);

	// Status register functions
	inline unsigned int GetStatus() const;
	inline void SetStatus(unsigned int data);
	inline bool GetStatusFlagFIFOEmpty() const;
	inline void SetStatusFlagFIFOEmpty(bool data);
	inline bool GetStatusFlagFIFOFull() const;
	inline void SetStatusFlagFIFOFull(bool data);
	inline bool GetStatusFlagF() const;
	inline void SetStatusFlagF(bool data);
	inline bool GetStatusFlagSpriteOverflow() const;
	inline void SetStatusFlagSpriteOverflow(bool data);
	inline bool GetStatusFlagSpriteCollision() const;
	inline void SetStatusFlagSpriteCollision(bool data);
	inline bool GetStatusFlagOddInterlaceFrame() const;
	inline void SetStatusFlagOddInterlaceFrame(bool data);
	inline bool GetStatusFlagVBlank() const;
	inline void SetStatusFlagVBlank(bool data);
	inline bool GetStatusFlagHBlank() const;
	inline void SetStatusFlagHBlank(bool data);
	inline bool GetStatusFlagDMA() const;
	inline void SetStatusFlagDMA(bool data);
	inline bool GetStatusFlagPAL() const;
	inline void SetStatusFlagPAL(bool data);

	// Port register functions
	inline unsigned int RegGetPortCode() const;
	inline void RegSetPortCode(unsigned int data);
	inline unsigned int RegGetPortAddress() const;
	inline void RegSetPortAddress(unsigned int data);
	inline bool RegGetPortWritePending() const;
	inline void RegSetPortWritePending(bool data);
	inline unsigned int RegGetReadBuffer() const;
	inline void RegSetReadBuffer(unsigned int data);
	inline bool RegGetReadHalfCached() const;
	inline void RegSetReadHalfCached(bool data);
	inline bool RegGetReadFullyCached() const;
	inline void RegSetReadFullyCached(bool data);

	// Interrupt register functions
	inline bool RegGetVINTPending() const;
	inline void RegSetVINTPending(bool data);
	inline bool RegGetHINTPending() const;
	inline void RegSetHINTPending(bool data);
	inline bool RegGetEXINTPending() const;
	inline void RegSetEXINTPending(bool data);

	// HV counter register functions
	inline unsigned int RegGetHVCounterExternal() const;
	inline unsigned int RegGetHCounterInternal() const;
	inline void RegSetHCounterInternal(unsigned int data);
	inline unsigned int RegGetVCounterInternal() const;
	inline void RegSetVCounterInternal(unsigned int data);
	inline unsigned int RegGetHCounterLatched() const;
	inline void RegSetHCounterLatched(unsigned int data);
	inline unsigned int RegGetVCounterLatched() const;
	inline void RegSetVCounterLatched(unsigned int data);

	// FIFO register functions
	inline unsigned int RegGetFIFOCode(unsigned int entryNo) const;
	inline void RegSetFIFOCode(unsigned int entryNo, unsigned int data);
	inline unsigned int RegGetFIFOAddress(unsigned int entryNo) const;
	inline void RegSetFIFOAddress(unsigned int entryNo, unsigned int data);
	inline unsigned int RegGetFIFOData(unsigned int entryNo) const;
	inline void RegSetFIFOData(unsigned int entryNo, unsigned int data);
	inline bool RegGetFIFOWritePending(unsigned int entryNo) const;
	inline void RegSetFIFOWritePending(unsigned int entryNo, bool data);
	inline bool RegGetFIFOHalfWritten(unsigned int entryNo) const;
	inline void RegSetFIFOHalfWritten(unsigned int entryNo, bool data);
	inline unsigned int RegGetFIFONextReadEntry() const;
	inline void RegSetFIFONextReadEntry(unsigned int data);
	inline unsigned int RegGetFIFONextWriteEntry() const;
	inline void RegSetFIFONextWriteEntry(unsigned int data);
};

#include "IS315_5313.inl"
#endif
