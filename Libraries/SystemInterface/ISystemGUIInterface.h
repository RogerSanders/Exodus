#ifndef __ISYSTEMGUIINTERFACE_H__
#define __ISYSTEMGUIINTERFACE_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include "CallbackSupport/CallbackSupport.pkg"
#include "InteropSupport/InteropSupport.pkg"
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
	static inline unsigned int ThisISystemGUIInterfaceVersion();
	virtual unsigned int GetISystemGUIInterfaceVersion() const = 0;

	//Path functions
	inline void SetCapturePath(const std::wstring& apath);

	//Logging functions
	inline std::vector<SystemLogEntry> GetEventLog() const;
	virtual unsigned int GetEventLogLastModifiedToken() const = 0;
	virtual void ClearEventLog() = 0;
	virtual unsigned int GetEventLogSize() const = 0;
	virtual void SetEventLogSize(unsigned int alogSize) = 0;

	//Embedded ROM functions
	inline std::list<unsigned int> GetEmbeddedROMIDs() const;
	virtual unsigned int GetEmbeddedROMInfoLastModifiedToken() const = 0;
	virtual bool GetEmbeddedROMInfo(unsigned int embeddedROMID, IEmbeddedROMInfo& embeddedROMInfo) const = 0;
	inline bool SetEmbeddedROMPath(unsigned int embeddedROMID, const std::wstring& filePath);
	virtual bool ReloadEmbeddedROMData(unsigned int embeddedROMID) = 0;

	//Module setting functions
	inline std::list<unsigned int> GetModuleSettingIDs(unsigned int moduleID) const;
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
	inline void UnregisterDevice(const std::wstring& deviceName);

	//Extension registration
	virtual bool RegisterExtension(const IExtensionInfo& entry, AssemblyHandle assemblyHandle) = 0;
	inline void UnregisterExtension(const std::wstring& extensionName);

	//Input functions
	inline KeyCode GetKeyCodeID(const std::wstring& keyCodeName) const;
	inline std::wstring GetKeyCodeName(KeyCode keyCode) const;
	virtual unsigned int GetInputDeviceListLastModifiedToken() const = 0;
	inline std::list<IDevice*> GetInputDeviceList() const;
	inline std::list<unsigned int> GetDeviceKeyCodeList(IDevice* targetDevice) const;
	inline std::list<KeyCode> GetDeviceKeyCodePreferredDefaultMappingList(IDevice* targetDevice, unsigned int deviceKeyCode) const;
	virtual bool IsKeyCodeMapped(KeyCode keyCode) const = 0;
	virtual bool IsDeviceKeyCodeMapped(IDevice* targetDevice, unsigned int targetDeviceKeyCode) const = 0;
	virtual KeyCode GetDeviceKeyCodeMapping(IDevice* targetDevice, unsigned int targetDeviceKeyCode) const = 0;
	virtual bool SetDeviceKeyCodeMapping(IDevice* targetDevice, unsigned int deviceKeyCode, KeyCode systemKeyCode) = 0;

	//Savestate functions
	//##TODO## Consider not making the name of a savestate xml file within a zip archive
	//be hardcoded to "save.xml". Come up with a way for the name within the archive to be
	//specified by the caller perhaps? The problem is, we don't want to have to load the
	//zip file twice.
	inline bool LoadState(const std::wstring& filePath, FileType fileType, bool debuggerState);
	inline bool SaveState(const std::wstring& filePath, FileType fileType, bool debuggerState);
	inline StateInfo GetStateInfo(const std::wstring& filePath, FileType fileType) const;
	inline bool LoadModuleRelationshipsNode(IHierarchicalStorageNode& node, ModuleRelationshipMap& relationshipMap) const;
	inline void SaveModuleRelationshipsNode(IHierarchicalStorageNode& node, bool saveFilePathInfo = false, const std::wstring& relativePathBase = L"") const;

	//Module loading and unloading
	//##TODO## Add the use of FileType to module loading, so that a module definition can
	//be contained either in a zip file or as a standalone file.
	//##TODO## As per the requirement for savestates above, make it possible to specify
	//the target name of a file within a zip archive. This will be necessary with modules,
	//where it is highly desirable to be able to create a zip file with a set of related
	//system definitions.
	inline void LoadModuleSynchronous(const std::wstring& filePath, const ConnectorMappingList& connectorMappings);
	virtual void LoadModuleSynchronousAbort() = 0;
	virtual float LoadModuleSynchronousProgress() const = 0;
	virtual bool LoadModuleSynchronousComplete() const = 0;
	virtual bool LoadModuleSynchronousResult() const = 0;
	virtual bool LoadModuleSynchronousAborted() const = 0;
	inline bool LoadModule(const std::wstring& filePath, const ConnectorMappingList& connectorMappings);
	inline bool SaveSystem(const std::wstring& filePath);
	virtual bool UnloadModule(unsigned int moduleID) = 0;
	virtual void UnloadAllModulesSynchronous() = 0;
	virtual bool UnloadAllModulesSynchronousComplete() const = 0;
	virtual void UnloadAllModules() = 0;
	inline bool ReadModuleConnectorInfo(const std::wstring& filePath, std::wstring& systemClassName, ConnectorImportList& connectorsImported, ConnectorExportList& connectorsExported) const;
	inline std::wstring LoadModuleSynchronousCurrentModuleName() const;
	inline std::wstring UnloadModuleSynchronousCurrentModuleName() const;

	//View functions
	virtual void BuildFileOpenMenu(IMenuSubmenu& menuSubmenu) const = 0;
	virtual void BuildSystemMenu(IMenuSubmenu& menuSubmenu) const = 0;
	virtual void BuildSettingsMenu(IMenuSubmenu& menuSubmenu) const = 0;
	virtual void BuildDebugMenu(IMenuSubmenu& menuSubmenu) const = 0;
	inline bool RestoreViewStateForSystem(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState) const;
	inline bool RestoreViewStateForModule(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID) const;
	inline bool RestoreViewStateForDevice(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID, const std::wstring& deviceInstanceName) const;
	inline bool RestoreViewStateForExtension(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, const std::wstring& extensionInstanceName) const;
	inline bool RestoreViewStateForExtension(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID, const std::wstring& extensionInstanceName) const;

protected:
	//Path functions
	virtual void SetCapturePathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& pathMarshaller) = 0;

	//Logging functions
	virtual void GetEventLogInternal(const InteropSupport::ISTLObjectTarget<std::vector<SystemLogEntry>>& marshaller) const = 0;

	//Embedded ROM functions
	virtual void GetEmbeddedROMIDsInternal(const InteropSupport::ISTLObjectTarget<std::list<unsigned int>>& marshaller) const = 0;
	virtual bool SetEmbeddedROMPathInternal(unsigned int embeddedROMID, const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller) = 0;

	//Module setting functions
	virtual void GetModuleSettingIDsInternal(unsigned int moduleID, const InteropSupport::ISTLObjectTarget<std::list<unsigned int>>& marshaller) const = 0;

	//Device registration
	virtual void UnregisterDeviceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& deviceNameMarshaller) = 0;

	//Extension registration
	virtual void UnregisterExtensionInternal(const InteropSupport::ISTLObjectSource<std::wstring>& extensionNameMarshaller) = 0;

	//Input functions
	virtual KeyCode GetKeyCodeIDInternal(const InteropSupport::ISTLObjectSource<std::wstring>& keyCodeNameMarshaller) const = 0;
	virtual void GetInputDeviceListInternal(const InteropSupport::ISTLObjectTarget<std::list<IDevice*>>& marshaller) const = 0;
	virtual void GetKeyCodeNameInternal(KeyCode keyCode, const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetDeviceKeyCodeListInternal(IDevice* targetDevice, const InteropSupport::ISTLObjectTarget<std::list<unsigned int>>& marshaller) const = 0;
	virtual void GetDeviceKeyCodePreferredDefaultMappingListInternal(IDevice* targetDevice, unsigned int deviceKeyCode, const InteropSupport::ISTLObjectTarget<std::list<KeyCode>>& marshaller) const = 0;

	//Savestate functions
	virtual bool LoadStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller, FileType fileType, bool debuggerState) = 0;
	virtual bool SaveStateInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller, FileType fileType, bool debuggerState) = 0;
	virtual void GetStateInfoInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller, FileType fileType, const InteropSupport::ISTLObjectTarget<StateInfo>& marshaller) const = 0;
	virtual bool LoadModuleRelationshipsNodeInternal(IHierarchicalStorageNode& node, const InteropSupport::ISTLObjectTarget<ModuleRelationshipMap>& relationshipMapMarshaller) const = 0;
	virtual void SaveModuleRelationshipsNodeInternal(IHierarchicalStorageNode& node, bool saveFilePathInfo, const InteropSupport::ISTLObjectSource<std::wstring>& relativePathBaseMarshaller) const = 0;

	//Module loading and unloading
	virtual bool GetModuleDisplayNameInternal(unsigned int moduleID, const InteropSupport::ISTLObjectTarget<std::wstring>& moduleDisplayNameMarshaller) const = 0;
	virtual bool GetModuleInstanceNameInternal(unsigned int moduleID, const InteropSupport::ISTLObjectTarget<std::wstring>& moduleInstanceNameMarshaller) const = 0;
	//##FIX## Shouldn't all the synchronous functions be available here, like progress,
	//result, and aborting?
	virtual void LoadModuleSynchronousInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller, const InteropSupport::ISTLObjectSource<ConnectorMappingList>& connectorMappingsMarshaller) = 0;
	virtual bool LoadModuleInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller, const InteropSupport::ISTLObjectSource<ConnectorMappingList>& connectorMappingsMarshaller) = 0;
	virtual bool SaveSystemInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller) = 0;
	virtual bool ReadModuleConnectorInfoInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& systemClassNameMarshaller, const InteropSupport::ISTLObjectTarget<ConnectorImportList>& connectorsImportedMarshaller, const InteropSupport::ISTLObjectTarget<ConnectorExportList>& connectorsExportedMarshaller) const = 0;
	virtual void LoadModuleSynchronousCurrentModuleNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void UnloadModuleSynchronousCurrentModuleNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//View functions
	virtual bool RestoreViewStateForSystemInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState) const = 0;
	virtual bool RestoreViewStateForModuleInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, unsigned int moduleID) const = 0;
	virtual bool RestoreViewStateForDeviceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, unsigned int moduleID, const InteropSupport::ISTLObjectSource<std::wstring>& deviceInstanceNameMarshaller) const = 0;
	virtual bool RestoreViewStateForExtensionInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, const InteropSupport::ISTLObjectSource<std::wstring>& extensionInstanceNameMarshaller) const = 0;
	virtual bool RestoreViewStateForExtensionInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, unsigned int moduleID, const InteropSupport::ISTLObjectSource<std::wstring>& extensionInstanceNameMarshaller) const = 0;
};

#include "ISystemGUIInterface.inl"
#endif
