#include "SystemMenuHandler.h"
#include "SettingsMenuHandler.h"
#include "DebugMenuHandler.h"
#include "SystemOptionMenuHandler.h"

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum System::InputEvent
{
	INPUTEVENT_KEYDOWN,
	INPUTEVENT_KEYUP
};

//----------------------------------------------------------------------------------------
enum System::SystemStateChangeType
{
	SYSTEMSTATECHANGETYPE_SETSYSTEMLINESTATE,
	SYSTEMSTATECHANGETYPE_SETCLOCKFREQUENCY,
	SYSTEMSTATECHANGETYPE_SETSYSTEMOPTION
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct System::DeviceLibraryEntry
{
	DeviceLibraryEntry()
	:Allocator(0), Destructor(0), assemblyHandle(0), deviceVersionNo(0)
	{}

	unsigned int deviceVersionNo;
	std::wstring deviceName;
	IDevice* (*Allocator)(const wchar_t* instanceName, unsigned int moduleID);
	void (*Destructor)(IDevice*);
	IDevice::AssemblyHandle assemblyHandle;
};

//----------------------------------------------------------------------------------------
struct System::ExtensionLibraryEntry
{
	ExtensionLibraryEntry()
	:Allocator(0), Destructor(0), assemblyHandle(0), extensionVersionNo(0)
	{}

	unsigned int extensionVersionNo;
	std::wstring deviceName;
	IExtension* (*Allocator)(const wchar_t* instanceName, unsigned int moduleID);
	void (*Destructor)(IExtension*);
	IExtension::AssemblyHandle assemblyHandle;
};

//----------------------------------------------------------------------------------------
struct System::LoadedDeviceInfo
{
	IDevice* device;
	DeviceContext* deviceContext;
	std::wstring name;
	unsigned int moduleID;
};

//----------------------------------------------------------------------------------------
struct System::ExportedDeviceInfo
{
	IDevice* device;
	DeviceContext* deviceContext;
	std::wstring exportingModuleDeviceInstanceName;
	std::wstring importName;
};

//----------------------------------------------------------------------------------------
struct System::ImportedDeviceInfo
{
	IDevice* device;
	DeviceContext* deviceContext;
	std::wstring exportingModuleDeviceInstanceName;
	std::wstring importName;
	unsigned int connectorID;
	unsigned int exportingModuleID;
	unsigned int importingModuleID;
	std::wstring importingModuleDeviceInstanceName;
};

//----------------------------------------------------------------------------------------
struct System::LoadedExtensionInfo
{
	IExtension* extension;
	std::wstring name;
	unsigned int moduleID;
};

//----------------------------------------------------------------------------------------
struct System::ExportedExtensionInfo
{
	IExtension* extension;
	std::wstring exportingModuleExtensionInstanceName;
	std::wstring importName;
};

//----------------------------------------------------------------------------------------
struct System::ImportedExtensionInfo
{
	IExtension* extension;
	std::wstring exportingModuleExtensionInstanceName;
	std::wstring importName;
	unsigned int connectorID;
	unsigned int exportingModuleID;
	unsigned int importingModuleID;
	std::wstring importingModuleExtensionInstanceName;
};

//----------------------------------------------------------------------------------------
struct System::LoadedBusInfo
{
	BusInterface* busInterface;
	std::wstring name;
	unsigned int moduleID;
};

//----------------------------------------------------------------------------------------
struct System::ExportedBusInfo
{
	BusInterface* busInterface;
	std::wstring exportingModuleBusInterfaceName;
	std::wstring importName;
	std::list<ExportedLineGroupInfo> exportedLineGroups;
};

//----------------------------------------------------------------------------------------
struct System::ImportedBusInfo
{
	BusInterface* busInterface;
	std::wstring exportingModuleBusInterfaceName;
	std::wstring importName;
	unsigned int connectorID;
	unsigned int exportingModuleID;
	unsigned int importingModuleID;
	std::wstring importingModuleBusInterfaceName;
	std::list<ImportedLineGroupInfo> importedLineGroups;
};

//----------------------------------------------------------------------------------------
struct System::LoadedClockSourceInfo
{
	ClockSource* clockSource;
	std::wstring name;
	unsigned int moduleID;
};

//----------------------------------------------------------------------------------------
struct System::ExportedClockSourceInfo
{
	ClockSource* clockSource;
	std::wstring exportingModuleClockSourceName;
	std::wstring importName;
};

//----------------------------------------------------------------------------------------
struct System::ImportedClockSourceInfo
{
	ClockSource* clockSource;
	std::wstring exportingModuleClockSourceName;
	std::wstring importName;
	unsigned int connectorID;
	unsigned int exportingModuleID;
	unsigned int importingModuleID;
	std::wstring importingModuleClockSourceName;
};

//----------------------------------------------------------------------------------------
struct System::ExportedLineGroupInfo
{
	unsigned int lineGroupID;
	std::wstring importName;
	std::wstring localName;
};

//----------------------------------------------------------------------------------------
struct System::LineGroupDetails
{
	BusInterface* busInterface;
	std::wstring lineGroupName;
	unsigned int lineGroupID;
};

//----------------------------------------------------------------------------------------
struct System::ImportedLineGroupInfo
{
	unsigned int lineGroupID;
	std::wstring importName;
	std::wstring localName;
};

//----------------------------------------------------------------------------------------
struct System::LoadedModuleInfoInternal
{
	LoadedModuleInfoInternal()
	:moduleValidated(false)
	{}

	//Internal data
	unsigned int moduleID;
	bool moduleValidated;

	//External information
	std::wstring fileDir;
	std::wstring fileName;

	//Required metadata
	bool programModule;
	std::wstring systemClassName;
	std::wstring className;
	std::wstring instanceName;
	std::wstring displayName;

	//Optional metadata
	std::wstring productionYear;
	std::wstring manufacturerCode;
	std::wstring manufacturerDisplayName;
};

//----------------------------------------------------------------------------------------
struct System::ConnectorInfoInternal
{
	//Internal data
	unsigned int connectorID;

	//Exporting module info
	unsigned int exportingModuleID;
	std::wstring connectorClassName;
	std::wstring exportingModuleConnectorInstanceName;
	std::wstring systemClassName;

	//Exported objects
	std::map<std::wstring, ExportedDeviceInfo> exportedDeviceInfo;
	std::map<std::wstring, ExportedExtensionInfo> exportedExtensionInfo;
	std::map<std::wstring, ExportedBusInfo> exportedBusInfo;
	std::map<std::wstring, ExportedClockSourceInfo> exportedClockSourceInfo;
	std::map<std::wstring, ExportedSystemLineInfo> exportedSystemLineInfo;
	std::map<std::wstring, ExportedSystemSettingInfo> exportedSystemSettingInfo;

	//Importing module info
	bool connectorUsed;
	unsigned int importingModuleID;
	std::wstring importingModuleConnectorInstanceName;
};

//----------------------------------------------------------------------------------------
struct System::InputMapEntry
{
	IDeviceContext::KeyCode keyCode;
	IDevice* targetDevice;
	unsigned int targetDeviceKeyCode;
};

//----------------------------------------------------------------------------------------
struct System::InputEventEntry
{
	InputEvent inputEvent;
	IDeviceContext::KeyCode keyCode;
	bool sent;
};

//----------------------------------------------------------------------------------------
struct System::LogEntryInternal
{
	std::wstring text;
	std::wstring source;
	ILogEntry::EventLevel eventLevel;
	std::wstring eventLevelString;
	std::wstring eventTimeString;
};

//----------------------------------------------------------------------------------------
struct System::ViewModelOpenRequest
{
	bool ownerSystem;
	unsigned int moduleID;
	std::wstring viewModelGroupName;
	std::wstring viewModelName;
	std::wstring deviceInstanceName;
};

//----------------------------------------------------------------------------------------
struct System::InputRegistration
{
	InputRegistration()
	:targetDevice(0), preferredSystemKeyCodeSpecified(false)
	{}

	unsigned int moduleID;
	IDevice* targetDevice;
	unsigned int deviceKeyCode;
	bool preferredSystemKeyCodeSpecified;
	IDeviceContext::KeyCode systemKeyCode;
};

//----------------------------------------------------------------------------------------
struct System::UnmappedLineStateInfo
{
	UnmappedLineStateInfo(unsigned int amoduleID, IDevice* atargetDevice, unsigned int alineNo, Data alineData)
	:moduleID(amoduleID), targetDevice(atargetDevice), lineNo(alineNo), lineData(alineData)
	{}

	unsigned int moduleID;
	IDevice* targetDevice;
	unsigned int lineNo;
	Data lineData;
};

//----------------------------------------------------------------------------------------
struct System::SystemSettingInfo
{
	SystemSettingInfo()
	:selectedOption(0), defaultOption(0)
	{}
	unsigned int moduleID;
	std::wstring name;
	unsigned int systemSettingID;
	std::wstring displayName;
	std::vector<SystemSettingOption> options;
	unsigned int defaultOption;
	unsigned int selectedOption;
};

//----------------------------------------------------------------------------------------
struct System::SystemSettingOption
{
	SystemSettingOption()
	:menuItemEntry(0)
	{}

	std::wstring name;
	std::wstring displayName;
	std::list<SystemStateChange> stateChanges;
	unsigned int menuItemID;
	IMenuSelectableOption* menuItemEntry;
};

//----------------------------------------------------------------------------------------
struct System::SystemStateChange
{
	unsigned int moduleID;
	SystemStateChangeType type;
	std::wstring targetElementName;
	unsigned int setLineStateValue;
	ClockSource::ClockType setClockRateClockType;
	double setClockRateValue;
	std::wstring setSystemOptionValue;
};

//----------------------------------------------------------------------------------------
struct System::SystemLineInfo
{
	unsigned int moduleID;
	std::wstring lineName;
	unsigned int lineID;
	unsigned int lineWidth;
	unsigned int currentValue;
};

//----------------------------------------------------------------------------------------
struct System::ExportedSystemLineInfo
{
	unsigned int systemLineID;
	std::wstring exportingModuleSystemLineName;
	std::wstring importName;
};

//----------------------------------------------------------------------------------------
struct System::ImportedSystemLineInfo
{
	unsigned int systemLineID;
	std::wstring exportingModuleSystemLineName;
	std::wstring importName;
	unsigned int connectorID;
	unsigned int exportingModuleID;
	unsigned int importingModuleID;
	std::wstring importingModuleSystemLineName;
};

//----------------------------------------------------------------------------------------
struct System::ExportedSystemSettingInfo
{
	unsigned int systemSettingID;
	std::wstring exportingModuleSystemSettingName;
	std::wstring importName;
};

//----------------------------------------------------------------------------------------
struct System::ImportedSystemSettingInfo
{
	unsigned int systemSettingID;
	std::wstring exportingModuleSystemSettingName;
	std::wstring importName;
	unsigned int connectorID;
	unsigned int exportingModuleID;
	unsigned int importingModuleID;
	std::wstring importingModuleSystemSettingName;
};

//----------------------------------------------------------------------------------------
struct System::SystemLineMapping
{
	SystemLineMapping()
	:targetDevice(0),
	 lineMaskAND(0xFFFFFFFF),
	 lineMaskOR(0),
	 lineMaskXOR(0)
	{}

	unsigned int moduleID;
	IDevice* targetDevice;
	unsigned int systemLineID;
	unsigned int systemLineBitCount;
	unsigned int targetLine;
	unsigned int targetLineBitCount;

	unsigned int lineMaskAND;
	unsigned int lineMaskOR;
	unsigned int lineMaskXOR;

	bool remapLines;
	DataRemapTable lineRemapTable;
};

//----------------------------------------------------------------------------------------
struct System::EmbeddedROMInfo
{
	unsigned int moduleID;
	IDevice* targetDevice;
	unsigned int interfaceNumber;
	unsigned int romRegionSize;
	std::wstring displayName;
	std::wstring filePath;
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::System(void* aassemblyHandle)
:stopSystem(false), systemStopped(true), initialize(false), enableThrottling(true), runWhenProgramModuleLoaded(true), assemblyHandle(aassemblyHandle)
{
	//Initialize the extension interface pointers
	systemExtensionInterface = this;
	guiExtensionInterface = 0;

	//Create the menu handlers
	systemMenuHandler = new SystemMenuHandler(this);
	systemMenuHandler->LoadMenuItems();
	settingsMenuHandler = new SettingsMenuHandler(this);
	settingsMenuHandler->LoadMenuItems();
	debugMenuHandler = new DebugMenuHandler(this);
	debugMenuHandler->LoadMenuItems();
	systemOptionMenuHandler = new SystemOptionMenuHandler(this);

	loggerLevel1Enabled = true;
	loggerLevel2Enabled = true;
	loggerLevel3Enabled = true;
	loggerLevel4Enabled = true;
	loggerLevel5Enabled = true;
	eventLogSize = 500;

	nextFreeModuleID = 0;
	nextFreeConnectorID = 1000;
	nextFreeLineGroupID = 2000;
	nextFreeSystemLineID = 3000;
	nextFreeSystemSettingID = 3000;
}

//----------------------------------------------------------------------------------------
System::~System()
{
	UnloadAllModules();

	//Delete the menu handler
	//##FIX## We can't safely call this here, since the interface class may have been
	//destroyed by now.
	//debugMenuHandler->ClearMenuItems();
	delete systemMenuHandler;
	delete settingsMenuHandler;
	delete debugMenuHandler;
	delete systemOptionMenuHandler;
}
