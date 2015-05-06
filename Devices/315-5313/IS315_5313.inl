//Disable warning about the presence of virtual functions without a virtual destructor.
//Our structures below use virtual functions to create code barriers between assemblies so
//that they can be marshalled, but they are never derived from, so a virtual destructor is
//not required.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4265)
#endif

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IS315_5313::IS315_5313DataSource
{
	RawRegister = 1,
	RegStatus,
	FlagFIFOEmpty,
	FlagFIFOFull,
	FlagF,
	FlagSpriteOverflow,
	FlagSpriteCollision,
	FlagOddInterlaceFrame,
	FlagVBlank,
	FlagHBlank,
	FlagDMA,
	FlagPAL,
	RegVSI,
	RegHSI,
	RegLCB,
	RegIE1,
	RegSS,
	RegPS,
	RegM2,
	RegES,
	RegEVRAM,
	RegDisplayEnabled,
	RegIE0,
	RegDMAEnabled,
	RegM3,
	RegMode5,
	RegSZ,
	RegMAG,
	RegNameTableBaseA,
	RegNameTableBaseWindow,
	RegNameTableBaseB,
	RegNameTableBaseSprite,
	RegPatternBaseSprite,
	Reg077,
	Reg076,
	RegBackgroundPaletteRow,
	RegBackgroundPaletteColumn,
	RegBackgroundScrollX,
	RegBackgroundScrollY,
	RegHINTData,
	Reg0B7,
	Reg0B6,
	Reg0B5,
	Reg0B4,
	RegIE2,
	RegVSCR,
	RegHSCR,
	RegLSCR,
	RegRS0,
	RegU1,
	RegU2,
	RegU3,
	RegSTE,
	RegLSM1,
	RegLSM0,
	RegRS1,
	RegHScrollDataBase,
	Reg0E57,
	RegPatternBaseA,
	Reg0E13,
	RegPatternBaseB,
	RegAutoIncrementData,
	Reg1067,
	RegVSZ,
	RegVSZ1,
	RegVSZ0,
	Reg1023,
	RegHSZ,
	RegHSZ1,
	RegHSZ0,
	Reg1156,
	RegWindowRight,
	RegWindowBaseX,
	Reg1256,
	RegWindowBottom,
	RegWindowBaseY,
	RegDMALength,
	RegDMASource,
	RegDMASourceData1,
	RegDMASourceData2,
	RegDMASourceData3,
	RegDMD1,
	RegDMD0,
	RegCode,
	RegAddress,
	RegPortWritePending,
	RegReadBuffer,
	RegReadHalfCached,
	RegReadFullyCached,
	RegVINTPending,
	RegHINTPending,
	RegEXINTPending,
	RegHVCounterExternal,
	RegHCounterInternal,
	RegVCounterInternal,
	RegHCounterLatched,
	RegVCounterLatched,
	RegFIFOCode,
	RegFIFOAddress,
	RegFIFOData,
	RegFIFOWritePending,
	RegFIFOWriteHalfWritten,
	RegNextFIFOReadEntry,
	RegNextFIFOWriteEntry,
	SettingsOutputPortAccessDebugMessages,
	SettingsOutputTimingDebugMessages,
	SettingsOutputRenderSyncDebugMessages,
	SettingsOutputInterruptDebugMessages,
	SettingsVideoDisableRenderOutput,
	SettingsVideoEnableSpriteBoxing,
	SettingsVideoHighlightRenderPos,
	SettingsVideoSingleBuffering,
	SettingsVideoFixedAspectRatio,
	SettingsVideoShowStatusBar,
	SettingsVideoEnableLineSmoothing,
	SettingsCurrentRenderPosOnScreen,
	SettingsCurrentRenderPosScreenX,
	SettingsCurrentRenderPosScreenY,
	SettingsVideoShowBoundaryActiveImage,
	SettingsVideoShowBoundaryActionSafe,
	SettingsVideoShowBoundaryTitleSafe,
	SettingsVideoEnableFullImageBufferInfo,
	SettingsVideoEnableLayerA,
	SettingsVideoEnableLayerAHigh,
	SettingsVideoEnableLayerALow,
	SettingsVideoEnableLayerB,
	SettingsVideoEnableLayerBHigh,
	SettingsVideoEnableLayerBLow,
	SettingsVideoEnableWindow,
	SettingsVideoEnableWindowHigh,
	SettingsVideoEnableWindowLow,
	SettingsVideoEnableSprite,
	SettingsVideoEnableSpriteHigh,
	SettingsVideoEnableSpriteLow
};

//----------------------------------------------------------------------------------------
enum class IS315_5313::PixelSource
{
	Sprite,
	LayerA,
	LayerB,
	Background,
	Window,
	CRAMWrite,
	Border,
	Blanking
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
public:
	//Constructors
	PortMonitorEntry(const std::wstring& atarget, const std::wstring& asource, unsigned int adata, double aaccessTime, unsigned int ahcounterPos, unsigned int avcounterPos)
	:target(atarget), source(asource), data(adata), accessTime(aaccessTime), hcounterPos(ahcounterPos), vcounterPos(avcounterPos)
	{}
	PortMonitorEntry(MarshalSupport::marshal_object_t, const PortMonitorEntry& sourceObject)
	{
		sourceObject.MarshalToTarget(source, target, data, accessTime, hcounterPos, vcounterPos);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(const MarshalSupport::Marshal::Out<std::wstring>& sourceMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& targetMarshaller, unsigned int& dataMarshaller, double& accessTimeMarshaller, unsigned int& hcounterPosMarshaller, unsigned int& vcounterPosMarshaller) const
	{
		sourceMarshaller = source;
		targetMarshaller = target;
		dataMarshaller = data;
		accessTimeMarshaller = accessTime;
		hcounterPosMarshaller = hcounterPos;
		vcounterPosMarshaller = vcounterPos;
	}

public:
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
struct IS315_5313::ImageBufferInfo
{
	ImageBufferInfo()
	:mappingData(32, 0)
	{}

	PixelSource pixelSource;
	unsigned int hcounter;
	unsigned int vcounter;
	unsigned int paletteRow;
	unsigned int paletteEntry;
	bool shadowHighlightEnabled;
	bool pixelIsShadowed;
	bool pixelIsHighlighted;
	unsigned int colorComponentR;
	unsigned int colorComponentG;
	unsigned int colorComponentB;

	unsigned int mappingVRAMAddress;
	Data mappingData;
	unsigned int patternRowNo;
	unsigned int patternColumnNo;

	unsigned int spriteTableEntryNo;
	unsigned int spriteTableEntryAddress;
	unsigned int spriteCellWidth;
	unsigned int spriteCellHeight;
	unsigned int spriteCellPosX;
	unsigned int spriteCellPosY;
};

//----------------------------------------------------------------------------------------
//Debug output
//----------------------------------------------------------------------------------------
bool IS315_5313::GetOutputPortAccessDebugMessages() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsOutputPortAccessDebugMessages, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetOutputPortAccessDebugMessages(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsOutputPortAccessDebugMessages, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetOutputTimingDebugMessages() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsOutputTimingDebugMessages, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetOutputTimingDebugMessages(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsOutputTimingDebugMessages, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetOutputRenderSyncMessages() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsOutputRenderSyncDebugMessages, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetOutputRenderSyncMessages(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsOutputRenderSyncDebugMessages, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetOutputInterruptDebugMessages() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsOutputInterruptDebugMessages, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetOutputInterruptDebugMessages(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsOutputInterruptDebugMessages, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoDisableRenderOutput() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoDisableRenderOutput, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoDisableRenderOutput(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoDisableRenderOutput, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoEnableSpriteBoxing() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableSpriteBoxing, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoEnableSpriteBoxing(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableSpriteBoxing, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoHighlightRenderPos() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoHighlightRenderPos, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoHighlightRenderPos(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoHighlightRenderPos, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoSingleBuffering() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoSingleBuffering, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoSingleBuffering(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoSingleBuffering, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoFixedAspectRatio() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoFixedAspectRatio, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoFixedAspectRatio(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoFixedAspectRatio, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoShowStatusBar() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoShowStatusBar, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoShowStatusBar(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoShowStatusBar, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoEnableLineSmoothing() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLineSmoothing, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoEnableLineSmoothing(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLineSmoothing, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetCurrentRenderPosOnScreen() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsCurrentRenderPosOnScreen, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetCurrentRenderPosOnScreen(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsCurrentRenderPosOnScreen, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::GetCurrentRenderPosScreenX() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsCurrentRenderPosScreenX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetCurrentRenderPosScreenX(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsCurrentRenderPosScreenX, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::GetCurrentRenderPosScreenY() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsCurrentRenderPosScreenY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetCurrentRenderPosScreenY(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsCurrentRenderPosScreenY, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoShowBoundaryActiveImage() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoShowBoundaryActiveImage, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoShowBoundaryActiveImage(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoShowBoundaryActiveImage, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoShowBoundaryActionSafe() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoShowBoundaryActionSafe, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoShowBoundaryActionSafe(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoShowBoundaryActionSafe, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoShowBoundaryTitleSafe() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoShowBoundaryTitleSafe, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoShowBoundaryTitleSafe(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoShowBoundaryTitleSafe, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetVideoEnableFullImageBufferInfo() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableFullImageBufferInfo, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetVideoEnableFullImageBufferInfo(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableFullImageBufferInfo, 0, data);
}

//----------------------------------------------------------------------------------------
//Layer removal
//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableLayerA() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableLayerA(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerA, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableLayerAHigh() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerAHigh, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableLayerAHigh(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerAHigh, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableLayerALow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerALow, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableLayerALow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerALow, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableLayerB() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerB, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableLayerB(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerB, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableLayerBHigh() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerBHigh, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableLayerBHigh(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerBHigh, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableLayerBLow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerBLow, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableLayerBLow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableLayerBLow, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableWindow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableWindow, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableWindow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableWindow, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableWindowHigh() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableWindowHigh, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableWindowHigh(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableWindowHigh, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableWindowLow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableWindowLow, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableWindowLow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableWindowLow, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableSprite() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableSprite, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableSprite(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableSprite, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableSpriteHigh() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableSpriteHigh, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableSpriteHigh(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableSpriteHigh, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetEnableSpriteLow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableSpriteLow, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetEnableSpriteLow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::SettingsVideoEnableSpriteLow, 0, data);
}

//----------------------------------------------------------------------------------------
//Raw register functions
//----------------------------------------------------------------------------------------
unsigned int IS315_5313::GetRegisterData(unsigned int location) const
{
	GenericAccessDataValueUInt data;
	RegisterDataContext dataContext(location);
	ReadGenericData((unsigned int)IS315_5313DataSource::RawRegister, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetRegisterData(unsigned int location, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	RegisterDataContext dataContext(location);
	WriteGenericData((unsigned int)IS315_5313DataSource::RawRegister, &dataContext, data);
}

//----------------------------------------------------------------------------------------
//Interpreted register functions
//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetVSI() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegVSI, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVSI(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegVSI, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetHSI() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegHSI, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHSI(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegHSI, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetLCB() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegLCB, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetLCB(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegLCB, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetIE1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegIE1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetIE1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegIE1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetSS() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegSS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetSS(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegSS, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetPS() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegPS, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPS(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegPS, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetM2() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegM2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetM2(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegM2, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetES() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegES, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetES(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegES, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetEVRAM() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegEVRAM, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetEVRAM(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegEVRAM, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetDisplayEnabled() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegDisplayEnabled, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDisplayEnabled(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegDisplayEnabled, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetIE0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegIE0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetIE0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegIE0, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetDMAEnabled() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegDMAEnabled, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMAEnabled(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegDMAEnabled, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetM3() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegM3, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetM3(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegM3, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetMode5() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegMode5, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetMode5(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegMode5, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetSZ() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegSZ, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetSZ(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegSZ, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetMAG() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegMAG, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetMAG(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegMAG, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetNameTableBaseScrollA() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegNameTableBaseA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetNameTableBaseScrollA(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegNameTableBaseA, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetNameTableBaseWindow() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegNameTableBaseWindow, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetNameTableBaseWindow(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegNameTableBaseWindow, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetNameTableBaseScrollB() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegNameTableBaseB, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetNameTableBaseScrollB(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegNameTableBaseB, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetNameTableBaseSprite() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegNameTableBaseSprite, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetNameTableBaseSprite(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegNameTableBaseSprite, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetPatternBaseSprite() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegPatternBaseSprite, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPatternBaseSprite(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegPatternBaseSprite, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet077() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::Reg077, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet077(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::Reg077, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet076() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::Reg076, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet076(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::Reg076, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetBackgroundPaletteRow() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegBackgroundPaletteRow, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetBackgroundPaletteRow(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegBackgroundPaletteRow, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetBackgroundPaletteColumn() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegBackgroundPaletteColumn, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetBackgroundPaletteColumn(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegBackgroundPaletteColumn, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetBackgroundScrollX() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegBackgroundScrollX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetBackgroundScrollX(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegBackgroundScrollX, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetBackgroundScrollY() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegBackgroundScrollY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetBackgroundScrollY(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegBackgroundScrollY, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHInterruptData() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegHINTData, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHInterruptData(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegHINTData, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet0B7() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::Reg0B7, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0B7(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::Reg0B7, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet0B6() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::Reg0B6, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0B6(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::Reg0B6, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet0B5() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::Reg0B5, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0B5(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::Reg0B5, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGet0B4() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::Reg0B4, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0B4(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::Reg0B4, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetIE2() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegIE2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetIE2(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegIE2, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetVSCR() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegVSCR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVSCR(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegVSCR, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetHSCR() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegHSCR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHSCR(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegHSCR, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetLSCR() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegLSCR, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetLSCR(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegLSCR, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetRS0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegRS0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetRS0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegRS0, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetU1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegU1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetU1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegU1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetU2() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegU2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetU2(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegU2, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetU3() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegU3, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetU3(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegU3, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetSTE() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegSTE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetSTE(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegSTE, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetLSM1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegLSM1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetLSM1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegLSM1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetLSM0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegLSM0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetLSM0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegLSM0, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetRS1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegRS1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetRS1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegRS1, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHScrollDataBase() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegHScrollDataBase, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHScrollDataBase(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegHScrollDataBase, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet0E57() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::Reg0E57, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0E57(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::Reg0E57, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetPatternBaseScrollA() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegPatternBaseA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPatternBaseScrollA(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegPatternBaseA, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet0E13() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegPatternBaseA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet0E13(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegPatternBaseA, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetPatternBaseScrollB() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegPatternBaseB, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPatternBaseScrollB(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegPatternBaseB, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetAutoIncrementData() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegAutoIncrementData, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetAutoIncrementData(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegAutoIncrementData, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet1067() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::Reg1067, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet1067(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::Reg1067, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetVSZ() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegVSZ, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVSZ(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegVSZ, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetVSZ1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegVSZ1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVSZ1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegVSZ1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetVSZ0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegVSZ0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVSZ0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegVSZ0, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet1023() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::Reg1023, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet1023(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::Reg1023, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHSZ() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegHSZ, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHSZ(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegHSZ, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetHSZ1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegHSZ1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHSZ1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegHSZ1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetHSZ0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegHSZ0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHSZ0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegHSZ0, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet1156() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::Reg1156, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet1156(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::Reg1156, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetWindowRightAligned() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegWindowRight, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetWindowRightAligned(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegWindowRight, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetWindowBasePointX() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegWindowBaseX, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetWindowBasePointX(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegWindowBaseX, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGet1256() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::Reg1256, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSet1256(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::Reg1256, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetWindowBottomAligned() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegWindowBottom, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetWindowBottomAligned(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegWindowBottom, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetWindowBasePointY() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegWindowBaseY, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetWindowBasePointY(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegWindowBaseY, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetDMALengthCounter() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegDMALength, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMALengthCounter(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegDMALength, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetDMASourceAddress() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegDMASource, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMASourceAddress(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegDMASource, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetDMASourceAddressByte1() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegDMASourceData1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMASourceAddressByte1(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegDMASourceData1, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetDMASourceAddressByte2() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegDMASourceData2, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMASourceAddressByte2(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegDMASourceData2, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetDMASourceAddressByte3() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegDMASourceData3, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMASourceAddressByte3(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegDMASourceData3, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetDMD1() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegDMD1, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMD1(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegDMD1, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetDMD0() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegDMD0, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetDMD0(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegDMD0, 0, data);
}

//----------------------------------------------------------------------------------------
//Status register functions
//----------------------------------------------------------------------------------------
unsigned int IS315_5313::GetStatus() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegStatus, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatus(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegStatus, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagFIFOEmpty() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::FlagFIFOEmpty, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagFIFOEmpty(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::FlagFIFOEmpty, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagFIFOFull() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::FlagFIFOFull, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagFIFOFull(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::FlagFIFOFull, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagF() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::FlagF, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagF(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::FlagF, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagSpriteOverflow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::FlagSpriteOverflow, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagSpriteOverflow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::FlagSpriteOverflow, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagSpriteCollision() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::FlagSpriteCollision, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagSpriteCollision(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::FlagSpriteCollision, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagOddInterlaceFrame() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::FlagOddInterlaceFrame, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagOddInterlaceFrame(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::FlagOddInterlaceFrame, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagVBlank() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::FlagVBlank, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagVBlank(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::FlagVBlank, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagHBlank() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::FlagHBlank, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagHBlank(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::FlagHBlank, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagDMA() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::FlagDMA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagDMA(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::FlagDMA, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::GetStatusFlagPAL() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::FlagPAL, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::SetStatusFlagPAL(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::FlagPAL, 0, data);
}

//----------------------------------------------------------------------------------------
//Port register functions
//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetPortCode() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegCode, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPortCode(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegCode, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetPortAddress() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegAddress, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPortAddress(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegAddress, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetPortWritePending() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegPortWritePending, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetPortWritePending(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegPortWritePending, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetReadBuffer() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegReadBuffer, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetReadBuffer(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegReadBuffer, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetReadHalfCached() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegReadHalfCached, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetReadHalfCached(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegReadHalfCached, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetReadFullyCached() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegReadFullyCached, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetReadFullyCached(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegReadFullyCached, 0, data);
}

//----------------------------------------------------------------------------------------
//Interrupt register functions
//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetVINTPending() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegVINTPending, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVINTPending(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegVINTPending, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetHINTPending() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegHINTPending, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHINTPending(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegHINTPending, 0, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetEXINTPending() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegEXINTPending, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetEXINTPending(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegEXINTPending, 0, data);
}

//----------------------------------------------------------------------------------------
//HV counter register functions
//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHVCounterExternal() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegHVCounterExternal, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHCounterInternal() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegHCounterInternal, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHCounterInternal(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegHCounterInternal, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetVCounterInternal() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegVCounterInternal, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVCounterInternal(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegVCounterInternal, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetHCounterLatched() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegHCounterLatched, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetHCounterLatched(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegHCounterLatched, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetVCounterLatched() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegVCounterLatched, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetVCounterLatched(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegVCounterLatched, 0, data);
}

//----------------------------------------------------------------------------------------
//FIFO register functions
//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetFIFOCode(unsigned int entryNo) const
{
	GenericAccessDataValueUInt data;
	FIFOEntryDataContext dataContext(entryNo);
	ReadGenericData((unsigned int)IS315_5313DataSource::RegFIFOCode, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFOCode(unsigned int entryNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	FIFOEntryDataContext dataContext(entryNo);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegFIFOCode, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetFIFOAddress(unsigned int entryNo) const
{
	GenericAccessDataValueUInt data;
	FIFOEntryDataContext dataContext(entryNo);
	ReadGenericData((unsigned int)IS315_5313DataSource::RegFIFOAddress, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFOAddress(unsigned int entryNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	FIFOEntryDataContext dataContext(entryNo);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegFIFOAddress, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetFIFOData(unsigned int entryNo) const
{
	GenericAccessDataValueUInt data;
	FIFOEntryDataContext dataContext(entryNo);
	ReadGenericData((unsigned int)IS315_5313DataSource::RegFIFOData, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFOData(unsigned int entryNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	FIFOEntryDataContext dataContext(entryNo);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegFIFOData, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetFIFOWritePending(unsigned int entryNo) const
{
	GenericAccessDataValueBool data;
	FIFOEntryDataContext dataContext(entryNo);
	ReadGenericData((unsigned int)IS315_5313DataSource::RegFIFOWritePending, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFOWritePending(unsigned int entryNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	FIFOEntryDataContext dataContext(entryNo);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegFIFOWritePending, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IS315_5313::RegGetFIFOHalfWritten(unsigned int entryNo) const
{
	GenericAccessDataValueBool data;
	FIFOEntryDataContext dataContext(entryNo);
	ReadGenericData((unsigned int)IS315_5313DataSource::RegFIFOWriteHalfWritten, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFOHalfWritten(unsigned int entryNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	FIFOEntryDataContext dataContext(entryNo);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegFIFOWriteHalfWritten, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetFIFONextReadEntry() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegNextFIFOReadEntry, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFONextReadEntry(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegNextFIFOReadEntry, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IS315_5313::RegGetFIFONextWriteEntry() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)IS315_5313DataSource::RegNextFIFOWriteEntry, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IS315_5313::RegSetFIFONextWriteEntry(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)IS315_5313DataSource::RegNextFIFOWriteEntry, 0, data);
}

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif
