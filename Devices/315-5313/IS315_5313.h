#ifndef __IS315_5313_H__
#define __IS315_5313_H__
#include "GenericAccess/GenericAccess.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "TimedBuffers/TimedBuffers.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include <list>

class IS315_5313 :public virtual IGenericAccess
{
public:
	//Constants
	static const unsigned int registerCount = 24;
	static const unsigned int registerCountM4 = 11;
	static const unsigned int vramSize = 0x10000;
	static const unsigned int cramSize = 0x80;
	static const unsigned int vsramSize = 0x50; //Note that this has been observed to be 0x80 bytes on the Genesis 3
	static const unsigned int spriteCacheSize = 0x140; //4 bytes cached per sprite, with 80 sprites max in H40 mode.
	static const unsigned int fifoBufferSize = 4;
	static const unsigned int statusRegisterMask = 0x03FF;
	static const unsigned int cellsPerColumn = 2;
	static const unsigned int imageBufferWidth = 512;
	static const unsigned int imageBufferHeight = 512;
	static const unsigned int imageBufferPlanes = 3;

	//Enumerations
	enum class IS315_5313DataSource;

	//Structures
	struct RegisterDataContext;
	struct FIFOEntryDataContext;
	struct PortMonitorEntry;
	struct SpriteMappingTableEntry;
	struct DecodedPaletteColorEntry;
	struct SpriteBoundaryLineEntry;

public:
	//Interface version functions
	static inline unsigned int ThisIS315_5313Version() { return 1; }
	virtual unsigned int GetIS315_5313Version() const = 0;

	//Device access functions
	virtual IDevice* GetDevice() = 0;

	//External buffer functions
	virtual void LockExternalBuffers() = 0;
	virtual void UnlockExternalBuffers() = 0;
	virtual ITimedBufferInt* GetVRAMBuffer() const = 0;
	virtual ITimedBufferInt* GetCRAMBuffer() const = 0;
	virtual ITimedBufferInt* GetVSRAMBuffer() const = 0;
	virtual ITimedBufferInt* GetSpriteCacheBuffer() const = 0;

	//Image buffer functions
	virtual unsigned int GetImageLastRenderedFrameToken() const = 0;
	virtual unsigned int GetImageCompletedBufferPlaneNo() const = 0;
	virtual unsigned int GetImageDrawingBufferPlaneNo() const = 0;
	virtual void LockImageBufferData(unsigned int planeNo) = 0;
	virtual void UnlockImageBufferData(unsigned int planeNo) = 0;
	virtual const unsigned char* GetImageBufferData(unsigned int planeNo) const = 0;
	virtual bool GetImageBufferOddInterlaceFrame(unsigned int planeNo) const = 0;
	virtual unsigned int GetImageBufferLineCount(unsigned int planeNo) const = 0;
	virtual unsigned int GetImageBufferLineWidth(unsigned int planeNo, unsigned int lineNo) const = 0;
	virtual void GetImageBufferActiveScanPosX(unsigned int planeNo, unsigned int lineNo, unsigned int& startPosX, unsigned int& endPosX) const = 0;
	virtual void GetImageBufferActiveScanPosY(unsigned int planeNo, unsigned int& startPosY, unsigned int& endPosY) const = 0;

	//Rendering functions
	virtual void DigitalRenderReadHscrollData(unsigned int screenRowNumber, unsigned int hscrollDataBase, bool hscrState, bool lscrState, unsigned int& layerAHscrollPatternDisplacement, unsigned int& layerBHscrollPatternDisplacement, unsigned int& layerAHscrollMappingDisplacement, unsigned int& layerBHscrollMappingDisplacement) const = 0;
	virtual void DigitalRenderReadVscrollData(unsigned int screenColumnNumber, unsigned int layerNumber, bool vscrState, bool interlaceMode2Active, unsigned int& layerVscrollPatternDisplacement, unsigned int& layerVscrollMappingDisplacement, Data& vsramReadCache) const = 0;
	virtual unsigned int CalculatePatternDataRowAddress(unsigned int patternRowNumberNoFlip, unsigned int patternCellOffset, bool interlaceMode2Active, const Data& mappingData) const = 0;
	virtual void CalculateEffectiveCellScrollSize(unsigned int hszState, unsigned int vszState, unsigned int& effectiveScrollWidth, unsigned int& effectiveScrollHeight) const = 0;
	virtual DecodedPaletteColorEntry ReadDecodedPaletteColor(unsigned int paletteRow, unsigned int paletteIndex) const = 0;
	virtual unsigned char ColorValueTo8BitValue(unsigned int colorValue, bool shadow, bool highlight) const = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<SpriteBoundaryLineEntry>> GetSpriteBoundaryLines(unsigned int planeNo) const = 0;

	//Sprite list debugging functions
	virtual SpriteMappingTableEntry GetSpriteMappingTableEntry(unsigned int entryNo) const = 0;
	virtual void SetSpriteMappingTableEntry(unsigned int entryNo, const SpriteMappingTableEntry& entry, bool useSeparatedData) = 0;

	//Port monitor functions
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
	virtual MarshalSupport::Marshal::Ret<std::list<PortMonitorEntry>> GetPortMonitorLog() const = 0;
	virtual unsigned int GetPortMonitorLogLastModifiedToken() const = 0;
	virtual void ClearPortMonitorLog() = 0;

	//Debug output
	inline bool GetOutputPortAccessDebugMessages() const;
	inline void SetOutputPortAccessDebugMessages(bool adata);
	inline bool GetOutputTimingDebugMessages() const;
	inline void SetOutputTimingDebugMessages(bool adata);
	inline bool GetOutputRenderSyncMessages() const;
	inline void SetOutputRenderSyncMessages(bool adata);
	inline bool GetOutputInterruptDebugMessages() const;
	inline void SetOutputInterruptDebugMessages(bool adata);
	inline bool GetVideoDisableRenderOutput() const;
	inline void SetVideoDisableRenderOutput(bool adata);
	inline bool GetVideoEnableSpriteBoxing() const;
	inline void SetVideoEnableSpriteBoxing(bool adata);
	inline bool GetVideoHighlightRenderPos() const;
	inline void SetVideoHighlightRenderPos(bool adata);
	inline bool GetVideoSingleBuffering() const;
	inline void SetVideoSingleBuffering(bool adata);
	inline bool GetVideoFixedAspectRatio() const;
	inline void SetVideoFixedAspectRatio(bool adata);
	inline bool GetVideoShowStatusBar() const;
	inline void SetVideoShowStatusBar(bool adata);
	inline bool GetVideoEnableLineSmoothing() const;
	inline void SetVideoEnableLineSmoothing(bool adata);
	inline bool GetCurrentRenderPosOnScreen() const;
	inline void SetCurrentRenderPosOnScreen(bool adata);
	inline unsigned int GetCurrentRenderPosScreenX() const;
	inline void SetCurrentRenderPosScreenX(unsigned int adata);
	inline unsigned int GetCurrentRenderPosScreenY() const;
	inline void SetCurrentRenderPosScreenY(unsigned int adata);
	inline bool GetVideoShowBoundaryActiveImage() const;
	inline void SetVideoShowBoundaryActiveImage(bool adata);
	inline bool GetVideoShowBoundaryActionSafe() const;
	inline void SetVideoShowBoundaryActionSafe(bool adata);
	inline bool GetVideoShowBoundaryTitleSafe() const;
	inline void SetVideoShowBoundaryTitleSafe(bool adata);

	//Layer removal
	inline bool GetEnableLayerA() const;
	inline void SetEnableLayerA(bool adata);
	inline bool GetEnableLayerAHigh() const;
	inline void SetEnableLayerAHigh(bool adata);
	inline bool GetEnableLayerALow() const;
	inline void SetEnableLayerALow(bool adata);
	inline bool GetEnableLayerB() const;
	inline void SetEnableLayerB(bool adata);
	inline bool GetEnableLayerBHigh() const;
	inline void SetEnableLayerBHigh(bool adata);
	inline bool GetEnableLayerBLow() const;
	inline void SetEnableLayerBLow(bool adata);
	inline bool GetEnableWindow() const;
	inline void SetEnableWindow(bool adata);
	inline bool GetEnableWindowHigh() const;
	inline void SetEnableWindowHigh(bool adata);
	inline bool GetEnableWindowLow() const;
	inline void SetEnableWindowLow(bool adata);
	inline bool GetEnableSprite() const;
	inline void SetEnableSprite(bool adata);
	inline bool GetEnableSpriteHigh() const;
	inline void SetEnableSpriteHigh(bool adata);
	inline bool GetEnableSpriteLow() const;
	inline void SetEnableSpriteLow(bool adata);

	//Raw register functions
	inline unsigned int GetRegisterData(unsigned int location) const;
	inline void SetRegisterData(unsigned int location, unsigned int adata);

	//Interpreted register functions
	inline bool RegGetVSI() const;
	inline void RegSetVSI(bool adata);
	inline bool RegGetHSI() const;
	inline void RegSetHSI(bool adata);
	inline bool RegGetLCB() const;
	inline void RegSetLCB(bool adata);
	inline bool RegGetIE1() const;
	inline void RegSetIE1(bool adata);
	inline bool RegGetSS() const;
	inline void RegSetSS(bool adata);
	inline bool RegGetPS() const;
	inline void RegSetPS(bool adata);
	inline bool RegGetM2() const;
	inline void RegSetM2(bool adata);
	inline bool RegGetES() const;
	inline void RegSetES(bool adata);
	inline bool RegGetEVRAM() const;
	inline void RegSetEVRAM(bool adata);
	inline bool RegGetDisplayEnabled() const;
	inline void RegSetDisplayEnabled(bool adata);
	inline bool RegGetIE0() const;
	inline void RegSetIE0(bool adata);
	inline bool RegGetDMAEnabled() const;
	inline void RegSetDMAEnabled(bool adata);
	inline bool RegGetM3() const;
	inline void RegSetM3(bool adata);
	inline bool RegGetMode5() const;
	inline void RegSetMode5(bool adata);
	inline bool RegGetSZ() const;
	inline void RegSetSZ(bool adata);
	inline bool RegGetMAG() const;
	inline void RegSetMAG(bool adata);
	inline unsigned int RegGetNameTableBaseScrollA() const;
	inline void RegSetNameTableBaseScrollA(unsigned int adata);
	inline unsigned int RegGetNameTableBaseWindow() const;
	inline void RegSetNameTableBaseWindow(unsigned int adata);
	inline unsigned int RegGetNameTableBaseScrollB() const;
	inline void RegSetNameTableBaseScrollB(unsigned int adata);
	inline unsigned int RegGetNameTableBaseSprite() const;
	inline void RegSetNameTableBaseSprite(unsigned int adata);
	inline unsigned int RegGetPatternBaseSprite() const;
	inline void RegSetPatternBaseSprite(unsigned int adata);
	inline bool RegGet077() const;
	inline void RegSet077(bool adata);
	inline bool RegGet076() const;
	inline void RegSet076(bool adata);
	inline unsigned int RegGetBackgroundPaletteRow() const;
	inline void RegSetBackgroundPaletteRow(unsigned int adata);
	inline unsigned int RegGetBackgroundPaletteColumn() const;
	inline void RegSetBackgroundPaletteColumn(unsigned int adata);
	inline unsigned int RegGetBackgroundScrollX() const;
	inline void RegSetBackgroundScrollX(unsigned int adata);
	inline unsigned int RegGetBackgroundScrollY() const;
	inline void RegSetBackgroundScrollY(unsigned int adata);
	inline unsigned int RegGetHInterruptData() const;
	inline void RegSetHInterruptData(unsigned int adata);
	inline bool RegGet0B7() const;
	inline void RegSet0B7(bool adata);
	inline bool RegGet0B6() const;
	inline void RegSet0B6(bool adata);
	inline bool RegGet0B5() const;
	inline void RegSet0B5(bool adata);
	inline bool RegGet0B4() const;
	inline void RegSet0B4(bool adata);
	inline bool RegGetIE2() const;
	inline void RegSetIE2(bool adata);
	inline bool RegGetVSCR() const;
	inline void RegSetVSCR(bool adata);
	inline bool RegGetHSCR() const;
	inline void RegSetHSCR(bool adata);
	inline bool RegGetLSCR() const;
	inline void RegSetLSCR(bool adata);
	inline bool RegGetRS0() const;
	inline void RegSetRS0(bool adata);
	inline bool RegGetU1() const;
	inline void RegSetU1(bool adata);
	inline bool RegGetU2() const;
	inline void RegSetU2(bool adata);
	inline bool RegGetU3() const;
	inline void RegSetU3(bool adata);
	inline bool RegGetSTE() const;
	inline void RegSetSTE(bool adata);
	inline bool RegGetLSM1() const;
	inline void RegSetLSM1(bool adata);
	inline bool RegGetLSM0() const;
	inline void RegSetLSM0(bool adata);
	inline bool RegGetRS1() const;
	inline void RegSetRS1(bool adata);
	inline unsigned int RegGetHScrollDataBase() const;
	inline void RegSetHScrollDataBase(unsigned int adata);
	inline unsigned int RegGet0E57() const;
	inline void RegSet0E57(unsigned int adata);
	inline unsigned int RegGetPatternBaseScrollA() const;
	inline void RegSetPatternBaseScrollA(unsigned int adata);
	inline unsigned int RegGet0E13() const;
	inline void RegSet0E13(unsigned int adata);
	inline unsigned int RegGetPatternBaseScrollB() const;
	inline void RegSetPatternBaseScrollB(unsigned int adata);
	inline unsigned int RegGetAutoIncrementData() const;
	inline void RegSetAutoIncrementData(unsigned int adata);
	inline unsigned int RegGet1067() const;
	inline void RegSet1067(unsigned int adata);
	inline unsigned int RegGetVSZ() const;
	inline void RegSetVSZ(unsigned int adata);
	inline bool RegGetVSZ1() const;
	inline void RegSetVSZ1(bool adata);
	inline bool RegGetVSZ0() const;
	inline void RegSetVSZ0(bool adata);
	inline unsigned int RegGet1023() const;
	inline void RegSet1023(unsigned int adata);
	inline unsigned int RegGetHSZ() const;
	inline void RegSetHSZ(unsigned int adata);
	inline bool RegGetHSZ1() const;
	inline void RegSetHSZ1(bool adata);
	inline bool RegGetHSZ0() const;
	inline void RegSetHSZ0(bool adata);
	inline unsigned int RegGet1156() const;
	inline void RegSet1156(unsigned int adata);
	inline bool RegGetWindowRightAligned() const;
	inline void RegSetWindowRightAligned(bool adata);
	inline unsigned int RegGetWindowBasePointX() const;
	inline void RegSetWindowBasePointX(unsigned int adata);
	inline unsigned int RegGet1256() const;
	inline void RegSet1256(unsigned int adata);
	inline bool RegGetWindowBottomAligned() const;
	inline void RegSetWindowBottomAligned(bool adata);
	inline unsigned int RegGetWindowBasePointY() const;
	inline void RegSetWindowBasePointY(unsigned int adata);
	inline unsigned int RegGetDMALengthCounter() const;
	inline void RegSetDMALengthCounter(unsigned int adata);
	inline unsigned int RegGetDMASourceAddress() const;
	inline void RegSetDMASourceAddress(unsigned int adata);
	inline unsigned int RegGetDMASourceAddressByte1() const;
	inline void RegSetDMASourceAddressByte1(unsigned int adata);
	inline unsigned int RegGetDMASourceAddressByte2() const;
	inline void RegSetDMASourceAddressByte2(unsigned int adata);
	inline unsigned int RegGetDMASourceAddressByte3() const;
	inline void RegSetDMASourceAddressByte3(unsigned int adata);
	inline bool RegGetDMD1() const;
	inline void RegSetDMD1(bool adata);
	inline bool RegGetDMD0() const;
	inline void RegSetDMD0(bool adata);

	//Status register functions
	inline unsigned int GetStatus() const;
	inline void SetStatus(unsigned int adata);
	inline bool GetStatusFlagFIFOEmpty() const;
	inline void SetStatusFlagFIFOEmpty(bool adata);
	inline bool GetStatusFlagFIFOFull() const;
	inline void SetStatusFlagFIFOFull(bool adata);
	inline bool GetStatusFlagF() const;
	inline void SetStatusFlagF(bool adata);
	inline bool GetStatusFlagSpriteOverflow() const;
	inline void SetStatusFlagSpriteOverflow(bool adata);
	inline bool GetStatusFlagSpriteCollision() const;
	inline void SetStatusFlagSpriteCollision(bool adata);
	inline bool GetStatusFlagOddInterlaceFrame() const;
	inline void SetStatusFlagOddInterlaceFrame(bool adata);
	inline bool GetStatusFlagVBlank() const;
	inline void SetStatusFlagVBlank(bool adata);
	inline bool GetStatusFlagHBlank() const;
	inline void SetStatusFlagHBlank(bool adata);
	inline bool GetStatusFlagDMA() const;
	inline void SetStatusFlagDMA(bool adata);
	inline bool GetStatusFlagPAL() const;
	inline void SetStatusFlagPAL(bool adata);

	//Port register functions
	inline unsigned int RegGetPortCode() const;
	inline void RegSetPortCode(unsigned int adata);
	inline unsigned int RegGetPortAddress() const;
	inline void RegSetPortAddress(unsigned int adata);
	inline bool RegGetPortWritePending() const;
	inline void RegSetPortWritePending(bool adata);
	inline unsigned int RegGetReadBuffer() const;
	inline void RegSetReadBuffer(unsigned int adata);
	inline bool RegGetReadHalfCached() const;
	inline void RegSetReadHalfCached(bool adata);
	inline bool RegGetReadFullyCached() const;
	inline void RegSetReadFullyCached(bool adata);

	//Interrupt register functions
	inline bool RegGetVINTPending() const;
	inline void RegSetVINTPending(bool adata);
	inline bool RegGetHINTPending() const;
	inline void RegSetHINTPending(bool adata);
	inline bool RegGetEXINTPending() const;
	inline void RegSetEXINTPending(bool adata);

	//HV counter register functions
	inline unsigned int RegGetHVCounterExternal() const;
	inline unsigned int RegGetHCounterInternal() const;
	inline void RegSetHCounterInternal(unsigned int adata);
	inline unsigned int RegGetVCounterInternal() const;
	inline void RegSetVCounterInternal(unsigned int adata);
	inline unsigned int RegGetHCounterLatched() const;
	inline void RegSetHCounterLatched(unsigned int adata);
	inline unsigned int RegGetVCounterLatched() const;
	inline void RegSetVCounterLatched(unsigned int adata);

	//FIFO register functions
	inline unsigned int RegGetFIFOCode(unsigned int entryNo) const;
	inline void RegSetFIFOCode(unsigned int entryNo, unsigned int adata);
	inline unsigned int RegGetFIFOAddress(unsigned int entryNo) const;
	inline void RegSetFIFOAddress(unsigned int entryNo, unsigned int adata);
	inline unsigned int RegGetFIFOData(unsigned int entryNo) const;
	inline void RegSetFIFOData(unsigned int entryNo, unsigned int adata);
	inline bool RegGetFIFOWritePending(unsigned int entryNo) const;
	inline void RegSetFIFOWritePending(unsigned int entryNo, bool adata);
	inline bool RegGetFIFOHalfWritten(unsigned int entryNo) const;
	inline void RegSetFIFOHalfWritten(unsigned int entryNo, bool adata);
	inline unsigned int RegGetFIFONextReadEntry() const;
	inline void RegSetFIFONextReadEntry(unsigned int adata);
	inline unsigned int RegGetFIFONextWriteEntry() const;
	inline void RegSetFIFONextWriteEntry(unsigned int adata);
};

#include "IS315_5313.inl"
#endif
