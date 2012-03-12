#ifndef __ISYSTEMEXTERNAL_H__
#define __ISYSTEMEXTERNAL_H__
#include "SystemInterface/SystemInterface.pkg"
#include "DeviceInfo.h"
#include <string>
#include <list>
#include <map>

class ISystemExternal
{
public:
	//Enumerations
	enum FileType;

	//Structures
	struct StateInfo;
	struct LoadedModuleInfo;
	struct ModuleRelationship;
	struct SavedRelationshipImportConnector;
	struct SavedRelationshipExportConnector;
	struct SavedRelationshipModule;
	struct ConnectorMapping;
	struct ConnectorInfo;
	struct ConnectorDefinitionImport;
	struct ConnectorDefinitionExport;

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
	//Constructors
	virtual ~ISystemExternal() = 0 {}

	//Savestate functions
	//##TODO## Consider not making the name of a savestate xml file within a zip archive
	//be hardcoded to "save.xml". Come up with a way for the name within the archive to
	//be specified by the caller perhaps? The problem is, we don't want to have to load
	//the zip file twice.
	virtual bool LoadState(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType) = 0;
	virtual bool SaveState(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType) = 0;
	virtual StateInfo GetStateInfo(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType) const = 0;
	virtual bool LoadDebuggerState(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType) = 0;
	virtual bool SaveDebuggerState(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType) = 0;
	virtual bool LoadModuleRelationshipsNode(IHeirarchicalStorageNode& node, ModuleRelationshipMap& relationshipMap) const = 0;
	virtual bool DoesLoadedModuleMatchSavedModule(const SavedRelationshipMap& savedRelationshipData, const SavedRelationshipModule& savedModuleInfo, const LoadedModuleInfo& loadedModuleInfo, const ConnectorInfoMapOnImportingModuleID& connectorDetailsOnImportingModuleID) const = 0;
	virtual void SaveModuleRelationshipsNode(IHeirarchicalStorageNode& relationshipsNode) const = 0;

	//Path functions
	virtual std::wstring GetCapturePath() const = 0;
	virtual void SetCapturePath(const std::wstring& apath) = 0;

	//System interface functions
	virtual void Initialize() = 0;
	virtual void RunSystem() = 0;
	virtual void StopSystem() = 0;
	virtual void FlagStopSystem() = 0;
	virtual bool GetThrottlingState() const = 0;
	virtual void SetThrottlingState(bool state) = 0;
	virtual bool SystemRunning() const = 0;

	//Device registration
	virtual bool RegisterDevice(const DeviceInfo& entry, IDevice::AssemblyHandle assemblyHandle) = 0;
	virtual void UnregisterDevice(const std::wstring deviceName) = 0;

	//Module loading and unloading
	//##TODO## Add the use of FileType to module loading, so that a module definition can
	//be contained either in a zip file or as a standalone file.
	//##TODO## As per the requirement for savestates above, make it possible to specify
	//the target name of a file within a zip archive. This will be necessary with modules,
	//where it is highly desirable to be able to create a zip file with a set of related
	//system definitions.
	virtual bool GetModuleDisplayName(unsigned int moduleID, std::wstring& moduleDisplayName) const = 0;
	virtual void LoadModuleSynchronous(const std::wstring& fileDir, const std::wstring& fileName, const ConnectorMappingList& connectorMappings, IViewModelLauncher& aviewModelLauncher) = 0;
	virtual void LoadModuleSynchronousAbort() = 0;
	virtual float LoadModuleSynchronousProgress() const = 0;
	virtual bool LoadModuleSynchronousComplete() const = 0;
	virtual bool LoadModuleSynchronousResult() const = 0;
	virtual bool LoadModuleSynchronousAborted() const = 0;
	virtual bool LoadModule(const std::wstring& fileDir, const std::wstring& fileName, const ConnectorMappingList& connectorMappings, IViewModelLauncher& aviewModelLauncher) = 0;
	virtual bool UnloadModule(unsigned int moduleID) = 0;
	virtual void UnloadAllModulesSynchronous() = 0;
	virtual bool UnloadAllModulesSynchronousComplete() const = 0;
	virtual void UnloadAllModules() = 0;
	virtual bool ReadModuleConnectorInfo(const std::wstring& fileDir, const std::wstring& fileName, std::wstring& systemClassName, ConnectorImportList& connectorsImported, ConnectorExportList& connectorsExported) const = 0;

	//Loaded module info functions
	virtual std::list<unsigned int> GetLoadedModuleIDs() const = 0;
	virtual bool GetLoadedModuleInfo(unsigned int moduleID, LoadedModuleInfo& moduleInfo) const = 0;

	//Connector info functions
	virtual std::list<unsigned int> GetConnectorIDs() const = 0;
	virtual bool GetConnectorInfo(unsigned int connectorID, ConnectorInfo& connectorInfo) const = 0;

	//View functions
	virtual void BuildDebugMenu(IMenuSubmenu& menuSubmenu, IViewModelLauncher& viewModelLauncher) const = 0;
	virtual void RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher) = 0;
	virtual bool RestoreViewModelStateForDevice(unsigned int moduleID, const std::wstring& deviceInstanceName, const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher) = 0;
	virtual void DeleteSystemViews() = 0;
};

#include "ISystemExternal.inl"
#endif
