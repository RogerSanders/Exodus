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
struct S315_5313::DecodedPaletteColorEntry
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
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
//Interpreted register functions
//----------------------------------------------------------------------------------------
//         ----------------------------------------
//         | 7  | 6  | 5  | 4 |   3   | 2 | 1 | 0 |
//0x00(0)  |--------------------------------------|
//         |*VSI|*HSI|*LCB|IE1|*HSM/SS|*PS|M2 |*ES|
//         ----------------------------------------
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
//*HSI: Horizontal Scroll Inhibit. Disable horizontal scrolling for rows 0-1. This
//      register has no apparent effect in mode 5.
//*LCB: Left Column Blank. Mask column 0. This setting from the SMS VDP is still present
//      in the Mega Drive VDP, even when mode 5 is active. This setting is mentioned in
//      the Accolade doc "The Sega Development System". Hardware tests have confirmed its
//      presence. This setting simply does what it says, which is to mask the first 8-cell
//      column of the active display. Instead of displaying pattern data in this cell, the
//      background layer is displayed instead. This setting affects layers A and B and
//      presumably sprites as well, since it just discards any pixel data within the
//      masked region at the time layer priority selection is performed.
//IE1:  Horizontal interrupt enable
//*HSM/SS: When this register is set in mode 5, the output mode of the HSYNC pin is
//      altered. Instead of the HSYNC line being asserted when the horizontal sync region
//      begins and negated when it ends, the HSYNC line is instead toggled at the start of
//      each HSYNC region. When HSYNC would normally be negated, nothing happens, and the
//      HSYNC line retains its current value until the next HSYNC region is reached, at
//      which point, the line output state is reversed. Note that the CSYNC line is
//      unaffected by this setting, which means that video output on the Mega Drive is
//      unaffected in H32 mode, but if this bit is set when the EDCLK input is being used,
//      as it should be in H40 mode on the Mega Drive, the video signal will be corrupted,
//      as the EDCLK generator relies on the HSYNC signal in order to generate the input
//      clock signal.In mode 4, this register is believed to enable Sprite Shift, where
//      all sprites are shifted 8 pixels to the left.
//*PS:  Palette Select. When 0, enables an undocumented mode where only bits 1, 5, and 9
//      of the CRAM data are used to generate colours.
//M2:   Enable HV counter latching on an external interrupt. Note that hardware tests have
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
bool S315_5313::RegGetVSI(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetVSI(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetHSI(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(6);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetHSI(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(6, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetLCB(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(5);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetLCB(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(5, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetIE1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(4);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetIE1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(4, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetSS(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetSS(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(3, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetPS(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(2);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetPS(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(2, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetM2(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetM2(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(1, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetES(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x00, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetES(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x00, accessTarget, GetRegisterData(0x00, accessTarget).SetBit(0, data));
}

//----------------------------------------------------------------------------------------
//         --------------------------------------
//         |  7   | 6  | 5 | 4 | 3 | 2 | 1 | 0  |
//0x01(1)  |------------------------------------|
//         |*EVRAM|DISP|IE0|M1 |M3 |M5 |*SZ|*MAG|
//         --------------------------------------
//*EVRAM: Enables the extended 128Kb VRAM mode. This is unavailable in the Mega Drive, as
//        only 64Kb of VRAM is present in the system. The only known systems to contain
//        128Kb of VRAM are the Sega TeraDrive, and according to documentation, the "Super
//        Mega Drive" development system produced by Sega.
//DISP:   Display enable. If this bit is cleared, the entire display is filled with the
//        backdrop colour. During this time, the VDP can be accessed at any time without
//        having to wait. Changing this bit takes effect immediately, and it can be
//        toggled mid-line. Note that although not officially documented, hardware tests
//        have confirmed that the vblank flag in the status register is always set while
//        the display is disabled, even during active scan.
//IE0 :   Vertical interrupt enable
//M1:     DMA enable
//M3:     V30 mode enable. When set, display is 30 columns high, when cleared it's 28
//        columns high.
//M5:     Enables Mode 5. When cleared, mode 4 is active.
//*SZ:    Sprite Size, in mode 4 only. When set, all sprites are 8x16 instead of 8x8. This
//        register has no apparent effect in mode 5.
//*MAG:   Sprite Zooming, in mode 4 only. This register actually has no apparent effect in
//        either mode 4 or mode 5, but in the 315-5124 VDP used in the SMS, this register
//        enabled sprite magnification, where all sprites are double the width and height,
//        IE, each pixel is 2x2. This feature is not supported by the 315-5313.
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
bool S315_5313::RegGetEVRAM(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetEVRAM(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetDisplayEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(6);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetDisplayEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(6, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetIE0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(5);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetIE0(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(5, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetDMAEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(4);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetDMAEnabled(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(4, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetM3(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetM3(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(3, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetMode5(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(2);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetMode5(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(2, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetSZ(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetSZ(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(1, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetMAG(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x01, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetMAG(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x01, accessTarget, GetRegisterData(0x01, accessTarget).SetBit(0, data));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------------
//         | 7 | 6  | 5  | 4  | 3  | 2  | 1  | 0 |
//         |-------------------------------------|
//0x02(2)  |   | Scroll A Name Table Address |   |
//         |   |-----------------------------|   |
//         |   |SA16|SA15|SA14|SA13|SA12|SA11|   |
//         ---------------------------------------
//SA16: This bit is only used in extended VRAM mode
//SA12/SA11: These bits are only used when mode 4 is enabled
//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetNameTableBaseScrollA(const AccessTarget& accessTarget, bool mode4Enabled, bool extendedVRAMModeEnabled) const
{
	if(mode4Enabled)
	{
		return GetRegisterData(0x02, accessTarget).GetDataSegment(1, 3) << 11;
	}
	return GetRegisterData(0x02, accessTarget).GetDataSegment(3, (extendedVRAMModeEnabled)? 4: 3) << 13;
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetNameTableBaseScrollA(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x02, accessTarget, Data(8, data >> 10));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------------
//         | 7 | 6  | 5  | 4  | 3  | 2  | 1  | 0 |
//         |-------------------------------------|
//         |   |  Window Name Table Address  |   |
//0x03(3)  | / |-----------------------------| / |
//         |   |WD16|WD15|WD14|WD13|WD12|WD11|   |
//         ---------------------------------------
//WD16: This bit is only used in extended VRAM mode
//WD11: This bit is only used in H32 mode. It is ignored in H40 mode, and is treated as 0.
//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetNameTableBaseWindow(const AccessTarget& accessTarget, bool h40ModeActive, bool extendedVRAMModeEnabled) const
{
	//According to official documentation, if we're in H40 mode, the WD11 bit of the
	//window name table base address is masked. We emulate that here. This behaviour
	//has been confirmed through hardware tests.
	if(h40ModeActive)
	{
		return GetRegisterData(0x03, accessTarget).GetDataSegment(2, (extendedVRAMModeEnabled)? 5: 4) << 12;
	}
	return GetRegisterData(0x03, accessTarget).GetDataSegment(1, (extendedVRAMModeEnabled)? 6: 5) << 11;
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetNameTableBaseWindow(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x03, accessTarget, Data(8, data >> 10));
}

//----------------------------------------------------------------------------------------
//         -------------------------------------
//         | 7 | 6 | 5 | 4 | 3  | 2  | 1  | 0  |
//         |-----------------------------------|
//         |               |   Scroll B Name   |
//0x04(4)  | /   /   /   / |   Table Address   |
//         |               |-------------------|
//         |               |SB16|SB15|SB14|SB13|
//         -------------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetNameTableBaseScrollB(const AccessTarget& accessTarget, bool extendedVRAMModeEnabled) const
{
	return GetRegisterData(0x04, accessTarget).GetDataSegment(0, (extendedVRAMModeEnabled)? 4: 3) << 13;
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetNameTableBaseScrollB(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x04, accessTarget, Data(8, data >> 13));
}

//----------------------------------------------------------------------------------------
//         ----------------------------------------
//         | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0 |
//         |--------------------------------------|
//         |    Sprite Attribute Table Address    |
//0x05(5)  |--------------------------------------|
//         |AT16|AT15|AT14|AT13|AT12|AT11|AT10|AT9|
//         ----------------------------------------
//AT16: This bit is only used in extended VRAM mode
//AT9: This bit is only used in H32 mode. It is ignored in H40 mode, and is treated as 0.
//##TODO## There are a lot of tests still required regarding the interaction of AT9 and
//the internal sprite cache, and how screen mode settings changes affect the sprite cache.
//See the notes in RegisterSpecialUpdateFunction for this register for more information.
//
//Note that in mode 4, the register layout is as follows:
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//         |-------------------------------|
//         |   |Sprite Attribute Table |   |
//0x05(5)  | / |     Base Address      | / |
//         |   |-----------------------|   |
//         |   |B13|B12|B11|B10|B9 |B8 |   |
//         ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetNameTableBaseSprite(const AccessTarget& accessTarget, bool mode4Enabled, bool h40ModeActive, bool extendedVRAMModeEnabled) const
{
	if(mode4Enabled)
	{
		return GetRegisterData(0x05, accessTarget).GetDataSegment(1, 6) << 8;
	}
	else if(h40ModeActive)
	{
		//According to official documentation, if we're in H40 mode, the AT9 bit of the
		//sprite table base address is masked. We emulate that here. Note that the
		//"Traveller's Tales" logo in Sonic 3D on the Mega Drive relies on AT9 being valid
		//in H32 mode.
		return GetRegisterData(0x05, accessTarget).GetDataSegment(1, (extendedVRAMModeEnabled)? 7: 6) << 10;
	}
	return GetRegisterData(0x05, accessTarget).GetDataSegment(0, (extendedVRAMModeEnabled)? 8: 7) << 9;
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetNameTableBaseSprite(const AccessTarget& accessTarget, unsigned int data, bool mode4Enabled)
{
	if(mode4Enabled)
	{
		SetRegisterData(0x05, accessTarget, Data(8, data >> 7));
	}
	else
	{
		SetRegisterData(0x05, accessTarget, Data(8, data >> 9));
	}
}

//----------------------------------------------------------------------------------------
//         -----------------------------------
//         | 7 | 6 | 5  | 4 | 3 | 2  | 1 | 0 |
//0x06(6)  |---------------------------------|
//         | / | / |AP16| / | / |AP13| / | / |
//         -----------------------------------
//AP16: This bit is only used in extended VRAM mode
//AP13: This bit is only used in mode 4
//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetPatternBaseSprite(const AccessTarget& accessTarget, bool mode4Enabled, bool extendedVRAMModeEnabled) const
{
	if(mode4Enabled)
	{
		return GetRegisterData(0x06, accessTarget).GetDataSegment(2, 1) << 13;
	}
	else if(extendedVRAMModeEnabled)
	{
		return GetRegisterData(0x06, accessTarget).GetDataSegment(5, 1) << 16;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetPatternBaseSprite(const AccessTarget& accessTarget, unsigned int data, bool mode4Enabled)
{
	if(mode4Enabled)
	{
		SetRegisterData(0x06, accessTarget, Data(8, data >> 13));
	}
	else
	{
		SetRegisterData(0x06, accessTarget, Data(8, data >> 16));
	}
}

//----------------------------------------------------------------------------------------
//         ------------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2  | 1  | 0  |
//         |----------------------------------|
//         |       |Backgnd|   Background     |
//0x07(7)  | /   / |Palette|  Palette Index   |
//         |       |Select |                  |
//         ------------------------------------
//----------------------------------------------------------------------------------------
bool S315_5313::RegGet077(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x07, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet077(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x07, accessTarget, GetRegisterData(0x07, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGet076(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x07, accessTarget).GetBit(6);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet076(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x07, accessTarget, GetRegisterData(0x07, accessTarget).SetBit(6, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetBackgroundPaletteRow(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x07, accessTarget).GetDataSegment(4, 2);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetBackgroundPaletteRow(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x07, accessTarget, GetRegisterData(0x07, accessTarget).SetDataSegment(4, 2, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetBackgroundPaletteColumn(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x07, accessTarget).GetDataSegment(0, 4);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetBackgroundPaletteColumn(const AccessTarget& accessTarget, unsigned int data)
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
unsigned int S315_5313::RegGetBackgroundScrollX(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x08, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetBackgroundScrollX(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x08, accessTarget, Data(8, data));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x09(9)  |-------------------------------|
//         |      Background Scroll Y      |
//         ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetBackgroundScrollY(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x09, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetBackgroundScrollY(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x09, accessTarget, Data(8, data));
}

//----------------------------------------------------------------------------------------
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x0A(10) |-------------------------------|
//         |H-Interrupt Data (Line Counter)|
//         ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetHInterruptData(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0A, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetHInterruptData(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x0A, accessTarget, Data(8, data));
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
bool S315_5313::RegGet0B7(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet0B7(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0B, accessTarget, GetRegisterData(0x0B, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGet0B6(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(6);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet0B6(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0B, accessTarget, GetRegisterData(0x0B, accessTarget).SetBit(6, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGet0B5(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(5);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet0B5(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0B, accessTarget, GetRegisterData(0x0B, accessTarget).SetBit(5, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGet0B4(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(4);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet0B4(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0B, accessTarget, GetRegisterData(0x0B, accessTarget).SetBit(4, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetIE2(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetIE2(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0B, accessTarget, GetRegisterData(0x0B, accessTarget).SetBit(3, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetVSCR(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(2);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetVSCR(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0B, accessTarget, GetRegisterData(0x0B, accessTarget).SetBit(2, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetHSCR(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetHSCR(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0B, accessTarget, GetRegisterData(0x0B, accessTarget).SetBit(1, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetLSCR(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0B, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetLSCR(const AccessTarget& accessTarget, bool data)
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
bool S315_5313::RegGetRS0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetRS0(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetU1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(6);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetU1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(6, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetU2(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(5);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetU2(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(5, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetU3(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(4);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetU3(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(4, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetSTE(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetSTE(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(3, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetLSM1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(2);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetLSM1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(2, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetLSM0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetLSM0(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(1, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetRS1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0C, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetRS1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x0C, accessTarget, GetRegisterData(0x0C, accessTarget).SetBit(0, data));
}

//----------------------------------------------------------------------------------------
//         ----------------------------------------
//         | 7 | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
//         |--------------------------------------|
//         |   |    HScroll Data Table Address    |
//0x0D(13) | / |----------------------------------|
//         |   |HS16|HS15|HS14|HS13|HS12|HS11|HS10|
//         ----------------------------------------
//HS16: This bit is only used in extended VRAM mode
//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetHScrollDataBase(const AccessTarget& accessTarget, bool extendedVRAMModeEnabled) const
{
	return GetRegisterData(0x0D, accessTarget).GetDataSegment(0, (extendedVRAMModeEnabled)? 7: 6) << 10;
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetHScrollDataBase(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x0D, accessTarget, Data(8, data >> 10));
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
unsigned int S315_5313::RegGet0E57(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0E, accessTarget).GetDataSegment(5, 3);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet0E57(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x0E, accessTarget, GetRegisterData(0x0E, accessTarget).SetDataSegment(5, 3, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetPatternBaseScrollA(const AccessTarget& accessTarget, bool extendedVRAMModeEnabled) const
{
	if(extendedVRAMModeEnabled)
	{
		return GetRegisterData(0x0E, accessTarget).GetDataSegment(0, 1) << 16;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetPatternBaseScrollA(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x0E, accessTarget, GetRegisterData(0x0E, accessTarget).SetDataSegment(4, 1, data >> 16));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGet0E13(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0E, accessTarget).GetDataSegment(1, 3);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet0E13(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x0E, accessTarget, GetRegisterData(0x0E, accessTarget).SetDataSegment(1, 3, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetPatternBaseScrollB(const AccessTarget& accessTarget, bool extendedVRAMModeEnabled) const
{
	if(extendedVRAMModeEnabled)
	{
		return RegGetPatternBaseScrollA(accessTarget, extendedVRAMModeEnabled) & (GetRegisterData(0x0E, accessTarget).GetDataSegment(4, 1) << 16);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetPatternBaseScrollB(const AccessTarget& accessTarget, unsigned int data)
{
	unsigned int newData = data >> 16;
	SetRegisterData(0x0E, accessTarget, GetRegisterData(0x0E, accessTarget).SetDataSegment(0, 1, newData));
	if(newData != 0)
	{
		RegSetPatternBaseScrollA(accessTarget, data);
	}
}


//----------------------------------------------------------------------------------------
//         ---------------------------------
//         | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//0x0F(15) |-------------------------------|
//         |      Auto Increment Data      |
//         ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetAutoIncrementData(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x0F, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetAutoIncrementData(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x0F, accessTarget, Data(8, data));
}

//----------------------------------------------------------------------------------------
//         -------------------------------------
//         | 7 | 6 | 5  | 4  | 3 | 2 | 1  | 0  |
//0x10(16) |-----------------------------------|
//         | /   / |VSZ1|VSZ0| /   / |HSZ1|HSZ0|
//         -------------------------------------
//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGet1067(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetDataSegment(6, 2);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet1067(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetDataSegment(6, 2, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetVSZ(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetDataSegment(4, 2);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetVSZ(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetDataSegment(4, 2, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetVSZ1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetBit(5);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetVSZ1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetBit(5, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetVSZ0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetBit(4);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetVSZ0(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetBit(4, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGet1023(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetDataSegment(2, 2);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet1023(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetDataSegment(2, 2, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetHSZ(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetDataSegment(0, 2);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetHSZ(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetDataSegment(0, 2, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetHSZ1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetHSZ1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x10, accessTarget, GetRegisterData(0x10, accessTarget).SetBit(1, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetHSZ0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x10, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetHSZ0(const AccessTarget& accessTarget, bool data)
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
bool S315_5313::RegGetWindowRightAligned(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x11, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetWindowRightAligned(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x11, accessTarget, GetRegisterData(0x11, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGet1156(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x11, accessTarget).GetDataSegment(5, 2);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet1156(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x11, accessTarget, GetRegisterData(0x11, accessTarget).SetDataSegment(5, 2, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetWindowBasePointX(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x11, accessTarget).GetDataSegment(0, 5);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetWindowBasePointX(const AccessTarget& accessTarget, unsigned int data)
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
bool S315_5313::RegGetWindowBottomAligned(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x12, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetWindowBottomAligned(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x12, accessTarget, GetRegisterData(0x12, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGet1256(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x12, accessTarget).GetDataSegment(5, 2);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSet1256(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x12, accessTarget, GetRegisterData(0x12, accessTarget).SetDataSegment(5, 2, data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetWindowBasePointY(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x12, accessTarget).GetDataSegment(0, 5);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetWindowBasePointY(const AccessTarget& accessTarget, unsigned int data)
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
unsigned int S315_5313::RegGetDMALengthCounter(const AccessTarget& accessTarget) const
{
	unsigned int result = GetRegisterData(0x13, accessTarget).GetData();
	result += GetRegisterData(0x14, accessTarget).GetData() << 8;
	return result;
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetDMALengthCounter(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x13, accessTarget, Data(8, data));
	SetRegisterData(0x14, accessTarget, Data(8, data >> 8));
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
unsigned int S315_5313::RegGetDMASourceAddress(const AccessTarget& accessTarget) const
{
	unsigned int result = GetRegisterData(0x15, accessTarget).GetData() << 1;
	result += GetRegisterData(0x16, accessTarget).GetData() << 9;
	result += GetRegisterData(0x17, accessTarget).GetDataSegment(0, 7) << 17;
	return result;
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetDMASourceAddress(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x15, accessTarget, Data(8, data >> 1));
	SetRegisterData(0x16, accessTarget, Data(8, data >> 9));
	SetRegisterData(0x17, accessTarget, GetRegisterData(0x17, accessTarget).SetDataSegment(0, 7, data >> 17));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetDMASourceAddressByte1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x15, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetDMASourceAddressByte1(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x15, accessTarget, GetRegisterData(0x15, accessTarget).SetData(data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetDMASourceAddressByte2(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x16, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetDMASourceAddressByte2(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x16, accessTarget, GetRegisterData(0x16, accessTarget).SetData(data));
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegGetDMASourceAddressByte3(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x17, accessTarget).GetDataSegment(0, 7);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetDMASourceAddressByte3(const AccessTarget& accessTarget, unsigned int data)
{
	SetRegisterData(0x17, accessTarget, GetRegisterData(0x17, accessTarget).SetDataSegment(0, 7, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetDMD1(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x17, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetDMD1(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x17, accessTarget, GetRegisterData(0x17, accessTarget).SetBit(7, data));
}

//----------------------------------------------------------------------------------------
bool S315_5313::RegGetDMD0(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x17, accessTarget).GetBit(6);
}

//----------------------------------------------------------------------------------------
void S315_5313::RegSetDMD0(const AccessTarget& accessTarget, bool data)
{
	SetRegisterData(0x17, accessTarget, GetRegisterData(0x17, accessTarget).SetBit(6, data));
}
