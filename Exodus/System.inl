#include "SystemMenuHandler.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum System::InputEvent
{
	INPUTEVENT_KEYDOWN,
	INPUTEVENT_KEYUP
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
struct System::ImportedLineGroupInfo
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
struct System::ConnectorDetails
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
	std::map<std::wstring, ExportedBusInfo> exportedBusInfo;
	std::map<std::wstring, ExportedClockSourceInfo> exportedClockSourceInfo;

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
	std::wstring menuHandlerName;
	int viewID;
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
//Constructors
//----------------------------------------------------------------------------------------
System::System(void* aassemblyHandle)
:stopSystem(false), systemStopped(true), initialize(false), enableThrottling(true), runWhenProgramModuleLoaded(true), assemblyHandle(aassemblyHandle)
{
	//Create the menu handlers
	systemMenuHandler = new SystemMenuHandler(this);
	systemMenuHandler->LoadMenuItems();
	debugMenuHandler = new DebugMenuHandler(this);
	debugMenuHandler->LoadMenuItems();

	loggerLevel1Enabled = true;
	loggerLevel2Enabled = true;
	loggerLevel3Enabled = true;
	loggerLevel4Enabled = true;
	loggerLevel5Enabled = true;
	eventLogSize = 500;

	nextFreeModuleID = 0;
	nextFreeConnectorID = 1000;
	nextFreeLineGroupID = 2000;
}

//----------------------------------------------------------------------------------------
System::~System()
{
	UnloadAllModules();

	//Delete the menu handler
	//##FIX## We can't safely call this here, since the interface class may have been
	//destroyed by now.
	//debugMenuHandler->ClearMenuItems();
	delete debugMenuHandler;
}
