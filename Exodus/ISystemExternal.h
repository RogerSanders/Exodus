#ifndef __ISYSTEMEXTERNAL_H__
#define __ISYSTEMEXTERNAL_H__
#include "SystemInterface/SystemInterface.pkg"
#include "DeviceInfo.h"
#include "ExtensionInfo.h"
#include <string>
#include <list>
#include <map>

//##TODO##
//Rename and simplify the base system interfaces. Here is what we have currently:
//-ISystemExternal: Used by ExodusInterface (non-exportable)
//-ISystemInternal: Used by DeviceContext (non-exportable)
//Here is what we need:
//-ISystemExtensionInterface: Used by extensions (exportable)
//-ISystemGuiInterface: Used by ExodusInterface (ideally exportable)
//-ISystemDeviceContextInterface: Used by DeviceContext (non-exportable)
class ISystemExternal :public ISystemExtensionInterface
{
public:
	//Enumerations
	enum FileType;

	//Structures
	struct StateInfo;
	struct ModuleRelationship;
	struct SavedRelationshipImportConnector;
	struct SavedRelationshipExportConnector;
	struct SavedRelationshipModule;
	struct ConnectorMapping;
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

	//Initialization functions
	virtual void BindToGUIExtensionInterface(IGUIExtensionInterface* aguiExtensionInterface) = 0;

	//Savestate functions
	//##TODO## Consider not making the name of a savestate xml file within a zip archive
	//be hardcoded to "save.xml". Come up with a way for the name within the archive to
	//be specified by the caller perhaps? The problem is, we don't want to have to load
	//the zip file twice.
	virtual bool LoadState(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType, bool debuggerState) = 0;
	virtual bool SaveState(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType, bool debuggerState) = 0;
	virtual StateInfo GetStateInfo(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType) const = 0;
	virtual bool LoadModuleRelationshipsNode(IHeirarchicalStorageNode& node, ModuleRelationshipMap& relationshipMap) const = 0;
	virtual void SaveModuleRelationshipsNode(IHeirarchicalStorageNode& relationshipsNode, bool saveFilePathInfo = false, const std::wstring& relativePathBase = L"") const = 0;

	//Path functions
	virtual std::wstring GetCapturePath() const = 0;
	virtual void SetCapturePath(const std::wstring& apath) = 0;

	//System interface functions
	virtual void FlagStopSystem() = 0;
	virtual bool GetThrottlingState() const = 0;
	virtual void SetThrottlingState(bool state) = 0;
	virtual bool GetRunWhenProgramModuleLoadedState() const = 0;
	virtual void SetRunWhenProgramModuleLoadedState(bool state) = 0;
	virtual bool GetEnablePersistentState() const = 0;
	virtual void SetEnablePersistentState(bool state) = 0;

	//Device registration
	virtual bool RegisterDevice(const DeviceInfo& entry, IDevice::AssemblyHandle assemblyHandle) = 0;
	virtual void UnregisterDevice(const std::wstring deviceName) = 0;

	//Extension registration
	virtual bool RegisterExtension(const ExtensionInfo& entry, IExtension::AssemblyHandle assemblyHandle) = 0;
	virtual void UnregisterExtension(const std::wstring extensionName) = 0;

	//Module loading and unloading
	//##TODO## Add the use of FileType to module loading, so that a module definition can
	//be contained either in a zip file or as a standalone file.
	//##TODO## As per the requirement for savestates above, make it possible to specify
	//the target name of a file within a zip archive. This will be necessary with modules,
	//where it is highly desirable to be able to create a zip file with a set of related
	//system definitions.
	virtual bool GetModuleDisplayName(unsigned int moduleID, std::wstring& moduleDisplayName) const = 0;
	virtual bool GetModuleInstanceName(unsigned int moduleID, std::wstring& moduleInstanceName) const = 0;
	virtual void LoadModuleSynchronous(const std::wstring& fileDir, const std::wstring& fileName, const ConnectorMappingList& connectorMappings, IViewModelLauncher& aviewModelLauncher) = 0;
	virtual void LoadModuleSynchronousAbort() = 0;
	virtual float LoadModuleSynchronousProgress() const = 0;
	virtual bool LoadModuleSynchronousComplete() const = 0;
	virtual bool LoadModuleSynchronousResult() const = 0;
	virtual bool LoadModuleSynchronousAborted() const = 0;
	virtual bool LoadModule(const std::wstring& fileDir, const std::wstring& fileName, const ConnectorMappingList& connectorMappings, IViewModelLauncher& aviewModelLauncher) = 0;
	virtual bool SaveSystem(const std::wstring& fileDir, const std::wstring& fileName) = 0;
	virtual bool UnloadModule(unsigned int moduleID) = 0;
	virtual void UnloadAllModulesSynchronous() = 0;
	virtual bool UnloadAllModulesSynchronousComplete() const = 0;
	virtual void UnloadAllModules() = 0;
	virtual bool ReadModuleConnectorInfo(const std::wstring& fileDir, const std::wstring& fileName, std::wstring& systemClassName, ConnectorImportList& connectorsImported, ConnectorExportList& connectorsExported) const = 0;
	virtual std::wstring LoadModuleSynchronousCurrentModuleName() const = 0;
	virtual std::wstring UnloadModuleSynchronousCurrentModuleName() const = 0;

	//View functions
	virtual void BuildFileOpenMenu(IMenuSubmenu& menuSubmenu, IViewModelLauncher& viewModelLauncher) = 0;
	virtual void BuildSystemMenu(IMenuSubmenu& menuSubmenu, IViewModelLauncher& viewModelLauncher) = 0;
	virtual void BuildSettingsMenu(IMenuSubmenu& menuSubmenu, IViewModelLauncher& viewModelLauncher) = 0;
	virtual void BuildDebugMenu(IMenuSubmenu& menuSubmenu, IViewModelLauncher& viewModelLauncher) = 0;
	virtual void RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher) = 0;
	virtual bool RestoreViewModelStateForDevice(unsigned int moduleID, const std::wstring& deviceInstanceName, const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher) = 0;
	virtual void DeleteSystemViews() = 0;
};

#include "ISystemExternal.inl"
#endif
