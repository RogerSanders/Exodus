//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum S315_5313::CELineID
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

//----------------------------------------------------------------------------------------
enum S315_5313::LineID
{
	LINE_IPL = 1,
	LINE_INT,
	LINE_INTAK,
	LINE_BR,
	LINE_BG,
	LINE_PAL,
	LINE_RESET
	//##TODO## Add the HL line
};

//----------------------------------------------------------------------------------------
enum S315_5313::ClockID
{
	CLOCK_MCLK = 1
};

//----------------------------------------------------------------------------------------
enum S315_5313::Layer
{
	LAYER_SPRITE = 0,
	LAYER_LAYERA = 1,
	LAYER_LAYERB = 2,
	LAYER_BACKGROUND = 3
};

//----------------------------------------------------------------------------------------
enum S315_5313::AccessContext
{
	ACCESSCONTEXT_INTLINECHANGE,
	ACCESSCONTEXT_BR_ASSERT,
	ACCESSCONTEXT_BR_RELEASE,
	ACCESSCONTEXT_DMAREAD,
	ACCESSCONTEXT_TIMINGPOINT,
	ACCESSCONTEXT_DEBUG
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct S315_5313::HScanSettings
{
	HScanSettings(unsigned int ahcounterActiveScanMaxValue,
	              unsigned int ahcounterBlankingInitialValue,
	              unsigned int avcounterIncrementPoint,
	              unsigned int ahblankSetPoint,
	              unsigned int ahblankClearedPoint,
	              unsigned int afflagSetPoint,
	              unsigned int aoddFlagTogglePoint,
	              unsigned int ahcounterMaxValue,
	              unsigned int avintFlagged,
	              unsigned int ahsyncAsserted,
	              unsigned int ahsyncNegated,
	              unsigned int aactiveDisplayHCounterFirstValue,
	              unsigned int aactiveDisplayHCounterLastValue,
	              unsigned int aactiveDisplayPixelCount,
	              unsigned int aleftBorderHCounterFirstValue,
	              unsigned int aleftBorderHCounterLastValue,
	              unsigned int aleftBorderPixelCount,
	              unsigned int arightBorderHCounterFirstValue,
	              unsigned int arightBorderHCounterLastValue,
	              unsigned int arightBorderPixelCount,
	              unsigned int aleftBlankingHCounterFirstValue,
	              unsigned int aleftBlankingHCounterLastValue,
	              unsigned int aleftBlankingPixelCount,
	              unsigned int arightBlankingHCounterFirstValue,
	              unsigned int arightBlankingHCounterLastValue,
	              unsigned int arightBlankingPixelCount)
	:hcounterActiveScanMaxValue(ahcounterActiveScanMaxValue),
	 hcounterBlankingInitialValue(ahcounterBlankingInitialValue),
	 vcounterIncrementPoint(avcounterIncrementPoint),
	 hblankSetPoint(ahblankSetPoint),
	 hblankClearedPoint(ahblankClearedPoint),
	 fflagSetPoint(afflagSetPoint),
	 oddFlagTogglePoint(aoddFlagTogglePoint),
	 hcounterMaxValue(ahcounterMaxValue),
	 vintFlagged(avintFlagged),
	 hsyncAsserted(ahsyncAsserted),
	 hsyncNegated(ahsyncNegated),
	 activeDisplayHCounterFirstValue(aactiveDisplayHCounterFirstValue),
	 activeDisplayHCounterLastValue(aactiveDisplayHCounterLastValue),
	 activeDisplayPixelCount(aactiveDisplayPixelCount),
	 leftBorderHCounterFirstValue(aleftBorderHCounterFirstValue),
	 leftBorderHCounterLastValue(aleftBorderHCounterLastValue),
	 leftBorderPixelCount(aleftBorderPixelCount),
	 rightBorderHCounterFirstValue(arightBorderHCounterFirstValue),
	 rightBorderHCounterLastValue(arightBorderHCounterLastValue),
	 rightBorderPixelCount(arightBorderPixelCount),
	 leftBlankingHCounterFirstValue(aleftBlankingHCounterFirstValue),
	 leftBlankingHCounterLastValue(aleftBlankingHCounterLastValue),
	 leftBlankingPixelCount(aleftBlankingPixelCount),
	 rightBlankingHCounterFirstValue(arightBlankingHCounterFirstValue),
	 rightBlankingHCounterLastValue(arightBlankingHCounterLastValue),
	 rightBlankingPixelCount(arightBlankingPixelCount),
	 hcounterStepsPerIteration(ahcounterActiveScanMaxValue + 1 + ((ahcounterMaxValue + 1) - ahcounterBlankingInitialValue))
	{}

	unsigned int hcounterActiveScanMaxValue;
	unsigned int hcounterBlankingInitialValue;
	unsigned int vcounterIncrementPoint;
	unsigned int hblankSetPoint;
	unsigned int hblankClearedPoint;
	unsigned int fflagSetPoint;
	unsigned int oddFlagTogglePoint;
	unsigned int hcounterMaxValue;
	unsigned int hcounterStepsPerIteration;
	unsigned int hintTriggerPoint;
	unsigned int vintFlagged;
	unsigned int hsyncAsserted;
	unsigned int hsyncNegated;

	unsigned int activeDisplayPixelCount;
	unsigned int leftBorderPixelCount;
	unsigned int rightBorderPixelCount;
	unsigned int leftBlankingPixelCount;
	unsigned int rightBlankingPixelCount;
	unsigned int activeDisplayHCounterFirstValue;
	unsigned int activeDisplayHCounterLastValue;
	unsigned int leftBorderHCounterFirstValue;
	unsigned int leftBorderHCounterLastValue;
	unsigned int rightBorderHCounterFirstValue;
	unsigned int rightBorderHCounterLastValue;
	unsigned int leftBlankingHCounterFirstValue;
	unsigned int leftBlankingHCounterLastValue;
	unsigned int rightBlankingHCounterFirstValue;
	unsigned int rightBlankingHCounterLastValue;
};

//----------------------------------------------------------------------------------------
struct S315_5313::VScanSettings
{
	VScanSettings(unsigned int avcounterActiveScanMaxValue,
	              unsigned int avcounterBlankingInitialValue,
	              unsigned int avcounterBlankingInitialValueOddFlag,
	              unsigned int avblankSetPoint,
	              unsigned int avblankClearedPoint,
	              unsigned int avcounterMaxValue,
	              unsigned int avsyncAssertedPoint,
	              unsigned int avsyncClearedPoint,
	              unsigned int alinesPerFrame,
	              unsigned int aactiveDisplayVCounterFirstValue,
	              unsigned int aactiveDisplayVCounterLastValue,
	              unsigned int aactiveDisplayLineCount,
	              unsigned int atopBorderVCounterFirstValue,
	              unsigned int atopBorderVCounterLastValue,
	              unsigned int atopBorderLineCount,
	              unsigned int abottomBorderVCounterFirstValue,
	              unsigned int abottomBorderVCounterLastValue,
	              unsigned int abottomBorderLineCount,
	              unsigned int atopBlankingVCounterFirstValue,
	              unsigned int atopBlankingVCounterLastValue,
	              unsigned int atopBlankingLineCount,
	              unsigned int abottomBlankingVCounterFirstValue,
	              unsigned int abottomBlankingVCounterLastValue,
	              unsigned int abottomBlankingLineCount)
	:vcounterActiveScanMaxValue(avcounterActiveScanMaxValue),
	 vcounterBlankingInitialValue(avcounterBlankingInitialValue),
	 vcounterBlankingInitialValueOddFlag(avcounterBlankingInitialValueOddFlag),
	 vblankSetPoint(avblankSetPoint),
	 vblankClearedPoint(avblankClearedPoint),
	 vcounterMaxValue(avcounterMaxValue),
	 vsyncAssertedPoint(avsyncAssertedPoint),
	 vsyncClearedPoint(avsyncClearedPoint),
	 linesPerFrame(alinesPerFrame),
	 activeDisplayVCounterFirstValue(aactiveDisplayVCounterFirstValue),
	 activeDisplayVCounterLastValue(aactiveDisplayVCounterLastValue),
	 activeDisplayLineCount(aactiveDisplayLineCount),
	 topBorderVCounterFirstValue(atopBorderVCounterFirstValue),
	 topBorderVCounterLastValue(atopBorderVCounterLastValue),
	 topBorderLineCount(atopBorderLineCount),
	 bottomBorderVCounterFirstValue(abottomBorderVCounterFirstValue),
	 bottomBorderVCounterLastValue(abottomBorderVCounterLastValue),
	 bottomBorderLineCount(abottomBorderLineCount),
	 topBlankingVCounterFirstValue(atopBlankingVCounterFirstValue),
	 topBlankingVCounterLastValue(atopBlankingVCounterLastValue),
	 topBlankingLineCount(atopBlankingLineCount),
	 bottomBlankingVCounterFirstValue(abottomBlankingVCounterFirstValue),
	 bottomBlankingVCounterLastValue(abottomBlankingVCounterLastValue),
	 bottomBlankingLineCount(abottomBlankingLineCount),
	 vcounterStepsPerIteration(avcounterActiveScanMaxValue + 1 + ((avcounterMaxValue + 1) - avcounterBlankingInitialValue)),
	 vcounterStepsPerIterationOddFlag(avcounterActiveScanMaxValue + 1 + ((avcounterMaxValue + 1) - avcounterBlankingInitialValueOddFlag))
	{}

	unsigned int vcounterActiveScanMaxValue;
	unsigned int vcounterBlankingInitialValue;
	unsigned int vcounterBlankingInitialValueOddFlag;
	unsigned int vblankSetPoint;
	unsigned int vblankClearedPoint;
	unsigned int vcounterMaxValue;
	unsigned int vcounterStepsPerIteration;
	unsigned int vcounterStepsPerIterationOddFlag;
	unsigned int vsyncAssertedPoint;
	unsigned int vsyncClearedPoint;
	unsigned int linesPerFrame;

	unsigned int activeDisplayVCounterFirstValue;
	unsigned int activeDisplayVCounterLastValue;
	unsigned int activeDisplayLineCount;
	unsigned int topBorderVCounterFirstValue;
	unsigned int topBorderVCounterLastValue;
	unsigned int topBorderLineCount;
	unsigned int bottomBorderVCounterFirstValue;
	unsigned int bottomBorderVCounterLastValue;
	unsigned int bottomBorderLineCount;
	unsigned int topBlankingLineCount;
	unsigned int topBlankingVCounterFirstValue;
	unsigned int topBlankingVCounterLastValue;
	unsigned int bottomBlankingLineCount;
	unsigned int bottomBlankingVCounterFirstValue;
	unsigned int bottomBlankingVCounterLastValue;
};

//----------------------------------------------------------------------------------------
struct S315_5313::TimesliceRenderInfo
{
	TimesliceRenderInfo()
	{}
	TimesliceRenderInfo(unsigned int atimesliceStartPosition)
	:timesliceStartPosition(atimesliceStartPosition)
	{}

	unsigned int timesliceStartPosition;
	unsigned int timesliceEndPosition;
};

//----------------------------------------------------------------------------------------
struct S315_5313::SpriteDisplayCacheEntry
{
	SpriteDisplayCacheEntry()
	:vpos(16, 0), sizeAndLinkData(16, 0), mappingData(16, 0), hpos(16, 0)
	{}

	unsigned int spriteTableIndex;
	unsigned int spriteRowIndex;
	Data vpos;
	Data sizeAndLinkData;
	Data mappingData;
	Data hpos;
};

//----------------------------------------------------------------------------------------
struct S315_5313::SpriteCellDisplayCacheEntry
{
	SpriteCellDisplayCacheEntry()
	:patternData(32, 0)
	{}

	unsigned int spriteDisplayCacheIndex;
	unsigned int spriteCellColumnNo;
	unsigned int patternCellOffset;
	unsigned int patternRowOffset;
	Data patternData;
};

//----------------------------------------------------------------------------------------
struct S315_5313::SpritePixelBufferEntry
{
	unsigned int paletteLine;
	unsigned int paletteIndex;
	bool layerPriority;
	bool entryWritten;
};

//----------------------------------------------------------------------------------------
struct S315_5313::VRAMRenderOp
{
	enum Op
	{
		HSCROLL,
		MAPPING_A,
		PATTERN_A,
		MAPPING_B,
		PATTERN_B,
		MAPPING_S,
		PATTERN_S,
		ACC_SLOT,
		REFRESH
	};

	Op operation;
	unsigned int index;
};

//----------------------------------------------------------------------------------------
struct S315_5313::InternalRenderOp
{
	enum Op
	{
		NONE,
		SPRITECACHE,
		SPRITEMAPPINGCLEAR,
		SPRITEPATTERNCLEAR
	};

	Op operation;
	unsigned int index;
};

//----------------------------------------------------------------------------------------
struct S315_5313::FIFOBufferEntry
{
	//##TODO## I have a theory, that there is no one single set of code and address
	//registers, but that each entry in the 4-word FIFO has its own set of code and
	//address registers, and that when you perform a control port write, it is the set of
	//code and address registers in the next available FIFO buffer slot that are modified,
	//and when you perform a data port write, the FIFO is advanced one slot, and the code
	//register and address register data from the previous slot are copied over into the
	//new slot. This is strongly supported by recent test results that conclusively show
	//that control port writes can be performed while entries are still waiting to be
	//written in the FIFO, and that these entries remain in the FIFO after the control
	//port write, and that the FIFO can be populated with pending writes to different
	//targets and addresses, all of which complete successfully.
	//##NOTE## It seems clear that CD5 is read live from the next available entry in the
	//FIFO when determining the DMA busy state, not old entries in the FIFO. Could DMA
	//transfers possibly leave CD5 set in old buffer entries?
	//##NOTE## The above note is incorrect, writes to the control port don't go directly
	//into the next available FIFO buffer. This is evidenced by a test we have performed
	//which demonstrates that control port writes can be made while the FIFO is full, with
	//4 pending writes all waiting to go to VRAM. The control port writes succeed, and
	//don't affect the pending writes.
	FIFOBufferEntry()
	:codeRegData(6), addressRegData(16), dataPortWriteData(16)
	{}

	Data codeRegData;
	Data addressRegData;

	Data dataPortWriteData;
	bool dataWriteHalfWritten;
	bool pendingDataWrite;
};

//----------------------------------------------------------------------------------------
struct S315_5313::HVCounterAdvanceSession
{
	HVCounterAdvanceSession()
	:hscanSettings(0), vscanSettings(0), pixelClockTicksAdvanced(0), mclkTicksAdvanced(0)
	{}

	const HScanSettings* hscanSettings;
	const VScanSettings* vscanSettings;

	unsigned int hcounterCurrent;
	unsigned int vcounterCurrent;

	bool interlaceEnabledCurrent;
	bool interlaceDoubleCurrent;
	bool screenModeRS0Current;
	bool screenModeRS1Current;
	bool screenModeV30Current;
	bool palModeCurrent;
	bool oddFlagCurrent;

	bool interlaceEnabledNew;
	bool interlaceDoubleNew;
	bool screenModeRS0New;
	bool screenModeRS1New;
	bool screenModeV30New;
	bool palModeNew;

	unsigned int pixelClockTicksAdvanced;
	unsigned int mclkTicksAdvanced;
};

//----------------------------------------------------------------------------------------
struct S315_5313::PortMonitorEntry
{
	PortMonitorEntry(const std::wstring& atarget, const std::wstring& asource, unsigned int adata, double aaccessTime, unsigned int ahcounterPos, unsigned int avcounterPos)
	:target(atarget), source(asource), data(adata), accessTime(aaccessTime), hcounterPos(ahcounterPos), vcounterPos(avcounterPos)
	{}

	std::wstring source;
	std::wstring target;
	unsigned int data;
	double accessTime;
	unsigned int hcounterPos;
	unsigned int vcounterPos;
};

//----------------------------------------------------------------------------------------
struct S315_5313::SpriteMappingTableEntry
{
	SpriteMappingTableEntry()
	:rawDataWord0(16), rawDataWord1(16), rawDataWord2(16), rawDataWord3(16)
	{}

	Data rawDataWord0;
	Data rawDataWord1;
	Data rawDataWord2;
	Data rawDataWord3;

	unsigned int blockNumber;
	unsigned int paletteLine;
	unsigned int xpos;
	unsigned int ypos;
	unsigned int width;
	unsigned int height;
	unsigned int link;
	bool priority;
	bool vflip;
	bool hflip;
};

//----------------------------------------------------------------------------------------
struct S315_5313::ImageBufferColorEntry
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

//----------------------------------------------------------------------------------------
struct S315_5313::SpriteBoundaryLineEntry
{
	int linePosXStart;
	int linePosXEnd;
	int linePosYStart;
	int linePosYEnd;
};

//----------------------------------------------------------------------------------------
//Status register functions
//----------------------------------------------------------------------------------------
//  -------------------------------------------------------------
//  |  9  |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
//  |-----------------------------------------------------------|
//  |EMPT |FULL |  F  |SOVR | COL | ODD | VB  | HB  | DMA | PAL |
//  -------------------------------------------------------------
//EMPT - FIFO empty flag. When there are no pending writes held in the 4-word FIFO, this
//       bit is set, otherwise it is clear.
//FULL - FIFO full flag. When there are 4 pending writes held in the 4-word FIFO, this bit
//       is set, otherwise it is clear.
//F    - VInt occurred. Set when a vertical interrupt event is being asserted by the VDP.
//       This bit is cleared when the interrupt is acknowledged.
//SOVR - Sprite Overflow. Set when either the maximum number of sprites on a line, or the
//       maximum number of sprite pixels on a line, is exceeded. This flag is set at the
//       time the sprite mapping data is being read, as soon as it is determined there is
//       another mapping block which cannot be read. In other words, this flag is set when
//       the last sprite mapping block on a line is read, if it is determined there is
//       another sprite mapping block after that one. This flag is cleared when the status
//       register is read.
//COL  - Sprite Collision. Set whenever two non-transparent pixels in two sprites overlap
//       on the screen. This occurs when the sprite pixel data is being read, at the
//       instant a sprite pixel is read which is determined to overlap with an existing
//       non-transparent pixel in the sprite line buffer. Since sprite pixel data is
//       mostly read during the blanking period on the previous line, this is where this
//       flag is usually asserted for the upcoming line. This flag is cleared when the
//       status register is read.
//ODD  - Set if the VDP is currently drawing an odd frame in interlace mode
//VB   - During VBlank. Note that although not officially documented, hardware tests have
//       shown this bit always returns true if the display is disabled.
//HB   - During HBlank. Unlike the VBlank flag, this bit is unaffected by the display
//       enable state.
//DMA  - Set if a DMA operation is currently in progress
//PAL  - Set if the PAL input line to the VDP is currently asserted
//----------------------------------------------------------------------------------------
bool S315_5313::GetStatusFlagFIFOEmpty() const
{
	return status.GetBit(9);
}

//----------------------------------------------------------------------------------------
void S315_5313::SetStatusFlagFIFOEmpty(bool state)
{
	status.SetBit(9, state);
}

//----------------------------------------------------------------------------------------
bool S315_5313::GetStatusFlagFIFOFull() const
{
	return status.GetBit(8);
}

//----------------------------------------------------------------------------------------
void S315_5313::SetStatusFlagFIFOFull(bool state)
{
	status.SetBit(8, state);
}

//----------------------------------------------------------------------------------------
bool S315_5313::GetStatusFlagF() const
{
	return status.GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::SetStatusFlagF(bool state)
{
	status.SetBit(7, state);
}

//----------------------------------------------------------------------------------------
bool S315_5313::GetStatusFlagSpriteOverflow() const
{
	return status.GetBit(6);
}

//----------------------------------------------------------------------------------------
void S315_5313::SetStatusFlagSpriteOverflow(bool state)
{
	status.SetBit(6, state);
}

//----------------------------------------------------------------------------------------
bool S315_5313::GetStatusFlagSpriteCollision() const
{
	return status.GetBit(5);
}

//----------------------------------------------------------------------------------------
void S315_5313::SetStatusFlagSpriteCollision(bool state)
{
	status.SetBit(5, state);
}

//----------------------------------------------------------------------------------------
bool S315_5313::GetStatusFlagOddInterlaceFrame() const
{
	return status.GetBit(4);
}

//----------------------------------------------------------------------------------------
void S315_5313::SetStatusFlagOddInterlaceFrame(bool state)
{
	status.SetBit(4, state);
}

//----------------------------------------------------------------------------------------
bool S315_5313::GetStatusFlagVBlank() const
{
	return status.GetBit(3);
}

//----------------------------------------------------------------------------------------
void S315_5313::SetStatusFlagVBlank(bool state)
{
	status.SetBit(3, state);
}

//----------------------------------------------------------------------------------------
bool S315_5313::GetStatusFlagHBlank() const
{
	return status.GetBit(2);
}

//----------------------------------------------------------------------------------------
void S315_5313::SetStatusFlagHBlank(bool state)
{
	status.SetBit(2, state);
}

//----------------------------------------------------------------------------------------
bool S315_5313::GetStatusFlagDMA() const
{
	return status.GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::SetStatusFlagDMA(bool state)
{
	status.SetBit(1, state);
}

//----------------------------------------------------------------------------------------
bool S315_5313::GetStatusFlagPAL() const
{
	return status.GetBit(0);
}

//----------------------------------------------------------------------------------------
void S315_5313::SetStatusFlagPAL(bool state)
{
	status.SetBit(0, state);
}

//----------------------------------------------------------------------------------------
//Raw register functions
//----------------------------------------------------------------------------------------
Data S315_5313::GetRegisterData(unsigned int location, const AccessTarget& accessTarget) const
{
	return reg.Read(location, accessTarget);
}

//----------------------------------------------------------------------------------------
void S315_5313::SetRegisterData(unsigned int location, const AccessTarget& accessTarget, const Data& data)
{
	reg.Write(location, data, accessTarget);
}

//----------------------------------------------------------------------------------------
//Mode 4 register functions
//##TODO## Fold the Mode 4 register access functions together with the mode 5 register
//access functions, and eliminate the use of "M5" prefixes on any methods. It's becoming
//clear that the operation of the VDP is almost the same in mode 4 and mode 5, and based
//on new research, there are very few register bits that have alternate meanings between
//the two modes. In fact, the only register with a known alternate meaning is bit 3 of
//register 0, which is the "HSM" register in mode 5 and alters the horizontal sync mode,
//and is the "SS" register in mode 4 and activates sprite shifting. Every other register
//value is either equivalent in both modes, or is only defined in one of the modes.
//----------------------------------------------------------------------------------------
//         ------------------------------------
//         | 7  | 6  | 5  | 4 | 3 | 2 | 1 | 0 |
//0x00(0)  |----------------------------------|
//         |*VSI|*HSI|*LCB|IE1|*SS|M4 |M2 |*ES|
//         ------------------------------------
//*VSI: Vertical Scroll Inhibit. Disable vertical scrolling for columns 24-31.
//*HSI: Horizontal Scroll Inhibit. Disable horizontal scrolling for rows 0-1.
//*LCB: Left Column Blank. Mask column 0.
//*SS:  Sprite Shift. Shift all sprites 8 pixels to the left.
//*ES:  External Sync.
//----------------------------------------------------------------------------------------
bool S315_5313::M4GetVScrollingDisabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetVScrollingDisabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetHScrollingDisabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(6);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetHScrollingDisabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(6, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetColumnZeroMasked(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(5);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetColumnZeroMasked(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(5, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetLineInterruptEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(4);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetLineInterruptEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(4, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetSpriteShift(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetSpriteShift(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(3, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetM4Bit(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(2);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetM4Bit(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(2, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetM2Bit(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetM2Bit(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(1, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetReg0Bit0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetReg0Bit0(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(0, data));
}

//----------------------------------------------------------------------------------------
//         ----------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0  |
//0x01(1)  |--------------------------------|
//         | / |DIS|IE |M1 |M3 | / |*SZ|*MAG|
//         ----------------------------------
//*SZ:  Sprite Size. All sprites are 8x16 instead of 8x8.
//*MAG: Sprite Zooming. Sprites are double the width and height. Each pixel is 2x2.
//----------------------------------------------------------------------------------------
bool S315_5313::M4GetDisplayEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(6);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetDisplayEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(6, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetFrameInterruptEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(5);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetFrameInterruptEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(5, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetM1Bit(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(4);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetM1Bit(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(4, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetM3Bit(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetM3Bit(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(3, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetSpritesDoubleHeight(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetSpritesDoubleHeight(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(1, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M4GetSpriteZooming(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetSpriteZooming(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(0, data));
}

//----------------------------------------------------------------------------------------
//         ----------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1  | 0 |
//         |--------------------------------|
//         |               |Name Table  |   |
//0x02(2)  | /   /   /   / |Base Address| / |
//         |               |------------|   |
//         |               |B13|B12|B11 |   |
//         ----------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M4GetNameTableBaseScroll(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x02, accessTarget).GetDataSegment(1, 3) << 11;
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetNameTableBaseScroll(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x02, accessTarget, GetRegisterData(0x02, accessTarget).SetDataSegment(1, 3, data >> 11));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//         |-------------------------------|
//         |   |Sprite Attribute Table |   |
//0x05(5)  | / |     Base Address      | / |
//         |   |-----------------------|   |
//         |   |B13|B12|B11|B10|B9 |B8 |   |
//         ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M4GetAttributeTableBaseSprite(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x05, accessTarget).GetDataSegment(1, 6) << 8;
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetAttributeTableBaseSprite(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x05, accessTarget, GetRegisterData(0x05, accessTarget).SetDataSegment(1, 6, data >> 8));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x06(6)  |-------------------------------|
//         | /   /   /   /   / |B13| /   / |
//         ---------------------------------
//B13: Sprite Pattern Generator Base Address - Bit 13.
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M4GetPatternBaseSprite(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x06, accessTarget).GetDataSegment(2, 1) << 13;
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetPatternBaseSprite(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x06, accessTarget, GetRegisterData(0x06, accessTarget).SetDataSegment(2, 1, data >> 13));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x07(7)  |-------------------------------|
//         | /   /   /   / |Backdrop Colour|
//         |               | Palette Index |
//         ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M4GetBackdropColorIndex(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x07, accessTarget).GetDataSegment(0, 4);
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetBackdropColorIndex(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x07, accessTarget, GetRegisterData(0x07, accessTarget).SetDataSegment(0, 4, data));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x08(8)  |-------------------------------|
//         |      Background Scroll X      |
//         ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M4GetBackgroundScrollX(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x08, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetBackgroundScrollX(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x08, accessTarget, GetRegisterData(0x08, accessTarget).SetData(data));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x09(9)  |-------------------------------|
//         |      Background Scroll Y      |
//         ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M4GetBackgroundScrollY(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x09, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetBackgroundScrollY(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x09, accessTarget, GetRegisterData(0x09, accessTarget).SetData(data));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x0A(10) |-------------------------------|
//         |         Line Counter          |
//         ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M4GetLineCounter(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0A, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::M4SetLineCounter(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x0A, accessTarget, GetRegisterData(0x0A, accessTarget).SetData(data));
}

//----------------------------------------------------------------------------------------
//Mode 5 register functions
//----------------------------------------------------------------------------------------
//         ------------------------------------
//         | 7  | 6 |  5 | 4 | 3  | 2 | 1 | 0 |
//0x00(0)  |----------------------------------|
//         |*VSI| / |*LCB|IE1|*HSM|*PS|M3 |*ES|
//         ------------------------------------
//*VSI: Vertical Scroll Inhibit. Disable vertical scrolling for columns 24-31. This
//      setting from the SMS VDP is still present in the Mega Drive VDP, even when mode 5
//      is active. This setting is mentioned in the Accolade doc "The Sega Development
//      System". We have done hardware tests and confirmed the behaviour of this register
//      setting. Enabling this bit stopped the rightmost 8 cells (4 2-cell blocks) from
//      being scrolled in H32 mode. When H40 mode was activated, the exact same blocks
//      were still fixed and non-scrolling, however, the rest of the blocks after that
//      point were now scrolling again, IE, this setting does not inhibit vertical
//      scrolling for columns 32 and onwards. We also noticed in this mode that vertical
//      scrolling isn't totally disabled for this region, but rather, only the highest
//      possible active bit of vertical scroll data affects the blocks. If we're running
//      with the H32 V128 scroll size, we see the scroll value jump from 0x000 to 0x200
//      when the scroll value places a block from the upper 64 cells at the top of the
//      screen. Also note that hardware tests have shown this register setting only
//      affects layer A, not layer B. We have also confirmed that horizontal scrolling
//      affects the locked region. Where both horizontal and vertical scrolling are
//      applied to this region, the region itself does scroll horizontally, up to the next
//      2-cell boundary, at which point, the fixed vscroll region jumps back. We also
//      tested with the window layer active, and found that the window layer being active
//      had no impact on the fixed vscroll region at all, and the fixed vscroll region had
//      no effect on the window. Both interacted exactly as layer A and window regions
//      normally would.
//*LCB: Left Column Blank. Mask column 0. This setting from the SMS VDP is still present
//      in the Mega Drive VDP, even when mode 5 is active. This setting is mentioned in
//      the Accolade doc "The Sega Development System". Hardware tests have confirmed its
//      presence. This setting simply does what it says, which is to mask the first 8-cell
//      column of the active display. Instead of displaying pattern data in this cell, the
//      background layer is displayed instead. This setting affects layers A and B and
//      presumably sprites as well, since it just discards any pixel data within the
//      masked region at the time layer priority selection is performed.
//IE1:  Horizontal interrupt enable
//*HSM: When this register is set, the output mode of the HSYNC pin is altered. Instead of
//      the HSYNC line being asserted when the horizontal sync region begins and negated
//      when it ends, the HSYNC line is instead toggled at the start of each HSYNC region.
//      When HSYNC would normally be negated, nothing happens, and the HSYNC line retains
//      its current value until the next HSYNC region is reached, at which point, the line
//      output state is reversed. Note that the CSYNC line is unaffected by this setting,
//      which means that video output on the Mega Drive is unaffected in H32 mode, but if
//      this bit is set when the EDCLK input is being used, as it should be in H40 mode on
//      the Mega Drive, the video signal will be corrupted, as the EDCLK generator relies
//      on the HSYNC signal in order to generate the input clock signal.
//*PS:  Palette Select. When 0, enables an undocumented mode where only bits 1, 5, and 9
//      of the CRAM data are used to generate colours.
//M3:   Enable HV counter latching on an external interrupt. Note that hardware tests have
//      shown that as soon as this bit is set to 1, the HV counter is actually frozen at
//      that point. The HV counter will latch a new value if the HL input line is asserted
//      after this point, otherwise, it will retain its current value until this bit is
//      cleared.
//*ES:  Setting this bit to 1 appears to disable the C-SYNC output signal. Instead of
//      outputting the CSYNC signal encoding the HSYNC and VSYNC signals together, the
//      CSYNC output pin is always held high. The HSYNC and VSYNC lines, along with the
//      general operation of the VDP, appear otherwise unaffected. Note however that a
//      CSYNC signal is required on the Mega Drive in order to generate a valid video
//      signal. This means setting this bit effectively removes all sync information from
//      the video signal, preventing most monitors from locking onto it and displaying it
//      correctly. The "Software Reference Manual for the Sega Mark III Console" lists
//      this register as "ES", with the only description of it being that it enables an
//      "External Sync" setting. This may indicate there's some way for an external device
//      to provide a sync input line to the VDP, where this input signal is output instead
//      of the internally generated CSYNC signal when this register is set to 1. Note that
//      this input pin may no longer exist on the 315-5313 VDP in the Mega Drive, but if
//      this feature exists, it should be present in the 315-5124 VDP used in the Mark III
//      and the original SMS system.
//##TODO## Test if bit 6 acts as the HSI (Horizontal Scroll Inhibit) bit from the mode 4
//screen mode if column or line based scrolling is enabled.
//----------------------------------------------------------------------------------------
bool S315_5313::M5GetHInterruptEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(4);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetHInterruptEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(4, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetPSEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(2);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetPSEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(2, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetHVCounterLatchEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetHVCounterLatchEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(1, data));
}

//----------------------------------------------------------------------------------------
//         -------------------------------------
//         |  7   | 6  | 5 | 4 | 3 | 2 | 1 | 0 |
//0x01(1)  |-----------------------------------|
//         |*EVRAM|DISP|IE0|M1 |M2 |M5 | / | / |
//         -------------------------------------
//*EVRAM: Enables the extended 128Kb VRAM mode. This is unavailable in the Mega Drive, as
//        only 64Kb of VRAM is present in the system. The only known systems to contain
//        128Kb of VRAM are the Sega TeraDrive, and according to documentation, the "Super
//        Mega Drive" development system produced by Sega.
//DISP:   Display enable. If this bit is cleared, the entire display is filled with the
//        backdrop colour. During this time, the VDP can be accessed at any time without
//        having to wait. Changing this bit takes effect immediately, and it can be
//        toggled mid-line. Note that according to notes in m5hvc.txt by Charles
//        MacDonald, the vblank flag may be set while this bit is cleared, even during
//        active scan.
//IE0 :   Vertical interrupt enable
//M1:     DMA enable
//M2:     V30 mode enable. When set, display is 30 columns high, when cleared it's 28
//        columns high.
//M5:     Enables Mode 5. When cleared, mode 4 is active.
//Note that bit 0 is reported to have an effect in genvdp.txt by Charles MacDonald, but
//all attempts to produce any effect from this bit on the real hardware have failed. Based
//on the description in genvdp.txt, I believe Charles may have been referring to the
//effect from setting reg 0 bit 0 to 1, and an error was made when writing the notes.
//##TODO## Add full documentation and support for the extended VRAM mode
//##TODO## Check if we setup the address register with bit 16 set while extended VRAM mode
//is disabled, then enable extended VRAM mode and do a partial control port write to setup
//a new address, if a VRAM write following this operation goes to the upper or lower VRAM
//area. This will tell us if bit 16 of the address is masked when the address register is
//set, or if it is masked later on.
//##TODO## Check if we have a write pending in the FIFO to the upper memory area when
//extended VRAM mode is disabled, then we enable extended VRAM mode before the write is
//performed, if the pending write ends up going to the lower VRAM area or the upper VRAM
//area. This will tell us if bit 16 of the target VRAM address is masked when entries are
//made into the FIFO, or if it is only masked when writes are being performed.
//##TODO## Confirm if the full VRAM region is available in one continuous segment in
//interlace mode 2 when extended VRAM mode is enabled.
//##TODO## Confirm if the pattern base address registers have any effect in interlace mode
//2.
//----------------------------------------------------------------------------------------
bool S315_5313::M5GetDisplayEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(6);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetDisplayEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(6, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetVInterruptEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(5);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetVInterruptEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(5, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetDMAEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(4);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetDMAEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(4, data));
}

//----------------------------------------------------------------------------------------
//##TODO## Rename these functions to the correct M5GetM2()/M5SetM2()
bool S315_5313::M5GetV30CellModeEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetV30CellModeEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(3, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetMode5Enabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(2);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetMode5Enabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(2, data));
}

//----------------------------------------------------------------------------------------
//         ------------------------------------
//         | 7 | 6 | 5  | 4  | 3  | 2 | 1 | 0 |
//         |----------------------------------|
//         |       |Scroll A Name |           |
//0x02(2)  | /   / |Table Address | /   /   / |
//         |       |--------------|           |
//         |       |SA15|SA14|SA13|           |
//         ------------------------------------
//##TODO## Add in the 17th address bit for all table base address registers, which is used
//in extended VRAM mode.
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetNameTableBaseScrollA(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x02, accessTarget).GetDataSegment(3, 3) << 13;
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetNameTableBaseScrollA(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x02, accessTarget, GetRegisterData(0x02, accessTarget).SetDataSegment(3, 3, data >> 13));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------------
//         | 7 | 6 | 5  | 4  | 3  | 2  |  1  | 0 |
//         |-------------------------------------|
//         |       |Window Name Table Address|   |
//0x03(3)  | /   / |-------------------------| / |
//         |       |WD15|WD14|WD13|WD12|WD11 |   |
//         ---------------------------------------
//WD11: This bit is only used in H32 mode. It is ignored in H40 mode, and is treated as 0.
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetNameTableBaseWindow(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x03, accessTarget).GetDataSegment(1, 5) << 11;
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetNameTableBaseWindow(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x03, accessTarget, GetRegisterData(0x03, accessTarget).SetDataSegment(1, 5, data >> 11));
}

//----------------------------------------------------------------------------------------
//         ------------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2  | 1  | 0  |
//         |----------------------------------|
//         |                   |Scroll B Name |
//0x04(4)  | /   /   /   /   / |Table Address |
//         |                   |--------------|
//         |                   |SB15|SB14|SB13|
//         ------------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetNameTableBaseScrollB(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x04, accessTarget).GetDataSegment(0, 3) << 13;
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetNameTableBaseScrollB(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x04, accessTarget, GetRegisterData(0x04, accessTarget).SetDataSegment(0, 3, data >> 13));
}

//----------------------------------------------------------------------------------------
//         ----------------------------------------
//         | 7 | 6  | 5  | 4  | 3  | 2  |  1  | 0 |
//         |--------------------------------------|
//         |   |  Sprite Attribute Table Address  |
//0x05(5)  | / |----------------------------------|
//         |   |AT15|AT14|AT13|AT12|AT11|AT10 |AT9|
//         ----------------------------------------
//AT0: This bit is only used in H32 mode. It is ignored in H40 mode, and is treated as 0.
//##TODO## There are a lot of tests still required regarding the interaction of AT9 and
//the internal sprite cache, and how screen mode settings changes affect the sprite cache.
//See the notes in RegisterSpecialUpdateFunction for this register for more information.
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetNameTableBaseSprite(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x05, accessTarget).GetDataSegment(0, 7) << 9;
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetNameTableBaseSprite(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x05, accessTarget, GetRegisterData(0x05, accessTarget).SetDataSegment(0, 7, data >> 9));
}

//----------------------------------------------------------------------------------------
//         ----------------------------------
//         | 7 | 6 | 5  | 4 | 3 | 2 | 1 | 0 |
//0x06(6)  |--------------------------------|
//         | / | / |AP16| / | / | / | / | / |
//         ----------------------------------
//AP16: Sprite Pattern Generator Base Address, bit 16. This bit only has an effect in
//      extended VRAM mode.
//----------------------------------------------------------------------------------------
//##TODO## Write functions to access this data

//----------------------------------------------------------------------------------------
//         ------------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2  | 1  | 0  |
//         |----------------------------------|
//         |       |Backgnd|   Background     |
//0x07(7)  | /   / |Palette|  Palette Index   |
//         |       |Select |                  |
//         ------------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetBackgroundColorPalette(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x07, accessTarget).GetDataSegment(4, 2);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetBackgroundColorPalette(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x07, accessTarget, GetRegisterData(0x07, accessTarget).SetDataSegment(4, 2, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetBackgroundColorIndex(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x07, accessTarget).GetDataSegment(0, 4);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetBackgroundColorIndex(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x07, accessTarget, GetRegisterData(0x07, accessTarget).SetDataSegment(0, 4, data));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x0A(10) |-------------------------------|
//         |H-Interrupt Data (Line Counter)|
//         ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetHInterruptData(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0A, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetHInterruptData(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x0A, accessTarget, GetRegisterData(0x0A, accessTarget).SetData(data));
}

//----------------------------------------------------------------------------------------
//         ------------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2  | 1  | 0  |
//0x0B(11) |----------------------------------|
//         |*U1|*U2| /   / |IE2|VSCR|HSCR|LSCR|
//         ------------------------------------
//*U1:  Mentioned by Charles MacDonald in c2tech.txt, described as follows:
//      "VDP controls color bus. Reading or writing color RAM at any address only affects
//      address zero, and the data read/written will often be corrupted."
//*U2:  Mentioned in Accolade doc "The Sega Development System". Description was
//      "Unknown - causes halt sometimes". Hardware tests have shown that setting this bit
//      to 1 can indeed cause some kind of lockup. Just setting the register value to 1
//      and trying to set it back again to 0 seems to lock up the system. If the enable
//      and clear of this register value happen as two halves of a long-word register
//      write however, no lockup occurs. The VDP doesn't request the bus, so it's not
//      clear what causes the lockup at this point. Additional tests required. Note that
//      this bit is also mentioned in c2tech.txt, also described as locking up the
//      hardware when set.
//IE2:  External interrupt enable
//VSCR: Vertical scroll mode. Full screen vertical scrolling when clear, 2-cell column
//      vertical scrolling when set.
//HSCR: If LSCR is not set, HSCR toggles between cell-based horizontal scrolling when set,
//      or full screen horizontal scrolling when clear.
//LSCR: Enables line-based horizontal scrolling. When this bit is set, the state of HSCR
//      is ignored.
//----------------------------------------------------------------------------------------
bool S315_5313::M5GetEXInterruptEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetEXInterruptEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0B, accessTarget, GetRegisterData(0x0B, accessTarget).SetBit(3, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetVSCR(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(2);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetVSCR(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0B, accessTarget, GetRegisterData(0x0B, accessTarget).SetBit(2, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetHSCR(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetHSCR(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0B, accessTarget, GetRegisterData(0x0B, accessTarget).SetBit(1, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetLSCR(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetLSCR(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0B, accessTarget, GetRegisterData(0x0B, accessTarget).SetBit(0, data));
}

//----------------------------------------------------------------------------------------
//         -----------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2  | 1  | 0 |
//0x0C(12) |---------------------------------|
//         |RS0|*U1|*U2|*U3|STE|LSM1|LSM0|RS1|
//         -----------------------------------
//RS0 : When set, the VDP uses the EDCLK input to drive SC directly. When clear, the VDP
//      calculates SC internally, as either MCLK/5 or MCLK/4, depending on the state of
//      the RS1 bit.
//*U1:  When this bit is set, it causes the VSYNC pin to output the internal pixel clock
//      (dot clock) instead of the VSYNC signal.
//*U2:  When this bit is set, the HSYNC line is never asserted. Effectively, the HSYNC
//      line is disabled. It is possible that this is actually causing the HSYNC line to
//      output some other kind of information, which under the circumstances that have
//      been tested so far is always false.
//*U3:  When set, the sprite/layer plane output line (SPA/B) is enabled. This causes that
//      pin to be asserted if the current pixel is a sprite pixel, and negated if it is
//      not.
//STE:  Shadow/highlight mode enable. When set, shadow/highlight mode is active.
//LSM1: Interlace double flag. When set, double interlace mode is active if the interlace
//      enable bit is set. If interlacing is enabled and this bit is not set, normal
//      interlace mode is active.
//LSM0: Interlace enable flag. When set, interlacing is active. Whether normal or double
//      interlace mode is active depends on the state of LSM1.
//RS1:  When set, changes the digital operation of the VDP to enable H40 mode instead of
//      the default H32 mode. Also switches the internally generated SC signal from the
//      MCLK/5 divider used for H32 mode, to an MCLK/4 divider for H40 mode. Note that
//      this signal is not able to be displayed on a normal TV, because the scan rate is
//      too fast. In the Mega Drive, the RS0 bit also needs to be set when enabling H40
//      mode, to enable the external EDCLK input to drive the SC signal. EDCLK has a
//      varying clock rate which increases the clock rate for active scan, while slowing
//      it during HSYNC to keep the overall drawing time of a line in H40 mode the same as
//      in H32 mode, which allows a normal TV to lock onto the signal.
//##TODO## The VDP pinout information in the newly obtained Mega Drive service manuals
//indicate that both CSYNC and HSYNC lines are I/O lines, with the sync information able
//to be input over these lines. It's possible the U2 register here enables external HSYNC
//input.
//----------------------------------------------------------------------------------------
bool S315_5313::M5GetRS0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetRS0(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetRS1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetRS1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(0, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetShadowHighlightEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetShadowHighlightEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(3, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetLSM1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(2);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetLSM1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(2, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetLSM0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetLSM0(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(1, data));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------------
//         | 7 | 6 | 5  | 4  | 3  | 2  | 1  | 0  |
//         |-------------------------------------|
//         |       | HScroll Data Table Address  |
//0x0D(13) | /   / |-----------------------------|
//         |       |HS15|HS14|HS13|HS12|HS11|HS10|
//         ---------------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetHScrollDataBase(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0D, accessTarget).GetDataSegment(0, 6) << 10;
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetHScrollDataBase(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x0D, accessTarget, GetRegisterData(0x0D, accessTarget).SetDataSegment(0, 6, data >> 10));
}

//----------------------------------------------------------------------------------------
//         -----------------------------------
//         | 7 | 6 | 5 | 4  | 3 | 2 | 1 | 0  |
//0x0E(14) |---------------------------------|
//         | / | / | / |PB16| / | / | / |PA16|
//         -----------------------------------
//PA16: When this bit is set and extended VRAM mode is enabled, layer A is rebased to the
//      upper memory area.
//PB16: When this bit and PA16 are both set and extended VRAM mode is enabled, layer B is
//      rebased to the upper memory area.
//----------------------------------------------------------------------------------------
//##TODO## Write functions to access this data

//----------------------------------------------------------------------------------------
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x0F(15) |-------------------------------|
//         |      Auto Increment Data      |
//         ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetAutoIncrementData(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0F, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetAutoIncrementData(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x0F, accessTarget, GetRegisterData(0x0F, accessTarget).SetData(data));
}

//----------------------------------------------------------------------------------------
//         -------------------------------------
//         | 7 | 6 | 5  | 4  | 3 | 2 | 1  | 0  |
//0x10(16) |-----------------------------------|
//         | /   / |VSZ1|VSZ0| /   / |HSZ1|HSZ0|
//         -------------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetVSZ(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetDataSegment(4, 2);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetVSZ(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetDataSegment(4, 2, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetHSZ(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetDataSegment(0, 2);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetHSZ(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetDataSegment(0, 2, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetVSZ1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetBit(5);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetVSZ1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetBit(5, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetVSZ0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetBit(4);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetVSZ0(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetBit(4, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetHSZ1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetHSZ1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetBit(1, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetHSZ0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetHSZ0(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetBit(0, data));
}

//----------------------------------------------------------------------------------------
//         ----------------------------------
//         | 7  | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x11(17) |--------------------------------|
//         |RIGT| /   / |Window H Base Point|
//         ----------------------------------
//Note that despite what is recorded in the Accolade document "The Sega Development
//System", hardware tests have shown that only the lower 5 bits of the register are used
//to calculate the horizontal and vertical window base point, not the lower 7 as recorded
//in this document from Accolade. Setting bits 5 and 6 of these registers has no visible
//effect on the window layer.
//----------------------------------------------------------------------------------------
bool S315_5313::M5GetWindowRightAligned(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x11, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetWindowRightAligned(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x11, accessTarget, GetRegisterData(0x11, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetWindowBasePointX(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x11, accessTarget).GetDataSegment(0, 5);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetWindowBasePointX(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x11, accessTarget, GetRegisterData(0x11, accessTarget).SetDataSegment(0, 5, data));
}

//----------------------------------------------------------------------------------------
//         ----------------------------------
//         | 7  | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x12(18) |--------------------------------|
//         |DOWN| /   / |Window V Base Point|
//         ----------------------------------
//----------------------------------------------------------------------------------------
bool S315_5313::M5GetWindowBottomAligned(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x12, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetWindowBottomAligned(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x12, accessTarget, GetRegisterData(0x12, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetWindowBasePointY(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x12, accessTarget).GetDataSegment(0, 5);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetWindowBasePointY(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x12, accessTarget, GetRegisterData(0x12, accessTarget).SetDataSegment(0, 5, data));
}

//----------------------------------------------------------------------------------------
//         -----------------------------------------
//         | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
//0x13(19) |---------------------------------------|
//         |LG7 |LG6 |LG5 |LG4 |LG3 |LG2 |LG1 |LG0 |
//         -----------------------------------------
//         -----------------------------------------
//         | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
//0x14(20) |---------------------------------------|
//         |LG15|LG14|LG13|LG12|LG11|LG10|LG9 |LG8 |
//         -----------------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetDMALengthCounter(const AccessTarget& accessTarget) const
{
	unsigned int result = GetRegisterData(0x13, accessTarget).GetData();
	result += GetRegisterData(0x14, accessTarget).GetData() << 8;
	return result;
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetDMALengthCounter(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x13, accessTarget, GetRegisterData(0x13, accessTarget).SetData(data));
	SetRegisterData(0x14, accessTarget, GetRegisterData(0x14, accessTarget).SetData(data >> 8));
}

//----------------------------------------------------------------------------------------
//         -----------------------------------------
//         | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
//0x15(21) |---------------------------------------|
//         |SA8 |SA7 |SA6 |SA5 |SA4 |SA3 |SA2 |SA1 |
//         -----------------------------------------
//         -----------------------------------------
//         | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
//0x16(22) |---------------------------------------|
//         |SA16|SA15|SA14|SA13|SA12|SA11|SA10|SA9 |
//         -----------------------------------------
//         -----------------------------------------
//         | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
//0x17(23) |---------------------------------------|
//         |DMD1|DMD0|SA22|SA21|SA20|SA19|SA18|SA17|
//         |    |SA23|    |    |    |    |    |    |
//         -----------------------------------------
//DMD0:   Note that DMD0 acts as both DMD0 and SA23, so there's deliberate overlap in the
//        access functions to these registers.
//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetDMASourceAddress(const AccessTarget& accessTarget) const
{
	unsigned int result = GetRegisterData(0x15, accessTarget).GetData() << 1;
	result += GetRegisterData(0x16, accessTarget).GetData() << 9;
	result += GetRegisterData(0x17, accessTarget).GetDataSegment(0, 7) << 17;
	return result;
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetDMASourceAddress(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x15, accessTarget, GetRegisterData(0x15, accessTarget).SetData(data >> 1));
	SetRegisterData(0x16, accessTarget, GetRegisterData(0x16, accessTarget).SetData(data >> 9));
	SetRegisterData(0x17, accessTarget, GetRegisterData(0x17, accessTarget).SetDataSegment(0, 7, data >> 17));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetDMASourceAddressByte1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x15, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetDMASourceAddressByte1(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x15, accessTarget, GetRegisterData(0x15, accessTarget).SetData(data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetDMASourceAddressByte2(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x16, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetDMASourceAddressByte2(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x16, accessTarget, GetRegisterData(0x16, accessTarget).SetData(data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::M5GetDMASourceAddressByte3(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x17, accessTarget).GetDataSegment(0, 7);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetDMASourceAddressByte3(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x17, accessTarget, GetRegisterData(0x17, accessTarget).SetDataSegment(0, 7, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetDMD1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x17, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetDMD1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x17, accessTarget, GetRegisterData(0x17, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::M5GetDMD0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x17, accessTarget).GetBit(6);
}

//----------------------------------------------------------------------------------------
void S315_5313::M5SetDMD0(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x17, accessTarget, GetRegisterData(0x17, accessTarget).SetBit(6, data));
}
