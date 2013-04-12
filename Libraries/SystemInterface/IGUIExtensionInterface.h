#ifndef __IGUIEXTENSIONINTERFACE_H__
#define __IGUIEXTENSIONINTERFACE_H__
#include "StreamInterface/StreamInterface.pkg"
#include <string>
#include <vector>

class IGUIExtensionInterface
{
public:
	//Constructors
	virtual ~IGUIExtensionInterface() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIGUIExtensionInterfaceVersion();
	virtual unsigned int GetIGUIExtensionInterfaceVersion() const = 0;

	//Main window functions
	virtual void* GetMainWindowHandle() const = 0;

	//Module functions
	inline bool CanModuleBeLoaded(const std::wstring& filePath) const;
	inline bool LoadModuleFromFile(const std::wstring& filePath);
	virtual void UnloadModule(unsigned int moduleID) = 0;
	virtual void UnloadAllModules() = 0;

	//Global preference functions
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
	virtual bool GetEnablePersistentState() const = 0;

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
	virtual bool CanModuleBeLoadedInternal(const wchar_t* filePath) const = 0;
	virtual bool LoadModuleFromFileInternal(const wchar_t* filePath) = 0;

	//Global preference functions
	virtual const wchar_t* GetGlobalPreferencePathModulesInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferencePathSavestatesInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferencePathPersistentStateInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferencePathWorkspacesInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferencePathCapturesInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferencePathAssembliesInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferenceInitialSystemInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferenceInitialWorkspaceInternal() const = 0;

	//Assembly functions
	virtual bool LoadAssemblyInternal(const wchar_t* filePath) = 0;

	//File selection functions
	virtual bool SelectExistingFileInternal(const wchar_t* selectionTypeString, const wchar_t* defaultExtension, const wchar_t* initialFilePath, const wchar_t* initialDirectory, bool scanIntoArchives, const wchar_t** selectedFilePath) const = 0;
	virtual bool SelectNewFileInternal(const wchar_t* selectionTypeString, const wchar_t* defaultExtension, const wchar_t* initialFilePath, const wchar_t* initialDirectory, const wchar_t** selectedFilePath) const = 0;
	virtual const wchar_t** PathSplitElementsInternal(const wchar_t* path, unsigned int& arraySize) const = 0;
	virtual void PathSplitElementsInternalFreeArray(const wchar_t** itemArray, unsigned int arraySize) const = 0;
	virtual Stream::IStream* OpenExistingFileForReadInternal(const wchar_t* path) const = 0;
};

#include "IGUIExtensionInterface.inl"
#endif
