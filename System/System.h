/*--------------------------------------------------------------------------------------*\
Things to do:
-Implement a way for lines to be explicitly set to a particular state within an XML file,
without having to map them to a device and control them actively. This is important for
control lines which simply get grounded, IE, the cart_in line for a cartridge.
-Implement our ideas for named, referenced ClockGenerator objects or the like, which
provide a clock source for devices, with rate change notifications, like line state
changes, and the ability to specify a ClockDivider, which take a source clock input, and
divides it by the specified amount (whole integers only), exporting it as a new clock
signal, with a unique name. This will be entirely implemented in system code, so all the
devices can make use of it. Note that devices will be able to optionally change clock rate
and divider settings.
-Allow the user to define "system packages", which specify a system to load, along with a
set of addons to load with that system, and how to connect them in the case of multiple
compatible connectors. This will be essential in order for the user to quickly load
complex systems, IE, a Mega Drive, with a MegaCD and 32x attached, and two controllers.
-Enhance the interface between the system class and the device worker threads. Messages
should be able to be sent to all devices simultaneously.
\*--------------------------------------------------------------------------------------*/
#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "BusInterface.h"
#include "ClockSource.h"
#include "DeviceContext.h"
#include "ExecutionManager.h"
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>

//Terminology:
//Assembly  - An assembly (IE, a dll) which contains the definition of one or more devices
//            or extensions.
//Device    - An emulation core for a logical chip or device in a system
//Extension - An extension to the emulation platform. Extensions have some limited access
//            to the emulation platform, and can reference and be referenced by other
//            devices and extensions, allowing forms of interaction between devices that
//            are not provided by the base emulation platform, and allowing new menu items
//            to be defined which can affect and interact with the system in ways that
//            devices alone cannot do.
//Module    - A physical or logical part of a system, usually defining one or more devices
//            and the connections between those devices, and exposing connectors for other
//            modules to interface with. In our case, an xml file describes a module.
//System    - The total combination of all currently loaded modules, with the current
//            state of all system settings defined within those modules. A system may be
//            seen as a convenience, allowing a complex system with many modules and
//            settings to be saved and loaded in one operation. A system is defined by an
//            xml file.
//Connector - An interface into a module where other modules are able to attach
//            themselves, and are able to observe and/or effect the operation of the
//            other module via the interface exposed over the connector. Connectors should
//            usually model physical connectors used on the real hardware.

//##TODO## Split the module loading and unloading code into a separate source file. This
//code currently occupies most of our System.cpp file, making it hard to find critical
//system functions.
class System :public ISystemGUIInterface
{
public:
	//Constructors
	System(IGUIExtensionInterface& aguiExtensionInterface);
	virtual ~System();

	//Interface version functions
	virtual unsigned int GetISystemDeviceInterfaceVersion() const;
	virtual unsigned int GetISystemExtensionInterfaceVersion() const;
	virtual unsigned int GetISystemGUIInterfaceVersion() const;

	//Savestate functions
	virtual bool LoadState(const MarshalSupport::Marshal::In<std::wstring>& filePath, FileType fileType, bool debuggerState);
	virtual bool SaveState(const MarshalSupport::Marshal::In<std::wstring>& filePath, FileType fileType, bool debuggerState);
	virtual MarshalSupport::Marshal::Ret<StateInfo> GetStateInfo(const MarshalSupport::Marshal::In<std::wstring>& filePath, FileType fileType) const;
	virtual bool LoadModuleRelationshipsNode(IHierarchicalStorageNode& node, const MarshalSupport::Marshal::Out<ModuleRelationshipMap>& relationshipMap) const;
	virtual void SaveModuleRelationshipsNode(IHierarchicalStorageNode& node, bool saveFilePathInfo = false, const MarshalSupport::Marshal::In<std::wstring>& relativePathBase = L"") const;

	//Logging functions
	virtual void WriteLogEvent(const ILogEntry& entry) const;
	virtual MarshalSupport::Marshal::Ret<std::vector<SystemLogEntry>> GetEventLog() const;
	virtual unsigned int GetEventLogLastModifiedToken() const;
	virtual void ClearEventLog();
	virtual unsigned int GetEventLogSize() const;
	virtual void SetEventLogSize(unsigned int alogSize);

	//Embedded ROM functions
	virtual MarshalSupport::Marshal::Ret<std::list<unsigned int>> GetEmbeddedROMIDs() const;
	virtual unsigned int GetEmbeddedROMInfoLastModifiedToken() const;
	virtual bool GetEmbeddedROMInfo(unsigned int embeddedROMID, IEmbeddedROMInfo& embeddedROMInfo) const;
	virtual bool SetEmbeddedROMPath(unsigned int embeddedROMID, const MarshalSupport::Marshal::In<std::wstring>& filePath);
	virtual bool ReloadEmbeddedROMData(unsigned int embeddedROMID);

	//Module setting functions
	virtual MarshalSupport::Marshal::Ret<std::list<unsigned int>> GetModuleSettingIDs(unsigned int moduleID) const;
	virtual bool GetModuleSettingInfo(unsigned int moduleID, unsigned int moduleSettingID, IModuleSettingInfo& moduleSettingInfo) const;
	virtual bool GetModuleSettingOptionInfo(unsigned int moduleID, unsigned int moduleSettingID, unsigned int moduleSettingOptionIndex, IModuleSettingOptionInfo& moduleSettingOptionInfo) const;
	virtual bool GetModuleSettingActiveOptionIndex(unsigned int moduleID, unsigned int moduleSettingID, unsigned int& activeModuleOptionIndex) const;
	virtual bool SetModuleSettingActiveOptionIndex(unsigned int moduleID, unsigned int moduleSettingID, unsigned int activeOptionIndex);
	virtual void ModuleSettingActiveOptionChangeNotifyRegister(unsigned int moduleID, unsigned int moduleSettingID, IObserverSubscription& observer);
	virtual void ModuleSettingActiveOptionChangeNotifyDeregister(unsigned int moduleID, unsigned int moduleSettingID, IObserverSubscription& observer);

	//Path functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetCapturePath() const;
	virtual void SetCapturePath(const MarshalSupport::Marshal::In<std::wstring>& apath);

	//System interface functions
	virtual void FlagInitialize();
	virtual void Initialize();
	virtual void InitializeDevice(IDevice* device);
	virtual bool GetThrottlingState() const;
	virtual void SetThrottlingState(bool state);
	virtual bool GetRunWhenProgramModuleLoadedState() const;
	virtual void SetRunWhenProgramModuleLoadedState(bool state);
	virtual bool GetEnablePersistentState() const;
	virtual void SetEnablePersistentState(bool state);

	//Device registration
	virtual bool RegisterDevice(const IDeviceInfo& entry, AssemblyHandle assemblyHandle);
	virtual void UnregisterDevice(const MarshalSupport::Marshal::In<std::wstring>& deviceName);

	//Extension registration
	virtual bool RegisterExtension(const IExtensionInfo& entry, AssemblyHandle assemblyHandle);
	virtual void UnregisterExtension(const MarshalSupport::Marshal::In<std::wstring>& extensionName);

	//Module loading and unloading
	virtual void LoadModuleSynchronous(const MarshalSupport::Marshal::In<std::wstring>& filePath, const MarshalSupport::Marshal::In<ConnectorMappingList>& connectorMappings);
	virtual void LoadModuleSynchronousAbort();
	virtual float LoadModuleSynchronousProgress() const;
	virtual bool LoadModuleSynchronousComplete() const;
	virtual bool LoadModuleSynchronousResult() const;
	virtual bool LoadModuleSynchronousAborted() const;
	virtual bool LoadModule(const MarshalSupport::Marshal::In<std::wstring>& filePath, const MarshalSupport::Marshal::In<ConnectorMappingList>& connectorMappings);
	virtual bool SaveSystem(const MarshalSupport::Marshal::In<std::wstring>& filePath);
	virtual bool UnloadModule(unsigned int moduleID);
	virtual void UnloadAllModulesSynchronous();
	virtual bool UnloadAllModulesSynchronousComplete() const;
	virtual void UnloadAllModules();
	virtual bool ReadModuleConnectorInfo(const MarshalSupport::Marshal::In<std::wstring>& filePath, const MarshalSupport::Marshal::Out<std::wstring>& systemClassName, const MarshalSupport::Marshal::Out<ConnectorImportList>& connectorsImported, const MarshalSupport::Marshal::Out<ConnectorExportList>& connectorsExported) const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> LoadModuleSynchronousCurrentModuleName() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> UnloadModuleSynchronousCurrentModuleName() const;

	//Loaded module info functions
	virtual MarshalSupport::Marshal::Ret<std::list<unsigned int>> GetLoadedModuleIDs() const;
	virtual bool GetLoadedModuleInfo(unsigned int moduleID, ILoadedModuleInfo& moduleInfo) const;
	virtual bool GetModuleDisplayName(unsigned int moduleID, const MarshalSupport::Marshal::Out<std::wstring>& moduleDisplayName) const;
	virtual bool GetModuleInstanceName(unsigned int moduleID, const MarshalSupport::Marshal::Out<std::wstring>& moduleInstanceName) const;
	virtual void LoadedModulesChangeNotifyRegister(IObserverSubscription& observer);
	virtual void LoadedModulesChangeNotifyDeregister(IObserverSubscription& observer);

	//Connector info functions
	virtual MarshalSupport::Marshal::Ret<std::list<unsigned int>> GetConnectorIDs() const;
	virtual bool GetConnectorInfo(unsigned int connectorID, IConnectorInfo& connectorInfo) const;

	//Loaded device info functions
	virtual MarshalSupport::Marshal::Ret<std::list<IDevice*>> GetLoadedDevices() const;
	virtual bool GetDeviceDisplayName(IDevice* device, const MarshalSupport::Marshal::Out<std::wstring>& deviceDisplayName) const;
	virtual bool GetDeviceInstanceName(IDevice* device, const MarshalSupport::Marshal::Out<std::wstring>& deviceInstanceName) const;
	virtual bool GetFullyQualifiedDeviceDisplayName(IDevice* device, const MarshalSupport::Marshal::Out<std::wstring>& fullyQualifiedDeviceDisplayName) const;

	//Loaded extension info functions
	virtual MarshalSupport::Marshal::Ret<std::list<IExtension*>> GetLoadedExtensions() const;

	//System execution functions
	virtual bool SystemRunning() const;
	virtual void RunSystem();
	virtual void StopSystem();
	virtual void FlagStopSystem();
	virtual void ExecuteDeviceStep(IDevice* device);
	virtual void ExecuteSystemStep(double maximumTimeslice);
	virtual bool IsSystemRollbackFlagged() const;
	virtual double SystemRollbackTime() const;
	virtual void SetSystemRollback(IDeviceContext* atriggerDevice, IDeviceContext* arollbackDevice, double timeslice, unsigned int accessContext, void (*callbackFunction)(void*), void* callbackParams);

	//View functions
	virtual void BuildFileOpenMenu(IMenuSubmenu& menuSubmenu) const;
	virtual void BuildSystemMenu(IMenuSubmenu& menuSubmenu) const;
	virtual void BuildSettingsMenu(IMenuSubmenu& menuSubmenu) const;
	virtual void BuildDebugMenu(IMenuSubmenu& menuSubmenu) const;
	virtual bool RestoreViewStateForSystem(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState) const;
	virtual bool RestoreViewStateForModule(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID) const;
	virtual bool RestoreViewStateForDevice(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID, const MarshalSupport::Marshal::In<std::wstring>& deviceInstanceName) const;
	virtual bool RestoreViewStateForExtension(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, const MarshalSupport::Marshal::In<std::wstring>& extensionInstanceName) const;
	virtual bool RestoreViewStateForExtension(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID, const MarshalSupport::Marshal::In<std::wstring>& extensionInstanceName) const;

	//Input functions
	virtual KeyCode GetKeyCodeID(const MarshalSupport::Marshal::In<std::wstring>& keyCodeName) const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetKeyCodeName(KeyCode keyCode) const;
	virtual bool TranslateKeyCode(unsigned int platformKeyCode, KeyCode& inputKeyCode) const;
	virtual bool TranslateJoystickButton(unsigned int joystickNo, unsigned int buttonNo, KeyCode& inputKeyCode) const;
	virtual bool TranslateJoystickAxisAsButton(unsigned int joystickNo, unsigned int axisNo, bool positiveAxis, KeyCode& inputKeyCode) const;
	virtual bool TranslateJoystickAxis(unsigned int joystickNo, unsigned int axisNo, AxisCode& inputAxisCode) const;
	virtual void HandleInputKeyDown(KeyCode keyCode);
	virtual void HandleInputKeyUp(KeyCode keyCode);
	virtual void HandleInputAxisUpdate(AxisCode axisCode, float newValue);
	virtual void HandleInputScrollUpdate(ScrollCode scrollCode, int scrollTicks);
	virtual unsigned int GetInputDeviceListLastModifiedToken() const;
	virtual MarshalSupport::Marshal::Ret<std::list<IDevice*>> GetInputDeviceList() const;
	virtual MarshalSupport::Marshal::Ret<std::list<unsigned int>> GetDeviceKeyCodeList(IDevice* targetDevice) const;
	virtual MarshalSupport::Marshal::Ret<std::list<KeyCode>> GetDeviceKeyCodePreferredDefaultMappingList(IDevice* targetDevice, unsigned int deviceKeyCode) const;
	virtual bool IsKeyCodeMapped(KeyCode keyCode) const;
	virtual bool IsDeviceKeyCodeMapped(IDevice* targetDevice, unsigned int targetDeviceKeyCode) const;
	virtual KeyCode GetDeviceKeyCodeMapping(IDevice* targetDevice, unsigned int targetDeviceKeyCode) const;
	virtual bool SetDeviceKeyCodeMapping(IDevice* targetDevice, unsigned int deviceKeyCode, KeyCode systemKeyCode);

private:
	//Enumerations
	enum class InputEvent;
	enum class SystemStateChangeType;

	//Structures
	struct DeviceLibraryEntry;
	struct ExtensionLibraryEntry;
	struct LoadedDeviceInfo;
	struct ExportedDeviceInfo;
	struct ImportedDeviceInfo;
	struct LoadedExtensionInfo;
	struct LoadedGlobalExtensionInfo;
	struct ExportedExtensionInfo;
	struct ImportedExtensionInfo;
	struct LoadedBusInfo;
	struct ExportedBusInfo;
	struct ImportedBusInfo;
	struct LoadedClockSourceInfo;
	struct ExportedClockSourceInfo;
	struct ImportedClockSourceInfo;
	struct LineGroupDetails;
	struct ExportedLineGroupInfo;
	struct ImportedLineGroupInfo;
	struct LoadedModuleInfoInternal;
	struct ConnectorInfoInternal;
	struct InputMapEntry;
	struct InputEventEntry;
	struct LogEntryInternal;
	struct ViewOpenRequest;
	struct InputRegistration;
	struct UnmappedLineStateInfo;
	struct SystemSettingInfo;
	struct SystemSettingOption;
	struct SystemStateChange;
	struct SystemLineInfo;
	struct ExportedSystemLineInfo;
	struct ImportedSystemLineInfo;
	struct ExportedSystemSettingInfo;
	struct ImportedSystemSettingInfo;
	struct SystemLineMapping;
	struct EmbeddedROMInfoInternal;

	//Typedefs
	typedef std::map<std::wstring, unsigned int> NameToIDMap;
	typedef std::pair<std::wstring, unsigned int> NameToIDMapEntry;
	typedef std::map<unsigned int, LoadedModuleInfoInternal> LoadedModuleInfoMap;
	typedef std::pair<unsigned int, LoadedModuleInfoInternal> LoadedModuleInfoMapEntry;
	typedef std::list<LoadedDeviceInfo> LoadedDeviceInfoList;
	typedef std::list<ImportedDeviceInfo> ImportedDeviceInfoList;
	typedef std::vector<DeviceContext*> DeviceArray;
	typedef std::list<LoadedExtensionInfo> LoadedExtensionInfoList;
	typedef std::map<std::wstring, LoadedGlobalExtensionInfo> LoadedGlobalExtensionInfoList;
	typedef std::pair<std::wstring, LoadedGlobalExtensionInfo> LoadedGlobalExtensionInfoListEntry;
	typedef std::list<ImportedExtensionInfo> ImportedExtensionInfoList;
	typedef std::map<std::wstring, DeviceLibraryEntry> DeviceLibraryList;
	typedef std::pair<std::wstring, DeviceLibraryEntry> DeviceLibraryListEntry;
	typedef std::map<std::wstring, ExtensionLibraryEntry> ExtensionLibraryList;
	typedef std::pair<std::wstring, ExtensionLibraryEntry> ExtensionLibraryListEntry;
	typedef std::list<LoadedBusInfo> BusInterfaceList;
	typedef std::list<ImportedBusInfo> ImportedBusInterfaceList;
	typedef std::list<LoadedClockSourceInfo> ClockSourceList;
	typedef std::list<ImportedClockSourceInfo> ImportedClockSourceList;
	typedef std::list<InputRegistration> InputRegistrationList;
	typedef std::map<KeyCode, InputMapEntry> InputKeyMap;
	typedef std::pair<KeyCode, InputMapEntry> InputKeyMapEntry;
	typedef std::map<unsigned int, ConnectorInfoInternal> ConnectorDetailsMap;
	typedef std::pair<unsigned int, ConnectorInfoInternal> ConnectorDetailsMapEntry;
	typedef std::map<unsigned int, LineGroupDetails> LineGroupDetailsMap;
	typedef std::pair<unsigned int, LineGroupDetails> LineGroupDetailsMapEntry;
	typedef std::list<UnmappedLineStateInfo> UnmappedLineStateList;
	typedef std::map<unsigned int, SystemSettingInfo*> SystemSettingsMap;
	typedef std::pair<unsigned int, SystemSettingInfo*> SystemSettingsMapEntry;
	typedef std::list<unsigned int> SystemSettingsIDList;
	typedef std::map<unsigned int, SystemSettingsIDList> ModuleSystemSettingMap;
	typedef std::pair<unsigned int, SystemSettingsIDList> ModuleSystemSettingMapEntry;
	typedef std::map<unsigned int, SystemLineInfo> SystemLineMap;
	typedef std::pair<unsigned int, SystemLineInfo> SystemLineMapEntry;
	typedef std::list<ImportedSystemLineInfo> ImportedSystemLineList;
	typedef std::list<ImportedSystemSettingInfo> ImportedSystemSettingList;
	typedef std::list<SystemLineMapping> SystemLineMappingList;

private:
	//Embedded ROM functions
	bool ReloadEmbeddedROMData(const EmbeddedROMInfoInternal& targetEmbeddedROMInfo);

	//Loaded entity functions
	IDevice* GetDevice(unsigned int moduleID, const std::wstring& deviceName) const;
	IExtension* GetExtension(unsigned int moduleID, const std::wstring& extensionName) const;
	IExtension* GetGlobalExtension(const std::wstring& extensionName) const;
	BusInterface* GetBusInterface(unsigned int moduleID, const std::wstring& busInterfaceName) const;
	ClockSource* GetClockSource(unsigned int moduleID, const std::wstring& clockSourceName) const;
	unsigned int GetSystemLineID(unsigned int moduleID, const std::wstring& systemLineName) const;
	unsigned int GetSystemSettingID(unsigned int moduleID, const std::wstring& systemSettingName) const;

	//Savestate functions
	bool LoadPersistentStateForModule(const std::wstring& filePath, unsigned int moduleID, FileType fileType, bool returnSuccessOnNoFilePresent);
	bool SavePersistentStateForModule(const std::wstring& filePath, unsigned int moduleID, FileType fileType, bool generateNoFileIfNoContentPresent);
	bool LoadSavedRelationshipMap(IHierarchicalStorageNode& node, SavedRelationshipMap& relationshipMap) const;
	void SaveModuleRelationshipsExportConnectors(IHierarchicalStorageNode& moduleNode, unsigned int moduleID) const;
	void SaveModuleRelationshipsImportConnectors(IHierarchicalStorageNode& moduleNode, unsigned int moduleID) const;
	bool DoesLoadedModuleMatchSavedModule(const SavedRelationshipMap& savedRelationshipData, const SavedRelationshipModule& savedModuleInfo, const LoadedModuleInfoInternal& loadedModuleInfo, const ConnectorInfoMapOnImportingModuleID& connectorDetailsOnImportingModuleID) const;

	//Module loading and unloading
	unsigned int GetFirstAvailableDeviceIndex() const;
	unsigned int GenerateFreeModuleID() const;
	unsigned int GenerateFreeConnectorID() const;
	unsigned int GenerateFreeLineGroupID() const;
	unsigned int GenerateFreeSystemLineID() const;
	unsigned int GenerateFreeSystemSettingID() const;
	unsigned int GenerateFreeEmbeddedROMID() const;
	bool LoadModule_Device(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Device_SetDependentDevice(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Device_ReferenceDevice(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Device_ReferenceExtension(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Device_ReferenceBus(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Device_ReferenceClockSource(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Device_RegisterInput(IHierarchicalStorageNode& node, unsigned int moduleID, std::list<InputRegistration>& inputRegistrationRequests);
	bool LoadModule_GlobalExtension(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Extension(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Extension_ReferenceDevice(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Extension_ReferenceExtension(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Extension_ReferenceBus(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Extension_ReferenceClockSource(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_DefineLineGroup(IHierarchicalStorageNode& node, unsigned int moduleID, NameToIDMap& lineGroupNameToIDMap);
	bool LoadModule_BusInterface_DefineCELineMemory(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_DefineCELinePort(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapCELineInputMemory(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapCELineInputPort(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapCELineOutputMemory(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapCELineOutputPort(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapDevice(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapPort(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapLine(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& lineGroupNameToIDMap);
	bool LoadModule_BusInterface_MapClockSource(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_UnmappedLineState(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_ClockSource(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_ClockSource_SetInputClockSource(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_System_OpenView(IHierarchicalStorageNode& node, unsigned int moduleID, std::list<ViewOpenRequest>& viewOpenRequests);
	bool LoadModule_System_ExportConnector(IHierarchicalStorageNode& node, unsigned int moduleID, const std::wstring& systemClassName, NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ExportDevice(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ExportExtension(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ExportBusInterface(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap, const NameToIDMap& lineGroupNameToIDMap);
	bool LoadModule_System_ExportClockSource(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ExportSystemLine(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ExportSystemSetting(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ImportConnector(IHierarchicalStorageNode& node, unsigned int moduleID, const std::wstring& systemClassName, const ConnectorMappingList& connectorMappings, NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ImportDevice(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ImportExtension(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ImportBusInterface(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap, NameToIDMap& lineGroupNameToIDMap);
	bool LoadModule_System_ImportClockSource(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ImportSystemLine(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ImportSystemSetting(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_DefineEmbeddedROM(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_System_DefineSystemLine(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_System_MapSystemLine(IHierarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_System_Setting(IHierarchicalStorageNode& node, unsigned int moduleID, const std::wstring& fileName);
	bool LoadModule_System_Setting_Option(IHierarchicalStorageNode& node, unsigned int moduleID, const std::wstring& fileName, SystemSettingOption& option, bool& defaultOption, bool& toggleSettingOnOption);
	bool LoadModule_System_SelectSettingOption(IHierarchicalStorageNode& node, unsigned int moduleID, SystemStateChange& stateChange);
	bool LoadModule_System_SetClockFrequency(IHierarchicalStorageNode& node, unsigned int moduleID, SystemStateChange& stateChange);
	bool LoadModule_System_SetLineState(IHierarchicalStorageNode& node, unsigned int moduleID, SystemStateChange& stateChange);
	bool LoadModule_ProcessViewQueue(const std::list<ViewOpenRequest>& viewOpenRequests);
	bool LoadModuleInternal(const std::wstring& filePath, const ConnectorMappingList& connectorMappings, std::list<ViewOpenRequest>& viewOpenRequests, std::list<InputRegistration>& inputRegistrationRequests, std::list<SystemStateChange>& systemSettingsChangeRequests, std::list<unsigned int>& addedModuleIDs);
	void UnloadModuleInternal(unsigned int moduleID);
	bool LoadSystem_Device_Settings(IHierarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap);
	bool LoadSystem_Device_MapInput(IHierarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap);
	bool LoadSystem_System_LoadEmbeddedROMData(const std::wstring& fileDir, IHierarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap);
	bool LoadSystem_System_SelectSettingOption(IHierarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap, SystemStateChange& stateChange);
	bool LoadSystem(const std::wstring& filePath, IHierarchicalStorageNode& rootNode, std::list<ViewOpenRequest>& viewOpenRequests, std::list<InputRegistration>& inputRegistrationRequests, std::list<SystemStateChange>& systemSettingsChangeRequests, std::list<unsigned int>& addedModuleIDs);
	void PushLoadModuleCurrentModuleName(const std::wstring& moduleName);
	void PopLoadModuleCurrentModuleName();
	void PushUnloadModuleCurrentModuleName(const std::wstring& moduleName);
	void PopUnloadModuleCurrentModuleName();

	//Device creation and deletion
	bool AddDevice(unsigned int moduleID, IDevice* device, DeviceContext* deviceContext);
	IDevice* CreateDevice(const std::wstring& deviceName, const std::wstring& instanceName, unsigned int moduleID) const;
	void DestroyDevice(const std::wstring& deviceName, IDevice* device) const;
	void UnloadDevice(IDevice* adevice);
	void RemoveDeviceFromDeviceList(DeviceArray& deviceList, IDevice* adevice) const;

	//Extension creation and deletion
	IExtension* CreateGlobalExtension(const std::wstring& extensionName) const;
	IExtension* CreateExtension(const std::wstring& extensionName, const std::wstring& instanceName, unsigned int moduleID) const;
	void DestroyExtension(const std::wstring& extensionName, IExtension* extension) const;
	void UnloadExtension(IExtension* aextension);

	//System interface functions
	void InitializeInternal();
	bool ValidateSystem();
	void SignalSystemStopped();

	//System execution functions
	double ExecuteSystemStepInternal(double maximumTimeslice);
	void ExecuteThread();

	//Output stream functions
	//##TODO## Implement video/audio output streams
//	VideoBuffer RegisterVideoOutput(const std::wstring& name);
	//##NOTE##
	//This stub is old. The current line of thinking for achieving generic video output is to
	//have two levels of abstraction. The first is called a Surface, and represents a single
	//physical display device, such as a TV or computer screen. Most systems will only have
	//one surface. The next level is called a layer. A layer is akin to layers in a graphics
	//program. It can be thought of as a texture with an alpha channel. The layers assigned
	//to a surface have a priority order, indicating the order in which they are drawn. High
	//priority layers are shown on top of low priority layers. Any number of layers can be
	//assigned to a surface. Note that a single layer may also be assigned to multiple
	//surfaces. This allows a secondary alternate debug output to be created, which uses the
	//same rendered content from the real surface, but with debug information overlayed.
	//Behind the scenes, layers will be implemented as textures while surfaces will be OpenGL
	//drawing surfaces, running in orthogonal mode. Textures will be rendered onto quads
	//drawn from highest priority to lowest priority (to take advantage of hardware to
	//eliminate hidden texels), with the z coordinate increasing by 1 for each additional
	//surface.
	//Note that support will need to exist for stretching the texture, to support non-square
	//pixel aspect ratios such as those used on the Mega Drive.
	//Note that it might be better not to make textures our first choice for rendering pixel
	//data. The low-level glDrawPixels is currently being used, and it has alpha support. If
	//we were to use this function to render surfaces, we would need to render in reverse
	//order in order to replace low priority layers with high priority content. We should be
	//able to make a higher-order surface class which uses textures internally, and adds
	//scaling support. We would however effectively have to always render from texture anyway
	//for the Mega Drive, as all its render layers need to support scaling. Note that when
	//using texture scaling, it would not be possible to change the rate of scaling partway
	//through a frame.

	//Input functions
	void UnmapAllKeyCodeMappingsForDevice(IDevice* adevice);
	void SendStoredInputEvents();
	void ClearSentStoredInputEvents();

	//System setting functions
	bool ApplySystemStateChange(const SystemStateChange& stateChange);

	//System line functions
	unsigned int GetSystemLineWidth(unsigned int systemLineID) const;
	bool SetSystemLineState(unsigned int systemLineID, const Data& lineData);

private:
	//Extension interfaces
	IGUIExtensionInterface& guiExtensionInterface;

	//Devices
	DeviceLibraryList deviceLibrary;
	//##TODO##
	//Note that as per the decision above to make names local, we also need to consider
	//that when presenting device names to the user. Debug menu items and window names now
	//need to include the addon name where a device belongs to an addon. Also note that
	//where one addon is loaded multiple times (eg, two identical controllers connected),
	//we need to add a number on to each instance of the addon, to distinguish them one
	//from the other.
	//##TODO## We need to carefully consider how savestates will be implemented when we
	//have addons loaded. In particular, what happens when a user tries to load a
	//savestate into a system with a different set of addons loaded? Obviously we don't
	//want something as trivial as a three button controller instead of a six button
	//controller being loaded to prevent a savestate being compatible. At the same time
	//though, what about identifying what addons were loaded at the time a savestate was
	//made, and automatically changing the system state to switch to that set of addons? I
	//would suggest we don't make the change automatic, but ask the user if they want to
	//switch the loaded addons in order to load the savestate. Further down this track, we
	//also need a way to identify the currently loaded "game", so that we can determine
	//which savestates to make available. How does this work when we have multiple "games"
	//loaded though, such as when we have S&K with another game locked on?
	//##NOTE## When implementing the addon system, we have decided we don't want to
	//restrict our emulator to running only a single system at any one time. We want it to
	//be possible to load up, for example, two completely disconnected and isolated
	//systems in the emulator at once, and play them side by side. We also need to allow
	//addons to link to more than one system, so that an addon can, for example, provide a
	//connection between two systems. An example of this might be in a link cable, such as
	//the one provided by Zero Tolerance. Note however, that a link cable, or addon, might
	//need to interface between two completely independent and normally unrelated systems,
	//and the addon needs to only be available if both are loaded.
	mutable unsigned int nextFreeModuleID;
	LoadedModuleInfoMap loadedModuleInfoMap;
	LoadedDeviceInfoList loadedDeviceInfoList;
	ImportedDeviceInfoList importedDeviceInfoList;
	ExecutionManager executionManager;
	DeviceArray devices;

	//Extensions
	ExtensionLibraryList extensionLibrary;
	LoadedExtensionInfoList loadedExtensionInfoList;
	ImportedExtensionInfoList importedExtensionInfoList;
	LoadedGlobalExtensionInfoList globalExtensionInfoList;

	//Bus interfaces
	BusInterfaceList busInterfaces;
	ImportedBusInterfaceList importedBusInterfaces;

	//Unmapped line state info
	UnmappedLineStateList unmappedLineStateList;

	//Clock sources
	ClockSourceList clockSources;
	ImportedClockSourceList importedClockSources;

	//Module settings
	mutable unsigned int nextFreeSystemSettingID;
	SystemSettingsMap systemSettings;
	std::set<SystemSettingInfo*> systemSettingsObjects;
	ModuleSystemSettingMap moduleSettings;
	ImportedSystemSettingList importedSystemSettings;

	//System lines
	mutable unsigned int nextFreeSystemLineID;
	SystemLineMap systemLines;
	ImportedSystemLineList importedSystemLines;
	SystemLineMappingList systemLineMappings;

	//Embedded ROM info
	mutable unsigned int nextFreeEmbeddedROMID;
	std::map<unsigned int, EmbeddedROMInfoInternal> embeddedROMInfoSet;
	unsigned int embeddedROMInfoLastModifiedToken;

	//System menu handlers
	std::set<IExtension*> systemMenuHandlers;

	//Input settings
	mutable std::mutex inputMutex;
	unsigned int inputDeviceListLastModifiedToken;
	InputRegistrationList inputRegistrationList;
	InputKeyMap inputKeyMap;
	std::list<InputEventEntry> inputEvents;

	//System settings
	std::wstring capturePath;
	bool enableThrottling;
	bool runWhenProgramModuleLoaded;
	bool enablePersistentState;

	//Connector settings
	mutable unsigned int nextFreeConnectorID;
	ConnectorDetailsMap connectorDetailsMap;

	//Line group settings
	mutable unsigned int nextFreeLineGroupID;
	LineGroupDetailsMap lineGroupDetailsMap;

	//Access mutexes
	mutable std::mutex systemRollbackMutex;
	mutable std::mutex systemStateMutex;
	mutable std::mutex moduleLoadMutex;
	mutable std::mutex loadedElementMutex;
	mutable std::mutex eventLogMutex;
	mutable std::mutex embeddedROMMutex;
	mutable std::mutex moduleSettingMutex;

	//Asynchronous notification settings
	mutable std::mutex moduleNameMutex;
	std::condition_variable notifySystemStopped;
	volatile bool stopSystem;
	volatile bool systemStopped;
	volatile bool initialize;
	volatile bool rollback;
	volatile float loadSystemProgress;
	volatile bool loadSystemComplete;
	volatile bool loadSystemResult;
	volatile bool loadSystemAbort;
	volatile bool clearSystemComplete;
	std::list<std::wstring> loadSystemCurrentModuleNameStack;
	std::list<std::wstring> unloadSystemCurrentModuleNameStack;

	//Rollback settings
	volatile double rollbackTimeslice;
	unsigned int rollbackContext;
	IDeviceContext* rollbackDevice;
	bool useRollbackFunction;
	void (*rollbackFunction)(void*);
	void* rollbackParams;

	//Event log settings
	unsigned int eventLogSize;
	mutable unsigned int eventLogLastModifiedToken;
	mutable std::list<SystemLogEntry> log;

	//Notification settings
	ObserverCollection loadedModuleChangeObservers;
};

#include "System.inl"
#endif
