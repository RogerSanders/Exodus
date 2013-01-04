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
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "BusInterface.h"
#include "ClockSource.h"
#include "DeviceContext.h"
#include "DeviceInfo.h"
#include "ISystemInternal.h"
#include "ExecutionManager.h"
#include <string>
#include <vector>
#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

//Terminology:
//Assembly  - An assembly (IE, a dll) which contains the definition of one or more devices
//Device    - An emulation core for a logical chip or device in a system
//Module    - A physical or logical part of a system, usually defining one or more devices
//            and the connections between devices. In our case, an xml file describes a
//            module.
//System    - The "base" module which defines a system. It may export connectors, but
//            imports none.
//Connector - An interface into a module or system where other modules are able to attach
//            themselves, and are able to observe and/or effect the operation of the
//            system via the interface exposed over the connector.

class System :public ISystemInternal
{
public:
	//Constructors
	inline System(void* aassemblyHandle);
	inline ~System();

	//Savestate functions
	virtual bool LoadState(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType, bool debuggerState);
	virtual bool SaveState(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType, bool debuggerState);
	virtual StateInfo GetStateInfo(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType) const;
	virtual bool LoadModuleRelationshipsNode(IHeirarchicalStorageNode& node, ModuleRelationshipMap& relationshipMap) const;
	virtual bool DoesLoadedModuleMatchSavedModule(const SavedRelationshipMap& savedRelationshipData, const SavedRelationshipModule& savedModuleInfo, const LoadedModuleInfo& loadedModuleInfo, const ConnectorInfoMapOnImportingModuleID& connectorDetailsOnImportingModuleID) const;
	virtual void SaveModuleRelationshipsNode(IHeirarchicalStorageNode& relationshipsNode, bool saveFilePathInfo = false) const;

	//Logging functions
	virtual void WriteLogEvent(const ILogEntry& entry) const;
	void ClearEventLog();
	void SaveEventLog(const std::wstring& fileName);
	unsigned int GetEventLogSize() const;
	void SetEventLogSize(unsigned int alogSize);

	//Path functions
	virtual std::wstring GetCapturePath() const;
	virtual void SetCapturePath(const std::wstring& apath);

	//System interface functions
	virtual void Initialize();
	virtual void RunSystem();
	virtual void StopSystem();
	virtual void FlagStopSystem();
	virtual bool GetThrottlingState() const;
	virtual void SetThrottlingState(bool state);
	virtual bool GetRunWhenProgramModuleLoadedState() const;
	virtual void SetRunWhenProgramModuleLoadedState(bool state);
	virtual bool SystemRunning() const;
	virtual bool IsSystemRollbackFlagged() const;
	virtual double SystemRollbackTime() const;
	virtual void SetSystemRollback(IDeviceContext* atriggerDevice, IDeviceContext* arollbackDevice, double timeslice, unsigned int accessContext, void (*callbackFunction)(void*), void* callbackParams);
	virtual void ExecuteDeviceStep(DeviceContext* device);

	//Device registration
	virtual bool RegisterDevice(const DeviceInfo& entry, IDevice::AssemblyHandle assemblyHandle);
	virtual void UnregisterDevice(const std::wstring deviceName);

	//Module loading and unloading
	virtual bool GetModuleDisplayName(unsigned int moduleID, std::wstring& moduleDisplayName) const;
	virtual void LoadModuleSynchronous(const std::wstring& fileDir, const std::wstring& fileName, const ConnectorMappingList& connectorMappings, IViewModelLauncher& aviewModelLauncher);
	virtual void LoadModuleSynchronousAbort();
	virtual float LoadModuleSynchronousProgress() const;
	virtual bool LoadModuleSynchronousComplete() const;
	virtual bool LoadModuleSynchronousResult() const;
	virtual bool LoadModuleSynchronousAborted() const;
	virtual bool LoadModule(const std::wstring& fileDir, const std::wstring& fileName, const ConnectorMappingList& connectorMappings, IViewModelLauncher& aviewModelLauncher);
	virtual bool SaveSystem(const std::wstring& fileDir, const std::wstring& fileName);
	virtual bool UnloadModule(unsigned int moduleID);
	virtual void UnloadAllModulesSynchronous();
	virtual bool UnloadAllModulesSynchronousComplete() const;
	virtual void UnloadAllModules();
	virtual bool ReadModuleConnectorInfo(const std::wstring& fileDir, const std::wstring& fileName, std::wstring& systemClassName, ConnectorImportList& connectorsImported, ConnectorExportList& connectorsExported) const;

	//Loaded module info functions
	virtual std::list<unsigned int> GetLoadedModuleIDs() const;
	virtual bool GetLoadedModuleInfo(unsigned int moduleID, LoadedModuleInfo& moduleInfo) const;

	//Connector info functions
	virtual std::list<unsigned int> GetConnectorIDs() const;
	virtual bool GetConnectorInfo(unsigned int connectorID, ConnectorInfo& connectorInfo) const;

	//Assembly handle functions
	void* GetAssemblyHandle() const;

	//View functions
	virtual void BuildDebugMenu(IMenuSubmenu& menuSubmenu, IViewModelLauncher& viewModelLauncher) const;
	virtual void RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual bool RestoreViewModelStateForDevice(unsigned int moduleID, const std::wstring& deviceInstanceName, const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void DeleteSystemViews();

	//Input functions
	virtual void HandleInputKeyDown(IDeviceContext::KeyCode keyCode);
	virtual void HandleInputKeyUp(IDeviceContext::KeyCode keyCode);

private:
	//Enumerations
	enum InputEvent;

	//Structures
	struct DeviceLibraryEntry;
	struct LoadedDeviceInfo;
	struct ExportedDeviceInfo;
	struct ImportedDeviceInfo;
	struct LoadedBusInfo;
	struct ExportedBusInfo;
	struct ImportedBusInfo;
	struct LoadedClockSourceInfo;
	struct ExportedClockSourceInfo;
	struct ImportedClockSourceInfo;
	struct ExportedLineGroupInfo;
	struct ImportedLineGroupInfo;
	struct LineGroupDetails;
	struct ConnectorDetails;
	struct InputMapEntry;
	struct InputEventEntry;
	struct LogEntryInternal;
	struct ViewModelOpenRequest;

	//Typedefs
	typedef std::map<std::wstring, unsigned int> NameToIDMap;
	typedef std::pair<std::wstring, unsigned int> NameToIDMapEntry;
	typedef std::list<LoadedModuleInfo> LoadedModuleInfoList;
	typedef std::list<LoadedDeviceInfo> LoadedDeviceInfoList;
	typedef std::list<ImportedDeviceInfo> ImportedDeviceInfoList;
	typedef std::vector<DeviceContext*> DeviceArray;
	typedef std::map<std::wstring, DeviceLibraryEntry> DeviceLibraryList;
	typedef std::pair<std::wstring, DeviceLibraryEntry> DeviceLibraryListEntry;
	typedef std::list<LoadedBusInfo> BusInterfaceList;
	typedef std::list<ImportedBusInfo> ImportedBusInterfaceList;
	typedef std::list<LoadedClockSourceInfo> ClockSourceList;
	typedef std::list<ImportedClockSourceInfo> ImportedClockSourceList;
	typedef std::map<IDeviceContext::KeyCode, InputMapEntry> InputKeyMap;
	typedef std::pair<IDeviceContext::KeyCode, InputMapEntry> InputKeyMapEntry;
	typedef std::map<unsigned int, ConnectorDetails> ConnectorDetailsMap;
	typedef std::pair<unsigned int, ConnectorDetails> ConnectorDetailsMapEntry;
	typedef std::map<unsigned int, LineGroupDetails> LineGroupDetailsMap;
	typedef std::pair<unsigned int, LineGroupDetails> LineGroupDetailsMapEntry;

	//View and menu classes
	class DebugMenuHandler;
	class LoggerViewModel;
	class LoggerDetailsViewModel;
	class DeviceControlViewModel;
	class LoggerView;
	class LoggerDetailsView;
	class DeviceControlView;
	friend class LoggerViewModel;
	friend class LoggerDetailsViewModel;
	friend class DeviceControlViewModel;
	friend class LoggerView;
	friend class LoggerDetailsView;
	friend class DeviceControlView;

private:
	//Loaded entity functions
	IDevice* GetDevice(unsigned int moduleID, const std::wstring& deviceName) const;
	BusInterface* GetBusInterface(unsigned int moduleID, const std::wstring& busInterfaceName) const;
	ClockSource* GetClockSource(unsigned int moduleID, const std::wstring& clockSourceName) const;

	//Savestate functions
	bool LoadSavedRelationshipMap(IHeirarchicalStorageNode& node, SavedRelationshipMap& relationshipMap) const;
	void SaveModuleRelationshipsExportConnectors(IHeirarchicalStorageNode& moduleNode, unsigned int moduleID) const;
	void SaveModuleRelationshipsImportConnectors(IHeirarchicalStorageNode& moduleNode, unsigned int moduleID) const;

	//Module loading and unloading
	unsigned int GenerateFreeModuleID() const;
	unsigned int GenerateFreeConnectorID() const;
	unsigned int GenerateFreeLineGroupID() const;
	bool LoadModule_Device(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Device_SetDependentDevice(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Device_ReferenceDevice(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Device_ReferenceBus(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Device_ReferenceClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_Device_RegisterInput(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_DefineLineGroup(IHeirarchicalStorageNode& node, unsigned int moduleID, NameToIDMap& lineGroupNameToIDMap);
	bool LoadModule_BusInterface_DefineCELineMemory(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_DefineCELinePort(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapCELineInputMemory(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapCELineInputPort(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapCELineOutputMemory(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapCELineOutputPort(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapDevice(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapPort(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_BusInterface_MapLine(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& lineGroupNameToIDMap);
	bool LoadModule_BusInterface_MapClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_ClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_ClockSource_SetInputClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID);
	bool LoadModule_System_OpenViewModel(IHeirarchicalStorageNode& node, unsigned int moduleID, std::list<ViewModelOpenRequest>& viewModelOpenRequests);
	bool LoadModule_System_ExportConnector(IHeirarchicalStorageNode& node, unsigned int moduleID, const std::wstring& systemClassName, NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ExportDevice(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ExportBusInterface(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap, const NameToIDMap& lineGroupNameToIDMap);
	bool LoadModule_System_ExportClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ImportConnector(IHeirarchicalStorageNode& node, unsigned int moduleID, const std::wstring& systemClassName, const ConnectorMappingList& connectorMappings, NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ImportDevice(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_System_ImportBusInterface(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap, NameToIDMap& lineGroupNameToIDMap);
	bool LoadModule_System_ImportClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap);
	bool LoadModule_ProcessViewModelQueue(const std::list<ViewModelOpenRequest>& viewModelOpenRequests, IViewModelLauncher& aviewModelLauncher);
	bool LoadModuleInternal(const std::wstring& fileDir, const std::wstring& fileName, const ConnectorMappingList& connectorMappings, std::list<ViewModelOpenRequest>& viewModelOpenRequests, std::list<LoadedModuleInfo>& addedModules);
	void UnloadModuleInternal(unsigned int moduleID);
	bool LoadSystem(const std::wstring& fileDir, const std::wstring& fileName, IHeirarchicalStorageNode& rootNode, std::list<ViewModelOpenRequest>& viewModelOpenRequests, std::list<LoadedModuleInfo>& addedModules);

	//Device creation and deletion
	bool AddDevice(unsigned int moduleID, IDevice* device, DeviceContext* deviceContext);
	IDevice* CreateDevice(const std::wstring& deviceName, const std::wstring& instanceName, unsigned int moduleID) const;
	void DestroyDevice(const std::wstring& deviceName, IDevice* device) const;
	void UnloadDevice(IDevice* adevice);
	void RemoveDeviceFromDeviceList(DeviceArray& deviceList, IDevice* adevice) const;

	//System interface functions
	bool ValidateSystem();
	void SignalSystemStopped();
	void ExecuteThread();
	double ExecuteSystemStep(double maximumTimeslice);

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
	IDeviceContext::KeyCode GetKeyCodeID(const std::wstring& keyCodeName) const;
	bool IsKeyCodeMapped(IDeviceContext::KeyCode keyCode) const;
	bool MapKeyCode(IDeviceContext::KeyCode keyCode, unsigned int targetDeviceKeyCode, IDevice* targetDevice);
	void UnmapKeyCode(IDeviceContext::KeyCode keyCode);
	void ClearKeyCodeMap();
	void SendStoredInputEvents();
	void ClearSentStoredInputEvents();
	void UnmapAllKeyCodeMappingsForDevice(IDevice* adevice);

	//Window functions
	void OpenLoggerDetailsView(const LogEntryInternal& alogEntry);

private:
	//Menu handling
	DebugMenuHandler* debugMenuHandler;

	//Module handle
	void* assemblyHandle;

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
	LoadedModuleInfoList loadedModuleInfoList;
	LoadedDeviceInfoList loadedDeviceInfoList;
	ImportedDeviceInfoList importedDeviceInfoList;
	ExecutionManager executionManager;
	DeviceArray devices;

	//Bus interfaces
	BusInterfaceList busInterfaces;
	ImportedBusInterfaceList importedBusInterfaces;

	//Clock sources
	ClockSourceList clockSources;
	ImportedClockSourceList importedClockSources;

	//Input settings
	mutable boost::mutex inputMutex;
	InputKeyMap inputKeyMap;
	std::list<InputEventEntry> inputEvents;

	//System settings
	std::wstring capturePath;
	bool enableThrottling;
	bool runWhenProgramModuleLoaded;

	//Connector settings
	mutable unsigned int nextFreeConnectorID;
	ConnectorDetailsMap connectorDetailsMap;

	//Line group settings
	mutable unsigned int nextFreeLineGroupID;
	LineGroupDetailsMap lineGroupDetailsMap;

	//Access mutexes
	mutable boost::mutex accessMutex;
	mutable boost::mutex controlMutex;
	mutable boost::mutex debugMutex;

	//Asynchronous notification settings
	boost::condition notifySystemStopped;
	volatile bool stopSystem;
	volatile bool systemStopped;
	volatile bool initialize;
	volatile bool rollback;
	volatile float loadSystemProgress;
	volatile bool loadSystemComplete;
	volatile bool loadSystemResult;
	volatile bool loadSystemAbort;
	volatile bool clearSystemComplete;

	//Rollback settings
	volatile double rollbackTimeslice;
	unsigned int rollbackContext;
	IDeviceContext* rollbackDevice;
	bool useRollbackFunction;
	void (*rollbackFunction)(void*);
	void* rollbackParams;

	//Event log settings
	mutable std::list<LogEntryInternal> log;
	bool loggerLevel1Enabled;
	bool loggerLevel2Enabled;
	bool loggerLevel3Enabled;
	bool loggerLevel4Enabled;
	bool loggerLevel5Enabled;
	unsigned int eventLogSize;
};

#include "System.inl"
#endif
