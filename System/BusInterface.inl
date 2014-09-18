//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct BusInterface::BusInterfaceParams
{
	BusInterfaceParams()
	:addressBusWidthDefined(false),
	 dataBusWidthDefined(false),
	 usePhysicalMemoryMapDefined(false),
	 portAddressBusWidthDefined(false),
	 portDataBusWidthDefined(false),
	 usePhysicalPortMapDefined(false)
	{}

	bool addressBusWidthDefined;
	bool dataBusWidthDefined;
	bool usePhysicalMemoryMapDefined;
	bool portAddressBusWidthDefined;
	bool portDataBusWidthDefined;
	bool usePhysicalPortMapDefined;

	unsigned int addressBusWidth;
	unsigned int dataBusWidth;
	bool usePhysicalMemoryMap;
	unsigned int portAddressBusWidth;
	unsigned int portDataBusWidth;
	bool usePhysicalPortMap;
};

//----------------------------------------------------------------------------------------
struct BusInterface::DeviceMappingParams
{
	DeviceMappingParams()
	:memoryMapBaseDefined(false),
	 memoryMapSizeDefined(false),
	 interfaceOffsetDefined(false),
	 interfaceNumberDefined(false),
	 addressMaskDefined(false),
	 addressDiscardLowerBitCount(false),
	 addressLineFilterDefined(false),
	 ceLineConditionsDefined(false),
	 addressLineMappingDefined(false),
	 dataLineMappingDefined(false)
	{}

	bool memoryMapBaseDefined;
	bool memoryMapSizeDefined;
	bool interfaceOffsetDefined;
	bool interfaceNumberDefined;
	bool addressMaskDefined;
	bool addressDiscardLowerBitCountDefined;
	bool addressLineFilterDefined;
	bool ceLineConditionsDefined;
	bool addressLineMappingDefined;
	bool dataLineMappingDefined;

	unsigned int memoryMapBase;
	unsigned int memoryMapSize;
	unsigned int interfaceOffset;
	unsigned int interfaceNumber;
	unsigned int addressMask;
	unsigned int addressDiscardLowerBitCount;
	std::wstring addressLineFilter;
	std::wstring ceLineConditions;
	std::wstring addressLineMapping;
	std::wstring dataLineMapping;
};

//----------------------------------------------------------------------------------------
struct BusInterface::LineMappingParams
{
	LineMappingParams()
	:sourceLineDefined(false),
	 targetLineDefined(false),
	 lineMaskANDDefined(false),
	 lineMaskORDefined(false),
	 lineMaskXORDefined(false),
	 lineMappingDefined(false),
	 declaringPartialMappingTemplate(false)
	{}

	bool sourceLineDefined;
	bool targetLineDefined;
	bool lineMaskANDDefined;
	bool lineMaskORDefined;
	bool lineMaskXORDefined;
	bool lineMappingDefined;
	bool declaringPartialMappingTemplate;
	std::wstring sourceLine;
	std::wstring targetLine;
	unsigned int lineMaskAND;
	unsigned int lineMaskOR;
	unsigned int lineMaskXOR;
	std::wstring lineMapping;
};

//----------------------------------------------------------------------------------------
struct BusInterface::ClockSourceMappingParams
{
	ClockSourceMappingParams()
	:targetClockLineDefined(false)
	{}

	bool targetClockLineDefined;
	std::wstring targetClockLine;
};

//----------------------------------------------------------------------------------------
struct BusInterface::MapEntry
{
	MapEntry()
	:device(0),
	 address(0),
	 addressEffectiveBitMaskForTargetting(0),
	 addressMask(0),
	 addressDiscardLowerBitCount(0),
	 ce(0),
	 ceMask(0),
	 interfaceSize(0),
	 interfaceOffset(0),
	 interfaceNumber(0),
	 remapAddressLines(false),
	 remapDataLines(false)
	{}

	unsigned int address;
	unsigned int addressEffectiveBitMaskForTargetting;
	unsigned int addressMask;
	unsigned int addressDiscardLowerBitCount;
	unsigned int ce;
	unsigned int ceMask;
	std::map<unsigned int, unsigned int> ceConditions;

	IDevice* device;
	unsigned int interfaceSize;
	unsigned int interfaceOffset;
	unsigned int interfaceNumber;

	bool remapAddressLines;
	bool remapDataLines;
	DataRemapTable addressLineRemapTable;
	DataRemapTable dataLineRemapTable;
};

//----------------------------------------------------------------------------------------
struct BusInterface::LineEntry
{
	LineEntry()
	:sourceDevice(0),
	 targetDevice(0),
	 lineMaskAND(0xFFFFFFFF),
	 lineMaskOR(0),
	 lineMaskXOR(0),
	 remapLines(false)
	{}
	LineEntry(IDevice* asourceDevice, unsigned int asourceLine, unsigned int asourceLineBitCount, IDevice* atargetDevice, unsigned int atargetLine, unsigned int atargetLineBitCount)
	:sourceDevice(asourceDevice),
	 sourceLine(asourceLine),
	 sourceLineBitCount(asourceLineBitCount),
	 targetDevice(atargetDevice),
	 targetLine(atargetLine),
	 targetLineBitCount(atargetLineBitCount),
	 lineMaskAND(0xFFFFFFFF),
	 lineMaskOR(0),
	 lineMaskXOR(0),
	 remapLines(false)
	{}

	IDevice* sourceDevice;
	IDevice* targetDevice;
	unsigned int sourceLine;
	unsigned int sourceLineBitCount;
	unsigned int targetLine;
	unsigned int targetLineBitCount;

	unsigned int lineMaskAND;
	unsigned int lineMaskOR;
	unsigned int lineMaskXOR;

	bool remapLines;
	DataRemapTable lineRemapTable;
};

//----------------------------------------------------------------------------------------
struct BusInterface::LineMappingTemplate
{
	IDevice* device;
	bool fromDeviceToGroup;
	std::wstring sourceLine;
	std::wstring targetLine;
	std::wstring lineMapping;
	bool lineMappingDefined;
};

//----------------------------------------------------------------------------------------
struct BusInterface::LineGroupMappingInfo
{
	std::list<LineMappingTemplate> lineMappingTemplates;
};

//----------------------------------------------------------------------------------------
struct BusInterface::CELineDefinition
{
	std::wstring busLineName; //This is defined in the module XML file
	unsigned int busLineID; //This is generated by the BusInterface when the line is defined
	unsigned int bitCount; //This is defined in the module XML file
	bool defaultValueDefined; //This is defined in the module XML file
	unsigned int defaultValue; //This is defined in the module XML file
};

//----------------------------------------------------------------------------------------
struct BusInterface::CELineDeviceLineInput
{
	std::wstring deviceLineName;
	unsigned int deviceLineID;
	unsigned int busLineID;
	bool bitNumberAllocated;
	unsigned int lineBitNumber;
	unsigned int lineBitmask;
};

//----------------------------------------------------------------------------------------
struct BusInterface::CELineDeviceLineOutput
{
	std::wstring deviceLineName; //This is defined in the module XML file
	unsigned int deviceLineID; //This is cached from the device
	unsigned int busLineID; //This is cached from the CELineDefinition
	bool bitNumberAllocated;
	unsigned int lineBitNumber; //This is generated by the BusInterface class
	unsigned int lineBitmask; //This is generated from lineBitNumber
};

//----------------------------------------------------------------------------------------
struct BusInterface::CELineDeviceEntry
{
	IDevice* device;
	unsigned int inputCELineMask; //This is a binary OR of lineBitmask in all lineInputs members
	unsigned int outputCELineMask; //This is a binary OR of lineBitmask in all lineOutputs members
	std::list<CELineDeviceLineInput> lineInputs;
	std::list<CELineDeviceLineOutput> lineOutputs;
};

//----------------------------------------------------------------------------------------
struct BusInterface::ClockSourceEntry
{
	ClockSourceEntry()
		:inputClockSource(0),
		targetDevice(0)
	{}

	IClockSource* inputClockSource;
	IDevice* targetDevice;
	unsigned int targetClockLine;
};
