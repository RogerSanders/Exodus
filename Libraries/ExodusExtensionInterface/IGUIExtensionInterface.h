#ifndef __IGUIEXTENSIONINTERFACE_H__
#define __IGUIEXTENSIONINTERFACE_H__
#include "StreamInterface/StreamInterface.pkg"
#include "InteropSupport/InteropSupport.pkg"
#include "IViewManager.h"
#include <string>
#include <vector>

//##TODO## Add a method here to create an image, so our system object doesn't have to
//explicitly link to the image library?
class IGUIExtensionInterface
{
public:
	//Constructors
	virtual ~IGUIExtensionInterface() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIGUIExtensionInterfaceVersion();
	virtual unsigned int GetIGUIExtensionInterfaceVersion() const = 0;

	//View manager functions
	virtual IViewManager& GetViewManager() const = 0;

	//Window functions
	virtual void* GetMainWindowHandle() const = 0;

	//Module functions
	inline bool CanModuleBeLoaded(const std::wstring& filePath) const;
	inline bool LoadModuleFromFile(const std::wstring& filePath);
	virtual void UnloadModule(unsigned int moduleID) = 0;
	virtual void UnloadAllModules() = 0;

	//Global preference functions
	//##TODO## Design a more extensible interface for working with preferences which
	//doesn't rely on separate functions for each preference
	inline std::wstring GetGlobalPreferencePathModules() const;
	inline std::wstring GetGlobalPreferencePathSavestates() const;
	inline std::wstring GetGlobalPreferencePathPersistentState() const;
	inline std::wstring GetGlobalPreferencePathWorkspaces() const;
	inline std::wstring GetGlobalPreferencePathCaptures() const;
	inline std::wstring GetGlobalPreferencePathAssemblies() const;
	inline std::wstring GetGlobalPreferenceInitialSystem() const;
	inline std::wstring GetGlobalPreferenceInitialWorkspace() const;
	virtual bool GetGlobalPreferenceEnableThrottling() const = 0;
	virtual bool GetGlobalPreferenceRunWhenProgramModuleLoaded() const = 0;
	virtual bool GetGlobalPreferenceEnablePersistentState() const = 0;
	virtual bool GetGlobalPreferenceLoadWorkspaceWithDebugState() const = 0;
	virtual bool GetGlobalPreferenceShowDebugConsole() const = 0;

	//Assembly functions
	inline bool LoadAssembly(const std::wstring& filePath);

	//File selection functions
	inline bool SelectExistingFile(const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, bool scanIntoArchives, std::wstring& selectedFilePath) const;
	inline bool SelectNewFile(const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath) const;
	inline std::vector<std::wstring> PathSplitElements(const std::wstring& path) const;
	inline Stream::IStream* OpenExistingFileForRead(const std::wstring& path) const;
	virtual void DeleteFileStream(Stream::IStream* stream) const = 0;

protected:
	//Module functions
	virtual bool CanModuleBeLoadedInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller) const = 0;
	virtual bool LoadModuleFromFileInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller) = 0;

	//Global preference functions
	virtual void GetGlobalPreferencePathModulesInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetGlobalPreferencePathSavestatesInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetGlobalPreferencePathPersistentStateInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetGlobalPreferencePathWorkspacesInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetGlobalPreferencePathCapturesInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetGlobalPreferencePathAssembliesInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetGlobalPreferenceInitialSystemInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetGlobalPreferenceInitialWorkspaceInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Assembly functions
	virtual bool LoadAssemblyInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller) = 0;

	//File selection functions
	virtual bool SelectExistingFileInternal(const InteropSupport::ISTLObjectSource<std::wstring>& selectionTypeStringMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& defaultExtensionMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& initialFilePathMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& initialDirectoryMarshaller, bool scanIntoArchives, const InteropSupport::ISTLObjectTarget<std::wstring>& selectedFilePathMarshaller) const = 0;
	virtual bool SelectNewFileInternal(const InteropSupport::ISTLObjectSource<std::wstring>& selectionTypeStringMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& defaultExtensionMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& initialFilePathMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& initialDirectoryMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& selectedFilePathMarshaller) const = 0;
	virtual void PathSplitElementsInternal(const InteropSupport::ISTLObjectTarget<std::vector<std::wstring>>& marshaller, const InteropSupport::ISTLObjectSource<std::wstring>& pathMarshaller) const = 0;
	virtual Stream::IStream* OpenExistingFileForReadInternal(const InteropSupport::ISTLObjectSource<std::wstring>& pathMarshaller) const = 0;
};

#include "IGUIExtensionInterface.inl"
#endif
