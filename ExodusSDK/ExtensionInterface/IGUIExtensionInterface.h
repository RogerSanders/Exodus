#ifndef __IGUIEXTENSIONINTERFACE_H__
#define __IGUIEXTENSIONINTERFACE_H__
#include "StreamInterface/StreamInterface.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include "IViewManager.h"
#include <string>
#include <vector>
using namespace MarshalSupport::Operators;

//##TODO## Add a method here to create an image, so our system object doesn't have to
// explicitly link to the image library?
class IGUIExtensionInterface
{
public:
	// Constructors
	virtual ~IGUIExtensionInterface() = 0 { }

	// Interface version functions
	static inline unsigned int ThisIGUIExtensionInterfaceVersion() { return 1; }
	virtual unsigned int GetIGUIExtensionInterfaceVersion() const = 0;

	// View manager functions
	virtual IViewManager& GetViewManager() const = 0;

	// Window functions
	virtual void* GetMainWindowHandle() const = 0;

	// Module functions
	virtual bool CanModuleBeLoaded(const Marshal::In<std::wstring>& filePath) const = 0;
	virtual bool LoadModuleFromFile(const Marshal::In<std::wstring>& filePath) = 0;
	virtual void UnloadModule(unsigned int moduleID) = 0;
	virtual void UnloadAllModules() = 0;

	// Global preference functions
	//##TODO## Design a more extensible interface for working with preferences which
	// doesn't rely on separate functions for each preference
	//##TODO## Consider using a system built on our generic data access system in order to
	// access preferences. We could request a preference using a generic value/string key
	// pair. The string keys themselves could be listed in our interface and referenced by
	// a named constant. This would be easily extensible and flexible, without the need for
	// specific functions for individual preferences.
	virtual Marshal::Ret<std::wstring> GetGlobalPreferencePathModules() const = 0;
	virtual Marshal::Ret<std::wstring> GetGlobalPreferencePathSavestates() const = 0;
	virtual Marshal::Ret<std::wstring> GetGlobalPreferencePathPersistentState() const = 0;
	virtual Marshal::Ret<std::wstring> GetGlobalPreferencePathWorkspaces() const = 0;
	virtual Marshal::Ret<std::wstring> GetGlobalPreferencePathCaptures() const = 0;
	virtual Marshal::Ret<std::wstring> GetGlobalPreferencePathAssemblies() const = 0;
	virtual Marshal::Ret<std::wstring> GetGlobalPreferenceInitialSystem() const = 0;
	virtual Marshal::Ret<std::wstring> GetGlobalPreferenceInitialWorkspace() const = 0;
	virtual bool GetGlobalPreferenceEnableThrottling() const = 0;
	virtual bool GetGlobalPreferenceRunWhenProgramModuleLoaded() const = 0;
	virtual bool GetGlobalPreferenceEnablePersistentState() const = 0;
	virtual bool GetGlobalPreferenceLoadWorkspaceWithDebugState() const = 0;
	virtual bool GetGlobalPreferenceShowDebugConsole() const = 0;

	// Assembly functions
	virtual bool LoadAssembly(const Marshal::In<std::wstring>& filePath) = 0;

	// File selection functions
	virtual bool SelectExistingFile(const Marshal::In<std::wstring>& selectionTypeString, const Marshal::In<std::wstring>& defaultExtension, const Marshal::In<std::wstring>& initialFilePath, const Marshal::In<std::wstring>& initialDirectory, bool scanIntoArchives, const Marshal::Out<std::wstring>& selectedFilePath) const = 0;
	virtual bool SelectNewFile(const Marshal::In<std::wstring>& selectionTypeString, const Marshal::In<std::wstring>& defaultExtension, const Marshal::In<std::wstring>& initialFilePath, const Marshal::In<std::wstring>& initialDirectory, const Marshal::Out<std::wstring>& selectedFilePath) const = 0;
	virtual Marshal::Ret<std::vector<std::wstring>> PathSplitElements(const Marshal::In<std::wstring>& path) const = 0;
	virtual Stream::IStream* OpenExistingFileForRead(const Marshal::In<std::wstring>& path) const = 0;
	virtual void DeleteFileStream(Stream::IStream* stream) const = 0;
};

#endif
