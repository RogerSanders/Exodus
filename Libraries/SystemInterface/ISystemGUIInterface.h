#ifndef __ISYSTEMGUIINTERFACE_H__
#define __ISYSTEMGUIINTERFACE_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include "CallbackSupport/CallbackSupport.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include "IEmbeddedROMInfo.h"
#include "IModuleSettingInfo.h"
#include "IModuleSettingOptionInfo.h"
#include <string>
#include <list>
#include <vector>
#include <map>

class ISystemGUIInterface :public ISystemExtensionInterface
{
public:
	//Enumerations
	enum class FileType;

	//Structures
	struct StateInfo;
	struct ModuleRelationship;
	struct SavedRelationshipImportConnector;
	struct SavedRelationshipExportConnector;
	struct SavedRelationshipModule;
	struct ConnectorMapping;
	struct ConnectorDefinitionImport;
	struct ConnectorDefinitionExport;
	struct SystemLogEntry;

	//Typedefs
	typedef std::map<unsigned int, ModuleRelationship> ModuleRelationshipMap;
	typedef std::pair<unsigned int, ModuleRelationship> ModuleRelationshipEntry;
	typedef std::map<unsigned int, SavedRelationshipModule> SavedRelationshipMap;
	typedef std::pair<unsigned int, SavedRelationshipModule> SavedRelationshipMapEntry;
	typedef std::map<unsigned int, std::list<ConnectorInfo>> ConnectorInfoMapOnImportingModuleID;
	typedef std::list<ConnectorMapping> ConnectorMappingList;
	typedef std::list<ConnectorDefinitionImport> ConnectorImportList;
	typedef std::list<ConnectorDefinitionExport> ConnectorExportList;

public:
	//Interface version functions
	static inline unsigned int ThisISystemGUIInterfaceVersion() { return 1; }
	virtual unsigned int GetISystemGUIInterfaceVersion() const = 0;

	//Path functions
	virtual void SetCapturePath(const MarshalSupport::Marshal::In<std::wstring>& apath) = 0;

	//Logging functions
	virtual MarshalSupport::Marshal::Ret<std::vector<SystemLogEntry>> GetEventLog() const = 0;
	virtual unsigned int GetEventLogLastModifiedToken() const = 0;
	virtual void ClearEventLog() = 0;
	virtual unsigned int GetEventLogSize() const = 0;
	virtual void SetEventLogSize(unsigned int alogSize) = 0;

	//Embedded ROM functions
	virtual MarshalSupport::Marshal::Ret<std::list<unsigned int>> GetEmbeddedROMIDs() const = 0;
	virtual unsigned int GetEmbeddedROMInfoLastModifiedToken() const = 0;
	virtual bool GetEmbeddedROMInfo(unsigned int embeddedROMID, IEmbeddedROMInfo& embeddedROMInfo) const = 0;
	virtual bool SetEmbeddedROMPath(unsigned int embeddedROMID, const MarshalSupport::Marshal::In<std::wstring>& filePath) = 0;
	virtual bool ReloadEmbeddedROMData(unsigned int embeddedROMID) = 0;

	//Module setting functions
	virtual MarshalSupport::Marshal::Ret<std::list<unsigned int>> GetModuleSettingIDs(unsigned int moduleID) const = 0;
	virtual bool GetModuleSettingInfo(unsigned int moduleID, unsigned int moduleSettingID, IModuleSettingInfo& moduleSettingInfo) const = 0;
	virtual bool GetModuleSettingOptionInfo(unsigned int moduleID, unsigned int moduleSettingID, unsigned int moduleSettingOptionIndex, IModuleSettingOptionInfo& moduleSettingOptionInfo) const = 0;
	virtual bool GetModuleSettingActiveOptionIndex(unsigned int moduleID, unsigned int moduleSettingID, unsigned int& activeOptionIndex) const = 0;
	virtual bool SetModuleSettingActiveOptionIndex(unsigned int moduleID, unsigned int moduleSettingID, unsigned int activeOptionIndex) = 0;
	virtual void ModuleSettingActiveOptionChangeNotifyRegister(unsigned int moduleID, unsigned int moduleSettingID, IObserverSubscription& observer) = 0;
	virtual void ModuleSettingActiveOptionChangeNotifyDeregister(unsigned int moduleID, unsigned int moduleSettingID, IObserverSubscription& observer) = 0;

	//System interface functions
	virtual bool GetThrottlingState() const = 0;
	virtual void SetThrottlingState(bool state) = 0;
	virtual bool GetRunWhenProgramModuleLoadedState() const = 0;
	virtual void SetRunWhenProgramModuleLoadedState(bool state) = 0;
	virtual bool GetEnablePersistentState() const = 0;
	virtual void SetEnablePersistentState(bool state) = 0;

	//Device registration
	virtual bool RegisterDevice(const IDeviceInfo& entry, AssemblyHandle assemblyHandle) = 0;
	virtual void UnregisterDevice(const MarshalSupport::Marshal::In<std::wstring>& deviceName) = 0;

	//Extension registration
	virtual bool RegisterExtension(const IExtensionInfo& entry, AssemblyHandle assemblyHandle) = 0;
	virtual void UnregisterExtension(const MarshalSupport::Marshal::In<std::wstring>& extensionName) = 0;

	//Input functions
	virtual KeyCode GetKeyCodeID(const MarshalSupport::Marshal::In<std::wstring>& keyCodeName) const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetKeyCodeName(KeyCode keyCode) const = 0;
	virtual unsigned int GetInputDeviceListLastModifiedToken() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<IDevice*>> GetInputDeviceList() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<unsigned int>> GetDeviceKeyCodeList(IDevice* targetDevice) const = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<KeyCode>> GetDeviceKeyCodePreferredDefaultMappingList(IDevice* targetDevice, unsigned int deviceKeyCode) const = 0;
	virtual bool IsKeyCodeMapped(KeyCode keyCode) const = 0;
	virtual bool IsDeviceKeyCodeMapped(IDevice* targetDevice, unsigned int targetDeviceKeyCode) const = 0;
	virtual KeyCode GetDeviceKeyCodeMapping(IDevice* targetDevice, unsigned int targetDeviceKeyCode) const = 0;
	virtual bool SetDeviceKeyCodeMapping(IDevice* targetDevice, unsigned int deviceKeyCode, KeyCode systemKeyCode) = 0;

	//Savestate functions
	//##TODO## Consider not making the name of a savestate xml file within a zip archive
	//be hardcoded to "save.xml". Come up with a way for the name within the archive to be
	//specified by the caller perhaps? The problem is, we don't want to have to load the
	//zip file twice.
	virtual bool LoadState(const MarshalSupport::Marshal::In<std::wstring>& filePath, FileType fileType, bool debuggerState) = 0;
	virtual bool SaveState(const MarshalSupport::Marshal::In<std::wstring>& filePath, FileType fileType, bool debuggerState) = 0;
	virtual MarshalSupport::Marshal::Ret<StateInfo> GetStateInfo(const MarshalSupport::Marshal::In<std::wstring>& filePath, FileType fileType) const = 0;
	virtual bool LoadModuleRelationshipsNode(IHierarchicalStorageNode& node, const MarshalSupport::Marshal::Out<ModuleRelationshipMap>& relationshipMap) const = 0;
	virtual void SaveModuleRelationshipsNode(IHierarchicalStorageNode& node, bool saveFilePathInfo = false, const MarshalSupport::Marshal::In<std::wstring>& relativePathBase = L"") const = 0;

	//Module loading and unloading
	//##TODO## Add the use of FileType to module loading, so that a module definition can
	//be contained either in a zip file or as a standalone file.
	//##TODO## As per the requirement for savestates above, make it possible to specify
	//the target name of a file within a zip archive. This will be necessary with modules,
	//where it is highly desirable to be able to create a zip file with a set of related
	//system definitions.
	//##FIX## Shouldn't all the synchronous functions be available here, like progress,
	//result, and aborting?
	virtual void LoadModuleSynchronous(const MarshalSupport::Marshal::In<std::wstring>& filePath, const MarshalSupport::Marshal::In<ConnectorMappingList>& connectorMappings) = 0;
	virtual void LoadModuleSynchronousAbort() = 0;
	virtual float LoadModuleSynchronousProgress() const = 0;
	virtual bool LoadModuleSynchronousComplete() const = 0;
	virtual bool LoadModuleSynchronousResult() const = 0;
	virtual bool LoadModuleSynchronousAborted() const = 0;
	virtual bool LoadModule(const MarshalSupport::Marshal::In<std::wstring>& filePath, const MarshalSupport::Marshal::In<ConnectorMappingList>& connectorMappings) = 0;
	virtual bool SaveSystem(const MarshalSupport::Marshal::In<std::wstring>& filePath) = 0;
	virtual bool UnloadModule(unsigned int moduleID) = 0;
	virtual void UnloadAllModulesSynchronous() = 0;
	virtual bool UnloadAllModulesSynchronousComplete() const = 0;
	virtual void UnloadAllModules() = 0;
	virtual bool ReadModuleConnectorInfo(const MarshalSupport::Marshal::In<std::wstring>& filePath, const MarshalSupport::Marshal::Out<std::wstring>& systemClassName, const MarshalSupport::Marshal::Out<ConnectorImportList>& connectorsImported, const MarshalSupport::Marshal::Out<ConnectorExportList>& connectorsExported) const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> LoadModuleSynchronousCurrentModuleName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> UnloadModuleSynchronousCurrentModuleName() const = 0;

	//View functions
	virtual void BuildFileOpenMenu(IMenuSubmenu& menuSubmenu) const = 0;
	virtual void BuildSystemMenu(IMenuSubmenu& menuSubmenu) const = 0;
	virtual void BuildSettingsMenu(IMenuSubmenu& menuSubmenu) const = 0;
	virtual void BuildDebugMenu(IMenuSubmenu& menuSubmenu) const = 0;
	virtual bool RestoreViewStateForSystem(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState) const = 0;
	virtual bool RestoreViewStateForModule(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID) const = 0;
	virtual bool RestoreViewStateForDevice(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID, const MarshalSupport::Marshal::In<std::wstring>& deviceInstanceName) const = 0;
	virtual bool RestoreViewStateForExtension(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, const MarshalSupport::Marshal::In<std::wstring>& extensionInstanceName) const = 0;
	virtual bool RestoreViewStateForExtension(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID, const MarshalSupport::Marshal::In<std::wstring>& extensionInstanceName) const = 0;
};

#include "ISystemGUIInterface.inl"
#endif
