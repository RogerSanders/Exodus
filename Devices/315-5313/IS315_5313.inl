//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IS315_5313::IS315_5313DataSource
{
	DATASOURCE_RAWREGISTER = 1,
	DATASOURCE_REG_STATUS,
	DATASOURCE_FLAG_FIFOEMPTY,
	DATASOURCE_FLAG_FIFOFULL,
	DATASOURCE_FLAG_F,
	DATASOURCE_FLAG_SPRITEOVERFLOW,
	DATASOURCE_FLAG_SPRITECOLLISION,
	DATASOURCE_FLAG_ODDINTERLACEFRAME,
	DATASOURCE_FLAG_VBLANK,
	DATASOURCE_FLAG_HBLANK,
	DATASOURCE_FLAG_DMA,
	DATASOURCE_FLAG_PAL,
	DATASOURCE_REG_VSI,
	DATASOURCE_REG_HSI,
	DATASOURCE_REG_LCB,
	DATASOURCE_REG_IE1,
	DATASOURCE_REG_SS,
	DATASOURCE_REG_PS,
	DATASOURCE_REG_M2,
	DATASOURCE_REG_ES,
	DATASOURCE_REG_EVRAM,
	DATASOURCE_REG_DISPLAYENABLED,
	DATASOURCE_REG_IE0,
	DATASOURCE_REG_DMAENABLED,
	DATASOURCE_REG_M3,
	DATASOURCE_REG_MODE5,
	DATASOURCE_REG_SZ,
	DATASOURCE_REG_MAG,
	DATASOURCE_REG_NAMETABLEBASEA,
	DATASOURCE_REG_NAMETABLEBASEWINDOW,
	DATASOURCE_REG_NAMETABLEBASEB,
	DATASOURCE_REG_NAMETABLEBASESPRITE,
	DATASOURCE_REG_PATTERNBASESPRITE,
	DATASOURCE_REG_077,
	DATASOURCE_REG_076,
	DATASOURCE_REG_BACKGROUNDPALETTEROW,
	DATASOURCE_REG_BACKGROUNDPALETTECOLUMN,
	DATASOURCE_REG_BACKGROUNDSCROLLX,
	DATASOURCE_REG_BACKGROUNDSCROLLY,
	DATASOURCE_REG_HINTDATA,
	DATASOURCE_REG_0B7,
	DATASOURCE_REG_0B6,
	DATASOURCE_REG_0B5,
	DATASOURCE_REG_0B4,
	DATASOURCE_REG_IE2,
	DATASOURCE_REG_VSCR,
	DATASOURCE_REG_HSCR,
	DATASOURCE_REG_LSCR,
	DATASOURCE_REG_RS0,
	DATASOURCE_REG_U1,
	DATASOURCE_REG_U2,
	DATASOURCE_REG_U3,
	DATASOURCE_REG_STE,
	DATASOURCE_REG_LSM1,
	DATASOURCE_REG_LSM0,
	DATASOURCE_REG_RS1,
	DATASOURCE_REG_HSCROLLDATABASE,
	DATASOURCE_REG_0E57,
	DATASOURCE_REG_PATTERNBASEA,
	DATASOURCE_REG_0E13,
	DATASOURCE_REG_PATTERNBASEB,
	DATASOURCE_REG_AUTOINCREMENTDATA,
	DATASOURCE_REG_1067,
	DATASOURCE_REG_VSZ,
	DATASOURCE_REG_VSZ1,
	DATASOURCE_REG_VSZ0,
	DATASOURCE_REG_1023,
	DATASOURCE_REG_HSZ,
	DATASOURCE_REG_HSZ1,
	DATASOURCE_REG_HSZ0,
	DATASOURCE_REG_1156,
	DATASOURCE_REG_WINDOWRIGHT,
	DATASOURCE_REG_WINDOWBASEX,
	DATASOURCE_REG_1256,
	DATASOURCE_REG_WINDOWBOTTOM,
	DATASOURCE_REG_WINDOWBASEY,
	DATASOURCE_REG_DMALENGTH,
	DATASOURCE_REG_DMASOURCE,
	DATASOURCE_REG_DMASOURCEDATA1,
	DATASOURCE_REG_DMASOURCEDATA2,
	DATASOURCE_REG_DMASOURCEDATA3,
	DATASOURCE_REG_DMD1,
	DATASOURCE_REG_DMD0,
	DATASOURCE_REG_CODE,
	DATASOURCE_REG_ADDRESS,
	DATASOURCE_REG_PORTWRITEPENDING,
	DATASOURCE_REG_READBUFFER,
	DATASOURCE_REG_READHALFCACHED,
	DATASOURCE_REG_READFULLYCACHED,
	DATASOURCE_REG_VINTPENDING,
	DATASOURCE_REG_HINTPENDING,
	DATASOURCE_REG_EXINTPENDING,
	DATASOURCE_REG_HVCOUNTEREXTERNAL,
	DATASOURCE_REG_HCOUNTERINTERNAL,
	DATASOURCE_REG_VCOUNTERINTERNAL,
	DATASOURCE_REG_HCOUNTERLATCHED,
	DATASOURCE_REG_VCOUNTERLATCHED,
	DATASOURCE_REG_FIFO_CODE,
	DATASOURCE_REG_FIFO_ADDRESS,
	DATASOURCE_REG_FIFO_DATA,
	DATASOURCE_REG_FIFO_WRITEPENDING,
	DATASOURCE_REG_FIFO_WRITEHALFWRITTEN,
	DATASOURCE_REG_NEXTFIFOREADENTRY,
	DATASOURCE_REG_NEXTFIFOWRITEENTRY,
	DATASOURCE_SETTINGS_OUTPUTPORTACCESSDEBUGMESSAGES,
	DATASOURCE_SETTINGS_OUTPUTTIMINGDEBUGMESSAGES,
	DATASOURCE_SETTINGS_OUTPUTRENDERSYNCMESSAGES,
	DATASOURCE_SETTINGS_OUTPUTINTERRUPTDEBUGMESSAGES,
	DATASOURCE_SETTINGS_VIDEODISABLERENDEROUTPUT,
	DATASOURCE_SETTINGS_VIDEOENABLESPRITEBOXING,
	DATASOURCE_SETTINGS_VIDEOHIGHLIGHTRENDERPOS,
	DATASOURCE_SETTINGS_VIDEOSINGLEBUFFERING,
	DATASOURCE_SETTINGS_VIDEOFIXEDASPECTRATIO,
	DATASOURCE_SETTINGS_VIDEOSHOWSTATUSBAR,
	DATASOURCE_SETTINGS_CURRENTRENDERPOSONSCREEN,
	DATASOURCE_SETTINGS_CURRENTRENDERPOSSCREENX,
	DATASOURCE_SETTINGS_CURRENTRENDERPOSSCREENY,
	DATASOURCE_SETTINGS_VIDEOSHOWBOUNDARYACTIVEIMAGE,
	DATASOURCE_SETTINGS_VIDEOSHOWBOUNDARYACTIONSAFE,
	DATASOURCE_SETTINGS_VIDEOSHOWBOUNDARYTITLESAFE,
	DATASOURCE_SETTINGS_VIDEOENABLELAYERAHIGH,
	DATASOURCE_SETTINGS_VIDEOENABLELAYERALOW,
	DATASOURCE_SETTINGS_VIDEOENABLELAYERBHIGH,
	DATASOURCE_SETTINGS_VIDEOENABLELAYERBLOW,
	DATASOURCE_SETTINGS_VIDEOENABLEWINDOWHIGH,
	DATASOURCE_SETTINGS_VIDEOENABLEWINDOWLOW,
	DATASOURCE_SETTINGS_VIDEOENABLESPRITEHIGH,
	DATASOURCE_SETTINGS_VIDEOENABLESPRITELOW
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct IS315_5313::RegisterDataContext :public IGenericAccess::DataContext
{
	RegisterDataContext(unsigned int aregisterNo = 0)
	:registerNo(aregisterNo)
	{}

	unsigned int registerNo;
};

//----------------------------------------------------------------------------------------
struct IS315_5313::FIFOEntryDataContext :public IGenericAccess::DataContext
{
	FIFOEntryDataContext(unsigned int aentryNo = 0)
	:entryNo(aentryNo)
	{}

	unsigned int entryNo;
};

//----------------------------------------------------------------------------------------
struct IS315_5313::PortMonitorEntry
{
	//##FIX## We're sharing STL objects here
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
struct IS315_5313::SpriteMappingTableEntry
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
struct IS315_5313::DecodedPaletteColorEntry
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
};

//----------------------------------------------------------------------------------------
struct IS315_5313::SpriteBoundaryLineEntry
{
	int linePosXStart;
	int linePosXEnd;
	int linePosYStart;
	int linePosYEnd;
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IS315_5313::ThisIS315_5313Version()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Rendering functions
//----------------------------------------------------------------------------------------
std::list<IS315_5313::SpriteBoundaryLineEntry> IS315_5313::GetSpriteBoundaryLines(unsigned int planeNo) const
{
	std::list<IS315_5313::SpriteBoundaryLineEntry> result;
	GetSpriteBoundaryLinesInternal(planeNo, InteropSupport::STLObjectTarget<std::list<IS315_5313::SpriteBoundaryLineEntry>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Debug output
//----------------------------------------------------------------------------------------
bool IS315_5313::GetOutputPortAccessDebugMessages() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_OUTPUTPORTACCESSDEBUGMESSAGES, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetOutputPortAccessDebugMessages(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_OUTPUTPORTACCESSDEBUGMESSAGES, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetOutputTimingDebugMessages() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_OUTPUTTIMINGDEBUGMESSAGES, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetOutputTimingDebugMessages(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_OUTPUTTIMINGDEBUGMESSAGES, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetOutputRenderSyncMessages() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_OUTPUTRENDERSYNCMESSAGES, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetOutputRenderSyncMessages(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_OUTPUTRENDERSYNCMESSAGES, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetOutputInterruptDebugMessages() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_OUTPUTINTERRUPTDEBUGMESSAGES, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetOutputInterruptDebugMessages(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_OUTPUTINTERRUPTDEBUGMESSAGES, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoDisableRenderOutput() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEODISABLERENDEROUTPUT, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoDisableRenderOutput(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEODISABLERENDEROUTPUT, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoEnableSpriteBoxing() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLESPRITEBOXING, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoEnableSpriteBoxing(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLESPRITEBOXING, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoHighlightRenderPos() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOHIGHLIGHTRENDERPOS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoHighlightRenderPos(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOHIGHLIGHTRENDERPOS, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoSingleBuffering() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOSINGLEBUFFERING, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoSingleBuffering(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOSINGLEBUFFERING, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoFixedAspectRatio() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOFIXEDASPECTRATIO, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoFixedAspectRatio(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOFIXEDASPECTRATIO, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoShowStatusBar() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOSHOWSTATUSBAR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoShowStatusBar(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOSHOWSTATUSBAR, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetCurrentRenderPosOnScreen() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_CURRENTRENDERPOSONSCREEN, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetCurrentRenderPosOnScreen(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_CURRENTRENDERPOSONSCREEN, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::GetCurrentRenderPosScreenX() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_CURRENTRENDERPOSSCREENX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetCurrentRenderPosScreenX(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_CURRENTRENDERPOSSCREENX, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::GetCurrentRenderPosScreenY() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_CURRENTRENDERPOSSCREENY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetCurrentRenderPosScreenY(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_CURRENTRENDERPOSSCREENY, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoShowBoundaryActiveImage() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOSHOWBOUNDARYACTIVEIMAGE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoShowBoundaryActiveImage(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOSHOWBOUNDARYACTIVEIMAGE, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoShowBoundaryActionSafe() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOSHOWBOUNDARYACTIONSAFE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoShowBoundaryActionSafe(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOSHOWBOUNDARYACTIONSAFE, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoShowBoundaryTitleSafe() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOSHOWBOUNDARYTITLESAFE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoShowBoundaryTitleSafe(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOSHOWBOUNDARYTITLESAFE, 0, data);
}

//----------------------------------------------------------------------------------------
//Layer removal
//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableLayerAHigh() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLELAYERAHIGH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableLayerAHigh(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLELAYERAHIGH, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableLayerALow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLELAYERALOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableLayerALow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLELAYERALOW, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableLayerBHigh() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLELAYERBHIGH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableLayerBHigh(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLELAYERBHIGH, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableLayerBLow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLELAYERBLOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableLayerBLow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLELAYERBLOW, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableWindowHigh() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLEWINDOWHIGH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableWindowHigh(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLEWINDOWHIGH, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableWindowLow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLEWINDOWLOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableWindowLow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLEWINDOWLOW, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableSpriteHigh() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLESPRITEHIGH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableSpriteHigh(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLESPRITEHIGH, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableSpriteLow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLESPRITELOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableSpriteLow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SETTINGS_VIDEOENABLESPRITELOW, 0, data);
}

//----------------------------------------------------------------------------------------
//Raw register functions
//----------------------------------------------------------------------------------------
unsigned int IS315_5313::GetRegisterData(unsigned int location) const
{
	GenericAccessDataValueUInt data;
	RegisterDataContext dataContext(location);
	ReadGenericData((unsigned int)DATASOURCE_RAWREGISTER, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetRegisterData(unsigned int location, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	RegisterDataContext dataContext(location);
	WriteGenericData((unsigned int)DATASOURCE_RAWREGISTER, &dataContext, data);
}

//----------------------------------------------------------------------------------------
//Interpreted register functions
//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetVSI() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_VSI, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVSI(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_VSI, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetHSI() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_HSI, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHSI(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_HSI, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetLCB() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_LCB, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetLCB(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_LCB, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetIE1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_IE1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetIE1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_IE1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetSS() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_SS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetSS(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_SS, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetPS() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_PS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPS(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_PS, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetM2() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_M2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetM2(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_M2, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetES() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_ES, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetES(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_ES, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetEVRAM() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_EVRAM, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetEVRAM(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_EVRAM, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetDisplayEnabled() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_DISPLAYENABLED, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDisplayEnabled(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_DISPLAYENABLED, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetIE0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_IE0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetIE0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_IE0, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetDMAEnabled() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_DMAENABLED, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMAEnabled(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_DMAENABLED, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetM3() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_M3, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetM3(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_M3, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetMode5() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_MODE5, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetMode5(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_MODE5, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetSZ() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_SZ, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetSZ(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_SZ, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetMAG() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_MAG, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetMAG(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_MAG, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetNameTableBaseScrollA() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_NAMETABLEBASEA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetNameTableBaseScrollA(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_NAMETABLEBASEA, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetNameTableBaseWindow() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_NAMETABLEBASEWINDOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetNameTableBaseWindow(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_NAMETABLEBASEWINDOW, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetNameTableBaseScrollB() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_NAMETABLEBASEB, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetNameTableBaseScrollB(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_NAMETABLEBASEB, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetNameTableBaseSprite() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_NAMETABLEBASESPRITE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetNameTableBaseSprite(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_NAMETABLEBASESPRITE, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetPatternBaseSprite() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_PATTERNBASESPRITE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPatternBaseSprite(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_PATTERNBASESPRITE, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet077() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_077, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet077(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_077, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet076() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_076, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet076(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_076, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetBackgroundPaletteRow() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_BACKGROUNDPALETTEROW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetBackgroundPaletteRow(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_BACKGROUNDPALETTEROW, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetBackgroundPaletteColumn() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_BACKGROUNDPALETTECOLUMN, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetBackgroundPaletteColumn(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_BACKGROUNDPALETTECOLUMN, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetBackgroundScrollX() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_BACKGROUNDSCROLLX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetBackgroundScrollX(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_BACKGROUNDSCROLLX, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetBackgroundScrollY() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_BACKGROUNDSCROLLY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetBackgroundScrollY(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_BACKGROUNDSCROLLY, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHInterruptData() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_HINTDATA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHInterruptData(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_HINTDATA, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet0B7() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_0B7, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0B7(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_0B7, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet0B6() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_0B6, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0B6(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_0B6, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet0B5() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_0B5, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0B5(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_0B5, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet0B4() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_0B4, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0B4(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_0B4, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetIE2() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_IE2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetIE2(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_IE2, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetVSCR() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_VSCR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVSCR(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_VSCR, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetHSCR() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_HSCR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHSCR(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_HSCR, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetLSCR() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_LSCR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetLSCR(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_LSCR, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetRS0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_RS0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetRS0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_RS0, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetU1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_U1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetU1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_U1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetU2() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_U2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetU2(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_U2, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetU3() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_U3, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetU3(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_U3, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetSTE() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_STE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetSTE(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_STE, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetLSM1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_LSM1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetLSM1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_LSM1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetLSM0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_LSM0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetLSM0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_LSM0, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetRS1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_RS1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetRS1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_RS1, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHScrollDataBase() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_HSCROLLDATABASE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHScrollDataBase(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_HSCROLLDATABASE, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet0E57() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_0E57, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0E57(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_0E57, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetPatternBaseScrollA() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_PATTERNBASEA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPatternBaseScrollA(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_PATTERNBASEA, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet0E13() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_PATTERNBASEA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0E13(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_PATTERNBASEA, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetPatternBaseScrollB() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_PATTERNBASEB, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPatternBaseScrollB(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_PATTERNBASEB, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetAutoIncrementData() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_AUTOINCREMENTDATA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetAutoIncrementData(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_AUTOINCREMENTDATA, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet1067() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_1067, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet1067(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_1067, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetVSZ() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_VSZ, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVSZ(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_VSZ, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetVSZ1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_VSZ1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVSZ1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_VSZ1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetVSZ0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_VSZ0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVSZ0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_VSZ0, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet1023() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_1023, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet1023(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_1023, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHSZ() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_HSZ, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHSZ(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_HSZ, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetHSZ1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_HSZ1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHSZ1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_HSZ1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetHSZ0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_HSZ0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHSZ0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_HSZ0, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet1156() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_1156, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet1156(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_1156, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetWindowRightAligned() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_WINDOWRIGHT, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetWindowRightAligned(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_WINDOWRIGHT, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetWindowBasePointX() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_WINDOWBASEX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetWindowBasePointX(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_WINDOWBASEX, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet1256() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_1256, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet1256(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_1256, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetWindowBottomAligned() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_WINDOWBOTTOM, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetWindowBottomAligned(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_WINDOWBOTTOM, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetWindowBasePointY() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_WINDOWBASEY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetWindowBasePointY(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_WINDOWBASEY, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetDMALengthCounter() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_DMALENGTH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMALengthCounter(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_DMALENGTH, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetDMASourceAddress() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_DMASOURCE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMASourceAddress(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_DMASOURCE, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetDMASourceAddressByte1() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_DMASOURCEDATA1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMASourceAddressByte1(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_DMASOURCEDATA1, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetDMASourceAddressByte2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_DMASOURCEDATA2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMASourceAddressByte2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_DMASOURCEDATA2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetDMASourceAddressByte3() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_DMASOURCEDATA3, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMASourceAddressByte3(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_DMASOURCEDATA3, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetDMD1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_DMD1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMD1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_DMD1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetDMD0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_DMD0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMD0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_DMD0, 0, data);
}

//----------------------------------------------------------------------------------------
//Status register functions
//----------------------------------------------------------------------------------------
unsigned int IS315_5313::GetStatus() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_STATUS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatus(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_STATUS, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagFIFOEmpty() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_FLAG_FIFOEMPTY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagFIFOEmpty(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_FLAG_FIFOEMPTY, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagFIFOFull() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_FLAG_FIFOFULL, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagFIFOFull(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_FLAG_FIFOFULL, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagF() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_FLAG_F, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagF(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_FLAG_F, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagSpriteOverflow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_FLAG_SPRITEOVERFLOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagSpriteOverflow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_FLAG_SPRITEOVERFLOW, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagSpriteCollision() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_FLAG_SPRITECOLLISION, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagSpriteCollision(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_FLAG_SPRITECOLLISION, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagOddInterlaceFrame() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_FLAG_ODDINTERLACEFRAME, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagOddInterlaceFrame(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_FLAG_ODDINTERLACEFRAME, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagVBlank() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_FLAG_VBLANK, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagVBlank(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_FLAG_VBLANK, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagHBlank() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_FLAG_HBLANK, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagHBlank(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_FLAG_HBLANK, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagDMA() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_FLAG_DMA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagDMA(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_FLAG_DMA, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagPAL() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_FLAG_PAL, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagPAL(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_FLAG_PAL, 0, data);
}

//----------------------------------------------------------------------------------------
//Port register functions
//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetPortCode() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_CODE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPortCode(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_CODE, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetPortAddress() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_ADDRESS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPortAddress(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_ADDRESS, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetPortWritePending() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_PORTWRITEPENDING, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPortWritePending(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_PORTWRITEPENDING, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetReadBuffer() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_READBUFFER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetReadBuffer(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_READBUFFER, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetReadHalfCached() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_READHALFCACHED, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetReadHalfCached(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_READHALFCACHED, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetReadFullyCached() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_READFULLYCACHED, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetReadFullyCached(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_READFULLYCACHED, 0, data);
}

//----------------------------------------------------------------------------------------
//Interrupt register functions
//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetVINTPending() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_VINTPENDING, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVINTPending(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_VINTPENDING, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetHINTPending() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_HINTPENDING, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHINTPending(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_HINTPENDING, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetEXINTPending() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_REG_EXINTPENDING, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetEXINTPending(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_EXINTPENDING, 0, data);
}

//----------------------------------------------------------------------------------------
//HV counter register functions
//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHVCounterExternal() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_HVCOUNTEREXTERNAL, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHCounterInternal() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_HCOUNTERINTERNAL, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHCounterInternal(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_HCOUNTERINTERNAL, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetVCounterInternal() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_VCOUNTERINTERNAL, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVCounterInternal(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_VCOUNTERINTERNAL, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHCounterLatched() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_HCOUNTERLATCHED, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHCounterLatched(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_HCOUNTERLATCHED, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetVCounterLatched() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_VCOUNTERLATCHED, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVCounterLatched(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_VCOUNTERLATCHED, 0, data);
}

//----------------------------------------------------------------------------------------
//FIFO register functions
//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetFIFOCode(unsigned int entryNo) const
{
	GenericAccessDataValueUInt data;
	FIFOEntryDataContext dataContext(entryNo);
	ReadGenericData((unsigned int)DATASOURCE_REG_FIFO_CODE, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFOCode(unsigned int entryNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	FIFOEntryDataContext dataContext(entryNo);
	WriteGenericData((unsigned int)DATASOURCE_REG_FIFO_CODE, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetFIFOAddress(unsigned int entryNo) const
{
	GenericAccessDataValueUInt data;
	FIFOEntryDataContext dataContext(entryNo);
	ReadGenericData((unsigned int)DATASOURCE_REG_FIFO_ADDRESS, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFOAddress(unsigned int entryNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	FIFOEntryDataContext dataContext(entryNo);
	WriteGenericData((unsigned int)DATASOURCE_REG_FIFO_ADDRESS, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetFIFOData(unsigned int entryNo) const
{
	GenericAccessDataValueUInt data;
	FIFOEntryDataContext dataContext(entryNo);
	ReadGenericData((unsigned int)DATASOURCE_REG_FIFO_DATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFOData(unsigned int entryNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	FIFOEntryDataContext dataContext(entryNo);
	WriteGenericData((unsigned int)DATASOURCE_REG_FIFO_DATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetFIFOWritePending(unsigned int entryNo) const
{
	GenericAccessDataValueBool data;
	FIFOEntryDataContext dataContext(entryNo);
	ReadGenericData((unsigned int)DATASOURCE_REG_FIFO_WRITEPENDING, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFOWritePending(unsigned int entryNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	FIFOEntryDataContext dataContext(entryNo);
	WriteGenericData((unsigned int)DATASOURCE_REG_FIFO_WRITEPENDING, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetFIFOHalfWritten(unsigned int entryNo) const
{
	GenericAccessDataValueBool data;
	FIFOEntryDataContext dataContext(entryNo);
	ReadGenericData((unsigned int)DATASOURCE_REG_FIFO_WRITEHALFWRITTEN, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFOHalfWritten(unsigned int entryNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	FIFOEntryDataContext dataContext(entryNo);
	WriteGenericData((unsigned int)DATASOURCE_REG_FIFO_WRITEHALFWRITTEN, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetFIFONextReadEntry() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_NEXTFIFOREADENTRY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFONextReadEntry(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_NEXTFIFOREADENTRY, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetFIFONextWriteEntry() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_REG_NEXTFIFOWRITEENTRY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFONextWriteEntry(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_REG_NEXTFIFOWRITEENTRY, 0, data);
}
